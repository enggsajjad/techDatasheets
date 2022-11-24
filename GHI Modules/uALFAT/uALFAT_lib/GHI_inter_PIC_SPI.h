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

///////////	THIS IS JUST AN EXAMPLE OF USING SPI INTERFACE //////////////////////////////
///////////	Must change to be ported appropriately to your processor ////////////////////////	 


#ifndef _GHI_INTERFACE_PIC_SPI_HEADER
#define _GHI_INTERFACE_PIC_SPI_HEADER


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

// error codes of the driver
#define NO_ERROR 0x00
#define UNKNOWN_ERROR 0xFF

// the pins used by driver and test software
#define LED0_PIN	LATBbits.LATB4
#define LED0_TRIS	TRISBbits.TRISB4
#define LED1_PIN	LATBbits.LATB3
#define LED1_TRIS	TRISBbits.TRISB3

#define RESET_TRIS	TRISCbits.TRISC0
#define RESET_PIN	LATCbits.LATC0

#define DATARDY_PIN PORTBbits.RB0
#define BUSY_PIN	PORTBbits.RB1
#define SSEL_TRIS	TRISCbits.TRISC2
#define SSEL_PIN	LATCbits.LATC2

// the software SPI protocol GHI uses
#define NDT 0xff
#define HDT 0xfe


void GHI_ToggleWakePin(void);
void GHI_Sleep(int16 ms);
int8 GHI_OpenInterface(void);
int8 GHI_CloseInterface(void);
int8 GHI_GetC(void);
void GHI_PutC(int8 c);
void GHI_PutS(int8 * str);
int8 GHI_DataIsReady(void);


#endif

#ifdef __cplusplus
}

#endif
