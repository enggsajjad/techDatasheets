//---------------------------------------
// Example read and write 2K On-chip EEPROM
// on AT89S8252
// KEIL C51 v7.5
// www.GetMicro.com
//---------------------------------------
#include<reg8252.h>
#include<stdio.h>

void InitSerial(void);
void WriteEEP(unsigned char xdata * ADDR,unsigned char EEP_Data);
unsigned char ReadEEP(unsigned char xdata * ADDR);

//---------------------------------------
// Main program
//---------------------------------------
void main(void)
{
    unsigned char MyEeprom;
	InitSerial();  		// Initialize serial port
	
	WriteEEP(0x123,0x55);
	MyEeprom = ReadEEP(0x123);
	while(1);
}

//---------------------------------------
// Initialize serial port
//---------------------------------------
void InitSerial(void)
{
  	SCON = 0x52;    // setup serial port control 
  	TMOD = 0x20;    // hardware (9600 BAUD @11.05592MHZ) 
  	TH1  = 0xFD;    // TH1
	TR1	 = 1;  		// Timer 1 on
}

//---------------------------------------
// Write EEPROM funcrion 
// Input : ADDR = address 000h-7FFh 
//         EEP_Data = Data
//---------------------------------------
void WriteEEP(unsigned char xdata * ADDR,unsigned char EEP_Data)
{
	EA=0;						// disable Interrupts during write
	WMCON|=EEMEN_ | EEMWE_;
	*ADDR=EEP_Data;
    WMCON &= ~EEMEN_;
	WMCON &= ~EEMWE_;	
	while((WMCON&WDTRST_)==0);					// wait until write is complete	
	EA=1;					
}

//---------------------------------------
// Read EEPROM function
// Input : ADDR = address 
// Output: unsigned char (8bit)
//---------------------------------------
unsigned char ReadEEP(unsigned char xdata *ADDR)
{
    unsigned char EEP_DATA;
	EA=0;						// disable Interrupts during write
	WMCON|=EEMEN_;
	EEP_DATA = *ADDR;
    WMCON &= ~EEMEN_;
	EA=1;    
	return EEP_DATA;		
}


