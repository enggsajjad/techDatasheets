
===================================================

  LCD Clock

  LCD Clock with Atmel AVR Microcontroller
  and Character LCD-Display (HD44780) using
  Big Numbers
  
  by Martin Thomas, Kaiserslautern, Germany
  mthomas@rhrk.uni-kl.de 
  eversmith@heizung-thomas.de
  www.siwawi.arubi.uni-kl.de/avr_projects

===================================================

12. Oct. 2008

- minor updates, no new functions

2. June 2005

lcd.c update to be compatible with avr-libc 1.2.3
- from Peter Fleury's library
Tested with WinAVR 2/2005 (avr-gcc 3.4.3/avr-libc 1.2.3)

7. Sept. 2004

Yet another AVR-based clock. This clock shows the
time in large numbers using user-defined characters
on a four-line character-LCD (HD44780 and compatible).

In this application the internal R/C-oscillator is
used as the controller's main clock-source. The
real-time-clock is driven in async mode using an
external 32kHz watch-crystal.

This Code is for avr-gcc 3.4.1 with avr-libc 1.0.4
(WinAVR July04-Edition).

Hardware for this example:
- Atmel AVR ATmega16 @ 1MHz internal R/C (factory default)
- 32kHz Watch-Crystal connected to the ATmega's TOSC1/TOSC2 pins
- Text-Mode LCD-display. In the example a Displaytech 164A
  (16*4) display has been used (KS0070B). For connection
  with an Atmel STK500 see connect.txt.
- Two buttons (active-low) at PORTB pin 0 and 1 to set the clock

To adopt this code to own developments:
- Change XTAL in lcd.h, the delay-loop in rtcasync.c and 
  the timer1 setup in main.c for main frequency other than 1MHz.
- Change the key-#defines in main.c for other button-
  assignments.
- Change lcd.h to reflect LCD-connection to AVR.
- The LCD line-to-DRAM-Addresses in lcd.h are set for
  a Displaytech 164A display with a KS0070B controller.
  These values may have to be changed for other controllers
  to reflect the values given in the controller's datasheet.
  (LCD_START_LINEx in lcd.h)
- Undefine "SPASS" in main.c to disable the startup-message.
- See the errata in the ATmega8 datasheet if you'd like
  to use an ATmega8.


Credits:
- to Peter Fleury for his text-mode LCD-library
- to the one who has put the CGRAM-patters in
  the www.mikrocontroller.net/forum

