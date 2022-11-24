//-----------------------------------------------------------------------------------
//              GHI Electronics, LLC
//               Copyright (c) 2008
//               All rights reserved
//-----------------------------------------------------------------------------------
/*
 * You can use this file if you agree to the following:
 *
 * 1. This header can't be changed under any condition.
 *    
 * 2. This is a free software and therefore is provided with NO warranty.
 * 
 * 3. Feel free to modify the code but we ask you to provide us with
 *	  any bugs reports so we can keep the code up to date.
 *
 * 4. This code may ONLY be used with GHI Electronics, LLC products.
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


///////////	THIS IS JUST AN EXAMPLE OF USING I2C INTERFACE //////////////////////////////
///////////	Must change to be ported appropriately to your processor ////////////////////
/////////// Needs modification to support I2C communications with firmware 3.05 and up //////////////////////

// keep compiler from generating errors
int I2C_INTERFACE_NOT_USED;


#include "types.h"
#define ADDRESS 0xA4


#include "GHI_inter_PIC_I2C.h"
#include <I2C.h>

// we need a local FIFO to hold the incoming data
#pragma idata gpr3
int8 rx_fifo[256]={0};
#pragma idata
int8 rx_fifo_in=0;
int8 rx_fifo_out=0;

//------------------------------------------
//------------------------------------------
void GHI_ToggleWakePin(void)
{
	// should be implemented
}
//------------------------------------------
void GHI_Sleep(int16 ms)
{
	// this is fine for out PIC runing on 32 Mhz
	// you will have to change this loop according to your speed
	int32 x,z;
	for(z=0;z<ms;z++)
		for(x=0;x<500;x++);
}
//------------------------------------------
int8 GHI_OpenInterface(void)
{
	OpenI2C(MASTER,SLEW_ON);
	SSPADD=50;//divider
	
	// reset the fifo
	rx_fifo_in=0;
	rx_fifo_out=0;
	
	IdleI2C();
	GHI_Sleep(10);
	while ( SSPCON2bits.SEN );
    
 	return NO_ERROR;
}
//------------------------------------------
int8 GHI_CloseInterface(void)
{
	//not needed
	return UNKNOWN_ERROR;
}
//------------------------------------------
int8 GHI_SetBaudRate(int32 baud)
{
	// not needed for SPI
	return UNKNOWN_ERROR;
}
//------------------------------------------
// You will notice below that we are using
// start and stop conditions for every cycle
// this will dramatically slow down the I2C bus
// You can make this faster by doing these conditions
// when necessary
//------------------------------------------
int8 GHI_GetC(void)
{
	int8 c;
	//GHI_Sleep(1);
	// wait untill we have data
	while(!GHI_DataIsReady());
	
	if(DATARDY_PIN)// are we going to get somethign new?
	{
		StartI2C();
	    while ( SSPCON2bits.RSEN );
	    //GHI_Sleep(1);
	    IdleI2C();
		WriteI2C(ADDRESS+1);
		IdleI2C();
		rx_fifo[rx_fifo_in++]=ReadI2C();
		//Delay1K(100);
		IdleI2C();
		StopI2C();
		IdleI2C();
			
	 	
	}
	// return what we have
	return rx_fifo[rx_fifo_out++];
}
//------------------------------------------
void GHI_PutC(int8 ch)
{
	while(BUSY_PIN);
	if(DATARDY_PIN)// are we going to get something new?
	{
		StartI2C();
	    while ( SSPCON2bits.RSEN );
	    IdleI2C();//GHI_Sleep(1);
		WriteI2C(ADDRESS+1);
		IdleI2C();
		rx_fifo[rx_fifo_in++]=ReadI2C();
		//Delay1K(100);
		IdleI2C();
		StopI2C();
		IdleI2C();
			
	 	
	}
	
	// now send the data
	StartI2C();
	while ( SSPCON2bits.RSEN );
	IdleI2C();//GHI_Sleep(1);
	WriteI2C(ADDRESS);
	IdleI2C();
	//we should have an ACK now
	//RestartI2C();             // generate I2C bus restart condition
	//while ( SSPCON2bits.RSEN );// wait until re-start condition is over 
	//Delay1K(1);
	WriteI2C(ch);
	//Delay1K(1);
	IdleI2C();
	//we should get another ack
	StopI2C();
	IdleI2C();
}
//------------------------------------------
void GHI_PutS(int8 * str)
{
	int8 c;
	
	c=0;
	while(str[c])	
		GHI_PutC(str[c++]);
}
//------------------------------------------
int8 GHI_DataIsReady(void)
{
	if(rx_fifo_out!=rx_fifo_in)
		return 1; // we got data in fifo
	if(DATARDY_PIN)
		return 1; // we got data 
	return 0;// no data is ready
}
//------------------------------------------

