//=============================================================================
// Copyright Atmel Corporation 2003. All Rights Reserved.
//
// File:			spm.h
// Compiler:		IAR Atmel AVR Assembler
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:	
//
// Support Mail:	avr@atmel.com
//
// Description:		Self-programming routines written in assembly
//
// Other Info:
//=============================================================================


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
