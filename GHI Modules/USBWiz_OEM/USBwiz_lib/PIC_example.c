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
 * 3. Feel free to modify the codes but we ask you to provide us with 
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

#include "Types.h"
#include "USBwiz_lib.h"
#include "GHI_inter.h"
#include "GHI_HD44780_LCD.h"
#include "Dev_Board_Test.h"
#include "HID_test.h"
#include "MS_Test.h"
#include "Print_Test.h"
#include "Serial_Test.h"
#include "LCDTest.h"

#include <usart.h>
#include <stdio.h>
#include <SPI.h>

#define BAUD_RATE 9600
#define CLOCK_FREQ    32000000
#define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16))
//---------------------------------------------
int8 USBwiz_Initialize(void)
{
	int8 minor, major,c;
	
	// in our example, MODE 0,1 are not connected and therefore
	// USBwiz is in SPI mode

	// reset USBwiz
	RESET_PIN = 0;
	GHI_Sleep(50);
	RESET_PIN = 1;
	GHI_Sleep(50);	
	
	// close interface just in case!
	GHI_CloseInterface();
	
	// Lets get the interface going
	if(GHI_OpenInterface())
	{
		// this will never happen!
		printf(ROM_TYPE"Failed to open our interface\r");
		SystemFail();
	}
	
	printf(ROM_TYPE"Trying to run USBwiz\r");
	
	while(1)
	{
		c=GHI_GetC();
		if(c == '!')
			break;
		// show what is coming
		while(BusyUSART());
		putcUSART(c);
	}
	
	// we need to get "00/r"
	c = 0;
	c =  (GHI_GetC() - '0') << 4;
	c |= (GHI_GetC() - '0');
	if(GHI_GetC() != '\r')
	{
		printf(ROM_TYPE"Error starting firmware\r");
		SystemFail();
	}
	if(c)	// is there error at start up?
	{
		printf(ROM_TYPE"Error # %X\r"
					   "Cannot start firmware\r", c);
		SystemFail();
	}
	
	printf(ROM_TYPE"\rFirmware is running\r\r");
	
	return 0;
}
//---------------------------------------------
void InitProcessor(void)
{
	ADCON1=0x0f;// all analog pins to digital
    CMCON=0x07;// Shut off all comparators

}
//---------------------------------------------
/////////////////////////////////////////////////////
void main(void)
{
	int8 c , bcd, var8;
	int16 var16;

	

    InitProcessor();
    
	// we will get UART going because we will use it top print debug mesages
	OpenUSART(USART_TX_INT_OFF&USART_TX_INT_OFF&
	          USART_ASYNCH_MODE&USART_EIGHT_BIT&
	          USART_CONT_RX&USART_BRGH_HIGH,SPBRG_VAL);
	
	printf(ROM_TYPE"\fready......\r");
	
	// set necessary pins
	RESET_TRIS=0;// make reset pin output
	LED0_TRIS=0;// make led pin output
	LED1_TRIS=0;// make led pin output
	LED0_PIN=1;// turn LED on
	LED1_PIN=1;// turn LED on
	
	// initialize USBwiz
	c = USBwiz_Initialize();
	if(c)
	{
		printf(ROM_TYPE"USBwiz couldn't run. --> %c\r",c);
		SystemFail();
	}
	
	
	// get library version
	
	var16 = GHI_GetLibraryVersion();

	///////////////////////////////////////////	
	///////// initilization is done ///////////
	///////////////////////////////////////////
	
	
	////////////////////////////
	/// Start user interface ///
	////////////////////////////
	
	while(1)
	{
		// Display banner on LCD
		
		LCD_Init();
		LCD_Clear();
		LCD_CursorHome();
		LCD_Putrs(ROM_TYPE"GHI Electronics");
		LCD_SetCursor(1,0);
		LCD_Putrs(ROM_TYPE"USBwiz Dev. Sys.");
		
		if(GHI_GetVersion(&c,&bcd)!=0)
		{	
			printf(ROM_TYPE"Failed to get version number\r");
			SystemFail();
		}
		
		printf(ROM_TYPE"\f   GHI Electronics, LLC\r");
		printf(ROM_TYPE  " USBwiz Development System\r");
		printf(ROM_TYPE  "  Library Version %X.%02X\r", (var16 >> 8), (var16 & 0XFF));
		printf(ROM_TYPE  "  USBwiz Version  %d.%02X\r",c,bcd);
		printf(ROM_TYPE  "---------------------------\r");
		printf(ROM_TYPE"Please select one of the options below\r\r\r");
		printf(ROM_TYPE"1 - Start RS232 <-> USBwiz SPI gateway\r");
		printf(ROM_TYPE"2 - Update firmware\r");
		
		#ifdef _INCLUDE_FAT_SYSTEM_SUPPORT_
			printf(ROM_TYPE"3 - Run FAT system/Mass Storage test\r");
		#endif  /////////////////////////////////
		
		#ifdef	_INCLUDE_HID_SUPPORT_
			printf(ROM_TYPE"4 - Run HID test (mouse, keyboard, ...)\r");
		#endif	// _INCLUDE_HID_SUPPORT_
		
		
		#ifdef _INCLUDE_PRINTER_SUPPORT_
			printf(ROM_TYPE"5 - Test a printer. Not all printers work!\r");
		#endif	//////////////////////////////////////
		
		#ifdef _INCLUDE_SERIAL_DEVICES_SUPPORT_
			printf(ROM_TYPE"6 - Serial device test (CDC, FTDI, ...)\r");
		#endif	///////////////////////////////
		
		printf(ROM_TYPE"7 - LCD Test\r");
		printf(ROM_TYPE">>\r");
	
		while(!DataRdyUSART());
		c = getcUSART();
		switch(c)
		{
			case '1':
				StartGateway();
				
				TestDone();				

			break;
			
			case '2':
				ReportError(UpdateFirmwareTest());
				
				
				break;
				
		#ifdef	_INCLUDE_FAT_SYSTEM_SUPPORT_
			case '3':
				printf(ROM_TYPE"Starting test...\r");
				ReportError(MassStorageTest());
				TestDone();
								
			break;
		#endif	///////////////////////////////

		#ifdef	_INCLUDE_HID_SUPPORT_
			case '4':
				printf(ROM_TYPE"Starting test...\r");
				ReportError(HIDTest());
				TestDone();
				
			break;
		#endif	//////////////////////////////////
			
		#ifdef	_INCLUDE_PRINTER_SUPPORT_			
			case '5':
				printf(ROM_TYPE"Starting test...\r");
				ReportError(PrinterTest());
				TestDone();
				
			break;
		#endif	// _INCLUDE_PRINTER_SUPPORT_
			
		#ifdef	_INCLUDE_SERIAL_DEVICES_SUPPORT_
			case '6':
				printf(ROM_TYPE"Starting test...\r");
				ReportError(SerialTest());
				TestDone();
				
			break;
		#endif //////////////////////////
			
			case '7':
				printf(ROM_TYPE"Starting test...\r");
				ReportError(LCDTest());
				TestDone();
				
			break;
			default:
				printf(ROM_TYPE"Unknown entry!\r");

		}
			
		//**/
		printf(ROM_TYPE"\r2 seconds delay ..");
		GHI_Sleep(500);
		printf(ROM_TYPE".");
		GHI_Sleep(500);
		printf(ROM_TYPE".");
		GHI_Sleep(500);
		printf(ROM_TYPE".");
		GHI_Sleep(500);//*/
	
	}
}
