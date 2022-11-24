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

#ifndef _GHI_INTERFACE_PIC_UART_HEADER
#define _GHI_INTERFACE_PIC_UART_HEADER


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

// this is used to calculate teh default baud rate
#define BAUD_RATE 9600
#define CLOCK_FREQ    8000000
#define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16))


// uncomment the folowing to use interupt driver
#define UART_USE_INTERRUPT
// error codes of the driver
#define NO_ERROR 0x00
#define UNKNOWN_ERROR 0xFF

// the pins
#define CTS_PIN	PORTCbits.RC4
// RTS is output from the PIC and going to CTS on USBwiz
#define RTS_PIN	LATCbits.LATC5

// These functions must be implemented by the end user
// This depends on your processor and hardware
// This should work for UART, SPI or I2C
// UART is same as serial port on your PC but with different voltage levels
// Level converter chip, such as MAX232, is required to connect UART to a PC

// this function is needed only if we need to put USBwiz to sleep
void GHI_ToggleWakePin(void);

// This function will be used to stop the processor for x ms
// It is used only for initialization and for error recovery
void GHI_Sleep(int16 ms);

// Initialize the intrface
// it can be UART, SPI or I2C
// returns zero if opened succesfully
int8 GHI_OpenInterface(void);


// This function will be used to disable the used interface
// in most applications this can be ignorefd
// returns zero if closed
int8 GHI_CloseInterface(void);


// Will read a byte from the interface
// If there is no data, it will wait till there is some
// The user can have timeout and return zero safily to protect teh system from lockups
int8 GHI_GetC(void);


// PutC send a byte on the interface
// it will wait first for the interface to become ready
// returns 1 is pass
void GHI_PutC(int8 c);

// similar to GHI_PutC but this will send the whole string
void GHI_PutS(int8 * str);

// will return 1 if there is at least 1 byte ready for GHI_GetC()
int8 GHI_DataIsReady(void);


#ifdef __cplusplus
}
#endif


#endif
