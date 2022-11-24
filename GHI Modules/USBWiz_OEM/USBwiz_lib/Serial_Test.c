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
 
 #include "Serial_Test.h"
 
 #ifdef	 _INCLUDE_SERIAL_DEVICES_SUPPORT_


int8 SerialTest(void)
{
	int8 device, c, size;
	int32 baudrate;
		
	// get the port
	printf(ROM_TYPE"Which port is the device connected to[0, 1]?\r");
	while(!DataRdyUSART());
	device = getcUSART();
	
	// get baudrate
	printf(ROM_TYPE"What is the baudrate?\r"
				   "1. 2400\r"
				   "2. 9600\r"
				   "3. 115200\r"
				   "0. Cancel\r");
	while(!DataRdyUSART());
	switch( getcUSART() )
	{
		case '0':
			return ERROR_NO_ERROR;
			
		case '1':
			baudrate = 2400;
		break;
		case '2':
			baudrate = 9600;
		break;
		case '3':
			baudrate = 115200;
		break;
		default:
			printf(ROM_TYPE"Wrong entry.\r");
			return ERROR_UNEXPECTED_VALUE;	
	}
	
	// what is the device?
	printf(ROM_TYPE"What is the device\r"
				   "1. CDC\r"
				   "2. FTDI\r"
				   "3. Silabs\r"
				   "4. Prolific V1\r"
				   "5. Prolific V2\r"
				   "0. Cancel\r");
	while(!DataRdyUSART());
	// register it
	switch( getcUSART() )
	{
		case '0':
			return ERROR_NO_ERROR;
			
		case '1':
			c = GHI_RegisterSerialDevice(device, SERIAL_CDC_DEVICE, baudrate);
			
		break;
		case '2':
			c = GHI_RegisterSerialDevice(device, SERIAL_FTDI_DEVICE, baudrate);
			
		break;
		case '3':
			c = GHI_RegisterSerialDevice(device, SERIAL_SILABS_DEVICE, baudrate);
			
		break;
		case '4':
			c = GHI_RegisterSerialDevice(device, SERIAL_PROLIFIC1_DEVICE, baudrate);
			
		break;
		case '5':
			c = GHI_RegisterSerialDevice(device, SERIAL_PROLIFIC2_DEVICE, baudrate);
			
		break;
		default:
			printf(ROM_TYPE"Wrong entry.\r");
			return ERROR_UNEXPECTED_VALUE;
	}
	if(c)
	{
		printf(ROM_TYPE"Cannot register the device\r");
		return c;
	}
	// start terminal
	
	printf(ROM_TYPE"Communication established.\r"
				   "Starting Terminal...\r"
				   "* When you type data, it will be sent to the device and\r"
				   "* any received data will be displayed.\r"
				   "* Press S2 button to exit\r");
	while(1)
	{
		if(S2_STATE)	// exit
		{
			return ERROR_NO_ERROR;
		}
		// is there data to send
		while(DataRdyUSART())
		{
			if(c = GHI_SerialWrite(device, 1))	// send one byte
			{
				printf(ROM_TYPE"Cannot send data\r");
				return c;
			}
			c = getcUSART();
			// send it
			GHI_PutC(c);
			// any errors?
			if(c = GHI_GetResult())
			{
				printf(ROM_TYPE"Error communicating with device\r");
				return c;
			}
		}
		// is there data to receive?
		if(c = GHI_SerialRead(device, &size))
		{
			printf(ROM_TYPE"Cannot read data\r");
			return c;
		}
		while(size)		
		{
			// print any data
			while(BusyUSART());
			putcUSART(GHI_GetC());
			size--;
		}
		if(c = GHI_GetResult())
		{
			printf(ROM_TYPE"Error reading data\r");
			return c;
		}
	}
}


#endif	//  _INCLUDE_SERIAL_DEVICES_SUPPORT_
