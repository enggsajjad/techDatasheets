/*--------------------------------------------------------------------
 *
 * File:        grlcd.c
 *
 * Purpose:     Driver for graphics LCD such as Optrex DMF 5005.
 *
 * Description: This is written so it can be run either on a PIC
 *              microprocessor or a PC with the LCD connected to
 *              the printer port for testing.
 *
 *              See accompanying document for more details.
 *
 * Revisions:   Original, Feb 2002.
 *
 *--------------------------------------------------------------------*/



#include "grlcd.h"


#if HI_TECH_C

#include <pic.h>

#else

#include <stdio.h>
#include <windows.h>
#include "pt_ioctl.c"

#endif

/*
 * Internal functions which should not be accessible from outside.
 */

static void grlcd_write_command (byte_t cmd);
static void grlcd_write_data (byte_t dat);
static void grlcd_write_data_auto_mode (byte_t dat);
// static byte_t grlcd_read_data (void);


/*
 * Most, but not all Toshiba T6963C function codes.
 */

                                                /* 2 preceding data. */

#define F_SET_TEXT_HOME_ADDRESS         0x40    /* low, high */
#define F_SET_TEXT_AREA                 0x41    /* columns, 0 */
#define F_SET_GRAPHIC_HOME_ADDRESS      0x42    /* low, high */
#define F_SET_GRAPHIC_AREA              0x43    /* columns, 0 */                             

#define F_SET_CURSOR_POINTER            0x21    /* x, y */
#define F_SET_OFFSET_REGISTER           0x22    /* data, 0 */
#define F_SET_ADDRESS_POINTER           0x24    /* low, high */

#define F_OR_MODE                       0x80
#define F_EXOR_MODE                     0x81
#define F_AND_MODE                      0x83
#define F_TEXT_ATTRIBUTE_MODE           0x08    /* OR to 0x8? above */

#define F_DISPLAY_OFF                   0x90  
#define F_CURSOR_ON                     0x92    /* Combine one or more */
#define F_BLINK_ON                      0x93
#define F_TEXT_ON                       0x94
#define F_GRAPHIC_ON                    0x98

#define F_ONE_LINE_CURSOR               0xA0
#define F_EIGHT_LINE_CURSOR             0xA7

#define F_SET_DATA_AUTO_WRITE           0xB0
#define F_SET_DATA_AUTO_READ            0xB1
#define F_AUTO_RESET                    0xB2

#define F_DATA_WRITE_AND_INCREMENT_ADP  0xC0    /* 1 preceding data. */
#define F_DATA_READ_AND_INCREMENT_ADP   0xC1
#define F_DATA_WRITE                    0xC4
#define F_DATA_READ                     0xC5

#define F_SCREEN_PEEK                   0xE0
#define F_SCREEN_COPY                   0xE8

#define F_BIT_RESET                     0xF0    /* + bit number */
#define F_BIT_SET                       0xF8


/*
 * Memory layout.
 */

static unsigned short graphics_base;	/* Starting address for graphics */
					/* memory. */
					/* This is a variable so we can */
					/* use double buffering. */
#define GRAPHICS_BASE_A  	0
#define GRAPHICS_BASE_B		(3*1024)

static unsigned short text_base;

#define TEXT_BASE_A		(GRAPHICS_BASE_A + 2*1024)
#define TEXT_BASE_B		(GRAPHICS_BASE_B + 2*1024)

#define FONT_BASE		(6*1026)


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_open
 *
 * Purpose:     Initialize the graphics LCD.
 *
 * Description: 
 *
 *--------------------------------------------------------------------*/

void grlcd_open ()
{
#if HI_TECH_C

	TRISA = 0;		/* Port A - all outputs. */
				/* Port B - set as required. */
	PORTA = 0xF;
				
	/* Turn on Vee after delay? */
#else
	OpenPortTalk();
#endif

#if DEBUG
	printf ("grlcd_open ()\n");
#endif

 	grlcd_write_command (F_DISPLAY_OFF); 

	grlcd_set_buffer (BUFFER_A);

 	grlcd_write_data ((byte_t)(FONT_BASE >> 11));
 	grlcd_write_data (0);
 	grlcd_write_command (F_SET_OFFSET_REGISTER);  

/*
 * We only need 30 bytes per row but 32 allows more efficient
 * address calculations.
 */
 	grlcd_write_data (GRLCD_BYTES_PER_ROW);
 	grlcd_write_data (0);
 	grlcd_write_command (F_SET_GRAPHIC_AREA);

	grlcd_write_data (GRLCD_BYTES_PER_ROW);
 	grlcd_write_data (0);
 	grlcd_write_command(F_SET_TEXT_AREA);

 	grlcd_write_command(F_OR_MODE);  /* Display graphics OR text. */

 	grlcd_write_command(F_ONE_LINE_CURSOR);

 	grlcd_write_data (0);
 	grlcd_write_data (0);
 	grlcd_write_command (F_SET_CURSOR_POINTER);  

	grlcd_clear ();

 	grlcd_write_command (F_TEXT_ON | F_GRAPHIC_ON); 
}


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_close
 *
 * Purpose:     Shutdown the graphics LCD.
 *
 * Description: ???
 *
 *--------------------------------------------------------------------*/


