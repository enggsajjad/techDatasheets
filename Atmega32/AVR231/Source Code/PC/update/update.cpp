//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:		Update.cpp
// Compiler:		Standard C++ compiler (Win32)	
// Output Size:
// Based on work by:ØE, VU
// Created:		27-Feb-2003	JP (Atmel Finland)
// Modified:		21-Mar-2005 (modified for AES)    RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		This program is used for sending an 
//			encrypted file into the
//			target AVR. Encrypted files are 
//			created using the 'create'
//			tool.
//
// Other Info:		
//=============================================================================


#include "ComPort.h"
#include "update.h"
#include <stdio.h>
#include <ctype.h>


//=============================================================================

char *progressChars =
	"--\\\\||//";


//=============================================================================

char *copyrightString =
	"update v1.0 - Copyright (C) 2003 Atmel Corporation\n\n";


//=============================================================================

const char *usageString = 
	"usage: update <filename.ext> [-COMx] [-baudrate]\n\n"
	"  filename.ext    File containing flash upgrade buffer.\n"
	"  -COMx           Communications port number.\n"
	"  -baudrate       Communication baudrate, e.g. 9600, 115200.\n"
	"                  Supported baudrates:\n"
	"                    110, 300, 600, 1200, 2400,\n"
	"                    4800, 9600, 14400, 19200, 38400,\n"
	"                    56000, 57600, 115200, 128000, 256000\n\n";


//=============================================================================
// Starts here! Opens the COM port, reads the file into buffer and sends the
// buffer to the target device via COM port.

int main(int argc, char* argv[])
{
	FILE *inFile;

	char comPortName[5];			// Name of the COM port that is used for
									// transferring
	unsigned char receivedByte;		// One byte received from COM port
	unsigned char retries = 0;		// Number of tries so far
	int index;				// Index in the data buffer
	int frameSize;				// Number of bytes in the current frame
	int bufferSize = 0;			// Total number of bytes in the buffer
	int bytesSent = 0;			// Number of bytes sent so far

	unsigned char buffer[0x40000];	// Data buffer for sending the data

	printf(copyrightString);
	
	ComPort *comPort = new ComPort;	// Instance of the ComPort object

	argc--;
	argv++;

	// If no arguments
	// print Errormessage.
	if ((argc != 3) || (argv[0][0] == '-'))
	{
		printf(usageString);
		return -1;
	}

	// Read com port number
	for( char * cp = argv[1]; *cp; cp++ ) {
		*cp = toupper( *cp );
	}
	if(strstr(argv[1], "-COM"))
		strcpy(comPortName, &argv[1][1]);
	else
	{
		printf(usageString);
		return -1;
	}

	// Read baudrate (the smaller ones are supperted for completeness only :-D )
	long baudrate;
	if( strcmp( argv[ 2 ], "-256000" ) == 0 ) {
		baudrate = CBR_256000;
	} else if( strcmp( argv[ 2 ], "-128000" ) == 0 ) {
		baudrate = CBR_128000;
	} else if( strcmp( argv[ 2 ], "-115200" ) == 0 ) {
		baudrate = CBR_115200;
	} else if( strcmp( argv[ 2 ], "-57600" ) == 0 ) {
		baudrate = CBR_57600;
	} else if( strcmp( argv[ 2 ], "-56000" ) == 0 ) {
		baudrate = CBR_56000;
	} else if( strcmp( argv[ 2 ], "-38400" ) == 0 ) {
		baudrate = CBR_38400;
	} else if( strcmp( argv[ 2 ], "-19200" ) == 0 ) {
		baudrate = CBR_19200;
	} else if( strcmp( argv[ 2 ], "-14400" ) == 0 ) {
		baudrate = CBR_14400;
	} else if( strcmp( argv[ 2 ], "-9600" ) == 0 ) {
		baudrate = CBR_9600;
	} else if( strcmp( argv[ 2 ], "-4800" ) == 0 ) {
		baudrate = CBR_4800;
	} else if( strcmp( argv[ 2 ], "-2400" ) == 0 ) {
		baudrate = CBR_2400;
	} else if( strcmp( argv[ 2 ], "-1200" ) == 0 ) {
		baudrate = CBR_1200;
	} else if( strcmp( argv[ 2 ], "-600" ) == 0 ) {
		baudrate = CBR_600;
	} else if( strcmp( argv[ 2 ], "-300" ) == 0 ) {
		baudrate = CBR_300;
	} else if( strcmp( argv[ 2 ], "-110" ) == 0 ) {
		baudrate = CBR_110;
	} else {
		printf(usageString);
		return -1;
	}
	
	inFile = fopen(argv[0], "rb");
	if (inFile == NULL)
	{
		printf("Error: File '%s' not found.\n", argv[0]);
		comPort->close();
		return -1;
	}

	// Open COM port
	if (!comPort->open(comPortName, baudrate, 2000))
	{
		printf("Error: %s in use or does not exist.\n", comPortName);
		return -1;
	}
	printf("%s opened.\n", comPortName);

	// Read the whole file into the buffer
	while(fscanf(inFile, "%2X", &buffer[bufferSize]) == 1)
		bufferSize++;

	char * progressCharP = progressChars;	
	for(index = 0; index < bufferSize; index += frameSize)
	{
		frameSize = ((buffer[index] << 8) | buffer[index + 1]) + 2;

		// Ignore any received bytes.
		comPort->setTimeOut(1);
		while (comPort->read(&receivedByte, 1));
		comPort->setTimeOut(4000);
		
		// Send the frame
		printf("\r                        \r"
		       "Transferring.. %c %d%% done.",
			*progressCharP++,
			100 * bytesSent / bufferSize);
		if( !(*progressCharP) ) progressCharP = progressChars;
		fflush( stdout );
		comPort->write(&buffer[index], frameSize);

		// Check the response
		if (comPort->read(&receivedByte, 1))
		{
			switch (receivedByte)
			{
				case Error_OK:
					bytesSent += frameSize;
					retries = 0;
					break;

				default: // Error_CRC:
					if (++retries < 4)
						index -= frameSize;
					else 
					{
						printf("\nCRC error. File damaged.\n");
						comPort->close();
						fclose(inFile);
						return -1;
					}
					break;
			}
		}
		else
		{
			printf("\nFailed: Target is not responding.\n");

			comPort->close();
			fclose(inFile);

			return -1;
		}
	}

	// All done
	printf("\rTransferring.. 100%% done.  \n");
	fclose(inFile);
	comPort->close();

	printf("Target updated successfully.\n");
	return 0;
}
