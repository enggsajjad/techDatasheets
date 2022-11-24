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
///////////	Must change to be ported appropriately to your processor ////////////////////////	 
/////////// Needs modification to support I2C communications with firmware 3.05 and up //////////////////////

/*
#ifndef _GHI_INTERFACE_PIC_I2C_HEADER
#define _GHI_INTERFACE_PIC_I2C_HEADER


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

// uALFAT I2C address
#define ADDRESS (0xa4)

// error codes of the driver
#define NO_ERROR 0x00
#define UNKNOWN_ERROR 0xFF

// the pins
#define DATARDY_PIN PORTDbits.RD2
#define BUSY_PIN	PORTDbits.RD1


void GHI_ToggleWakePin(void);
void GHI_Sleep(int16 ms);
int8 GHI_OpenInterface(void);
int8 GHI_CloseInterface(void);
int8 GHI_GetC(void);
void GHI_PutC(int8 c);
void GHI_PutS(int8 * str);
int8 GHI_DataIsReady(void);


#ifdef __cplusplus
}
#endif


#endif

*/