void grlcd_close ()
{
#if HI_TECH_C
#else
	ClosePortTalk();
#endif

}



/*--------------------------------------------------------------------
 *
 * Name:        set_pixel
 *
 * Purpose:     Set a single pixel on the display.
 *
 * Inputs:      x       - Range: 0 ... GRLCD_WIDTH-1, 0=left
 *              y       - Range: 0 ... GRLCD_HEIGHT-1, 0=top
 *
 * Returns:     None.
 *
 * Description: This is called a lot, and the original generated
 *		a shocking amount of code, so I've tried to 
 *		trim it down a bit.
 *
 *              There are 3 different versions.
 *              (1) Original, general case.
 *              (2) Better, based on certain assumptions.
 *              (3) Even better, but with PIC dependencies.
 *
 *              In general, I've been quite impressed with the
 *              code generated by HI-TECH C but this the results
 *              of this function have been very disappointing.
 *
 * Assumptions: x and y are within appropriate ranges otherwise
 *              strange things could appear.
 *
 *--------------------------------------------------------------------*/
 

#if 0


/*--------------------------------------------------------------------
 *
 * Original version.
 *
 * General case, written in the obvious way.
 *
 * The amount of code generated, for such an innocent looking
 * function, is shocking.
 *
 *--------------------------------------------------------------------*/

void set_pixel (coord_t x, coord_t y)
{
        unsigned short addr;


        addr = y * GRLCD_BYTES_PER_ROW + x / 8 + graphics_base;
        grlcd_write_data ((byte_t) addr);      
        grlcd_write_data ((byte_t) (addr >> 8));

        grlcd_write_command (F_SET_ADDRESS_POINTER);
        grlcd_write_command (F_BIT_SET | ( ~x & 7 ));
}

#endif


/*--------------------------------------------------------------------
 *
 * Second version.
 *
 * We have several assumptions here:
 *	- Bytes per row is a power of two so we can shift
 *		and mask instead of a general purpose multiply.
 *	- The graphics memory base address is a multiple of
 *		256 so we don't have to worry about a carry
 *		between bytes.  Therefore we can calculate two
 *		independent 8 bit values instead of a 16 bit value.
 *	- Memory addressing is "little-endian".  This is the case
 *		for the PIC and Intel/IBM PC, but could be the 
 *		opposite for other architectures.
 *
 * Better, but code is still much larger and slower than necessary.
 *
 *--------------------------------------------------------------------*/

#if (GRLCD_BYTES_PER_ROW == 32) && \
		((GRAPHICS_BASE_A & 0xff) == 0) && \
                ((GRAPHICS_BASE_B & 0xff) == 0) && ! HI_TECH_C

