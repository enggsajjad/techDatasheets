/***************************************************************************
 *                                                                         *
 *	 FILE: LCD106x56.C                                                     *
 *   LCD Display Controller Interface Routines for use with Tian-ma        *
 *   106x56 Graphics module with onboard S6B0724X01-B0CY controller        *
 *                                                                         *
 *   Copyright (C) 2003 by Carousel Design Solutions                       *
 *                                                                         *
 *									Written by:                            *
 *									Michael J. Karas                       *
 *									Carousel Design Solutions              *
 *									4217 Grimes Ave South                  *
 *									Edina MN 55416                         *
 *									(952) 929-7537                         *
 *                                                                         *
 ***************************************************************************/

#pragma CODE

#include <c8051f020.h>                 // SFR declarations
#include "lcd106x56.h"
#include "fonts.h"

/* pixel level bit masks for display */
/* this array is setup to map the order */
/* of bits in a byte to the vertical order */
/* of bits at the LCD controller */
const unsigned char code l_mask_array[8] =
         {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/* the LCD display image memory */
/* buffer arranged so page memory is sequential in RAM */
unsigned char xdata l_display_array[Y_BYTES][X_BYTES];

/* 
**
** Low level LCD Controller Interface Support Routines
** The LCD Controller interfaces to the microcontroller
** using the connections as shown below. 
**
**		P3^0  LCD Controller Reset (RST/) signal
**		P3^1  LCD Controller Chip Select (CS/) signal
**		P3^2  LCD Controller Ctl/Data Select (C/D) signal
**		P3^2  LCD Controller Serial Clcok (SCLK) signal
**		P3^3  LCD Controller Serial Data (SDAT) signal
**
**
*/

/*
** 
** routine to initialize the operation of the LCD display subsystem
**
*/

void lcd_init(void)
{
	int i;

	/* initialize the port control lines to the LCD module */
	LCD_RST = 1;					/* set RST signal high off output */

	LCD_CS = 1;						/* set chip select high off output */
	
	LCD_CD = 0;						/* set the CD line low as output */
	
	LCD_SCLK = 1;					/* set SCLK line high */

	LCD_SDAT = 0;		   			/* set SDAT line low */

	/* reset the LCD controller chip */
	LCD_RST = 0;					/* set the reset line low */
	for(i=0; i<1000; i++)			/* delay for the reset time */
	{
	}
	LCD_RST = 1;					/* release reset to back high */

	/* program the controller operational state */
	lcd_out_ctl(LCD_SET_ADC_REV);	/* set ADC reverse */
	lcd_out_ctl(LCD_SET_SHL_NOR);	/* set SHL normal */
	lcd_out_ctl(LCD_SET_BIAS_0);	/* set for the low bias mode */
	lcd_out_ctl(LCD_PWR_CTL+5);		/* turn on the VC bit */
	for(i=0; i<1000; i++)			/* delay for the converter on */
	{
	}
	lcd_out_ctl(LCD_PWR_CTL+6);		/* now turn on VC+VR bits */
	for(i=0; i<1000; i++)			/* delay for the regulator on */
	{
	}
	lcd_out_ctl(LCD_PWR_CTL+7);		/* now turn on the VC+VR+VF */
	lcd_out_ctl(LCD_REG_RESISTOR+4); /* set default resistor ratio */
	lcd_out_ctl(LCD_REF_VOLT_MODE);	 /* prime for the reference voltage */
	lcd_out_ctl(LCD_REF_VOLT_REG+42);	 /* set default reference voltage select */
	for(i=0; i<1000; i++)			 /* delay for power stabilize */
	{
	}
	lcd_out_ctl(LCD_DISP_ON);		/* put the display on */

	lcd_out_ctl(LCD_SET_LINE+56);	/* set line for row 0 of display */
	lcd_out_ctl(LCD_SET_PAGE+0);	/* set page 0 */
	lcd_out_ctl(LCD_SET_COL_HI+0);	/* set column 0 */
	lcd_out_ctl(LCD_SET_COL_LO+0);
/*
** 
** program test loop to find right resistor ratio 0-8
** (use by break pointing at each loop step)
**
*/

#if 0
	for(p=0; p<7; p++)
	{
		lcd_out_ctl(LCD_REG_RESISTOR+p); /* set resistor ratio */
	}
#endif

/*
** 
** program test loop to find reference voltage setting 0-63
** (use by break pointing at each loop step)
**
*/

#if 0
	for(p=0; p<63; p++)
	{
		lcd_out_ctl(LCD_REF_VOLT_MODE);	 	 /* prime for the reference voltage */
		lcd_out_ctl(LCD_REF_VOLT_REG+p);	 /* set reference voltage select */
	}
#endif
} 

/* 
**
** low level routine to send a byte value out the serial bus
** to the LCD controller data register. entry argument
** is the data to output.
**
*/

void lcd_out_dat(char dat)
{
	unsigned char i=8;				/* serial bit counter */

	EA = 0;							/* disable interrupts */
	
	LCD_CD = 1;						/* select register for data port */
	LCD_CS = 0;						/* enable interface via chip select */

	while(i--)
	{
		LCD_SDAT = (dat & 0x80 ? 1 : 0);  /* transmit data from MSB */
		LCD_SCLK = 0;				/* turn the clock on */
		dat <<= 1;					/* shift data left 1 place */
		LCD_SCLK = 1;				/* clock back off */	
	}

	LCD_CD = 1;						/* force chip select back off */

	/* re-establish the exit interrupt state */
	EA = 1;
} 

/* 
**
** low level routine to send a byte value out the serial bus
** to the LCD controller control register. entry argument is
** the data to output.
**
*/

void lcd_out_ctl(char dat)
{
	unsigned char i=8;				/* serial bit counter */

	EA = 0;							/* disable interrupts */
	
	LCD_CD = 0;						/* select register for command port */
	LCD_CS = 0;						/* enable interface via chip select */

	while(i--)
	{
		LCD_SDAT = (dat & 0x80 ? 1 : 0);  /* transmit data from MSB */
		LCD_SCLK = 0;				/* turn the clock on */
		dat <<= 1;					/* shift data left 1 place */
		LCD_SCLK = 1;				/* clock back off */	
	}

	LCD_CD = 1;						/* force chip select back off */

	/* re-establish the exit interrupt state */
	EA = 1;
} 
 
/* 
**
** routine to erase the LCD screen, This erases whole
** display memory of the S6B0724 LCD controller.
**
*/

void lcd_erase(void)
{
	unsigned char p;
	unsigned char i;

	for(p=0; p<9; p++)
	{
		lcd_out_ctl(LCD_SET_PAGE+p);	/* set page */
		lcd_out_ctl(LCD_SET_COL_HI+0);	/* set column 0 */
		lcd_out_ctl(LCD_SET_COL_LO+0);
		for(i=0; i<132; i++)
		{
			lcd_out_dat(0);				/* clear the data */
		}
	}
}

/* 
**
** routine to display a test pattern on the LCD screen,
**
*/

unsigned char code testpat[4][8]={
				   {0x0F,0x0F,0x0F,0x0F,0xF0,0xF0,0xF0,0xF0},
				   {0xF0,0xF0,0xF0,0xF0,0x0F,0x0F,0x0F,0x0F},
	               {0xFF,0x81,0xBD,0xBD,0xBD,0xBD,0x81,0xFF},
				   {0x00,0x7E,0x42,0x42,0x42,0x42,0x7E,0x00}
				  };

void lcd_test(unsigned char pattern)
{
	unsigned char p;
	unsigned char i;

	for(p=0; p<7; p++)
	{
		lcd_out_ctl(LCD_SET_PAGE+p);	/* set page */
		lcd_out_ctl(LCD_SET_COL_HI+26/16);	/* set column 0 */
		lcd_out_ctl(LCD_SET_COL_LO+26%16);
		for(i=0; i<106; i++)
		{
			lcd_out_dat(testpat[pattern][i%8]);
		}
	}
}

/*
**
** 	Clears the display memory starting at the left/top  and going to
**  the right/bottom . No runtime error checking is performed. It is 
**  assumed that left is less than right and that top is less than 
**  bottom
**
*/

void lcd_clear_area(unsigned char left,  unsigned char top,    
			        unsigned char right, unsigned char bottom)
{
	unsigned char bit_pos;
	unsigned char x;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = top & 0x07;					/* get starting bit offset into byte */

	for(x = left; x <= right; x++)
	{
		byte_offset = top >> 3;				/* get byte offset into y direction */
		y_bits = (bottom - top) + 1;		/* get length in the y direction to write */
		remaining_bits = 8 - bit_pos;		/* number of bits left in byte */
		mask = l_mask_array[bit_pos];		/* get mask for this bit */

		while(y_bits)						/* while there are still bits to write */
		{
			if((remaining_bits == 8) && (y_bits > 7))
			{
				/* here if we are byte aligned and have at least 1 byte to write */
				/* do the entire byte at once instead of bit by bit */
				while(y_bits > 7)			/* while there are at least 8 more bits to do */
				{
					l_display_array[byte_offset][x] = 0x00;
					byte_offset++;
					y_bits -= 8;
				}
			}
			else
			{
				/* here if not byte aligned or an entire byte does not need written */
				/* thus do bit by bit */
				l_display_array[byte_offset][x] &= ~mask;
				if(l_mask_array[0] & 0x80)
				{
					mask >>= 1;
				}
				else
				{
					mask <<= 1;
				}
				y_bits--;
				remaining_bits--;
				if(remaining_bits == 0)
				{
					/* might have bust gotton byte aligned */
					/* so reset for beginning of a byte */
					remaining_bits = 8;
					byte_offset++;
					mask = l_mask_array[0];
				}
			}
		}
	}
}

/*
**
** Inverts the display memory starting at the left/top and going to
** the right/bottom. No runtime error checking is performed. It is 
** assumed that left is less than right and that top is less than 
** bottom 
** 
*/

void lcd_invert_area(unsigned char left,  unsigned char top,    
			         unsigned char right, unsigned char bottom)
{
	unsigned char bit_pos;
	unsigned char x;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = top & 0x07;					/* get starting bit offset into byte */

	for(x = left; x <= right; x++)
  	{
		byte_offset = top >> 3;				/* get byte offset into y direction */
		y_bits = (bottom - top) + 1;		/* get length in the x direction to write */
		remaining_bits = 8 - bit_pos;		/* number of bits left in byte */
		mask = l_mask_array[bit_pos];		/* get mask for this bit */

		while(y_bits)						/* while there are still bits to write */
    	{
			if((remaining_bits == 8) && (y_bits > 7))
			{
				/* here if we are byte aligned and have at least 1 byte to write */
				/* do the entire byte at once instead of bit by bit */
				while(y_bits > 7)			/* while there are at least 8 more bits to do */
				{
					l_display_array[byte_offset][x] ^= 0xFF;
					byte_offset++;
					y_bits -= 8;
				}
      		}
      		else
      		{
				/* here if not byte aligned or an entire byte does not need written */
				/* thus do bit by bit */
				l_display_array[byte_offset][x] ^= mask;
				if(l_mask_array[0] & 0x80)
				{
					mask >>= 1;
				}
				else
				{
					mask <<= 1;
				}
				y_bits--;
				remaining_bits--;
				if(remaining_bits == 0)
				{
					/* might have bust gotton byte aligned */
					/* so reset for beginning of a byte */
					remaining_bits = 8;
					byte_offset++;
					mask = l_mask_array[0];
				}
			}
		}
	}
}

/*
**
** Draws a line into the display memory starting at left going to
** right, on the given row. No runtime error checking is performed.  
** It is assumed that left is less than right.
**
*/

void lcd_horz_line(unsigned char left, unsigned char right,
		           unsigned char row)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char mask;
	unsigned char col;

  	bit_pos = row & 0x07;			/* get the bit offset into a byte */
  	byte_offset = row >> 3;		    /* get the byte offset into x array */
  	mask = l_mask_array[bit_pos]; 	/* get the mask for this bit */

  	for(col = left; col <= right; col++)
  	{
    	l_display_array[byte_offset][col] |= mask;
  	}
}

