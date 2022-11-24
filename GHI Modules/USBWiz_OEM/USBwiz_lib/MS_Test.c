/*
 * Copyright (c) 2006, GHI Electronics, LLC
 * All rights reserved. 
 *
 * You can use this file if you agree to the following
 *
 * 1. This header can't be changed under any condition
 *    
 * 2. This is free software and therefore is providede with NO warranty 
 * 
 * 3. Fell free to modify the codes but we ask you to provide us with 
 *	  any bugs reports so we can keep the code up to date
 *
 * 4. This code may ONLY be used with GHI Electronics, LLC products
 *
 * THIS SOFTWARE IS PROVIDED BY GHI ELECTRONICS, LLC ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL 
 * GHI ELECTRONICS, LLC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR ORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 *	Specs are subject to change without any notice
 */


#include "MS_Test.h"

#ifdef _INCLUDE_FAT_SYSTEM_SUPPORT_

#define ONE_MB_OVER_16 0x10000

int8 FileSystemTest(void)
{
	int8 file_name[12], c, file_ext[4], attr;
	int32 var32, size;
	
	strcpypgm2ram((char *) file_name, ROM_TYPE"GHITEST");
	printf(ROM_TYPE"Creating Test Directory \"GHITEST\" \r");
	c = GHI_MakeDirectory(file_name);
	if(c == DIR_ENT_FOUND)	// already exits
	{
		printf(ROM_TYPE"Directory already exist. Please delete it and re-try\r");
		return c;
	}
	else if(c)		// if some error
	{
		printf(ROM_TYPE"Could not create directory\r");
		return c;
	}
	printf(ROM_TYPE"Done...\r");
		
	printf(ROM_TYPE"Creating directories inside GHITEST\r");
	if(c = GHI_ChangeDirectory(file_name))
	{
		printf(ROM_TYPE"Can't access GHITEST\r");
		return c;
	}
	// creating DIR1, DIR2, DIR3
	strcpypgm2ram((char *)file_name, ROM_TYPE"DIR#");
	for(var32 = 1; var32 <= 3; var32++)
	{
		file_name[3] = var32 + '0';
		printf(ROM_TYPE"%s", file_name);
		if(c = GHI_MakeDirectory(file_name))
		{
			printf(ROM_TYPE" ,Cannot be created.\r");
			return c;
		}
		printf(ROM_TYPE" ,Created succesfully.\r");
	}
	// Creating file.ext using handle0
	printf(ROM_TYPE"Creating file.ext inside GHITEST\r");
	strcpypgm2ram((char *)file_name, ROM_TYPE"file.ext");
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_WRITE_MODE))
	{
		printf(ROM_TYPE"Cannot create file\r");
		return c;
	}
	printf(ROM_TYPE"Writing 6 Bytes to the file...\r");
	// send write command
	if(c = GHI_SendWriteCommand(FILE_HANDLE_0, 6))
	{
		printf(ROM_TYPE"Cannot write to the file\r");
		return c;
	}
	// send 6 bytes
	for(c = 0; c < 6; c++)
	{
		GHI_PutC(c + '0');
	}
	// check write results
	if(c = GHI_GetReadAndWriteResult(&var32))
	{
		printf(ROM_TYPE"Error writing to the file\r");
		return c;
	}
	GHI_PrintDWORD(var32);
	printf(ROM_TYPE" bytes were successfully written to the file.\r");
	// Close file and reopen for reaing
	printf(ROM_TYPE"Reading data from %s\r", file_name);
	if(c = GHI_CloseFile(FILE_HANDLE_0))
	{
		printf(ROM_TYPE"Cannot close file after writing..\r");
		return c;
	}
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_READ_MODE))
	{
		printf(ROM_TYPE"Cannot open the file...\r");
		return c;
	}
	// read 8 bytes
	printf(ROM_TYPE"Reading 8 bytes using * as a filler\r");
	if(c = GHI_SendReadCommand(FILE_HANDLE_0 ,8, '*'))
	{
		printf(ROM_TYPE"Cannot write to the file.\r");
		return c;
	}
	// read data
	for(c = 0; c < 8; c++)
	{
		while(BusyUSART());
		putcUSART(GHI_GetC());
	}
	while(BusyUSART());
	putcUSART('\r');
	// get read results
	if(c = GHI_GetReadAndWriteResult(&var32))
	{
		printf(ROM_TYPE"Error reading from the file\r");
		return c;
	}
	GHI_PrintDWORD(var32);
	printf(ROM_TYPE" bytes were successfully read from the file.\r");
	if(c =GHI_CloseFile(FILE_HANDLE_0))
	{
		printf(ROM_TYPE"Cannot close the file.\r");
		return c;
	}
		
	// enumerate files
	printf(ROM_TYPE"Listing Files:\r");
	if(c = GHI_InitGetFile())
	{
		printf(ROM_TYPE"Cannot Get files list\r");
		return c;
	}
	var32 = 0;
	// terminate buffer at the end!!
	file_name[8] = 0;
	file_ext[3] = 0;
	while(1)
	{
		var32 = GHI_GetNextFile(file_name, file_ext, &attr, &size);
		if(var32 == ERROR_END_OF_DIR_LIST)
		{
			break;
		}
		if(var32)	// error
		{
			printf(ROM_TYPE"Error Getting files list\r");
			return var32;
		}
	
		printf(ROM_TYPE"%s.%s  %02X  ", file_name, file_ext, attr);
		GHI_PrintDWORD(size);
		while(BusyUSART());
		putcUSART('\r');
	}
	if(var32 != ERROR_END_OF_DIR_LIST)	// error
	{
		printf(ROM_TYPE"Error in lising files.\r");
		return var32;
	}
	TestDone();
	
	return ERROR_NO_ERROR;
}
//---------------------------------------------
int8 DataLoggerTest(void)
{
	int8 var8, c, file_name[16], buffer[64];
	int32 var32, size;
	
	// Initialize time
	while(1)
	{
		printf(ROM_TYPE"Is a 32 khz crystal and a back-up battery connected?[Y,N]\r");
		while(!DataRdyUSART());
		var8 = getcUSART();
		if(var8 == 'y' || var8 == 'Y')
		{
			c = GHI_InitializeTime(1);
			break;
		}
		else if(var8 == 'n' || var8 == 'N')
		{
			c = GHI_InitializeTime(0);
			break;
		}
		else
		{
			printf(ROM_TYPE"Wrong entry. Try again.\r");
			continue;
		}
	}
	if(c)	// error
	{
		printf(ROM_TYPE"Error initializing time.\r");
		return c;
	}
	// ask the user for the time
	printf(ROM_TYPE"Enter the time in the following format :\r"
				   "mm/dd/yyyy hh:mm:ss\r");
	// fill in the structure from user
	if(c = SetTimeStructure(&var32))
	{
		printf(ROM_TYPE"Error parsing time structure\r");
		return c;
	}
	// send it to USBwiz
	if(c = GHI_SetTime(var32))
	{
		printf(ROM_TYPE"Could not set the time\r");
		return c;
	}
	// open log.txt
	strcpypgm2ram((char*)file_name, ROM_TYPE"LOG.TXT");
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_WRITE_MODE))
	{
		printf(ROM_TYPE"Cannot open %s\r", file_name);
		return c;
	}
	printf(ROM_TYPE"LOG.TXT is created.\r"
				   "This file stores the time and date at which S2, S3 buttons\r"
				   "are pressed. press S4 button to exit and display the log!!\r");
	while(1)
	{
		if(PORTEbits.RE0 || PORTEbits.RE1)		// if a button is pressed
		{
			var8 = PORTE & 0x3; // store the button
			// wait till the button releases
			while(PORTE & 0x3);
			if(var8 == 1)
			{
				printf(ROM_TYPE"Button S2 is pressed\r");
				strcpypgm2ram((char*)buffer, ROM_TYPE"S2 is pressed on ");
			}
			if(var8 == 2)
			{
				printf(ROM_TYPE"Button S3 is pressed\r");
				strcpypgm2ram((char*)buffer, ROM_TYPE"S3 is pressed on ");
			}
			// read the time at the end of buffer
			var32 = strlen((char*)buffer);
			if(c = GHI_GetFormattedTime(buffer + var32))
			{
				printf(ROM_TYPE"Could not process time\r");
				return c;
			}
			// append "\r\n"
			size = strlen((char*)buffer);
			buffer[size] = '\r';
			buffer[size + 1] = '\n';
			size += 2;
			// write it
			if(c = GHI_WriteFile(FILE_HANDLE_0, buffer, size, &var32))
			{
				printf(ROM_TYPE"Could not write to the log\r");
				return c;
			}
			// since there was no errors, we can assume that var23 == size == "Actual written data"
		}
		if(PORTEbits.RE2)
		{
			if(c = GHI_CloseFile(FILE_HANDLE_0))
			{
				printf(ROM_TYPE"Cannot close %s\r", file_name);
				return c;
			}
			printf(ROM_TYPE"Log created succussfully\r"
			 			   "Displaying LOG.TXT\r");
			// read log
			if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_READ_MODE))
			{
				printf(ROM_TYPE"Cannnot open %s\r", file_name);
				return c;
			}
			while(1)
			{
				if(c = GHI_ReadFile(FILE_HANDLE_0, buffer, 1, '*', &var32))
				{
					printf(ROM_TYPE"Error reading from log\r");
					return c;
				}
				if(var32 == 0)	// Can't read any more, Done
				{
					// close file
					if(c = GHI_CloseFile(FILE_HANDLE_0))
					{
						printf(ROM_TYPE"Cannot close the log file\r");
						return c;
					}
					// done
					GHI_PutC('\r');
					TestDone();
					return ERROR_NO_ERROR;
				}
				// print it
				while(BusyUSART());
				putcUSART(*buffer);
			}
		}
	}
				
}
//-------------------------------------------------
// this gets the already initialized device as an argument
int8 DifferentMediasTest(int8 device1)
{
	int8 c, device2, file_name[16], buffer[] = "123456";
	int32 size;
	
	printf(ROM_TYPE"Connect a new storage media and then select which\r"
				   "port it is connected to\r"
				   "1. USB port0\r"
				   "2. USB port1\r"
				   "3. SD card\r"
				   "0. Cancel\r");
	
	while(1)
	{
		while(!DataRdyUSART());
		device2 = getcUSART();
	
		switch(device2)
		{
			case '1':
				device2 = USB_DEVICE_PORT_0;
			
			break;
			case '2':
				device2 = USB_DEVICE_PORT_1;
				
			break;
			case '3':
				device2 = SD_DEVICE;
				
			break;
			case '0':
				return ERROR_NO_ERROR;
				
			break;
			default:
				printf(ROM_TYPE"Wrong entry. Try again\r");
				continue;
			
		}
		
		break;
	}
	if(device2 == device1)		// same device?
	{
		printf(ROM_TYPE"Media already initialized\r");
		return ERROR_UNEXPECTED_VALUE;
	}
	
	c = GHI_MountFATFileSystem(device2);
	if(c)
	{
		printf(ROM_TYPE"Error initializing media\r");
		return c;
	}
	
	// write a file
	strcpypgm2ram((char*)file_name, ROM_TYPE"FIRSTFIL.TST");
	printf(ROM_TYPE"Creating %s on the first media\r", file_name);
	c = GHI_SwitchDevice(device1);
	if(c)
	{
		printf(ROM_TYPE"Cannot switch to the first media\r");
		return c;
	}
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_WRITE_MODE))
	{
		printf(ROM_TYPE"Cannot create file\r");
		return c;
	}
	printf(ROM_TYPE"Writing 6 bytes to %s\r", file_name);
	if(c = GHI_WriteFile(FILE_HANDLE_0, buffer, 6, &size))
	{
		printf(ROM_TYPE"Cannot write to file\r");
		return c;
	}
	if(size != 6)
	{
		printf(ROM_TYPE"Error writing to file\r");
		return c;
	}
	if(c = GHI_CloseFile(FILE_HANDLE_0))
	{
		printf(ROM_TYPE"Cannot close file\r");
		return c;
	}
	// read and write to different medias
	printf(ROM_TYPE"Reading %s from the first media and writing it to the second media\r", file_name);
	for(c = 0; c < 6;c++)		// reset buffer
		buffer[c] = 0;
	// open file for read on the first media
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_READ_MODE))
	{
		printf(ROM_TYPE"Cannot open file\r");
		return c;
	}
	// switch to the other media
	if(c = GHI_SwitchDevice(device2))
	{
		printf(ROM_TYPE"Cannot switch to the second media\r");
		return c;
	}
	// open file for write on the second media
	if(c = GHI_OpenFile(FILE_HANDLE_1, file_name, FILE_WRITE_MODE))
	{
		printf(ROM_TYPE"Cannot create the file on the second media\r");
		return c;
	}
	// read and write
	// read from first media
	if(c = GHI_SendReadWriteFileCommand(FILE_HANDLE_0, FILE_HANDLE_1, 6))
	{
		printf(ROM_TYPE"Error in Read/Write command\r");
		return c;
	}
	if(c = GHI_GetReadAndWriteResult(&size))
	{
		printf(ROM_TYPE"Error in Read/Write file\r");
		return c;
	}
	// since there was no errors, we can assume that size == "Actual written data"
	if(c = GHI_CloseFile(FILE_HANDLE_0))
	{
		printf(ROM_TYPE"Cannot close file\r");
		return c;
	}
	if(c = GHI_CloseFile(FILE_HANDLE_1))
	{
		printf(ROM_TYPE"Cannot close file\r");
		return c;
	}
	// read the file on the second media
	printf(ROM_TYPE"Done.\r"
				   "Reading 8 bytes from the second media using\r" 
				   "\'*\' as a filler\r");
		
	if(c = GHI_OpenFile(FILE_HANDLE_1, file_name, FILE_READ_MODE))
	{
		printf(ROM_TYPE"Cannot open file\r");
		return c;
	}
	if(c = GHI_SendReadCommand(FILE_HANDLE_1, 8, '*'))
	{
		printf(ROM_TYPE"Cannot read from file\r");
		return c;
	}
	for(c = 0; c < 8; c++)
	{
		while(BusyUSART());
		putcUSART(GHI_GetC());
	}
	while(BusyUSART());
	putcUSART('\r');
	if(c = GHI_GetReadAndWriteResult(&size))
	{
		printf(ROM_TYPE"Error reading from file\r");
		return c;
	}
	// since there was no errors, we can assume that size == "Actual written data"
	if(c = GHI_CloseFile(FILE_HANDLE_1))
	{
		printf(ROM_TYPE"Cannot close file\r");
		return c;
	}
	
	TestDone();
	
	return ERROR_NO_ERROR;
}
//-----------------------------------------------
int8 OneMBTest(int8 device)
{
	int8 c, file_name[] = "ONEMB.EXT", var8, index;
	int32 size, var32;
	
	printf(ROM_TYPE"Reading/writing one MB of data takes an average of ?? seconds.\r"
				   "However, on low-end media this might take considerabley longer time!\r");

	// writing 1 MB to a file
	printf(ROM_TYPE"* 1. Writing 1MB of data to a file *\r"
				   "Opening file ONEMB.EXT\r");
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_WRITE_MODE))
	{
		printf(ROM_TYPE"Could not open file\r");
		return c;
	}
	printf(ROM_TYPE"Writing...\rPlease wait... \r");
				
				
	size = 0;	// size of data written
	var8 = 4;// initialize the percentage counter
	for(index = 0; index < 16; index++)		// repeat 16 times
	{
		// working indicator
		printf(ROM_TYPE"%d%% ",var8+=6);
					
		// write first ONE_MB_OVER_16
		if(c = GHI_SendWriteCommand(FILE_HANDLE_0, ONE_MB_OVER_16))
		{
			printf(ROM_TYPE"Cannot write to the file\r");
			return c;
		}
		for(var32 = 0; var32 < ONE_MB_OVER_16; var32++)
		{
			GHI_PutC('X');
		}
		if(c = GHI_GetReadAndWriteResult(&var32))
		{
			printf(ROM_TYPE"Error Writing to file\r");
			return c;
		}
		// add how many bytes we successfully written
		size += var32;
					
	}
	printf(ROM_TYPE"\rDone...\r");
	GHI_PrintDWORD(size);
	printf(ROM_TYPE" bytes of data were written successfully\r");
	GHI_CloseFile(FILE_HANDLE_0);
				
	// Reading 1MB
	printf(ROM_TYPE"\r\r* 2. Reading 1MB of data from a file *\r"
				   "Opening file ONEMB.EXT\r");
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_READ_MODE))
	{
		printf(ROM_TYPE"Could not open file\r");
		return c;
	}
	printf(ROM_TYPE"Reading...\r"
				   "Please wait... \r");
				
	size = 0;	// size of data read
	var8 = 4;// initialize the percentage counter
	for(index = 0; index < 16; index++)		// repeat 16 times
	{
		// working indicator
		printf(ROM_TYPE"%d%% ",var8+=6);
					
		// read first ONE_MB_OVER_16
		if(c = GHI_SendReadCommand(FILE_HANDLE_0, ONE_MB_OVER_16, '*'))
		{
			printf(ROM_TYPE"Cannot read from the file\r");
			return c;
		}
		for(var32 = 0; var32 < ONE_MB_OVER_16; var32++)
		{
			GHI_GetC();
		}
		if(c = GHI_GetReadAndWriteResult(&var32))
		{
			printf(ROM_TYPE"Error reading from file\r");
			return c;
		}
		// add how many bytes we successfully read
		size += var32;
	}
	printf(ROM_TYPE"\rDone...\r");
	GHI_PrintDWORD(size);			   
	printf(ROM_TYPE" bytes were read succussfully.\r");
	GHI_CloseFile(FILE_HANDLE_0);
	
	return ERROR_NO_ERROR;			
}
//-----------------------------------------------------------
int8 MassStorageTest(void)
{
	int8 device, c, last_lun;
	int32 var32;
	
	// Initialize the media
	printf(ROM_TYPE"Select which media to test:\r"
				   "1. SD card\r"
				   "2. USB port0\r"
				   "3. USB port1\r"
				   "0. Cancel\r");
	while(1)
	{
		while(!DataRdyUSART());
		device = getcUSART();
	
		switch(device)
		{
			case '0':
				return ERROR_NO_ERROR;
			
			break;
			case '1':
				device = SD_DEVICE;
			
			break;
			case '2':
				device = USB_DEVICE_PORT_0;
				
			break;
			case '3':
				device = USB_DEVICE_PORT_1;
			
			break;
			default:
				printf(ROM_TYPE"Wrong entry. Try again\r");
				continue;
		}	
		
		break;
	}
	c = GHI_MountFATFileSystem(device);
	if(c)		// error
	{
		printf(ROM_TYPE"Cannot initialize media\r");
		return c;
	}
				
	// start testing //
	while(1)
	{
		printf(ROM_TYPE"\rPlease select one of the options below\r\r"
					   "1. File system test\r"
					   "2. Data logger\r"
					   "3. Read/Write from different medias\r"
					   "4. Read/Write performance (NOT READY YET!)\r"
				   	   "0. Exit\r"
	               	   "\r>>\r");
	  	while(!DataRdyUSART());
	  	c = getcUSART();
	  	if(c == '0')		// Exit
	  		return ERROR_NO_ERROR;
	  	switch(getcUSART())
		{
			case '1':
				if(c = FileSystemTest())		// error
					return c;
			break;
			
			case '2':
				if(c = DataLoggerTest())
					return c;
					
			break;
			
			case '3':
				if(c = DifferentMediasTest(device))
					return c;
				
			break;
			
			case '4':
				// not ready yet
				return ERROR_NO_ERROR;
				//
				if(c = OneMBTest(device))
					return c;
					
			break;
			default:
				printf(ROM_TYPE"Wrong Entry. Try again\r");
				break;
		}
	  	
	 }

	return ERROR_NO_ERROR;	
}
//---------------------------------------------------------


#endif	// _INCLUDE_FAT_SYSTEM_SUPPORT_
