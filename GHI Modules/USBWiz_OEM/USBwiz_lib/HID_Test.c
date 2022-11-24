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
 
 #include "HID_Test.h"
 
 #ifdef	_INCLUDE_HID_SUPPORT_

// to read a byte from USBwiz
int8 ReadByte(void)
{
	int8 temp[3] = {0};
	temp[0] = GHI_GetC();
	temp[1] = GHI_GetC();
	
	return HEXstrToDouble(temp);
}
//-------------------------------------------------
int8 MouseTest(int8 device)
{
	int8 c, buttons, temp[3], actual_report_size;
	signed char dx, dy, ds;
		
	printf(ROM_TYPE"\rThis will keep track of the pressed buttons and movement.\r"
				   "Press the S2 button on your board to exit.\r"
				   "LB, RB, MB are left, right and middle buttons respectivley.\r"
				   "1: button is pressed, 0 button is up\r"
				   "dX, dY, dS shows the change in X, Y and scroll position\r\r");  
	while(1)
	{
		if(S2_STATE)	// if a button is pressed
			return ERROR_NO_ERROR;
		c = GHI_PrepareHIDReport(device, 4, &actual_report_size);
		// now actual_report_size should be 4
		
		if(c == HID_HAS_NO_DATA)		// there is no data
			continue;
			
		if(c)		// error
		{
			printf(ROM_TYPE"Cannot read HID\r");
			return c;
		}
		
		// right report size?
		if(actual_report_size != 4)	// this should not happen
		{
			printf(ROM_TYPE"Error report size, current size is: %d", actual_report_size);
			// ignore any data
			while(GHI_DataIsReady())
			{
				GHI_GetC();
			}
			return ERROR_UNEXPECTED_VALUE;
							
		}
		// read data
		// first byte is buttons
		buttons = ReadByte();
		dx = (signed char)ReadByte();
		dy = (signed char)ReadByte();
		ds = (signed char)ReadByte();
		printf(ROM_TYPE"LB RB MB   dX   dY   dS\r");
		printf(ROM_TYPE" %d  %d  %d %+4d %+4d %+4d\r",  buttons & 1  	   // left
												     , (buttons >> 1) & 1  // right 
												     , (buttons >> 2) & 1  // middle
												     ,  dx   // dX
												     ,  dy   // dY
												     ,  ds   // dS
			  );
		if(c = GHI_GetResult())
		{
			printf(ROM_TYPE"Error reading report\r");
			return c;
		}
		
	}
}
//--------------------------------------------------------
int8 HIDReport(int8 device)
{
	int8 index, c, actual_report_size;
	printf(ROM_TYPE"This will show the report sent by the HID byte by byte\r"
				   "Press the S2 button on your board to exit.\r");
	
	while(1)
	{
		if(S2_STATE)	// if a button is pressed
			return ERROR_NO_ERROR;
		// request 0 report size to read everything
		c = GHI_PrepareHIDReport(device, 0, &actual_report_size);
		if(c == HID_HAS_NO_DATA)		// there is no data
			continue;
		if(c)		// error
		{
			printf(ROM_TYPE"Cannot read HID\r");
			return c;
		}
		// read data
		printf(ROM_TYPE"Report size: %d,  ", actual_report_size);
		for(index = 0; index < actual_report_size; index++)
		{
			// print first nibble
			while(BusyUSART());	
			putcUSART(GHI_GetC());	
			
			// print second nibble
			while(BusyUSART());	
			putcUSART(GHI_GetC());
			
			// print space
			while(BusyUSART());
			putcUSART(' ');	
		}
		while(BusyUSART());
		putcUSART('\r');
		
		// get error results
		if(c = GHI_GetResult())
			return c;
	}
}
//-------------------------------------------------
int8 HIDTest(void)
{
	int8 c, device;
	
	printf(ROM_TYPE"Which port is the HID connected to[0, 1]?\r");
	while(!DataRdyUSART());
	device = getcUSART();
	if(c = GHI_RegisterHID(device))
	{
		printf(ROM_TYPE"Cannot initialize the HID\r");
		return c;
	}
	
	printf(ROM_TYPE"Did you want to do a mouse test[Y]\r"
				   "* This does a specific mouse test.\r"
				   "* Results maybe hard to track on low-end mouses. If\r"
				   "* this is the case, don't choose a mouse test!\r\r");
	while(!DataRdyUSART());
	c = getcUSART();
	if(c == 'y' || c == 'Y')
		return MouseTest(device);
		
	// do a general HIDReport();
	return HIDReport(device);
}


#endif	// _INCLUDE_HID_SUPPORT_