void set_pixel (coord_t x, coord_t y)
{

/* 
 * The next statement generates 20 instructions. 
 * With looping, it takes about 35 instruction cycle times.
 *
 * Some of the code generated seems rather silly.
 * For example, the shift-right-by-3 produces:
 *		
 *		movwf	btemp
 *		bcf	3,0
 *		rrf	btemp
 *		bcf	3,0
 *		rrf	btemp
 *		bcf	3,0
 *		rrf	btemp,w
 *
 * Why clear the Carry flag each time?  Just mask the result:
 *
 *		rrf	btemp
 *		rrf	btemp
 *              rrf     btemp,w
 *              andlw   0x1f
 *
 * The shift-left-by-5 is even worse.  Looping adds two more
 * instruction times to each position shifted.  Compiler produces:
 *
 *		movwf	btemp+1
 *		movlw	5
 *		movwf	btemp+2
 * u15
 *		bcf	3,0
 *		rlf	btemp+1
 *		decfsz	btemp+2
 *		goto	u15
 *		movf	btemp+1,w
 *
 * Clearing carry every time around is so wasteful!  Why not swap the 
 * nybbles, rotate once, and mask to retain the 3 useful bits? 
 *
 *		swapf	btemp+1
 *		rlf	btemp+1
 *		movf	btemp+1,w
 *		andlw	0xe0
 *		
 * For shift-a-byte-left-by-5, the original 23 instruction times 
 * can easily be reduced to 4.
 */

           grlcd_write_data ((y << 5) | (x >> 3));

/*
 * What's going on here?  What happened to "graphics_base>>8"?  
 *
 * Although the function call context requires only a single
 * byte for an argument, the compiler generates a 16 bit value
 * and ends up not using the high order half.
 *
 * We can use this form to simply load the high order byte
 * and save some time and memory.
 *
 * This is no longer portable because it depends "little-endian"
 * data storage order.  This is appropriate for the Intel/IBM PC
 * and the HI-TECH PIC C compiler but some other architectures
 * have the opposite order.
 */
        grlcd_write_data (*(((byte_t *)(&graphics_base))+1) + (y >> 3));
        grlcd_write_command (F_SET_ADDRESS_POINTER);

        grlcd_write_command (F_BIT_SET | ( ~x & 7 ));
}

#endif


/*--------------------------------------------------------------------
 *
 * Third version.
 *
 * Using the techniques described above, we can easily make
 * this function much smaller and faster.
 * 
 * (Approximate; I could be a little off.)
 *
 *			Size	Time
 *			----	----
 *	First		48	67
 *	Second		38	53
 *	Third		30	30
 *
 * This is nothing far-fetched.  A decent peephole optimizer
 * could be expected to do something like this.
 *
 * Normally, I think that using assembly language is a bad idea
 * because it is very time-consuming, error-prone, and unmaintainable.
 * However, in a case like this, it is hard to resist when a hot spot
 * can be made so much smaller and faster.
 *
 *--------------------------------------------------------------------*/

#if (GRLCD_BYTES_PER_ROW == 32) && \
		((GRAPHICS_BASE_A & 0xff) == 0) && \
                ((GRAPHICS_BASE_B & 0xff) == 0) && HI_TECH_C

void set_pixel (coord_t x, coord_t y)
{
	byte_t hi, lo;

/*
 *	lo = (y << 5) | (x >> 3);
 */
#asm
	movwf	btemp			; assume first arg, x, is in w.
	rrf	btemp
	rrf	btemp
	rrf	btemp,w
	andlw	0x1f				; x >> 3
	movwf	(?a_set_pixel+1)& (0+127) 	; partial result to lo.
;
	movf	?_set_pixel& (0+127),w		; get y.
	movwf	btemp
	swapf	btemp				; instead of 4 rlf
	rlf	btemp,w
	andlw	0xe0				; y << 5
;
	iorwf	(?a_set_pixel+1)& (0+127)	; result in lo      
#endasm
	grlcd_write_data (lo);

/*
 *	hi = *(((byte_t *)(&graphics_base))+1) + (y >> 3);
 */
#asm
	; We don't need y again so shift in-place instead of copying 
	; to btemp.  It would not be reasonable to expect a compiler 
	; to do something bizarre like this.
	; On the other hand, a compiler should know that hi and lo
	; are only used as function arguments so the results could
	; just be left in w rather than storing them in RAM.

	rrf	?_set_pixel& (0+127)		; y
	rrf	?_set_pixel& (0+127)
	rrf	?_set_pixel& (0+127),w
	andlw	0x1f				; y >> 3
	addwf	((_graphics_base& (0+255)+1))& (0+127),w ; graphics_base >> 8
	movwf	?a_set_pixel& (0+127)        	; result in hi
#endasm
	grlcd_write_data (hi);

        grlcd_write_command (F_SET_ADDRESS_POINTER);

        grlcd_write_command (F_BIT_SET | ( ~x & 7 ));
}

#endif





