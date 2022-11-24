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


///////////	THIS IS JUST AN EXAMPLE OF USING USART INTERFACE //////////////////////////////
///////////	Must change to be ported appropriately to your processor ////////////////////////	 


#ifndef _GHI_INTERFACE_PIC_UART_HEADER
#define _GHI_INTERFACE_PIC_UART_HEADER


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

// this is used to calculate the default baud rate
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
// RTS is output from the PIC and going to CTS on uALFAT
#define RTS_PIN	LATCbits.LATC5


void GHI_ToggleWakePin(void);
void GHI_Sleep(int16 ms);
int8 GHI_OpenInterface(void);
int8 GHI_CloseInterface(void);
int8 GHI_GetC(void);
void GHI_PutC(int8 c);
void GHI_PutS(int8 * str);
int8 GHI_DataIsReady(void);

// Sets the rate of the data coming out on UART
// this is needed for UART only
int8 GHI_SetBaudRate(int32 baud);


#ifdef __cplusplus
}
#endif

#endif