/*
**
** Draws a vertical line into display memory starting at the top
** going to the bottom in the given column. No runtime error checking 
** is performed. It is assumed that top is less than bottom and that 
** the column is in range.
**
*/

void lcd_vert_line(unsigned char top, unsigned char bottom,
		           unsigned char column)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = top & 0x07;		   /* get starting bit offset into byte */

	byte_offset = top >> 3;		   /* get byte offset into y direction */
	y_bits = (bottom - top) + 1;   /* get length in the x direction to write */
	remaining_bits = 8 - bit_pos;  /* number of bits left in byte */
	mask = l_mask_array[bit_pos];  /* get mask for this bit */

	while(y_bits)				   /* while there are still bits to write */
	{
		if((remaining_bits == 8) && (y_bits > 7))
		{
			/* here if we are byte aligned and have at least 1 byte to write */
			/* do the entire byte at once instead of bit by bit */
			while(y_bits > 7)			/* while there are at least 8 more bits to do */
			{
				l_display_array[byte_offset][column] = 0xFF;
				byte_offset++;
				y_bits -= 8;
			}
		}
		else
		{
      		/* we are not byte aligned or an entire byte does not need written */
      		/* do each individual bit                                          */
			l_display_array[byte_offset][column] |= mask;
			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			y_bits--;
			remaining_bits--;
			if(remaining_bits == 0)
			{
				/* might have bust gotton byte aligned */
				/* so reset for beginning of a byte */
				remaining_bits = 8;
				byte_offset++;
				mask = l_mask_array[0];
			}
		}
	}
}

