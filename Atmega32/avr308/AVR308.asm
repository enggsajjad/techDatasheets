;**************************************************************************
;*
;* Title:           LIN slave 
;* Version:         04
;* Last updated:    99.08.10
;* Target:          AT90S1200
;*
;* Support E-mail:  avr@atmel.com
;*
;* Ver04: Can run with frequencies over 1 MHz. At 3 MHZ the data is received 
;*        correctly without any in-frame-responce-space. But the baudrate must 
;*        be higher than apx 12Kbps.
;* Ver03: One Stack level free. Outputs pulses on PIND,0 when sampling 
;*        received data.
;* Ver02: Send and REceive. 1-2 MHz only. The master must have an in-frame-
;*        response space of at least 25us (with three identifiers in EEPROM. 
;*        With 16 IDs in EEPROM the infr. resp space should be apx 0.5)
;* Ver01: Only to send data.
;**************************************************************************

.include "1200def.inc"

.equ    RxD         = 2         ;must be pin 2 (Ext int 0)
.equ    TxD         = 1
.equ    IDcountaddr = 32        ;EEPROM addr for ID count variable
    
.equ    Rxdata_addr = 0         ;defines which register holding Received data0.
                                ;Data0-data7 must be placed in adjacent 
                                ;registers in ascending order.
.equ    Txdata_addr = 0         ;defines which register holding Transmit data0.
                                ;Data0-data7 must be placed in adjacent 
                                ;registers in ascending order.
;**********Register definitions                         

.def    RXTXdata0     =r0         ;data to be sent to the LIN master, byte0
.def    RXTXdata1     =r1         ;data to be sent to the LIN master, byte1
.def    RXTXdata2     =r2         ;data to be sent to the LIN master, byte2
.def    RXTXdata3     =r3         ;data to be sent to the LIN master, byte3
.def    RXTXdata4     =r4         ;data to be sent to the LIN master, byte4
.def    RXTXdata5     =r5         ;data to be sent to the LIN master, byte5
.def    RXTXdata6     =r6         ;data to be sent to the LIN master, byte6
.def    RXTXdata7     =r7         ;data to be sent to the LIN master, byte7  

.def    mode          =r8         ;mode1= get symch break, mode2= get synchfield, 
                                  ;mode 4= get identifier/send data.
.def    identifier    =r9         ;Received identifier. Also used for data to send   
                                                          
.def    SREG1         =r10        ;Temporarily storage for SREG (Ext int0 routine)
.def    SREG2         =r11        ;Temporarily storage for SREG (Timer0 ovf routine)

.def    temp          =r16        ;Scratchregister
.def    temp2         =r17        ;Scratchregister2
.def    bitcount      =r18        ;Bit counter, also used as general variable
.def    counter       =r19        ;MSB of bittime counter, also used as general
                                  ;variable
.def    status        =r20        ;status reg, Bit7 = bit error, bit 6 = parity error, 
                                  ;bit5 = chacksum error, bit0 = sleep. must be cleared in user program.
.def    bittime       =r21        ;Pulsewith for 1 bit 

; Free registers: r12,r13,r14,r15,r22,r23,r24,r25,r26,27,r28,r29,r31


;NB r30 is used as Pointer to the data block and must not be used by the user program.

;**************************************************************************
;*
;*  PROGRAM START - EXECUTION STARTS HERE
;*
;**************************************************************************
    .cseg

    .org    $0000
    rjmp    init                ;Reset handler

    .org    INT0addr
    rjmp    ext_int0            ;External interrupt handler

    .org    OVF0addr
    rjmp    tim0_ovf            ;Timer0 overflow handler

;**************************************************************************
;*
;*  EXT_INT0 - External Interrupt Routine 0
;*  This routine takes care off all communication with the master.
;*  Both synchronisation, and transmit/receive. This interruptroutine
;*  is called when detecting the "startbit" of the synch break, all falling 
;*  edges of the synchronization bits, and the startbit of the identifier.
;*  Which task to be performed is determioned by the mode-variable.
;*  Mode1 = get synch break, mode2=detect bitrate and mode4=get identifier
;*  and transmitt data.
;*
;**************************************************************************

ext_int0:
    in      SREG1,SREG          ;Store Status Register
    
    sbrs    mode,0              ;check if mode 1 (get synch break)
    rjmp    mode2

;********** mode 1 (get synch break)
   
    clr     bitcount                    
    mov     temp2,bittime
    lsr     temp2
    subi    temp2, 14           ;subtract 14 cycles (executed since interrupt)
                                ;and this instruction (1 cycle)
