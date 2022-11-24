/*-------------------------------------------------------------------
 *
 * File:	demo.c 		(uses grlcd.c)
 *
 * Purpose:	Little demo program for 240 x 64 graphics LCD such
 *		as the Optrex DMF 5005 or similar.
 *
 * Description:	Depending on compile options, this can be built
 *		in three different ways:
 *
 *		(1) demof.exe - No special hardware required, 
 *			run on PC, put results in image files.
 *
 *		(2) demo.exe - Run on PC, connect graphics LCD to 
 *			printer port.
 *
 *		(3) demo.hex - Run on PIC microprocessor, display on LCD.
 *
 *-------------------------------------------------------------------*/

#if HI_TECH_C

#include <pic.h>

/*------------------------------------------------------------------------
 *
 * Certain options must be selected when programming the PIC.
 * 
 * Select
 *      code protect off,
 *      power up timer on,
 *      watch dog timer off,
 *      XT oscillator.
 *
 * Compiler manual mentions symbols like FOSC1 but I don't see 
 * them anywhere.
 *
 * It should be possible for the compiler to put the required
 * options in the file for the programmer but I haven't been able
 * to figure this out yet.
 *
 *
 * The configuration word is different for the C and F versions:
 *
 * 16C84:
 *
 *      FOSC0   0x01            1 = XT osc.
 *      FOSC1   0x02            0 = XT osc.
 *      WDTE    0x04            0 = Watch dog timer off.
 *      PWRTE   0x08            1 = Power on timer enable. 
 *      CP      0x10            1 = Code Protection off. 
 *
 * 16F84:
 *
 *      FOSC0   0x0001          1 = XT osc.
 *      FOSC1   0x0002          0 = XT osc.
 *      WDTE    0x0004          0 = Watch dog timer off.
 *      PWRTE   0x0008          0 = Power on timer enable. (opposite)
 *      CP	    0x3ff0          all 1 = Code Protection off. 
 *
 *-----------------------------------------------------------------------*/

/* 0b11001 produces "digit out of range" error. */
/* Use the assember version with trailing upper case B. */


#if _16C84
__CONFIG(11001B);   
#error Not enough RAM, use 'F84                    
#elif _16F84
__CONFIG(0011111111110001B);
#else
#error Unexpected device type.
#endif

#else				/* Run on PC. */

#include <stdio.h>

#endif


#include "grlcd.h"
#include "line.h"
#include "rect.h"
#include "circle.h"
#include "text.h"

void draw_demo (signed char frame);


#if GD				/* Write to file. */
#include "gd.h"

static gdImagePtr im;
static int background, foreground;

/* Use instead of version in grlcd.c. */

void set_pixel (coord_t x, coord_t y)
{
	int value = 1;

	gdImageSetPixel (im, x, y, value ? foreground : background);
}

#endif



/*-------------------------------------------------------------------
 *
 * Name:	x
 *
 * Purpose:	x
 *
 * Inputs:	x
 *
 * Outputs:	x
 *
 * Description:	x
 *
 *-------------------------------------------------------------------*/


#if HI_TECH_C

