//-----------------------------------------------------------------------------
// main.c
//-----------------------------------------------------------------------------
// Copyright 2003 Carousel Design Solutions
//
// AUTH: Michael Karas
// DATE: 09 May 2003
//
// Target: C8051F020
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//

#pragma CODE


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <c8051f020.h>                 	// SFR declarations
#include <config.h>
#include <lcd106x56.h>					// definitions for the LCD controller

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

sbit SWT = P3^7;					/* eval board switch */

void main (void) 
{
	config();								// setup all the SFR initialization

	lcd_init();								// initialize LCD display

	lcd_erase();							// erase the display 

	
	while(1)
	{
		// switch over to the buffered interface
		lcd_clear_area(SCRN_LEFT,SCRN_TOP,SCRN_RIGHT,SCRN_BOTTOM);

		lcd_box(SCRN_LEFT,SCRN_TOP,SCRN_RIGHT,SCRN_BOTTOM);
		lcd_box(SCRN_LEFT+2,SCRN_TOP+2,SCRN_RIGHT-2,SCRN_BOTTOM-2);

		lcd_text(6,3,1,"Michael Karas");

		lcd_text(11,15,2,"1234");

		lcd_text(5,40,0,"carouseldesign.com");

		lcd_update(SCRN_TOP,SCRN_BOTTOM);

		while(SWT == 1);
		while(SWT == 0);

		lcd_invert_area(SCRN_LEFT+2,SCRN_TOP+2,SCRN_RIGHT-2,SCRN_BOTTOM-2);

		lcd_update(SCRN_TOP,SCRN_BOTTOM);

		while(SWT == 1);
		while(SWT == 0);
	}
}

