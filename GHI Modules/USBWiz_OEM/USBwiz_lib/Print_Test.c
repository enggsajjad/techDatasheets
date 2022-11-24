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

#include "Print_Test.h"

#ifdef	_INCLUDE_PRINTER_SUPPORT_

int8 buffer[256];

int8 PrinterTest(void)
{
	int8 c, device, file_media, file_name[] = "TEST.PRT", last_lun, var8;
	int32 actual_size;
	printf(ROM_TYPE"Which port is the printer connected to[0, 1]\r");
	while(!DataRdyUSART());
	device = getcUSART();
	// initialize printer
	if(c = GHI_RegisterPrinter(device))
	{
		printf(ROM_TYPE"Cannot connect to printer\r");
		return c;
	}
	if(c = GHI_ResetPrinter(device))
	{
		printf(ROM_TYPE"Error Communicating with printer\r");
		return c;
	}
	// get the file
	printf(ROM_TYPE"Select which media has TEST.PRT\r"
				   "* For information on how to make\r"
				   "  TEST.PRT see the print example\r"
				   "  in the library manual\r\r"
				   "1. USB port0\r"
				   "2. USB port1\r"
				   "3. SD card\r"
				   "0. Cancel\r");
	while(!DataRdyUSART());
	file_media = getcUSART();
	
	switch(file_media)
	{
		case '0':
			return ERROR_NO_ERROR;
		
		break;
		
		case '1':
			file_media = USB_DEVICE_PORT_0;
			
		break;
		
		case '2':
			file_media = USB_DEVICE_PORT_1;
			
		break;
		
		case '3':
			file_media = SD_DEVICE;
			
		break;
		default:
			printf(ROM_TYPE"Error entry...\r");
			return ERROR_UNEXPECTED_VALUE;
	}
	// open the file
	if(c = GHI_MountFATFileSystem(file_media))
	{
		printf(ROM_TYPE"Cannot mount FAT on media\r");
		return c;
	}
	
	if(c = GHI_OpenFile(FILE_HANDLE_0, file_name, FILE_READ_MODE))
	{
		printf(ROM_TYPE"Cannot open file\r");
		return c;
	}
		
	printf(ROM_TYPE"Sending file...\r");
	// send 255 at a time "max size to send"
	while(1)	
	{
		// read the data
		if(c = GHI_ReadFile(FILE_HANDLE_0, buffer, 0xFF, '*', &actual_size))
		{
			printf(ROM_TYPE"Cannot read file\r");
			return c;
		}
		
		// since there is no errors the actual_size should be 0xFF unless we reached end of file
		if(actual_size == 0)	// we sent all the file
			break;
			
		// Prepare printer to recieve data
		if(c = GHI_PrinterPrint(device, actual_size))
		{
			printf(ROM_TYPE"Error communicating with printer\r");
			return c;
		}
		// send it
		for(var8 = 0; var8 < actual_size; var8++)
		{
			GHI_PutC(buffer[var8]);
		}
		// done, get print results
		if(c = GHI_GetResult())
		{
			printf(ROM_TYPE"Error communicating with printer\r");
			return c;
		}
		// working indicator
		while(BusyUSART());
		putcUSART('*');	
	}
	// done
	while(BusyUSART());
	putcUSART('\r');
	printf(ROM_TYPE"Printing complete...\r");
	
	return ERROR_NO_ERROR;
}


#endif	// _INCLUDE_PRINTER_SUPPORT_
