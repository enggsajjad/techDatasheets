// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* - File              : avr448.c
* - Compiler          : CodeVision 1.24.6 Evaluation Version.
*
* - Support mail      : avr@atmel.com
*
* - Supported devices : All devices with PWM and Pin Change Interrupt can be used.
*                       The example is written for ATmega48.
*
* - AppNote           : AVR448 - Control of High Voltage 3-Phase BLDC Motor.
*
* - Description       : Example of how to control a BLDC motor using pin change
*                       interrupts connected to hall sensor output to control
*                       motor commutation, and PWM-controlled power to the drive stage.
*
* $Revision: 1.1 $
* $Date: Thursday, June 16, 2005 08:31:22 UTC $
*****************************************************************************/
#include <mega48.h>
#include "bit_defs.h"



// Global variables.
unsigned char RunClockwise = 1; //!< Start with clockwise rotation.
unsigned char IPM_B4_PWM = 0xFF; //!< Control signals to IPM driver chip.



// Function prototypes.
void main(void);
void initialization(void);
void PWM2_initialize(void); 
void ADC_setup(void);



//! Initialize I/O-ports, ADC and PWM output.
void initialization(void)
{
	PORTB= ~0x3f; // 6 bits of PORTB to be high
	DDRB = 0x3f; // set PORTB6 bits as output
	ADC_setup(); // Initialize the ADC
	PWM2_initialize(); // Initialize PWM2.
}



//! Initialize ADC to CPU/4 speed, channel 5, free running mode.
void ADC_setup(void)
{
	ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADATE)|(1<<ADSC); //Enable ADC, ADC clk = CK/4, free running.
//	ADMUX = 0x05 | (1<<REFS0) | (1<<ADLAR); // Channel 5 with AVCC as ref.
	ADMUX = 0x05 | (0<<REFS0) | (1<<ADLAR); // Channel 5 with AREF as ref.
}



//! Initialize PWM output OC2B from Timer/Counter2 at 20kHz base freq @ 8MHz CPU.
void PWM2_initialize(void)
{
	TCCR2A = (1<<COM2B1)|(1<<COM2B0)| // Set OC2B (DIP pin 5) on Compare Match,
	         (0<<WGM21)|(1<<WGM20);   // phase correct PWM & OCR2A as TOP.
	TCCR2B = (1<<WGM22)|(0<<CS22)|(0<<CS21)|(1<<CS20);
	OCR2A  = 200; // 200 decimal TOP.
	OCR2B  = 100; // This starts PWM output at 50%
	
	DDRD |= (1<<DDB3);
}



//! Pin Change Interrupt for PORTC (PCINT8..14).
interrupt [PCINT1] void Pin_Change_Int_Serv(void)
{
	static unsigned char Hall_In;
	
	Hall_In = PINC & 0x07;
	
	if( RunClockwise ) {
		// This is clockwise Switch statement.
		// Based on Hall inputs select 1 of 6 patterns to
		// send to IPM via main().
		switch(Hall_In) {
			case 1: IPM_B4_PWM = 0b101011; break;
			case 5: IPM_B4_PWM = 0b110011; break;
			case 4: IPM_B4_PWM = 0b110101; break;
			case 6: IPM_B4_PWM = 0b011101; break;
			case 2: IPM_B4_PWM = 0b011110; break;
			case 3: IPM_B4_PWM = 0b101110; break;
			
			// All outputs off if illegal Hall sensors.
			default: IPM_B4_PWM = 0b111111;
		}
	} else {
		// This is counter-clockwise Switch statement.
		// Based on Hall inputs, select 1 of 6 patterns to
		// send to IPM via main().
		switch(Hall_In) {
			case 6: IPM_B4_PWM = 0b011110; break;
			case 4: IPM_B4_PWM = 0b011101; break;
			case 5: IPM_B4_PWM = 0b110101; break;
			case 1: IPM_B4_PWM = 0b110011; break;
			case 3: IPM_B4_PWM = 0b101011; break;
			case 2: IPM_B4_PWM = 0b101110; break;

			// All outputs off if illegal Hall sensors.
			default: IPM_B4_PWM = 0b111111;
		}
	}
}



//! Application starts here.
void main(void)
{
	unsigned char speed;
	
	initialization(); // Initialize Peripherals.
	
	// Set up and Enable Interrupts
	PCMSK1 = (1<<PCINT10) | (1<<PCINT9)| (1<<PCINT8);
	PCICR = (1<<PCIE1);// Enable Pin Change Ints from port C.
	
	// Start interrupts by forcing Hall Inputs to 0, then switching pins to inputs.
	DDRC = 7; // 3 Hall Inputs pulled to 0 volts.
	PORTC = 0;
	PORTC = 1;
	PORTC = 2;
	PORTC = 4; // Start interrupts with forced pin change
	DDRC = 0; // Port C switched back to all inputs. 
	SREG |= 0x80;// Enable Interrupts  via SEI bit
	
	PORTD |= (1<<PD0) | (1<<PD1); // Direction control input pullups, undocumented.
	
	while(1) { // IPM_B4_PWM is updated via a Pin Change Interrupt
	
                // Direction control, undocumented.
	        if( !(PIND & (1<<PD0)) ) RunClockwise = 1;
	        if( !(PIND & (1<<PD1)) ) RunClockwise = 0;
	
	        // Update speed setting from ADC reading.
                speed = ADCH;
                if( speed > 199 ) speed = 199;
                OCR2B = speed;
		
		 // If OC2B is high, send inactive outputs to IPM.
		if( PIND & (1 << PD3) ) {
			PORTB = ~0xFF; // <== If motor driver has active high inputs.
//			PORTB = 0xFF; // <== If motor driver has active low inputs.
		} else {
			PORTB = ~IPM_B4_PWM; // <== If motor driver has active high inputs.
//			PORTB = IPM_B4_PWM; // <== If motor driver has active low inputs.
		}
	}
}
