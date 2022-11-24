;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number		:AVRxxx
;* File Name		:"stepmot.asm
;* Title		:Simple high speed step motor controller
;* Date                 :98.07.02
;* Version              :1.00
;* Support telephone	:+47 72 88 43 88 (ATMEL Norway)
;* Support fax		:+47 72 88 43 99 (ATMEL Norway)
;* Support E-mail	:avr@atmel.com
;* Target MCU		:All AVR devices
;*
;***************************************************************************

.include "..\8515def.inc"

;*************** Define global registers ********************************

.def	temp	= R16

;*************** Define constants ********************************

.equ	c_value	= 500		;Compare value for output compare interrupt
				; 500 cycles@5Mhz = 100us

;************************************************************************=
;*
;*      PROGRAM START - EXECUTION STARTS HERE
;*
;************************************************************************=

.cseg
					;Initialize interrupt vectors
.org 0x00
		rjmp	main		
.org OC1Aaddr				;Initialize Output compare A interrupt vector
		rjmp	OC1A


;************************************************************************=
;*
;* OC1A	- Timer1 Output compare A interrupt routine
;*
;*
;* DESCRIPTION
;*
;* This interrupt routine load new step motor value from the step motor 
;* table in SRAM. The values in the table have two functions, the lower 
;* nibble contains the value to output to the step motor. The upper nibble 
;* holds the address of the next value. First the step value is output to 
;* the port, next the address is moved to the XL register.
;*
;* Number of words	:6 + return
;* Number of cycles	:7 + return
;* Low registers used	:None
;* High registers used  :3 (temp,XL,XH)	
;************************************************************************=


OC1A:	ld	temp,X			;Load temp with X pointer value
	mov 	XL,temp			;Move value to X pointer
	andi 	temp,0x0F		;Mask away upper nibble 
	out 	PORTB,temp		;Output lower nibble to step motor
	swap 	XL			;Swap upper and lower nibble
	andi 	XL,0x0F			;Mask away upper nibble, next address is ready
	reti

;****************************************************************************
;*
;* Main Program
;*
;* This program initialize Timer 1 output compare interrupt to occur
;* with a interval defined with the c_value constant.
;* The step motor lookup table is loaded from the flash and stored in SRAM
;* address 0x0100 to achieve maximum speed.
;*
;****************************************************************************

;***** Code
	
main:	ldi	r16,high(RAMEND) 	;Intialize stackpointer
	out	SPH,r16			
	ldi	r16,low(RAMEND)	
	out	SPL,r16			
	ldi 	temp,0x0F		;Set PORTB as output
	out 	DDRB,temp
	ldi 	temp,0x00		
	out 	PORTB,temp		;Write initial value to PORTB
	ldi 	temp,high(c_value)	;Load compare high value
	out	OCR1AH,temp		 
	ldi 	temp,low(c_value)	;Load compare low value 
	out	OCR1AL,temp		
	ldi 	temp,0x00	
	out 	TCNT1H,temp		;Clear timer high byte
	out 	TCNT1L,temp		;Clear timer low byte
	out 	TCCR1A,temp		;Clear timer control reg A
	ldi 	temp,0x40
	out	TIFR,temp		;Clear pending timer interrupt
	out	TIMSK,temp		;Enable Timer compare interrupt

	ldi	ZH,high(step*2)		;Initialize Z pointer to step motor table in flash
	ldi 	ZL,low(step*2)	
	ldi	XH,high(0x0100)		;Initialize X pointer to RAM location
	ldi	XL,low(0x0100)
	ldi 	temp,0x04		;Load counter value

load:	lpm				;Load step value from flash
	st	X+,R0			;Store step value in RAM
	adiw	ZL,0x01			;Increment flash pointer
	dec 	temp			;Decrement counter
	brne	load			;Continue until table is loaded

	ldi	XH,high(0x0100)		;Initialize X pointer to RAM location
	ldi	XL,low(0x0100)

	ldi 	temp,0x9
	out 	TCCR1B,temp		;Clear timer on compare match,CK/1
	sei				;Enble global interrupt
loop:	rjmp 	loop			;Do something else

step:      .db     0x19,0x2C,0x36,0x03	;Step motor lookup table 