/*
**
** Clears a line into the display memory starting at left going to
** right, on the given row. No runtime error checking is performed.  
** It is assumed that left is less than right.
**
*/

void lcd_clr_horz_line(unsigned char left, unsigned char right,
		               unsigned char row)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char mask;
	unsigned char col;

  	bit_pos = row & 0x07;			/* get the bit offset into a byte */
  	byte_offset = row >> 3;		    /* get the byte offset into x array */
  	mask = l_mask_array[bit_pos]; 	/* get the mask for this bit */

  	for(col = left; col <= right; col++)
  	{
    	l_display_array[byte_offset][col] &= ~mask;
  	}
}


/*
**
** Clears a vertical line into display memory starting at the top
** going to the bottom in the given column. No runtime error checking 
** is performed. It is assumed that top is less than bottom and that 
** the column is in range.
**
*/

void lcd_clr_vert_line(unsigned char top, unsigned char bottom,
		               unsigned char column)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = top & 0x07;		   /* get starting bit offset into byte */

	byte_offset = top >> 3;		   /* get byte offset into y direction */
	y_bits = (bottom - top) + 1;   /* get length in the x direction to write */
	remaining_bits = 8 - bit_pos;  /* number of bits left in byte */
	mask = l_mask_array[bit_pos];  /* get mask for this bit */

	while(y_bits)				   /* while there are still bits to write */
	{
		if((remaining_bits == 8) && (y_bits > 7))
		{
			/* here if we are byte aligned and have at least 1 byte to write */
			/* do the entire byte at once instead of bit by bit */
			while(y_bits > 7)			/* while there are at least 8 more bits to do */
			{
				l_display_array[byte_offset][column] = 0x00;
				byte_offset++;
				y_bits -= 8;
			}
		}
		else
		{
      		/* we are not byte aligned or an entire byte does not need written */
      		/* do each individual bit                                          */
			l_display_array[byte_offset][column] &= ~mask;
			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			y_bits--;
			remaining_bits--;
			if(remaining_bits == 0)
			{
				/* might have bust gotton byte aligned */
				/* so reset for beginning of a byte */
				remaining_bits = 8;
				byte_offset++;
				mask = l_mask_array[0];
			}
		}
	}
}

