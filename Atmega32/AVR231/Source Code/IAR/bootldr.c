//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			bootldr.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		Start point of the program execution. Customize this
//					for your own needs. Currently, if switch SW7 is
//					pressed during reset, the boot loader is switched to
//					update mode i.e. ready to receive encrypted frames and
//					program the contents into flash or eeprom memory.
//
//					Before executing the application, the boot loader checks
//					the condition of the Application Section (if CRC_CHECK is
//					defined in 'bootldr.h' which is generated using 'create'
//					tool). If Application Section is not intact, it is not
//					executed.
//
// Other Info:		
//=============================================================================

#include <ioavr.h>
#include <inavr.h>
#include "loader.h"
#include "bus.h"
#include "spm.h"
#include "crc.h"


//=============================================================================
// Starts here!

__C_task void main(void)
{
	__disable_interrupt();
	busInit();

	// Loop forever (loops only if Application Section is damaged)
	for (;;)
	{
		// Key pressed? Yes -> run the loader routine
		if (!(PIND & (1 << PD2)))
			// Typecast the function to a not-__C_TASK function (__C_TASK is
			// defined just to remove some unneeded register storing from the
			// loader() function)
			((void (*)())loader)();	
		else
	
		#ifdef CRC_CHECK
		// Check that the Application Section contents is undamaged
		// by calculating the CRC of the whole memory.
		{
			unsigned int crc = 0;
			unsigned char APPFLASH *p = (unsigned char APPFLASH *)0x000000;
			unsigned char APPFLASH *n = (unsigned char APPFLASH *)MEM_SIZE;
	
			do
			{
				crc = CRC(crc, *p++);
			}
			while (--n);
	
			// Application Section damaged
			//   -> do not jump to Reset Vector of the Application Section
			if (crc) {
				DDRB = 0xff;
				do {
					PORTB ^= 0xff;
					__delay_cycles( 1000000 );
				} while(1);
//				continue;
			}
		}
		#endif
		
		((void (*)())0x0000)();
	}
}
