/*****************************************************************************
*
* Atmel Corporation
*
* File              : spm.h
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
* Description:	      Self-programming routines written in assembly
*
******************************************************************************/

#ifndef SPM_H
#define SPM_H

#include "loader.h"

extern void spmWriteWord(unsigned char APPFLASH *addr, unsigned int data);
extern int  spmReadWord(unsigned char APPFLASH *addr);
extern void spmErasePage(unsigned char APPFLASH *addr);
extern void spmProgramPage(unsigned char APPFLASH *addr);
extern void spmEEWriteByte(unsigned int addr, unsigned char data);
extern void spmWriteLockBits(unsigned char data);

#endif  // SPM_H
