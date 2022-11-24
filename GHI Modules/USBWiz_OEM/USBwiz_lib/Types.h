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
 
#ifndef _TYPES_HEADER_
#define _TYPES_HEADER_


#define int8    unsigned char
#define int16   unsigned short
#define int32   unsigned long


// Select what driver you want to use
// ONLY ONE of the follwing can be left uncommented
// This is the only thing you need ot change in this file
// if you are wrting you own driver the use GHI_INTR_USER...
// ...don't forget to fill in the functions in GHI_inter_user.c
#define _USE_GHI_INTR_PIC_SPI_ // use this for uPICFAT board
//#define _USE_GHI_INTR_PIC_I2C_
//#define _USE_GHI_INTR_PIC_UART_
//#define _USE_GHI_INTR_PC_SERIAL_
//#define _USE_GHI_INTR_USER_


#ifdef _USE_GHI_INTR_USER_
// ToDo: Add your header file
#include "GHI_inter_user.h"
#endif

// now include the heaqder of your processor
#if defined (_USE_GHI_INTR_PIC_SPI_) || defined (_USE_GHI_INTR_PIC_I2C_) || defined (_USE_GHI_INTR_PIC_UART_)

#include "p18f458.h"
#include "USART.h"
#include "SPI.h"
#include "I2C.h"

// the pins used by driver and test software
#define LED0_PIN		LATBbits.LATB4
#define LED0_TRIS	TRISBbits.TRISB4
#define LED1_PIN		LATBbits.LATB3
#define LED1_TRIS	TRISBbits.TRISB3

#define S2_STATE	PORTEbits.RE0
#define S3_STATE	PORTEbits.RE1
#define S4_STATE	PORTEbits.RE2

#define RESET_TRIS	TRISCbits.TRISC0
#define RESET_PIN	LATCbits.LATC0

#define DATARDY_PIN PORTBbits.RB0
#define BUSY_PIN	PORTBbits.RB1
#define SSEL_TRIS	TRISCbits.TRISC2
#define SSEL_PIN	LATCbits.LATC2


// we will use the define to stop PIC C compiler from complaining
// you will probably not use this with other compilers
// on PIC, RAM and ROM are not on the same range (block)
// most micros have ROM and RAM on the same address lines
#define ROM_TYPE (char rom near*)
#define ROM_NEAR_TYPE rom near
#endif


// the header of windows if we are using a PC
// we may need RS232 level converter. Check your board
#ifdef _USE_GHI_INTR_PC_SERIAL_
#include <windows.h>
#pragma warning( disable : 4761 4244 4101 4005 ) 

#define ROM_NEAR_TYPE
#define ROM_TYPE
#endif


#endif