/*--------------------------------------------------------------------
 *
 * Name:        grlcd_raster
 *
 * Purpose:     Set a group of 8 pixels on the screen.
 *
 * Inputs:      x       - Range: 0 ... GRLCD_WIDTH-1, 0=left
 *				Must be a multiple of 8.
 *              y       - Range: 0 ... GRLCD_HEIGHT-1, 0=top
 *		d	- 8 pixels, LSB to right.
 *
 * Returns:     None.
 *
 * Description: Sets 8 pixels in one operation.  This is handy
 *		for speeding up draws of images and horizontal lines.
 *
 * Assumptions: x and y are within appropriate ranges otherwise
 *              strange things could appear.
 *		x should be a multiple of 8.  The low order 3 bits
 *		are discarded.
 *
 * Speed Up:	This could benefit from the same techniques described
 *		in set_pixel.
 *
 *--------------------------------------------------------------------*/

void grlcd_raster (coord_t x, coord_t y, byte_t d)
{

#if DEBUG
	printf ("set_pixel (%d, %d, %d)\n", x, y, d);
#endif


#if (GRLCD_BYTES_PER_ROW == 32) && \
		((GRAPHICS_BASE_A & 0xff) == 0) && \
		((GRAPHICS_BASE_B & 0xff) == 0)

        grlcd_write_data ((y << 5) | (x >> 3));
        grlcd_write_data ((graphics_base >> 8) + (y >> 3));

#else

/* 
 * More general case.
 */
        unsigned short addr;

        addr = y * GRLCD_BYTES_PER_ROW + x / 8 + graphics_base;

        grlcd_write_data ((byte_t) addr);
        grlcd_write_data ((byte_t) (addr >> 8));

#endif
        grlcd_write_command (F_SET_ADDRESS_POINTER);

        grlcd_write_data (d);
        grlcd_write_command (F_DATA_WRITE);
}


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_char
 *		grlcd_string
 *
 * Purpose:     Put a character or string on screen using built-in
 *		character generator.
 *
 * Inputs:      x       - 0 (left) to GRLCD_WIDTH/8-1.
 *              y       - 0 (top) to GRLCD_HEIGHT/8-1.
 *              ch	- Single character.
 *		str	- Character string.
 *
 * Returns:     None.
 *
 * Description: Note that coordinates are in character units, not pixels.
 *
 *		For the DMF5005 (240 x 64 pixels) we have 30 x 8 characters.
 *
 * Assumptions: x and y are within appropriate ranges otherwise
 *              strange things could appear.
 *
 *--------------------------------------------------------------------*/

void grlcd_char (byte_t x, byte_t y, byte_t ch)
{
        unsigned short addr;

#if DEBUG
	printf ("grlcd_char (%d, %d, '%c')\n", x, y, ch);
#endif

        addr = y * GRLCD_BYTES_PER_ROW + x + text_base;

        grlcd_write_data ((byte_t) addr);
        grlcd_write_data ((byte_t) (addr >> 8));
        grlcd_write_command (F_SET_ADDRESS_POINTER);

        grlcd_write_data (ch - ' ');
        grlcd_write_command (F_DATA_WRITE);
}


void grlcd_string (byte_t x, byte_t y, byte_t *str)
{
	while (*str) {
	    grlcd_char (x, y, *str);
	    x++;
	    str++;
	}
}


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_set_buffer
 *
 * Purpose:     The display module has plenty of memory so we take
 *		advantage of it with double buffering to reduce flicker
 *		while making updates.
 *
 * Inputs:      draw_to - one of the following codes:
 *
 *			BUFFER_A - Draw to and display first buffer.
 *			BUFFER_B - Draw to and display second buffer.
 *			BUFFER_FLIP - Display the current draw buffer.
 *				Draw to the opposite one.
 *
 * Description: 
 *
 *--------------------------------------------------------------------*/

void grlcd_set_buffer (enum buff_t draw_to)
{
	if (draw_to == BUFFER_A) {
	    graphics_base = GRAPHICS_BASE_A;
	    text_base = TEXT_BASE_A;
	}
	else if (draw_to == BUFFER_B) {
	    graphics_base = GRAPHICS_BASE_B;
	    text_base = TEXT_BASE_B;
	}

 	grlcd_write_data (graphics_base & 0xff);
 	grlcd_write_data (graphics_base >> 8);
 	grlcd_write_command (F_SET_GRAPHIC_HOME_ADDRESS); 

 	grlcd_write_data (text_base & 0xff);
 	grlcd_write_data (text_base >> 8);
 	grlcd_write_command (F_SET_TEXT_HOME_ADDRESS);  

	if (draw_to == BUFFER_FLIP) {
	    if (graphics_base == GRAPHICS_BASE_A) {
		graphics_base = GRAPHICS_BASE_B;
		text_base = TEXT_BASE_B;
	    }
	    else {
		graphics_base = GRAPHICS_BASE_A;
		text_base = TEXT_BASE_A;
	    }
	}
}



