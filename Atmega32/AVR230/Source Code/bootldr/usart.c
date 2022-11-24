//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			usart.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		USART interface.
//
// Other Info:		
//=============================================================================

#include <inavr.h>
#include <ioavr.h>
#include "bus.h"


//=============================================================================
// Initialize bus interface

void busInit(void)
{
	// 9600 baud @ 8.00MHz
	UBRRL = 51;
		
	// Enable Rx and Tx.
	UCSRB = (1 << RXEN) | (1 << TXEN);
}


//=============================================================================
// Prepare bus (in this case, wait for the first byte to arrive).

void busPrepare(void)
{
	while (!(UCSRA & (1 << RXC)));
}


//=============================================================================
// Reply byte

void busReplyByte(unsigned char data)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = data;
}


//=============================================================================
// Receive byte

unsigned char busReceiveByte(void)
{
	busPrepare();
	
	return UDR;
}


//=============================================================================
// Set bus to a busy state. Requires no action in USART.

void busBusy(void)
{
	return;
}
