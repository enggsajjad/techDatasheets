//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			loader.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Based on work by:�E, VU
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		Loader receives encrypted data frames (generated using
//					'create' tool and sent by 'update' tool), decrypts them and
//					executes the commands described in frames.
//
//					For more information, refer to the Application Note
//					Documentation.
//
// Other Info:		
//=============================================================================


#include <inavr.h>
#include <ioavr.h>
#include "spm.h"
#include "aes.h"
#include "loader.h"
#include "bootldr.h"
#include "bus.h"
#include "crc.h"


//=============================================================================
// Error Codes

#define ERROR_OK 0x11
#define ERROR_CRC 0x22


//=============================================================================
// Frame types

#define TYPE_EOF 0
#define TYPE_ERASE 1
#define TYPE_PREPARE 2
#define TYPE_DATA 3
#define TYPE_PROGRAM 4
#define TYPE_EEPROM 5
#define TYPE_LOCKBITS 6
#define TYPE_RESET 7


//=============================================================================
// Initial vector for cipher block unchaining (INITIALVECTOR_x are
// defined in 'bootldr.h' which is generated by 'create' tool)

#if KEY_COUNT > 0
BOOTFLASH unsigned char initialVector[16] =
{
	(unsigned char)(INITIALVECTOR_3 >> 24),
	(unsigned char)(INITIALVECTOR_3 >> 16),
	(unsigned char)(INITIALVECTOR_3 >> 8),
	(unsigned char)(INITIALVECTOR_3 >> 0),
	(unsigned char)(INITIALVECTOR_2 >> 24),
	(unsigned char)(INITIALVECTOR_2 >> 16),
	(unsigned char)(INITIALVECTOR_2 >> 8),
	(unsigned char)(INITIALVECTOR_2 >> 0),
	(unsigned char)(INITIALVECTOR_1 >> 24),
	(unsigned char)(INITIALVECTOR_1 >> 16),
	(unsigned char)(INITIALVECTOR_1 >> 8),
	(unsigned char)(INITIALVECTOR_1 >> 0),
	(unsigned char)(INITIALVECTOR_0 >> 24),
	(unsigned char)(INITIALVECTOR_0 >> 16),
	(unsigned char)(INITIALVECTOR_0 >> 8),
	(unsigned char)(INITIALVECTOR_0 >> 0)
};
#endif // KEY_COUNT > 0

//=============================================================================
// Loader routine. For more information about the format of frames, please
// refer to the Application Note Documentation.

union {
	struct {
		unsigned char m_rxBuffer[BUFFER_SIZE];	// Receive buffer
		unsigned char m_pageBuffer[PAGE_SIZE];	// Page is assembled here before
							// getting programmed to flash mem
	} part1;

#if KEY_COUNT > 0
	struct {
		unsigned char m_tempbuf[256]; // Temp buffer for aesInit.
	} part2;
#endif
} sharedbufs;

#define rxBuffer   sharedbufs.part1.m_rxBuffer
#define pageBuffer sharedbufs.part1.m_pageBuffer
#if KEY_COUNT > 0
	#define tempbuf    sharedbufs.part2.m_tempbuf
#endif

__C_task void loader(void)
{
	#if KEY_COUNT > 0
		unsigned char chainCipherBlock[16];		// Buffer for Cipher Block
												// Unchaining
	
		// Copy Initial Vector to start the cipher block unchaining
		{
			unsigned char *p = chainCipherBlock;
			unsigned char BOOTFLASH *q = initialVector;
			unsigned char n = 16;
		
			do
			{
				*p++ = *q++;
			}
			while (--n);
		}
		
		aesInit( tempbuf ); // Init AES algorithm.
	#endif

	// Prepare bus for incoming frames...
	busPrepare();
	
	// Start the Watchdog Timer (sorry for the mess, there is a bug in
	// iom8.h (in version 2.28A) which is informed to IAR).
#if 1
	#if !defined(WDTCR) && !defined(__IOM8_H)
		#define WDTCR WDTCSR
	#endif

	#if defined(__IOM16_H) || defined(__IOM32_H)
		WDTCR = (7 << WDP0) | (1 << WDE);
	#else
		#if defined(__IOM8_H)
			WDTCR = (1 << WDTOE) | (1 << WDE);
		#else
			WDTCR = (1 << WDCE) | (1 << WDE);
		#endif
		WDTCR = (7 << WDP0) | (1 << WDE);
	#endif
#endif

	// Loop forever (the loop is broken by Watchdog when RESET record is
	// received)
	for (;;)
	{
		frameindex_t frameSize;
		unsigned int crc;
		
		// Get the frame size
		frameSize =	((frameindex_t)busReceiveByte() << 8) |	busReceiveByte();

		// Receive a frame of data from communication interface and calculate
		// its CRC
		{
			unsigned char *p = rxBuffer;
			frameindex_t n = frameSize;

			crc = 0;
			do
			{
				unsigned char ch;

				ch = busReceiveByte();

				*p++ = ch;
				crc = CRC(crc, ch);
				
				__watchdog_reset();
			}
			while (--n);
			
			busBusy();
		}

		// CRC is OK?
		if (crc == 0x0000)
		{
			unsigned char *p = rxBuffer;			

			// Decrypt 16 bytes, CRC-bytes are ignored.
			#if KEY_COUNT > 0
			{
				unsigned char *p = rxBuffer;

				frameSize -= 2;
				do
				{
			    	__watchdog_reset();
					aesDecrypt(p, chainCipherBlock);
					p += 16;
				}
				while (frameSize -= 16);
			}
			#endif // KEY_COUNT > 0

			// Check that the signature is correct
			if ((*p++ == (unsigned char)(SIGNATURE >> 24)) &&
				(*p++ == (unsigned char)(SIGNATURE >> 16)) &&
				(*p++ == (unsigned char)(SIGNATURE >>  8)) &&
				(*p++ == (unsigned char)(SIGNATURE >>  0)))
			{
				unsigned char APPFLASH *address;
				unsigned char bits;
				frameindex_t size;
				unsigned char type;

				// Continue parsing the frames until the 'End Of Frame' is
				// received
				while ((type = *p++) != TYPE_EOF)
				{
				    	__watchdog_reset();

					// Receive Lock Bits (used in TYPE_LOCKBITS)
					bits = *p++;

					// Destination Address (note: 'bits' and 'address' overlap)
#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
					address =
						(unsigned char APPFLASH *)((unsigned long)bits << 16);
					address = (unsigned char APPFLASH *)
						((unsigned long)address | ((unsigned long)*p++ << 8));
					address = (unsigned char APPFLASH *)
						((unsigned long)address | *p++);
#pragma diag_default=Pe1053 // Back to default.

					size = ((unsigned int)*p++ << 8);
					size |= *p++;

					switch (type)
					{
						// Erase page
						case TYPE_ERASE:
							spmErasePage(address);
							// Fall-through!

						// Prepare for incoming data chunks by copying the page
						// contents into the page buffer
						case TYPE_PREPARE:
						{
							unsigned char *q = pageBuffer;
							unsigned char APPFLASH *r = address;

							do
							{
								*q++ = *r++;
							}
							while (--size);
							break;
						}

						// Chunk of data is written into the page buffer
						case TYPE_DATA:
						{
							unsigned char *r =
								&pageBuffer[(unsigned long)(address) & 0xffff];

							do
							{
								*r++ = *p++;
							}
							while (--size);
							break;
						}

						// Program page buffer into flash page
						case TYPE_PROGRAM:
						{
							unsigned int *q = (unsigned int *)pageBuffer;
							unsigned char APPFLASH *r = address;

							do
							{
						    	__watchdog_reset();
								spmWriteWord(r, *q++);
								r += 2;
							}
							while (--size);

							spmErasePage(address);
							spmProgramPage(address);
							break;
						}

						// Write a chunk of data into EEPROM
						case TYPE_EEPROM:
							do
							{
						    	__watchdog_reset();
								spmEEWriteByte(
									(unsigned long)address++ & 0xffff, *p++
								);
							}
							while (--size);
							break;

						// Write Lock Bits
						case TYPE_LOCKBITS:
							spmWriteLockBits(bits);
							break;

						// Reset (let the Watchdog Timer expire)!
						case TYPE_RESET:
							busReplyByte(ERROR_OK);
							for(;;);
							
						// Nonsense frames, one byte long
						default:
							p -= 5;
							break;
					}
				}
			}

			busReplyByte(ERROR_OK);
		}
    	else
    		busReplyByte(ERROR_CRC);   		
  	}
}