/*--------------------------------------------------------------------
 *
 * Name:        grlcd_clear
 *
 * Purpose:     Clear the current draw buffer.
 *
 * Inputs:      ??
 *
 * Returns:     ??
 *
 * Description: It's rather odd that the "blank" character is binary
 *		value 0 but it turns out to be handy in this case.
 *
 *		Use the special auto write mode so we don't have to
 *		set a different address each time.
 *
 *--------------------------------------------------------------------*/

void grlcd_clear ()
{
	unsigned short n;


	n = 2 * 1024 + 256;	/* Assumes 240 x 64. */
				/* Should derive this from size constants. */

        grlcd_write_data ((byte_t) graphics_base);
        grlcd_write_data ((byte_t) (graphics_base >> 8));
        grlcd_write_command (F_SET_ADDRESS_POINTER);
        
	grlcd_write_command (F_SET_DATA_AUTO_WRITE);
	
	do {
            grlcd_write_data_auto_mode (0);
 
	} while (--n);

        grlcd_write_command (F_AUTO_RESET);
}



/*--------------------------------------------------------------------
 *
 * Low-level stuff which actually talks to the LCD controller.
 *
 * Bit masks for wiggling LCD control lines.
 *
 * Note that the PC printer port inverts some of them.
 * The bit-map definitions take care of it.
 *
 * Also platform-specific commands to access I/O ports.
 *
 * Other examples I've seen all wiggle a bit at a time which
 * is considerably slower and uses more code space.
 *
 *--------------------------------------------------------------------*/

#if HI_TECH_C                      /* ---- PIC PORT A ---- */

#define IN_0	0
#define IN_1	0

#define CD_0    0
#define CD_1    0x08

#define CE_0    0
#define CE_1    0x04

#define RD_0    0
#define RD_1    0x02

#define WR_0    0
#define WR_1    0x01

/*
 * Why the NOP when reading?
 * Output occurs at the end of an instruction cycle.
 * Input occurs 1/4 from the beginning of an instruction cycle.
 * This doesn't leave much time for the LCD controller to respond.
 * We could get away without the extra delay for low clock speeds
 * and short wires but put it in to be safe.
 *
 * It would be nice to include this in the INPUT_DATA macro 
 * somehow so it isn't accidentally left out.
 */

#define OUTPUT_CONTROL(x)       PORTA = (x)
#define OUTPUT_DATA(x)          PORTB = (x)      
#define NOP 			asm("NOP")
#define INPUT_DATA()            PORTB

#else                           /* ---- PC Printer Port ---- */

#define IN_0    0
#define IN_1    0x20

#define CD_0    0x08
#define CD_1    0

#define CE_0    0x01
#define CE_1    0

#define RD_0    0x02
#define RD_1    0

#define WR_0    0
#define WR_1    0x04

#define LPT_DATA        0x378           /* Usual LPT1 address. */
#define LPT_CONTROL     LPT_DATA+2
#define LPT_STATUS      LPT_DATA+1

#if DEBUG

void OUTPUT_CONTROL (int x)
{
	printf ("OUTPUT_CONTROL(0x%02x)");
	if ((x & (IN_0 | IN_1)) == IN_1) printf (" IN"); else printf ("   ");
	if ((x & (CD_0 | CD_1)) == CD_1) printf (" C"); else printf (" D");
	if ((x & (CE_0 | CE_1)) == CE_0) printf (" CE"); else printf ("   ");
 	if ((x & (RD_0 | RD_1)) == RD_0) printf (" RD"); else printf ("   ");
	if ((x & (WR_0 | WR_1)) == WR_0) printf (" WR"); else printf ("   ");
	printf ("\n", x);
	outportb (LPT_CONTROL, x);
}

void OUTPUT_DATA (int x)
{
	printf ("OUTPUT_DATA(0x%02x)\n", x);
        outportb (LPT_DATA, x);
}

#define NOP
       
int INPUT_DATA (void)
{
	int x;

	printf ("INPUT_DATA() = ");
	x = inportb (LPT_DATA);
	printf ("0x%02x\n", x);
	return (x);
}

#else

#define OUTPUT_CONTROL(x)       outportb (LPT_CONTROL, (x))
#define OUTPUT_DATA(x)          outportb (LPT_DATA, (x)) 
#define NOP       
#define INPUT_DATA()            inportb (LPT_DATA)