delay1:
    subi    temp2,3
    brcc    delay1
    
get_synch_break:
        
    sbic    PIND,RxD            ;check if input is low.
    rjmp    return              ;If not, return
    
    inc     bitcount            ;if low, increment bitcounter
    cpi     bitcount,11
    breq    new_mode            ;if 11 succeeding low bits, new mode
    
    mov     temp2,bittime
    subi    temp2, 9            ;subtract 9 cycles (executed between two samples)

delay2:
    subi    temp2,3
    brcc    delay2

    rjmp    get_synch_break
    
new_mode:
    lsl     mode                ;select next mode
    clr     bitcount            ;Prepare bitcount variable for next interrupt
    rjmp    return              ;return from interrupt
    
;********** END mode 1 (get synch break)

mode2:
    sbrs    mode,1              ;Check if mode 2
    rjmp    mode4               ;If not, mode4
    
;********** Mode 2 (get bitrate)

    
    cpi     bitcount, 0x00      ;Check if first edge of synch byte
    brne    next                ;If not, check if fifth edge
    
                                ;If first edge of synch byte...
    ldi     temp, 0b00000001    
    out     TCCR0,temp          ;start timer 0 counting on CK

    ldi     temp, 19            ;19 cycles since Ext 0 interrupt occurred
    out     TCNT0,temp          ;init timer0   
    
    clr     counter
    
    ser     temp
    out     TIFR,temp           ;clear timer ovf flag
    out     TIMSK,temp          ;enable timer ovf interrupt
    
next:
    cpi     bitcount, 0x04
    brne    next1               ;if not fifth edge, return
    
    in      bittime,TCNT0
    
    subi    bittime,21          ;21 cycles since int. occurred
    sbci    counter,0           ;Subtract high byte with carry. 
                                ;'Bittime' and Counter forms a 16
                                ;bit value containing the bittime
                                ;for 8 bits
    ldi     temp,3              
divide:
                                ;divide counter::bittime by 8 to
    clc                         ;get bittime for one bit
    ror     counter
    ror     bittime 
    dec     temp
    brne    divide

    brcc    correct_value       ;if carry cleared value is correct
    inc     bittime             ;if carry set, round value upwards

correct_value:

loop:
    clr     temp                        
    out     TIFR,temp           ;clear timer ovf flag
    out     TIMSK,temp          ;disable timer ovf interrupt
    out     TCCR0,temp          ;stop timer 0
       
    rjmp    new_mode            ;next mode (get identifier)
    
next1:


    inc     bitcount            ;wait for next edge
    rjmp    return
    
;********** END mode 2 (get bitrate)


;********** Mode 4 (get identifier)
mode4:

    ldi     temp, 26            ;Number to subtract in getchar routine to get correct timing
    rcall   getchar             ;Receive identifier

;********** Identifier received (residing in identifier register)

;********** Check parity P0

    mov     counter,identifier  ;copy identifier to unused register
    andi    counter, 0b01010111
    rcall   check_parity
    
    sbrc    bitcount,0          ;check if even parity
    rjmp    id_parity_error     ;if not, set error
    
;********** Check parity P1

    mov     counter,identifier  ;copy identifier to unused register
    andi    counter, 0b10111010
    rcall   check_parity
    
    sbrs    bitcount,0          ;check if odd parity
    rjmp    id_parity_error     ;if not, set error

;********** parity ok, find data response length

    mov     bitcount,identifier ;copy identifier to bitcount register
    swap    bitcount            ;Swap nibbles.
    andi    bitcount,0x03       ;mask all bits but ID5&ID4
    lsl     bitcount         
    brne    check_next
    ldi     bitcount,0x02       ;if bitcount=0x00,bitcount=0x02
check_next:
    cpi     bitcount,0x06
    brne    chk_sleep
    ldi     bitcount,0x08       ;if bitcount=0x06,bitcount=0x08 Bitcount now contains number of bits to read/send.

chk_sleep:                                    
;********** Check for sleep message

    mov     temp,identifier        ;copy identifier to unused register

    cpi     temp,0x80
    brne    IDcheck                 ;If ID=0x80

sleepmode:

    ori     status,0b00000001       ;Set sleepstatus
    
;********** Incorrect identifier 

incorrect_identifier:
    ldi     temp, 0x01          
    mov     mode,temp           ;When an incorrect identifier is detected 
                                ;mode is reset to 0x01
                                
