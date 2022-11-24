//=============================================================================
// Copyright Atmel Corporation 2003. All Rights Reserved.
//
// File:			crc.asm
// Compiler:		IAR Atmel AVR Assembler
// Output Size:
// Created:			20-Feb-2003	JP (Atmel Finland)
// Modified:	
//
// Support Mail:	avr@atmel.com
//
// Description:		CRC calculation routine
//
// Other Info:
//=============================================================================

//=============================================================================
// Polynome used in CRC calculations

#define	CRC_POLYNOME 0x8005


//=============================================================================

PUBLIC	CRC

RSEG	CODE


//=============================================================================
// CRC calculation routine

CRC:
	ldi		r20, 0x08
	ldi		r21, LOW(CRC_POLYNOME)
	ldi		r22, HIGH(CRC_POLYNOME)

CRC_Loop:
	; Rotate left. If MSb is 1 -> divide with Generator-polynome.
	lsl		r18
	rol		r16
	rol		r17
	brcc	CRC_SkipEor

	eor		r16, r21
	eor		r17, r22

CRC_SkipEor:
	dec		r20
	brne	CRC_Loop
	ret

//=============================================================================

END
