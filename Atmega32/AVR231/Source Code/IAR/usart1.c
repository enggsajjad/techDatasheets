/*****************************************************************************
*
* Atmel Corporation
*
* File              : usart1.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 2.0 $
* Date              : $Date: Wednesday, February 15, 2006 09:52:02 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with Bootloader Capabilities 
*                     , and at least 1-KB SRAM can be used.
*                     The example is written for • ATmega8
*                                                • ATmega16
*                                                • ATmega162
*                                                • ATmega169
*                                                • ATmega32
*                                                • ATmega64
*                                                • ATmega128
*
* AppNote           : AVR231 - AES Bootloader
*
* Description       : USART interface.
*
******************************************************************************/

#include <inavr.h>
#include <ioavr.h>
#include "bus.h"


//=============================================================================
// Initialize bus interface

void busInit(void)
{
	// 115200 baud @ 3.6864MHz
	UBRR1L = 1;	
	// 9600 baud @ 3.6864MHz
//	UBRR1L = 23;	
		
	// Enable Rx and Tx.
	UCSR1B = (1 << RXEN1) + (1 << TXEN1);
}


//=============================================================================
// Prepare bus (in this case, wait for the first byte to arrive).

void busPrepare(void)
{
	while (!(UCSR1A & (1 << RXC1)));
}


//=============================================================================
// Receive byte

unsigned char busReceiveByte(void)
{
	busPrepare();
	
	return UDR1;
}


//=============================================================================
// Set bus to a busy state. Requires no action in USART.

void busBusy(void)
{
	return;
}


//=============================================================================
// Reply byte

void busReplyByte(unsigned char data)
{
	while(!(UCSR1A & (1 << UDRE1)));
	UDR1 = data;
}
