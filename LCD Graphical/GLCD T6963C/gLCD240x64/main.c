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

#include <stdio.h>
#include "c8051f020.h"                 	// SFR declarations
#include "config.h"
#include "lcd240x64.h"					// definitions for the LCD controller
#include "fonts.h"						// definitions for the fonts

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
		lcd_clear_area(SCRN_LEFT,SCRN_TOP,SCRN_RIGHT,SCRN_BOTTOM);


		lcd_box(SCRN_LEFT,SCRN_TOP,SCRN_RIGHT,SCRN_BOTTOM);

		lcd_box(SCRN_LEFT+2,SCRN_TOP+2,SCRN_RIGHT-2,SCRN_BOTTOM-2);

		lcd_text(21,4,FONT_NINE_DOT,"Carousel Design Solutions");

		lcd_text(6,28,FONT_TEN_DOT,"240x64");
		lcd_text(62,31,FONT_SEVEN_DOT,"Supporting Toshiba T6963C");

		lcd_text(34,52,FONT_SIX_DOT,"visit web site: http://www.carouseldesign.com");

		lcd_update(SCRN_TOP,SCRN_BOTTOM);

		while(SWT == 1);
		while(SWT == 0);


		lcd_invert_area(SCRN_LEFT+3,SCRN_TOP+3,SCRN_RIGHT-3,SCRN_BOTTOM-3);
		lcd_update(SCRN_TOP,SCRN_BOTTOM);

		while(SWT == 1);
		while(SWT == 0);

	}
}