;********** Return from Ext_int 0 routine
return:
    clr     temp
    out     SREG,SREG1          ;restore SREG. None of the following 
                                ;instructions affects the SREG.
    out     GIMSK,temp          ;following four instr. clears pending ext0 
                                ;interrupts.
    out     MCUCR,temp          ;since the EXTint0 flag is not
    ldi     temp,0b00000010     ;accessible by the user, the flag is
    out     MCUCR,temp          ;cleared by first selecting level trigged
    ser     temp                ;interrupt, and then reselecting edge
    out     GIMSK,temp          ;trigged interrupt.
    reti                        ;return

;********** Check for valid identifier
    
IDcheck:    
    ori     temp,0b11110000     ;mask 4 MSB of identifier (parity bits and datalength)
    
    ldi     temp2,IDcountaddr   ;Address for RxIDcount value
    out     EEAR,temp2
    
    sbi     EECR,EERE           ;Enable EEPROM read
    in      counter,EEDR        ;Get number of IDs in EEPROM

L1:
    in      temp2,EEAR
    inc     temp2
    out     EEAR,temp2

    sbi     EECR,EERE           ;Enable EEPROM read
    in      temp2,EEDR          ;Get first ID
    bst     temp2,7             ;store bit7(read/write)
    
    ori     temp2,0b10000000    ;mask MSB                                
    cp      temp2,temp
    breq    correct_identifier
    dec     counter             ;If not correct identifier, check next
    brne    L1                  ;If no more identifiers
        
    rjmp    incorrect_identifier ;If no matching identifiers found.
    
correct_identifier:

    bld     counter,0
    sbrc    counter,0           ;if MSB of identifier mask (EEPROM) was cleared --> send data
    rjmp    read_data           
    
;********** Create checksum

; The checksum can not be created while sending data since putchar
; makes use of the carry bit. Therefore the checksum must be 
; generated before transmitting data. the checksum resides in
; variable counter.

checksum:
    mov     temp2,bitcount      ;Copy number of bits to be transmitted
    clr     counter
    ldi     r30,Txdata_addr     ;load z reg with address of data 0
    clc                         ;delete carry
add_next:   
    ld      temp,Z              ;load data
    inc     r30
    adc     counter,temp        ;add to checksum
    dec     temp2
    brne    add_next            ;if not all data sent, continue
    com     counter             ;invert checksum

;********** transmit data

transmit:   
    ldi     r30,Txdata_addr       ;load z reg with address of data 1
transmit_next:  
    ld      identifier,Z           ;load data
    inc     r30
    rcall   putchar             ;send data
    dec     bitcount
    brne    transmit_next       ;if not all data sent, continue
    
transmit_chechksum:

    mov     identifier,counter 
    rcall   putchar

    ldi     temp, 0x01          ;select mode1 (get synch break)
    mov     mode,temp           
    rjmp    return
    
;********* indicate id parity error

id_parity_error:
    ori     status, 0b0100000        
    rjmp    incorrect_identifier ;return


;********* Read data from master

read_data:

    mov     temp2,bitcount      ;Copy number of bits to be transmitted

    ldi     r30,Rxdata_addr

read_next:
    sbic    PIND,RXD            ;Wait until start bit received
    rjmp    read_next
    
    ldi     temp, 17            ;Number to subtract in getchar routine to get correct timing
    rcall   getchar             ;Receive data
    st      Z,identifier        ;Store data
    inc     r30
    dec     bitcount            ;Check if all bytes has been received.  
    brne    read_next           ;If not, read next. 

read_chksum:
    sbic    PIND,RXD            ;Wait until start bit received
    rjmp    read_chksum
    
    ldi     temp, 17            ;Number to subtract in getchar routine to get correct timing
    rcall   getchar             ;Receive data

; Checksum now resides in reg 'identifier'    
    
    clr     counter
    ldi     r30,Rxdata_addr     ;load z reg with address of data 0
    clc                         ;delete carry
add_next_:   
    ld      bitcount,Z          ;load data
    inc     r30
    adc     counter,bitcount    ;add to checksum
    dec     temp2
    brne    add_next_           ;if not all data added
    com     counter 
    
    add     counter,temp
    cpi     counter,0xff
    brne    wrong_checksum
    rjmp    incorrect_identifier ;return
    
wrong_checksum:

    ori     status, 0b00100000
    rjmp    incorrect_identifier ;return
    
        

;********************* END mode 4 (get identifier) ****************************     

