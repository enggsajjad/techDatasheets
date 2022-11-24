// Dev_Board_Test.c

#include "Dev_Board_Test.h"

//---------------------------------------------
void BlinkEverySecond(void)
{
	// set LED pin to output
	LED0_TRIS=0;
	while(1)
	{
		// toggle the LED
		LED0_PIN=!LED0_PIN;
		// wait for 500 ms
		GHI_Sleep(500);
	}
}

//-----------------------------------------------
void SystemFail(void)
{
	printf(ROM_TYPE"System failed.....");

	while(1)
	{
		// toggle the LED
		LED0_PIN=!LED0_PIN;
		// wait for 100 ms (50 high and 50 low!)
		GHI_Sleep(50);
	}
}
//------------------------------------------------------
void TestDone(void)
{
	printf(ROM_TYPE"\rDone.\rPress any key to continue...\r");
	while(!DataRdyUSART());
	getcUSART();
}
//-----------------------------------------------------
void StartGateway(void)
{
	printf(ROM_TYPE"Gateway is enabled\r press S2 to exit\r");
	while(1)
	{	
		if(DataRdyUSART())
		{
			GHI_PutC(getcUSART());
		}
		if(GHI_DataIsReady())
		{
			while(BusyUSART());
			putcUSART(GHI_GetC());
		}
		if(S2_STATE)
			return;
	}	
}
//--------------------------------------------------------
void ReportError(int8 error)
{
	if(error)
	{
		printf(ROM_TYPE"\rError ---> %02X.\r", error);
	}
}
//---------------------------------------------------------
// printf sometimes doesn't work for 32 variables, so we use our own function
void GHI_PrintNibble(int8 n)
{
	n&=0x0f;
	if(n<10)
	{
		while(BusyUSART());
		putcUSART(n+'0');
	}
	else
	{
		while(BusyUSART());
		putcUSART((n-10)+'A');
	}
}
//---------------------------------------------
void GHI_PrintDWORD(int32 dw)
{
	// send the high nibble first
	GHI_PrintNibble(dw>>28);
	GHI_PrintNibble(dw>>24);
	GHI_PrintNibble(dw>>20);
	GHI_PrintNibble(dw>>16);
	GHI_PrintNibble(dw>>12);
	GHI_PrintNibble(dw>>8);
	GHI_PrintNibble(dw>>4);
	GHI_PrintNibble(dw);
}
//---------------------------------------------
void ProcessCharFromUSART(int8 *character)
{
	// get the char
	while(!DataRdyUSART());
	*character = getcUSART();
	// echo back
	while(BusyUSART());
	putcUSART(*character);
}
//----------------------------------------------
// this function echoes the input
int8 SetTimeStructure(int32 *time)
{	
	int8 buffer[5] = {0}, temp;
	int32 year, month, day, hours, minutes, seconds;
	
	
	// get month
	ProcessCharFromUSART(buffer);
	ProcessCharFromUSART(buffer + 1);
	ProcessCharFromUSART(&temp);	// we don't need this character
	if(temp != '/')
		return ERROR_UNEXPECTED_VALUE;
	month = atoi((char*)buffer);
	// get day
	ProcessCharFromUSART(buffer);
	ProcessCharFromUSART(buffer + 1);
	ProcessCharFromUSART(&temp);	// we don't need this character
	if(temp != '/')
		return ERROR_UNEXPECTED_VALUE;
	day = atoi((char*)buffer);
	// get year
	ProcessCharFromUSART(buffer);
	ProcessCharFromUSART(buffer + 1);
	ProcessCharFromUSART(buffer + 2);
	ProcessCharFromUSART(buffer + 3);
	ProcessCharFromUSART(&temp);	// we don't need this character
	if(temp != ' ')
		return ERROR_UNEXPECTED_VALUE;
	year = atoi((char*)buffer);
	//get hours
	buffer[2] = 0;
	ProcessCharFromUSART(buffer);
	ProcessCharFromUSART(buffer + 1);
	ProcessCharFromUSART(&temp);	// we don't need this character
	if(temp != ':')
		return ERROR_UNEXPECTED_VALUE;
	hours = atoi((char*)buffer);
	// get minutes
	ProcessCharFromUSART(buffer);
	ProcessCharFromUSART(buffer + 1);
	ProcessCharFromUSART(&temp);	// we don't need this character
	if(temp != ':')
		return ERROR_UNEXPECTED_VALUE;
	minutes = atoi((char*)buffer);
	// get seconds
	ProcessCharFromUSART(buffer);
	ProcessCharFromUSART(buffer + 1);
	seconds = atoi((char*)buffer);
	
	// Set the time
	*time = GHI_GetFATTimeStructure(year, month, day, hours, minutes, seconds);
	
	// done. put the '\r'
	while(BusyUSART());
	putcUSART('\r');
		
	return ERROR_NO_ERROR;
}
//--------------------------------------------------------------------
int8 UpdateFirmwareTest(void)
{
	int8 device, var8, port_number;
		
	printf(ROM_TYPE"Which media do you want to upload the firmware from:\r"
				   "1. SD Card\r"
				   "2. USB port0\r"
				   "3. USB port1\r"
				   "0. Cancel\r");
	while(!DataRdyUSART());
	var8 = getcUSART();
	
	switch(var8)
	{
		case '0':	// cancel
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
			printf(ROM_TYPE"Wrong entry...\r");
			return ERROR_UNEXPECTED_VALUE;
	}
	
	
	
	// update
	printf(ROM_TYPE"Please wait...\r");
	var8 = GHI_UpdateFirmware(device);
	
	if(var8 == ERROR_NO_ERROR) 
		printf(ROM_TYPE"Firmware updated successfully.\r");
	else
		printf(ROM_TYPE"Error Updating firmware.\r");
	
	return var8;
}
//------------------------------------------------------------
