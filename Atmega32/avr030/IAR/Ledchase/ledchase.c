/*! \file ********************************************************************
*
* Atmel Corporation
*
* - File              : ledchase.c
* - Compiler          : IAR EWAAVR 3.20
*
* - Support mail      : avr@atmel.com
*
* - Supported devices : All AVR devices can be used.
*                       The example is written for ATmega16
*
* - AppNote           : AVR030 - Getting Started with IAR EWAVR
*
* - Description       : Ledchaser
*
* $Revision: 1.1 $
* $Date: Thursday, September 30, 2004 08:52:26 UTC $
*****************************************************************************/

// function prototype
void wait(void);

// Include definition file
#include <ioavr.h>
#include <inavr.h>

int main( void )
{
    unsigned char  led;

    PORTB   = 0xFF;  //Give PORTB and "led" a initial startvalue
    led     = 1;

    DDRB    = 0xFF;  //Set PORTB as output

    while(1){        // Eternal loop
        PORTB = ~led;      // Invert the output since a zero means: LED on
        led <<= 1;         // Move to next LED by performing a rotate left

        if (!led) led = 1; // If overflow: start with led1 again

        wait();            // Uncomment this line for the 'Release' build...
    }
}



// *** Primitive wait() function ***
void wait(void){
unsigned char i,j;

for(i=0;i<255;i++)
  for(j=0;j<255;j++);
}