/*
**
** 	Draws a box in display memory starting at the left/top and going
**  to the right/bottom. No runtime error checking is performed.
**  It is assumed that left is less than right and that top is less 
**  than bottom.
** 
*/

void lcd_box(unsigned char left, unsigned char top,
             unsigned char right, unsigned char bottom)
{
  	/* to draw a box requires two vertical lines */
  	lcd_vert_line(top,bottom,left);
  	lcd_vert_line(top,bottom,right);

  	/* and two horizonal lines */
  	lcd_horz_line(left,right,top);
  	lcd_horz_line(left,right,bottom);
}

/*
**
** Clears a box in display memory starting at the Top left and going
** to the bottom right. No runtime error checking is performed and
** it is assumed that Left is less than Right and that Top is less 
** than Bottom.
**
*/

void lcd_clr_box(unsigned char left, unsigned char top,
             unsigned char right, unsigned char bottom)
{
  	/* to undraw the box undraw the two vertical lines */
  	lcd_clr_vert_line(top,bottom,left);
  	lcd_clr_vert_line(top,bottom,right);

  	/* and the two horizonal lines that comprise it */
  	lcd_clr_horz_line(left,right,top);
    lcd_clr_horz_line(left,right,bottom);
}

/*
**
** Writes a glyph to the display at location x,y
**
** Arguments are:
**    column     - x corrdinate of the left part of glyph          
**    row        - y coordinate of the top part of glyph       
**    width  	 - size in pixels of the width of the glyph    
**    height 	 - size in pixels of the height of the glyph   
**    glyph      - an unsigned char pointer to the glyph pixels 
**                 to write assumed to be of length "width"
**
*/

