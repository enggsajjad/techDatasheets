//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			usart0.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		USART interface.
//
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
	UBRR0L = 51;	
		
	// Enable Rx and Tx.
	UCSR0B = (1 << RXEN0) + (1 << TXEN0);
}


//=============================================================================
// Prepare bus (in this case, wait for the first byte to arrive).

void busPrepare(void)
{
	while (!(UCSR0A & (1 << RXC0)));
}


//=============================================================================
// Receive byte

unsigned char busReceiveByte(void)
{
	busPrepare();
	
	return UDR0;
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
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

