//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:		GenTemp.cpp
// Compiler:		Standard C++ compiler.
// Output Size:
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		21-Feb-2005 (converted for AES)   RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		This program generates a template for
//			configuration file
//			used by the 'create' tool.
//
//			Three random keys, a random signature and a
//			random initial
//			vector are generated. Other parameters 
//			are left unfilled.
//
// Other Info:		
//=============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//=============================================================================
// Copyright string, displayed always.

char *copyrightString =
	"\ngentemp v1.0AES - Copyright (C) 2005 Atmel Corporation\n\n";


//=============================================================================
// Usage string that is displayed, if no arguments are given

char *usageString =
	"Usage: gentemp <filename>\n\n";


//=============================================================================
// Generate random, but parity checked bytes, make sure there is
// room for an inserted parity bit after every 8th bit, ie. the
// destination buffer must have room for one extra byte for every 8
// bytes, eg. size=8 gives 9 bytes in the destination buffer.

void generateRandomParityCheckedBytes(unsigned char *destination, int size)
{
	int bitposition = 0; // Keeps track of position after last parity bit.

	while (size--)
	{
		unsigned char byte = rand();
		unsigned char parity = byte;

		parity ^= parity >> 4;
		parity ^= parity >> 2;
		parity ^= parity >> 1;

		parity = ~parity & 0x01; // Not bit 0 is the parity bit.

		if( bitposition%8 == 0 ) {
			destination[ bitposition/8     ] = byte;
			destination[ bitposition/8 + 1 ] = parity << 7;
		} else {
			destination[ bitposition/8     ] |=
				byte >> (bitposition%8);
			destination[ bitposition/8 + 1 ] =
				byte << (8-bitposition%8) |
				parity << (7-bitposition%8);
		}

		bitposition += 9;
	}
}


//=============================================================================
// Generate random bytes

void generateRandomBytes(unsigned char *destination, int size)
{
	while (size--)
		*destination++ = rand();
}


//=============================================================================
// Convert bytes to a hex string

char *bytesToHexString(char *string, unsigned char *bytes, int size)
{
	int index = 0;

	while (size--)
	{
		int value = *bytes++;

		sprintf(&string[index], "%02X", value);
		index += 2;
	}

	return string;
}


//=============================================================================
// Main, starts here!

int main(int argc, char* argv[])
{	
	printf(copyrightString);

	// Choose a random seed based on current time
	srand((unsigned)time(NULL));
	
	// Skip first command line arg (string containing the name of the program)
	argv++;
	argc--;

	// If no arguments given, show usage
	if (argc == 0 || argc > 1)
	{
		printf(usageString);
		return -1;
	}

	char *filename = argv[0];
	FILE *outputFile = NULL;

	if (outputFile = fopen(filename, "w"))
	{
		unsigned char tempBytes[18];
		char tempString[37];

		fprintf(outputFile,
			"PAGE_SIZE      = [FILL IN: Target AVR page size in bytes]\n");
		fprintf(outputFile,
			"MEM_SIZE       = [FILL IN: Application Section size in bytes]\n");
		fprintf(outputFile,
			"CRC_ENABLE     = [FILL IN: YES/NO]\n");

		generateRandomParityCheckedBytes(tempBytes, 18);
		fprintf(outputFile, "KEY1           = %s\n",
			bytesToHexString(tempString, tempBytes, 18)
		);
		
		generateRandomParityCheckedBytes(tempBytes, 9);
		fprintf(outputFile, "KEY2           = %s\n",
			bytesToHexString(tempString, tempBytes, 9)
		);

		generateRandomParityCheckedBytes(tempBytes, 9);
		fprintf(outputFile, "KEY3           = %s\n",
			bytesToHexString(tempString, tempBytes, 9)
		);

		generateRandomBytes(tempBytes, 16);
		fprintf(outputFile, "INITIAL_VECTOR = %s\n",
			bytesToHexString(tempString, tempBytes, 16)
		);

		generateRandomBytes(tempBytes, 4);
		fprintf(outputFile, "SIGNATURE      = %s\n",
			bytesToHexString(tempString, tempBytes, 4)
		);

		printf("%s generated.\n", filename);

		fclose(outputFile);
		return 0;
	}
	else
	{
		printf("Error: Couldn't create file %s.", filename);
		return -1;
	}
}