;**************************************************************************
;*                          check parity
;*
;*  This routine runs trough the counter variable and counts the number of ones
;*  This number is returned in the variable 'counter'. If the LSB is 1
;*  the number of ones is odd. If LSB is 0, the number is even. This is used 
;*  to check for correct parity. 
;*
;**************************************************************************
check_parity:
    clr     bitcount
new:
    sbrc    counter,0           ;check if LSB=1
    inc     bitcount            ;if so, increment counter
    lsr     counter
    brne    new                 ;if bitcount >0 find next

    ret

;**************************************************************************
;* TIM0_OVF - Timer/Counter 0 Overflow Interrupt
;**************************************************************************
tim0_ovf:   
    
    in  SREG2,SREG              ;SREG must be stored since inc affects SREG
    inc counter
    out SREG,SREG2
    reti

;***************************************************************************
;*
;* "putchar"
;*
;* This subroutine transmits the byte stored in the "identifier" register
;*
;*
;***************************************************************************

putchar:    
    ldi     temp,10             ;number of bits to send (1stb+8+1stpb) 
    com     identifier          ;Invert everything
    sec                         ;Start bit

putchar0:
    brcc    putchar1            ;If carry cleared, branch. Else..
    nop                         ;nop to equalize the executiontime for sending a 1 or a 0.
    cbi     PORTD,TxD           ;send a '0'
    sbic    PIND,TxD            ;check if pin value is the same as sendt
    ori     status, 0b10000000  ;indicate bit error
    rjmp    putchar2            ;else   

putchar1:
    sbi     PORTD,TxD           ;send a '1' 
    sbis    PIND,TxD            ;check if pin value is the same as sendt
    ori     status, 0b10000000  ;indicate bit error
    nop                         ;nop to equalize the executiontime  for sending a 1 or a 0.
    nop
    
putchar2:
    
    mov     temp2, bittime              
    subi    temp2, 16           ;bittime - 13 cycles executed between two bits

delay3:
    subi    temp2,3
    brcc    delay3

    lsr     identifier          ;Get next bit
    dec     temp                ;If not all bit sent
    brne    putchar0            ;send next
                                ;else
    ret                         ;return

;***************************************************************************
;*
;* "getchar"
;*
;* This subroutine receives a byte
;*
;* Received character resides in identifier register
;***************************************************************************

getchar:

    mov     temp2, bittime      ;wait bittime
    sub     temp2, temp         ;subtract walue to give correct timing

delay4:
    subi    temp2,3
    brcc    delay4

    mov     temp2, bittime      ;wait bittime/2
    lsr     temp2                           
    subi    temp2, (3+1)        ;subtract 3 cycles (executed since last delay)
                                ;plus 1 cycle executed after the delay, before line
                                ;sampling (the clc instruction)
delay5:
    subi    temp2,3
    brcc    delay5

    ldi     temp,9              ;8 data bit + 1 stop bit


getchar2:

    clc                         ;clear carry
    sbi     PORTD,0
    sbic    PIND,RxD            ;if RX pin high
    sec             
    cbi     PORTD,0
    dec     temp                ;If bit is stop bit
    breq    continue            ;proceed
    ror     identifier          ;else shift bit into identifier

    mov     temp2, bittime      ;wait bittime
    subi    temp2, 16           ;subtract 16 cycles (executed between two samples)

delay6:
    subi    temp2,3
    brcc    delay6
    
    rjmp    getchar2            ;go get next

continue:
    ret

;**************************************************************************
;*
;*  Wake up
;*
;*  This routine may be called to wake up the bus master.
;*
;*
;**************************************************************************    
 wakeup:
 
    ldi     temp,0x80
    mov     identifier,temp
    rcall   putchar
    ret
    
;**************************************************************************
;*
;* start
;**************************************************************************



init: 

    clr     status              ;clear status byte. this byte should also be 
                                ;cleared by the user when it is read to 
                                ;delete all error flags
    ldi     bittime,0x10

    ser     temp
    out     GIMSK,temp          ;enable ext interrupt

    ldi     temp, 0x01          ;select mode1 (get synch break)
    mov     mode,temp

    ldi     temp,0b00000010     ;set ext int 0 on falling edge
    out     MCUCR,temp
    
    ldi     temp,0b00000001 | (1<<TxD)
    out     DDRD,temp           ;Set pinD0 (debug) and TxD as outputs
    
    sbi     PORTD,TxD           ;High level out on TxD pin
    sei                         ;set global interrupt enable flag

main:
    
;   Here the users application code must be placed.
    
    rjmp    main                ;Eternal loop