void lcd_glyph(unsigned char left, unsigned char top,
			   unsigned char width, unsigned char height,
			   unsigned char *glyph, unsigned char store_width)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;
	unsigned char char_mask;
	unsigned char x;
	unsigned char *glyph_scan;
	unsigned char glyph_offset;

  	bit_pos = top & 0x07;		/* get the bit offset into a byte */

	glyph_offset = 0;			/* start at left side of the glyph rasters */
    char_mask = 0x80;			/* initial character glyph mask */

  	for (x = left; x < (left + width); x++)
  	{
    	byte_offset = top >> 3;        	/* get the byte offset into y direction */
		y_bits = height;				/* get length in y direction to write */
		remaining_bits = 8 - bit_pos;	/* number of bits left in byte */
		mask = l_mask_array[bit_pos];	/* get mask for this bit */
		glyph_scan = glyph + glyph_offset;	 /* point to base of the glyph */

    	/* boundary checking here to account for the possibility of  */
    	/* write past the bottom of the screen.                        */
    	while((y_bits) && (byte_offset < Y_BYTES)) /* while there are bits still to write */
    	{
			/* check if the character pixel is set or not */
			if(*glyph_scan & char_mask)
			{
				l_display_array[byte_offset][x] |= mask;	/* set image pixel */
			}
			else
			{
      			l_display_array[byte_offset][x] &= ~mask;	/* clear the image pixel */
			}

			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			
			y_bits--;
			remaining_bits--;
      		if(remaining_bits == 0)
      		{
				/* just crossed over a byte boundry, reset byte counts */
				remaining_bits = 8;
				byte_offset++;
				mask = l_mask_array[0];
      		}

			/* bump the glyph scan to next raster */
			glyph_scan += store_width;
		}

		/* shift over to next glyph bit */
		char_mask >>= 1;
		if(char_mask == 0)				/* reset for next byte in raster */
		{
			char_mask = 0x80;
			glyph_offset++;
	    }
	}
}

/*
**
**	Prints the given string at location x,y in the specified font.
**  Prints each character given via calls to lcd_glyph. The entry string
**  is null terminated and non 0x20->0x7e characters are ignored.
**
**  Arguments are:                                                   
**      left       coordinate of left start of string.                
**      top        coordinate of top of string.
**      font       font number to use for display                
**      str   	   text string to display
**
*/

void lcd_text(unsigned char left, unsigned char top, unsigned char font, char *str)
{
  	unsigned char x = left;
  	unsigned char glyph;
  	unsigned char width;
	unsigned char height;
	unsigned char store_width;
	unsigned char code *glyph_ptr;

  	while(*str != 0x00)
  	{
    	glyph = (unsigned char)*str;

		/* check to make sure the symbol is a legal one */
		/* if not then just replace it with a "." character */
		if((glyph < ASCII_BOT) || (glyph > ASCII_TOP))
		{
			glyph = '.';
		}

    	/* the fonts start at ASCII_BOT, so to get the index into the font array */
    	/* subtract ASCII_BOT from the glyph                  */
    	glyph -= ASCII_BOT;
    	width = fonts[font].fixed_width;	/* check if it is a fixed width */
		if(width == 0)
		{
			width=fonts[font].width_table[glyph];	/* get the variable width instead */
		}

		height = fonts[font].glyph_height;
		store_width = fonts[font].store_width;

		glyph_ptr = fonts[font].glyph_table + (glyph * store_width * height);

		lcd_glyph(x,top,width,height,glyph_ptr,store_width);  /* plug symbol into buffer */

		x += width;							/* move right for next character */
		str++;								/* point to next character in string */
	}
}

/*
**
** Updates area of the display. Writes data from display 
** RAM to the lcd display controller.
** 
** Arguments Used:                                      
**    top     top line of area to update.         
**    bottom  bottom line of area to update.
**
*/

void lcd_update(unsigned char top, unsigned char bottom)
{
	unsigned char x;
	unsigned char y;
	unsigned char yt;
	unsigned char yb;
	unsigned char *colptr;

	yt = top >> 3;				/* setup bytes of range */
	yb = bottom >> 3;

	for(y = yt; y <= yb; y++)
	{
		/* setup the page number for the y direction */
		lcd_out_ctl(LCD_SET_PAGE+y);	/* set page */
	
		/* setup column of update to left side */
		lcd_out_ctl(LCD_SET_COL_HI+(26/16));	/* set column 26 */
		lcd_out_ctl(LCD_SET_COL_LO+(26%16));

		colptr = &l_display_array[y][0];
		for (x=0; x < X_BYTES; x++)
		{
			lcd_out_dat(*colptr++);
		}
	}
}
      
