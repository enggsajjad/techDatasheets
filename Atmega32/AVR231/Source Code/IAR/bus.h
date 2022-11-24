/*****************************************************************************
*
* Atmel Corporation
*
* File              : bus.h
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
* Description:	    General software interface for the bus used for data
*                   transmission of encrypted frames.
*
******************************************************************************/

#ifndef BUS_H
#define BUS_H

// Initialize bus
extern void busInit(void);

// Prepare bus for self-programming purposes
extern void busPrepare(void);

// Receive one byte
extern unsigned char busReceiveByte(void);

// Set the bus to indicate that the device is busy processing the
// previous received frame
extern void busBusy(void);

// Reply one byte as a response to the received frame
extern void busReplyByte(unsigned char data);

#endif // BUS_H
