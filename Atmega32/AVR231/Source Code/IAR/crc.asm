/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.c
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
****************************************************************************/

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
