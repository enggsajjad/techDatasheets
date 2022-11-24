// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* - File              : main.c
* - Compiler          : IAR EWAAVR 4.10b
*
* - Support mail      : avr@atmel.com
*
* - Supported devices : All devices with a TWI module can be used.
*                       The example is written for ATmega32
*
* - AppNote           : AVR316 - SMBus slave
*
* - Description       : Initialization and LED display code.
*
* $Revision: 1.5 $
* $Date: Thursday, September 29, 2005 12:10:38 UTC $
*****************************************************************************/

/*! \mainpage
\section Intro Introduction
This documents data structures, functions, variables, defines, enums, and
typedefs in the software for application note AVR316: SMBus slave using the
TWI module. This is an example on how to use the TWI module of an AVR to
implement an SMBus slave.\n\n

\section CI Compilation Info
The source code is written for the IAR C/C++ compiler.
Follow these steps to compile the project with IAR Embedded workbench:
- Unzip all source code files into the same directory.
- Create a new project and add main.c, SMBslave.c and SMBExample.c.
- Under 'Project -> Options' select the device you are compiling for. Select
'Enable bit definitions in I/O include files'. For AVRStudio compatible output
format use ubrof8 for Debug and intel_extended for Release.
- Open the file SMBSlave.h and edit the section marked 'Configuration
parameters'. The usage of each parameter is described in detail in the file.
- Select 'Project -> Build' to build the project.
- For Release target add the following to Linker 'Extra Options':\n
'-y(CODE)'\n
'-Ointel-extended,(XDATA)=$TARGET_DIR$\\$PROJ_FNAME$_eeprom.hex' \n
This generates a separate hex-file for EEPROM data, and thus circumvent
limitations in the intel extended file format.\n\n

\section DI Device Info
All devices with a TWI module can be used. See appnote for details on how
to add support for new devices.\n
The LEDs in the demo example doesn't work correctly with all devices.\n\n

\section CSZ Code Size
1532 bytes of code for the complete application with PEC lookup.\n


*
*/


#include <ioavr.h>
#include <inavr.h>
#include "device_specific.h"
#include "SMBExample.h"
#include "SMBSlave.h"

unsigned volatile char leds[32];         //!< Array of values to display on LEDs
unsigned volatile char ledLength = 0;    //!< Number of values to display on LEDs
unsigned volatile char ledIndex = 0;     //!< Index of the currently displayed LED value


/*! \brief Program entry point
 *
 *  Initializes peripherals, then goes into an infinite loop.
 */
void main(void)
{
    // Set data direction of PORTB as output and turn off LEDs.
    DDRB = 0xff;
    PORTB = 0xff;

    // Initialize timer1
    TCCR1B = (1 << CS11) | (1 << CS10);
    TIMSK1 |= (1 << TOIE1);

    // Initialize SMBus
    SMBusInit();
    SMBEnable();

    // Enable interrupts globally
    __enable_interrupt();
    for (;;)
    {

    }
}


/*! \brief Timer/Counter1 overflow interrupt, controls the LEDs.
 *
 *  The Timer/Counter1 overflow interrupt routine is used to
 *  output data received, to the LEDs of the STK500 starter kit/
 *  development board at a rate that is visible to the human eye.
 */
#pragma vector = TIMER1_OVF_VECT
__interrupt void TCNT1_OVF_ISR(void)
{
    unsigned char tempLength; // Only used to specify volatile access order

    tempLength = ledLength;
    // Output next LED value in LED buffer
  ledIndex++;
    if (ledIndex >= tempLength)
    {
        ledIndex = 0;
    }
    PORTB = ~leds[ledIndex];
}