void main ()
{
	signed char frame = -1;


 	OPTION = 0;		/* Enable weak pullups. */

#else

int main (int argc, char *argv[])
{

/*
 * When run on a PC, the optional command line argument contains
 * the frame number of the animation (0 - 31) or -1 to indicate looping.
 */
	int frame = -1;

	if (argc > 1) {
	    frame = atoi(argv[1]);
	    if (frame > 31)
		frame = 31;
	}

#endif

#if GD		/* PC, file output. */

        im = gdImageCreate (GRLCD_WIDTH, GRLCD_HEIGHT);

        background = gdImageColorAllocate (im, 255, 255, 255);
        foreground = gdImageColorAllocate (im, 0, 0, 255);

	if (frame < 0) {
	    for (frame=0; frame<32; frame++) {
		draw_demo (frame);
	    }
	}
	else {
	    draw_demo (frame);
	}

      	gdImageDestroy (im);

#else		/* Using LCD. */

	grlcd_open ();
	
	if (frame < 0) {
	    while (1) {
	        for (frame=0; frame <= 31; frame++) {

		    grlcd_set_buffer (BUFFER_FLIP);
		    grlcd_clear ();
		    draw_demo (frame);
                }
	        for (frame=31; frame >= 0; frame--) {

		    grlcd_set_buffer (BUFFER_FLIP);
		    grlcd_clear ();
		    draw_demo (frame);
	        }
            }
	}
	else {
	    draw_demo (frame);
	}

	grlcd_close ();

#endif   
      	//exit (0);
}

/*-------------------------------------------------------------------
 *
 * Name:	draw_quadrant
 *
 * Purpose:	Draw a quarter of a circle.
 *
 * Inputs:	cx, cy	- Center.
 *		r	- Radius.
 *		q	- Quadrants numbered as:
 *                              2   3
 *                              1   0
 *
 * Outputs:	Calls set_pixel which draws to file or actual LCD.
 *
 * Description:	Usual Bresenham's circle algorithm, modified to 
 *		draw only a quarter of a circle.
 *
 *-------------------------------------------------------------------*/


void draw_quadrant (coord_t cx, coord_t cy, coord_t r, byte_t q)
{
        coord_t x = 0;
	coord_t y = r;
	short p = 1 - r;

	do {
	    if (q == 0) set_pixel (cx+x, cy+y);
	    if (q == 3) set_pixel (cx+y, cy-x);
	    if (q == 2) set_pixel (cx-x, cy-y);
	    if (q == 1) set_pixel (cx-y, cy+x);

	    if (x >= y)
		return;
	    x++;
	    if (p >= 0) {
		y--;
		p -= 2 * y;
	    }
	    p += 2 * x + 1;
	    
	    if (q == 0) set_pixel (cx+y, cy+x);
	    if (q == 3) set_pixel (cx+x, cy-y);
	    if (q == 2) set_pixel (cx-y, cy-x);
	    if (q == 1) set_pixel (cx-x, cy+y);
	} while (x < y);
}


/*-------------------------------------------------------------------
 *
 * Name:	draw_demo
 *
 * Purpose:	Draw one frame of the animation.
 *
 * Inputs:	frame	- Controls size of the spirals.
 *
 * Outputs:	None.
 *
 * Description:	
 *
 *-------------------------------------------------------------------*/

void draw_demo (signed char frame) 
{
	signed char r;
	byte_t q;
#if GD
	char name[20];
#endif

	
#define LCX 32			/* Left spiral center. */
#define LCY 32

#define RCX (239-32)		/* Right spiral center. */
#define RCY 32

	coord_t ltopy, rtopy;	/* The tops. */
#if GD 
	FILE *fp;

        gdImageFillRect (im, 0, 0, GRLCD_WIDTH, GRLCD_HEIGHT, background);
#endif
#ifndef HI_TECH_C
	printf ("draw_demo (%d)\n", frame);
#endif
	ltopy = frame;
	rtopy = 31 - frame;

/* 
 * Draw left spiral.
 */
	r = LCY - ltopy;
	q = 2;
	while (r > 0) {
	    draw_quadrant (LCX, LCY, r, q);
	    r--;
	    q = (q - 1) & 3;
	}

/* 
 * Draw right spiral.
 */
	r = RCY - rtopy;
	q = 3;
	while (r > 0) {
	    draw_quadrant (RCX, RCY, r, q);
	    r--;
	    q = (q + 1) & 3;
	}

/*
 * Connect the tops with a line.
 */
	draw_line (LCX, ltopy, RCX, rtopy);

/*
 * Throw in some text.
 */

#ifndef HI_TECH_C 

 		/* SORRY, OUT OF MEMORY. */
 		 		
	draw_string (86+frame, 38, "It's");
	draw_string (84, 50, "HI-TECH C");
	draw_string (109-frame, 62, "for me!");
#endif
	
#ifndef GD
	/* grlcd_string (0, 0, "The quick brown fox"); */
	/* grlcd_string (6, 7, "jumps over the lazy dog."); */
#endif



/*
 * All done.  Write the file.
 */
#if GD
	sprintf (name, "demo%02d.gif", frame);

	fp = fopen (name, "wb");

        gdImageGif (im, fp);
	fclose (fp);
#endif

} /* end draw_demo */

/* end demo.c */
