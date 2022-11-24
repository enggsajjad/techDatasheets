;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;*
;* Number               : AVR130
;* File Name            : "avr130.asm"
;* Title                : Setup and Use The Timers
;* Date                 : 00.09.01
;* Last updated			: 10.25.02 by ØE, fixed bug in Ex3_init (sbic ASSR, OCR2UB)
;* Version              : 1.0
;* Target MCU           : Any AVR with Timer
;*
;* DESCRIPTION
;* This Application note shows how to enable and use some features of the 
;* AVR's on-board Timers. 
;*
;* This file contains the code of all the examples. To switch between the examples
;* you will have to enable the call of the corresponding initialization routine
;* (see the Reset section at the bottom) 
;*
;* This code contains markers (e.g. CL_1). These markers indicate to which step 
;* of the check list the 1-2 code lines which follow the marker refer to. 
;*
;***************************************************************************

.include "8535def.inc"


; 16 bit access Macros (see Application Note AVR072)
.macro outw
;cli					; see (Note 1)
out	@0,@1
out	@0-1,@2
;sei					; see (Note 1)
.endmacro

.macro inw
;cli					; see (Note 1)
in	@1,@2-1
in	@0,@2
;sei					; see (Note 1)
.endmacro


; Interrupt Vectore Table
.cseg	
.org	0x00				; Reset-Address
	rjmp	Reset
.org	INT0addr			; External Interrupt Request 0
	reti
.org	INT1addr			; External Interrupt Request 1
	reti
.org	OC2addr 			; Timer/Counter2 Compare Match
	rjmp	ISR_OCIE2
.org	OVF2addr			; Timer/Counter2 Overflow
	reti
.org	ICP1addr			; Timer/Counter1 Capture Event 
	rjmp	TIM1_CAPT
.org	OC1Aaddr			; Timer/Counter1 Compare Match A
	reti
.org	OC1Baddr			; Timer/Counter1 Compare Match B
	reti
.org	OVF1addr			; Timer/Counter1 Overflow
	reti
.org	OVF0addr			; Timer/Counter0 Overflow
	rjmp	ISR_TOV0
.org	SPIaddr 			; SPI Serial Transfer Complete
	reti
.org	URXCaddr			; UART, Rx Complete
	reti
.org	UDREaddr			; UART Data Register Empty
	reti
.org	UTXCaddr			; UART, Tx Complete
	reti
.org	ADCCaddr			; ADC Conversion Complete
	reti	
.org	ERDYaddr			; EEPROM Ready
	reti
.org	ACIaddr 			; Analog Comparator
	reti


init_Ex1:
	ldi r16,(1<<CS02)|(1<<CS00)
	out TCCR0,r16 ; Timer clock = system clock / 1024
	ldi r16,1<<TOV0
	out TIFR,r16 ; Clear TOV0/ clear pending interrupts
	ldi r16,1<<TOIE0
	out TIMSK,r16 ; Enable Timer/Counter0 Overflow Interrupt
	ser r16
	out DDRB,r16 ; Set Port B as output
	ret

ISR_TOV0:
	push r16
	in r16,SREG
	push r16
	in r16,PORTB ; Read Port B
	com r16 ; Invert bits of r16 register
	out PORTB,r16 ; Write Port B
	pop r16
	out SREG,r16
	pop r16
	reti



init_Ex2:
	ldi r16,(1<<CS11)|(1<<CS10)
	out TCCR1B,r16	; timer clock = system clock/64
	ldi r16,1<<ICF1
	out TIFR,r16	; Clear ICF1/clear pending interrupts
	ldi r16,1<<TICIE1
	out TIMSK,r16	; Timer/Counter1 Capture Event Interrupt
	ser r16			; Set all bits in register
	out DDRB,r16	; Set Port B as output
	cbi DDRD,PD6	; Set PD6/ICP as input
	ret

TIM1_CAPT:
	push r16
	in r16,SREG
	push r16
	in r16,ICR1L	; Read ICR low byte and high byte/ save high byte in Temp
	in r16,ICR1H	; Read ICR high byte
	com r16			; Invert bits - 1 complement (see Note 1)
	out PORTB,r16	; Write ICR1H to PORTB
	clr r16
	out TCNT1H,r16	; Write Temp register
	out TCNT1L,r16	; Clear the 16 bit register
	pop r16
	out SREG,r16
	pop r16
	reti


init_Ex3:
	ldi r16,1<<AS2
	out ASSR,r16	; Enable asynchronous mode
					; Clear timer on compare match / Timer Clock = system clock / 1024
	ldi r16,(1<<CTC2)|(1<<CS22)|(1<<CS21)|(1<<CS20)
	out TCCR2,r16	; Timer clock = system clock/1024
	ldi r16,1<<OCF2
	out TIFR,r16	; Clear OCF2/clear pending interrupts
	ldi r16,1<<OCIE2
	out TIMSK,r16	; Enable timer output compare interrupt
	ldi r16,32
	out OCR2,r16	; Set output compare value to 32
	ser r16
	out DDRB,r16	; Set Port D as output
loop:
	in r16, ASSR
	andi r16, ~(1<<OCR2UB)
	brne loop
	ret


ISR_OCIE2:
	push r16
	in r16,SREG
	push r16
	in r16,PORTB	; Read Port B
	com r16			; Invert bits of r16 register
	out PORTB,r16	; Write Port B
	pop r16
	out SREG,r16
	pop r16
	reti



init_Ex4:
	; 8 bit PWM non-inverted (Fck/510)
	ldi r16,(1<<PWM2)|(1<<COM21)|(1<<CS20)
	out TCCR2,r16	; 8 bit PWM non-inverted (Fck/510)
	ldi r16,0xE0
	out OCR2,r16	; Set compare value/duty cycle ratio
	ldi r16,0x8F
	out DDRD,r16	; Set PD7/OC2 and low nibble Port D as output
	ret


; *** Begin of Program Execution ***
Reset:
; Initialization of the Stack Pointer
	ldi	r16,low(RAMEND)
	out	SPL,r16
	ldi	r16,high(RAMEND)
	out 	SPH,r16

; Enable the next line for Example 1 disable for other Examples
	;rcall	init_Ex1
; Enable the next line for Example 2 disable for other Examples
	;rcall	init_Ex2
; Enable the next line for Example 3 disable for other Examples
	;rcall	init_Ex3
; Enable the next line for Example 4 disable for other Examples
	rcall	init_Ex4
; (CL_2.2) 
	sei				; Global Interrupts enabled

; Endlessloop
loopforever:
rjmp	loopforever

