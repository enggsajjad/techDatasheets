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
*
****************************************************************************/

NAME	spm(16)
PUBLIC	spmWriteWord
PUBLIC	spmWriteLockBits
PUBLIC	spmErasePage
PUBLIC	spmProgramPage
PUBLIC	spmEEWriteByte

#define __ENABLE_BIT_DEFINITIONS__
#include INCLUDE_FILE

#if !defined( EEMWE )
	#define EEMWE EEMPE
#endif
#if !defined( EEWE )
	#define EEWE EEPE
#endif


//=============================================================================
// I/O registers used


RSEG	CODE

//=============================================================================
// Writes one word to a temporary page buffer

spmWriteWord:
	#ifdef __RAMPZ__
		movw	r1:r0, r21:r20
	#else
		movw	r1:r0, r19:r18
	#endif
	ldi		r22, (1 << SPMEN)
	rjmp	spmSPM

//=============================================================================
// Writes Lock Bits

spmWriteLockBits:
	mov		r0, r16
	ldi		r22, (1 << BLBSET) | (1 << SPMEN)
	rjmp	spmSPM


//=============================================================================
// Erases one flash page

spmErasePage:
	ldi		r22, (1 << PGERS) | (1 << SPMEN)
	rjmp	spmSPM


//=============================================================================
// Programs the temporary buffer to flash memory

spmProgramPage:
	ldi		r22, (1 << PGWRT) | (1 << SPMEN)


//=============================================================================
// Executes self-programming command

spmSPM:
	movw	r31:r30, r17:r16

	rcall	spmWait

	in		r20, SREG
	cli

	#ifdef __RAMPZ__
		in		r21, RAMPZ
		out		RAMPZ, r18
	#endif

	#ifdef __MEMSPM__
		sts		SPMREG, r22
	#else		
		out		SPMREG, r22
	#endif
	
	#ifdef __RAMPZ__
		espm
	#else
		spm
	#endif
		
	dw		0xFFFF
	nop
		
	#ifdef __RAMPZ__
		out		RAMPZ, r21
	#endif
	
	out		SREG, r20
	ret

spmWait:
	#ifdef __MEMSPM__
		lds		r23, SPMREG
		andi	r23, (1 << SPMEN)
		brne	spmWait	
	#else
		in		r23, SPMREG
		sbrc	r23, SPMEN
		rjmp	spmWait
	#endif

	ret


//=============================================================================
// Writes one byte to EEPROM memory

spmEEWriteByte:
	rcall	spmWait
	rcall	spmEEWriteByteComplete

	out		EEARL, r16
	out		EEARH, r17
	out		EEDR, r18

	sbi		EECR, EEMWE
	sbi		EECR, EEWE
		
spmEEWriteByteComplete:
	sbic	EECR, EEWE
	rjmp	spmEEWriteByteComplete
	ret


END
