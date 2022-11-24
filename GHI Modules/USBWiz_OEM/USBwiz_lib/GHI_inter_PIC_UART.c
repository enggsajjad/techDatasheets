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

#include "types.h"

#ifdef _USE_GHI_INTR_PIC_UART_

#include "GHI_inter_PIC_UART.h"
#include <USART.h>

#ifdef UART_USE_INTERRUPT
// we need a local FIFO to hold the incoming data
#pragma idata gpr3
int8 rx_fifo[256]={0};
#pragma idata
int8 rx_fifo_in=0;
int8 rx_fifo_out=0;
#endif
//----------------------------------------------------------------------------
//------------------------------------------
void GHI_ToggleWakePin(void)
{
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
#ifdef UART_USE_INTERRUPT
#pragma interrupt ISRhandlerHigh
void ISRhandlerHigh(void)
{
    if(PIR1bits.TMR1IF)	
	{
		rx_fifo[rx_fifo_in++]=getcUSART();
    	if((rx_fifo_in-rx_fifo_out)>100)
      		RTS_PIN=1;// we can't acept more data
      		
     	INTCONbits.TMR0IF = 0;//clear interrupt flag
    }
}
#pragma code
//------------------------------------------
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh (void)
{
  _asm
    goto ISRhandlerHigh //jump to interrupt routine
  _endasm
}
#pragma code
//------------------------------------------
#endif
//------------------------------------------
int8 GHI_OpenInterface(void)
{
	//make RTS output
	TRISCbits.TRISC5=0;
	RTS_PIN=1;// we are not ready for data
	
	#ifdef UART_USE_INTERRUPT
	// reset the fifo
	rx_fifo_in=0;
	rx_fifo_out=0;

	OpenUSART(USART_TX_INT_OFF&USART_TX_INT_ON&
	          USART_ASYNCH_MODE&USART_EIGHT_BIT&
	          USART_CONT_RX&USART_BRGH_HIGH,SPBRG_VAL);
	RTS_PIN=0;// we are ready for data

	#else
	OpenUSART(USART_TX_INT_OFF&USART_TX_INT_OFF&
	          USART_ASYNCH_MODE&USART_EIGHT_BIT&
	          USART_CONT_RX&USART_BRGH_HIGH,SPBRG_VAL);
	 #endif    
	
	
 	return NO_ERROR;
}
//------------------------------------------
int8 GHI_CloseInterface(void)
{
	//not needed
	return UNKNOWN_ERROR;
}
//------------------------------------------
int8 GHI_GetC(void)
{
	#ifdef UART_USE_INTERRUPT
	while(!GHI_DataIsReady());
	if((rx_fifo_in-rx_fifo_out)<50)
      RTS_PIN=1;// we can accept more data
      
	return rx_fifo[rx_fifo_out++];
	
	#else
	
	int8 c;
	
	RTS_PIN=0;//accept data
	while(!GHI_DataIsReady());
	RTS_PIN=1;//do not accept more data
	
	return getcUSART();
	#endif
}
//------------------------------------------
void GHI_PutC(int8 ch)
{
	while(CTS_PIN);
	
	while(BusyUSART());
	putcUSART(ch);
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
	#ifdef UART_USE_INTERRUPT
	if(rx_fifo_in!=rx_fifo_out)
		return 1;// we have data
	return 0;// no data
	#else
	return DataRdyUSART();
	#endif
}
//------------------------------------------
#endif