#endif	/* DEBUG */
#endif  /* PIC */


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_write_command
 *
 * Purpose:     Send a command to the LCD controller.
 *
 * Inputs:      cmd     - Function code such as F_DATA_WRITE.
 *
 * Description: Put the bidirectional I/O port in input mode.
 *              Wait until the two low order status bits are 1 and 1.
 *
 *              Put bidirectional I/O port in output mode.
 *              The C/-D line is high to indicate a command.
 *
 *--------------------------------------------------------------------*/

static void grlcd_write_command (byte_t cmd)
{
        byte_t status;

#if HI_TECH_C
        TRISB = 0xFF;
#else
        OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
#endif
        do {
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_0 | RD_0 | WR_1 );
            NOP;
            status = INPUT_DATA ();
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
        } while ((status & 3) != 3);


#if HI_TECH_C
        TRISB = 0;
#endif
        OUTPUT_CONTROL ( IN_0 | CD_1 | CE_0 | RD_1 | WR_0 );
        OUTPUT_DATA (cmd);
        OUTPUT_CONTROL ( IN_0 | CD_1 | CE_1 | RD_1 | WR_1 );
}


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_write_data
 *		grlcd_write_data_auto_mode
 *
 * Purpose:     Send data to the LCD controller.
 *
 * Inputs:      dat     - A byte of data generally preceding a command.
 *
 * Description: Put the bidirectional I/O port in input mode.
 *              Wait until the two low order status bits are 1 and 1.
 *
 *		The auto mode variation looks at bit 3 instead.
 *
 *              Put bidirectional I/O port in output mode.
 *              The C/-D line is low to indicate data.
 *
 *--------------------------------------------------------------------*/

static void grlcd_write_data (byte_t dat)
{
        byte_t status;

#ifdef HI_TECH_C
        TRISB = 0xFF;
#else
        OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
#endif
        do {
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_0 | RD_0 | WR_1 );
            NOP;
            status = INPUT_DATA ();
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
        } while ((status & 3) != 3);

#ifdef HI_TECH_C
        TRISB = 0;
#endif
        OUTPUT_CONTROL ( IN_0 | CD_0 | CE_0 | RD_1 | WR_0 );
        OUTPUT_DATA (dat);
        OUTPUT_CONTROL ( IN_0 | CD_0 | CE_1 | RD_1 | WR_1 );
}


/* 
 * Same thing but look at bit 3 instead of bits 0 & 1. 
 */

static void grlcd_write_data_auto_mode (byte_t dat)
{
        byte_t status;

#ifdef HI_TECH_C
        TRISB = 0xFF;
#else
        OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
#endif
        do {
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_0 | RD_0 | WR_1 );
            NOP;
            status = INPUT_DATA ();
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
        } while ( ! (status & 8));

#ifdef HI_TECH_C
        TRISB = 0;
#endif
        OUTPUT_CONTROL ( IN_0 | CD_0 | CE_0 | RD_1 | WR_0 );
        OUTPUT_DATA (dat);
        OUTPUT_CONTROL ( IN_0 | CD_0 | CE_1 | RD_1 | WR_1 );
}


/*--------------------------------------------------------------------
 *
 * Name:        grlcd_read_data
 *
 * Purpose:     Get data from the LCD controller.
 *
 * Returns:     A byte.
 *
 * Description: Put the bidirectional I/O port in input mode.
 *              Wait until the two low order status bits are 1 and 1.
 *
 *              Read with the C/-D line low to indicate data.
 *
 *--------------------------------------------------------------------*/

#if HI_TECH_C
				/* Not used yet.  */
				/* Don't waste precious memory. */
#else

static byte_t grlcd_read_data (void)
{
        byte_t result;
        byte_t status;

#ifdef HI_TECH_C
        TRISB = 0xFF;
#else
        OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
#endif
        do {
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_0 | RD_0 | WR_1 );
            NOP;
            status = INPUT_DATA ();
            OUTPUT_CONTROL ( IN_1 | CD_1 | CE_1 | RD_1 | WR_1 );
        } while ((status & 3) != 3);


        OUTPUT_CONTROL ( IN_1 | CD_0 | CE_0 | RD_0 | WR_1 );
        NOP;
        result = INPUT_DATA ();
        OUTPUT_CONTROL ( IN_1 | CD_0 | CE_1 | RD_1 | WR_1 );

        return (result);
}

#endif

/* end grlcd.c */
