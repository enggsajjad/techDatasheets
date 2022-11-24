/***************************************************************************
 *                                                                         *
 *	 FILE: LCD.C                                                           *
 *   LCD Display Controller Interface Routines for use with Optrex         *
 *   240x64 Graphics module with onboard Toshiba T6963 LSI Controller      *
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

#include "c8051f020.h"                 	// SFR declarations
#include "lcd240x64.h"
#include "fonts.h"

/* pixel level bit masks for display */
/* this array is setup to map the order */
/* of bits in a byte to the vertical order */
/* of bits at the LCD controller */
const unsigned char code l_mask_array[8] =
         {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

/* the LCD display image memory */
/* buffer arranged so page memory is sequential in RAM */
unsigned char xdata l_display_array[Y_BYTES][X_BYTES];

/* 
**
** Low level LCD Controller Interface Support Routines
** The LCD Controller interfaces to the microcontroller
** using the connections as shown below. 
**
**		P2^0 \
**		  to  +- Eight bit Data Bus to/from the LCD controller
**		P2^7 /
**		P3^0  LCD Controller Chip Select (CE) signal
**		P3^1  LCD Controller Ctl/Data Select (C/D) signal
**		P3^2  LCD Controller Write (WR/) signal
**		P3^3  LCD Controller Read (RD/) signal
**		P3^4  LCD Controller Reset (RST/) signal
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
	int err=0;

	/* initialize the port control lines to the LCD module */
	LCD_CE = 1;						/* set chip select high off output */

	LCD_RST = 1;					/* set RST signal high off output */
	
	LCD_CD = 0;						/* set the CD line low as output */
	
	LCD_RD = 1;						/* set RD line high as output */

	LCD_WR = 1;		   				/* set WR line high as output */

	LCD_FS = 0;						/* force font select line low */

	/* reset the LCD controller chip */
	LCD_RST = 0;					/* set the reset line low */
	for(i=0; i<10000; i++)			/* delay for the reset time */
	{
	}
	LCD_RST = 1;					/* release reset to back high */

	/* program the controller mode of operation to graphics only mode */
	lcd_cmd_2(0x42,0);				/* set graphics start at zero */
	lcd_cmd_2(0x43,30);				/* set number of bytes in horz raster */
	lcd_cmd_2(0x40,30*64);			/* set text area after the graphics memory */
	lcd_cmd_2(0x41,40);				/* set number of text characters on row @6 bits */
	lcd_cmd(0x81);					/* setup mode set for text / graphics XOR */
	lcd_cmd(0x98);					/* enable graphics only mode / no text no cursor */
	lcd_cmd_2(0x24,0);				/* setup the AP to 0 */
} 

/* 
**
** low level routine to send a byte value out port 2 bus
** to the LCD controller data register. entry argument
** is the data to output.
**
*/

void lcd_out_dat(char dat)
{
	LCD_RD = 1;						/* make sure the RD/ line is high */
	LCD_CD = 0;						/* clr the CD line low to data */
	P2 = dat;						/* output the data to bus */
	LCD_CE = 0;						/* enable chip select */
	LCD_WR = 0;						/* set the WR/ line low */
    LCD_WR = 0;						/* extra delay for pulse width */
	LCD_WR = 1;						/* set the WR/ line back high */
	LCD_CE = 1;						/* force the CE back high */
	P2 = 0xFF;						/* put the I/O lines back as inputs */
} 

/* 
**
** low level routine to send a byte value out port 2 bus
** to the LCD controller control register. entry argument is
** the data to output.
**
*/

void lcd_out_ctl(char dat)
{
	LCD_RD = 1;						/* make sure the RD/ line is high */
	LCD_CD = 1;						/* set the CD line high to cmd */
	P2 = dat;						/* output the data to bus */
	LCD_CE = 0;						/* enable chip select */
	LCD_WR = 0;						/* set the WR/ line low */
    LCD_WR = 0;						/* extra delay for pulse width */
	LCD_WR = 1;						/* set the WR/ line back high */
	LCD_CE = 1;						/* force the CE back high */
	P2 = 0xFF;
} 
 
/* 
**
** low level routine to read a byte value from the LCD 
** controller data register in via the port 2 bus.
**
*/

char lcd_in_dat(void)
{
	char dat;

	LCD_WR = 1;						/* make sure the WR/ line is high */
	LCD_CD = 0;						/* clr the CD line low to data */
	P2 = 0xFF;						/* make port 2 input */
	LCD_CE = 0;						/* enable chip select */
	LCD_RD = 0;						/* set the RD/ line low */
	LCD_RD = 0; 					/* extra read delay */
	LCD_RD = 0;						/* more extra read time */
	dat=P2;							/* fetch in the data */
	LCD_RD = 1;						/* set the RD/ line back high */
	LCD_CE = 1;						/* force the CE back high */

	return(dat);
} 

/* 
**
** low level routine to read a byte value from the LCD 
** controller status register in via the data bus. the
** entry argument is the interrupt thread mode.
**
*/

char lcd_in_sta(void)
{
	char dat;

	LCD_WR = 1;						/* make sure the WR/ line is high */
	LCD_CD = 1;						/* set the CD line high for status */
	P2 = 0xFF;						/* make port A input */
	LCD_CE = 0;						/* enable chip select */
	LCD_RD = 0;						/* set the RD/ line low */
	LCD_RD = 0; 					/* extra read delay */
	LCD_RD = 0;						/* more extra read time */
	dat=P2;							/* fetch in the data */
	LCD_RD = 1;						/* set the RD/ line back high */
	LCD_CE = 1;						/* force the CE back high */

	return(dat);
}
 
/* 
**
** low level routine to poll the LCD till the status bits STA0 and STA1
** both report back as 1's to indicate that the chip is ready to take
** a command.
**
*/

void lcd_busy_wait(void)
{
	char dat;

	do
	{
		dat=lcd_in_sta();	
	}
	while((dat & 0x03) != 0x03);		/* wait till STA1=1 && STA0=1 */
}

/*
**
** low level routine to send a two byte command to the LCD
** controller. the two entry parameters are the command code 
** and the two byte parameter.
**
*/

void lcd_cmd_2(char cmd,unsigned int parm)
{
	lcd_busy_wait();					/* wait for the status */
	lcd_out_dat(parm&0xFF);				/* send out the low byte of the data */
	lcd_busy_wait();					/* wait again for status */
	lcd_out_dat(parm>>8);				/* send out the high byte of the data */
	lcd_busy_wait();					/* wait for status on command */
	lcd_out_ctl(cmd);					/* send out the command itself */
}

/*
**
** low level routine to send a one byte command to the LCD
** controller. the two entry parameters are the command code 
** and the one byte parameter.
**
*/

void lcd_cmd_1(char cmd,unsigned char parm)
{
	lcd_busy_wait();					/* wait for the status */
	lcd_out_dat(parm);					/* send out the byte of the data */
	lcd_busy_wait();					/* wait for status on command */
	lcd_out_ctl(cmd);					/* send out the command itself */
}

/*
**
** low level routine to send a command with no parameters to the LCD
** controller. the entry parameter is the command code 
**
*/

void lcd_cmd(char cmd)
{
	lcd_busy_wait();					/* wait for status on command */
	lcd_out_ctl(cmd);					/* send out the command itself */
}


/*
** 
** routine to blank the whole LCD display 
**
*/

void lcd_erase(void)
{
	int x;
	int y;

	/* loops to write whole LCD memory to a blank pattern */
	lcd_cmd_2(0x24,0);			/* setup the AP to 0 */
	for(x=0; x<X_BYTES; x++)
	{
		for(y=0; y<Y_BYTES; y++)
		{
			lcd_cmd_1(0xC0,0);
		}
	}
}

/*
** 
** routine to test the LCD display by writing a pattern to the LCD controller 
** memory and then reading it back to verify the correct operation. 
** The entry argument will represent the even/odd checkboard pattern selected 
** via a 0/1 value.
**
*/

int lcd_test(char test)
{
	unsigned char row,byte;
	int	err_flag=0;
	unsigned char dat;

	/* write all of the memory with the pattern data */
	for(row=0; row<Y_BYTES; row++)
	{
		/* set addr ptr to start of line */
		lcd_cmd_2(0x24,row*X_BYTES);
		for(byte=0; byte<X_BYTES; byte++)
		{
			if(( (row&0x04) &&  test) ||
			   (!(row&0x04) && !test))
			{
				lcd_cmd_1(0xC0,0x0F);	
			}
			else
			{
				lcd_cmd_1(0xC0,0xF0);	
			}
		}
	}

	/* read back the pattern data to verify that it is correct */
	for(row=0; row<Y_BYTES; row++)
	{
		/* set addr ptr to start of line */
		lcd_cmd_2(0x24,row*X_BYTES);
		for(byte=0; byte<X_BYTES; byte++)
		{
			if(( (row&0x04) &&  test) ||
			   (!(row&0x04) && !test))
			{
					lcd_cmd(0xC1);
					lcd_busy_wait();
					dat=lcd_in_dat();	
					if(dat != 0x0F)
					{
						err_flag=1;				/* show read back failure */
					}
			}
			else
			{
					lcd_cmd(0xC1);
					lcd_busy_wait();
					dat=lcd_in_dat();	
					if(dat != 0xF0)
					{
						err_flag=1;				/* show read back failure */
					}
			}
		}
	}
	return(err_flag);							/* show test result */
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
	unsigned char y;
	unsigned char byte_offset;
	unsigned char x_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = left & 0x07;					/* get starting bit offset into byte */

	for(y = top; y <= bottom; y++)
	{
		byte_offset = left >> 3;			/* get byte offset into x direction */
		x_bits = (right - left) + 1;		/* get length in the x direction to write */
		remaining_bits = 8 - bit_pos;		/* number of bits left in byte */
		mask = l_mask_array[bit_pos];		/* get mask for this bit */

		while(x_bits)						/* while there are still bits to write */
		{
			if((remaining_bits == 8) && (x_bits > 7))
			{
				/* here if we are byte aligned and have at least 1 byte to write */
				/* do the entire byte at once instead of bit by bit */
				while(x_bits > 7)			/* while there are at least 8 more bits to do */
				{
					l_display_array[y][byte_offset] = 0x00;
					byte_offset++;
					x_bits -= 8;
				}
			}
			else
			{
				/* here if not byte aligned or an entire byte does not need written */
				/* thus do bit by bit */
				l_display_array[y][byte_offset] &= ~mask;
				if(l_mask_array[0] & 0x80)
				{
					mask >>= 1;
				}
				else
				{
					mask <<= 1;
				}
				x_bits--;
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
	unsigned char y;
	unsigned char byte_offset;
	unsigned char x_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = left & 0x07;					/* get starting bit offset into byte */

	for(y = top; y <= bottom; y++)
  	{
		byte_offset = left >> 3;			/* get byte offset into x direction */
		x_bits = (right - left) + 1;		/* get length in the x direction to write */
		remaining_bits = 8 - bit_pos;		/* number of bits left in byte */
		mask = l_mask_array[bit_pos];		/* get mask for this bit */

		while(x_bits)						/* while there are still bits to write */
    	{
			if((remaining_bits == 8) && (x_bits > 7))
			{
				/* here if we are byte aligned and have at least 1 byte to write */
				/* do the entire byte at once instead of bit by bit */
				while(x_bits > 7)			/* while there are at least 8 more bits to do */
				{
					l_display_array[y][byte_offset] ^= 0xFF;
					byte_offset++;
					x_bits -= 8;
				}
      		}
      		else
      		{
				/* here if not byte aligned or an entire byte does not need written */
				/* thus do bit by bit */
				l_display_array[y][byte_offset] ^= mask;
				if(l_mask_array[0] & 0x80)
				{
					mask >>= 1;
				}
				else
				{
					mask <<= 1;
				}
				x_bits--;
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
	unsigned char x_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = left & 0x07;		   /* get starting bit offset into byte */

	byte_offset = left >> 3;		   /* get byte offset into y direction */
	x_bits = (right - left) + 1;   /* get length in the x direction to write */
	remaining_bits = 8 - bit_pos;  /* number of bits left in byte */
	mask = l_mask_array[bit_pos];  /* get mask for this bit */

	while(x_bits)				   /* while there are still bits to write */
	{
		if((remaining_bits == 8) && (x_bits > 7))
		{
			/* here if we are byte aligned and have at least 1 byte to write */
			/* do the entire byte at once instead of bit by bit */
			while(x_bits > 7)			/* while there are at least 8 more bits to do */
			{
				l_display_array[row][byte_offset] = 0xFF;
				byte_offset++;
				x_bits -= 8;
			}
		}
		else
		{
      		/* we are not byte aligned or an entire byte does not need written */
      		/* do each individual bit                                          */
			l_display_array[row][byte_offset] |= mask;
			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			x_bits--;
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
	unsigned char mask;
	unsigned char row;

  	bit_pos = column & 0x07;		/* get the bit offset into a byte */
  	byte_offset = column >> 3;		/* get the byte offset into x array */
  	mask = l_mask_array[bit_pos]; 	/* get the mask for this bit */

  	for(row = top; row <= bottom; row++)
  	{
    	l_display_array[row][byte_offset] |= mask;
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
	unsigned char x_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = left & 0x07;		   /* get starting bit offset into byte */

	byte_offset = left >> 3;	   /* get byte offset into x direction */
	x_bits = (right - left) + 1;   /* get length in the x direction to write */
	remaining_bits = 8 - bit_pos;  /* number of bits left in byte */
	mask = l_mask_array[bit_pos];  /* get mask for this bit */

	while(x_bits)				   /* while there are still bits to write */
	{
		if((remaining_bits == 8) && (x_bits > 7))
		{
			/* here if we are byte aligned and have at least 1 byte to write */
			/* do the entire byte at once instead of bit by bit */
			while(x_bits > 7)			/* while there are at least 8 more bits to do */
			{
				l_display_array[row][byte_offset] = 0x00;
				byte_offset++;
				x_bits -= 8;
			}
		}
		else
		{
      		/* we are not byte aligned or an entire byte does not need written */
      		/* do each individual bit                                          */
			l_display_array[row][byte_offset] &= ~mask;
			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			x_bits--;
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
	unsigned char mask;
	unsigned char row;

  	bit_pos = column & 0x07;		/* get the bit offset into a byte */
  	byte_offset = column >> 3;		/* get the byte offset into x array */
  	mask = l_mask_array[bit_pos]; 	/* get the mask for this bit */

  	for(row = top; row <= bottom; row++)
  	{
    	l_display_array[row][byte_offset] &= ~mask;
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
	unsigned char x_bits;
	unsigned char remaining_bits;
	unsigned char mask;
	unsigned char char_mask;
	unsigned char y;
	unsigned char *glyph_scan;
	unsigned char glyph_offset;

  	bit_pos = left & 0x07;		/* get the bit offset into a byte */

	glyph_offset = 0;			/* start at top of the glyph rasters */
  	for (y = top; y < (top + height); y++)
  	{
    	byte_offset = left >> 3;        /* get the byte offset into x direction */
		x_bits = width;					/* get length in y direction to write */
		remaining_bits = 8 - bit_pos;	/* number of bits left in byte */
		mask = l_mask_array[bit_pos];	/* get mask for this bit */

		glyph_scan = glyph + glyph_offset;	 /* point to start of glyph raster */

    	char_mask = 0x80;			/* initial character glyph mask */

    	/* boundary checking here to account for the possibility of  */
    	/* write past the right of the screen.                        */
    	while((x_bits) && (byte_offset < X_BYTES)) /* while there are bits still to write */
    	{
			/* check if the character pixel is set or not */
			if(*glyph_scan & char_mask)
			{
				l_display_array[y][byte_offset] |= mask;	/* set image pixel */
			}
			else
			{
      			l_display_array[y][byte_offset] &= ~mask;	/* clear the image pixel */
			}

			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			
			x_bits--;
			remaining_bits--;
      		if(remaining_bits == 0)
      		{
				/* just crossed over a byte boundry, reset byte counts */
				remaining_bits = 8;
				byte_offset++;
				mask = l_mask_array[0];
      		}

			/* shift over to next glyph bit */
			char_mask >>= 1;
			if(char_mask == 0)				/* reset for next byte in raster */
			{
				char_mask = 0x80;
				glyph_scan++;
		    }
		}

		/* bump the offset to next raster */
		glyph_offset += store_width;
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
		/* if not then just replace it with the default character */
		if((glyph < fonts[font].glyph_beg) || (glyph > fonts[font].glyph_end))
		{
			glyph = fonts[font].glyph_def;
		}

    	/* make zero based index into the font data arrays */
    	glyph -= fonts[font].glyph_beg;
    	width = fonts[font].fixed_width;	/* check if it is a fixed width */
		if(width == 0)
		{
			width=fonts[font].width_table[glyph];	/* get the variable width instead */
		}

		height = fonts[font].glyph_height;
		store_width = fonts[font].store_width;

		glyph_ptr = fonts[font].glyph_table + ((unsigned int)glyph * (unsigned int)store_width * (unsigned int)height);

		/* range check / limit things here */
		if(x > SCRN_RIGHT)
		{
			x = SCRN_RIGHT;
		}
		if((x + width) > SCRN_RIGHT+1)
		{
			width = SCRN_RIGHT - x + 1;
		}
		if(top > SCRN_BOTTOM)
		{
			top = SCRN_BOTTOM;
		}
		if((top + height) > SCRN_BOTTOM+1)
		{
			height = SCRN_BOTTOM - top + 1;
		}

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
	unsigned char dat;
	unsigned char *colptr;

	for(y = top; y <= bottom; y++)
	{
		/* set addr ptr to start of line */
		lcd_cmd_2(0x24,y*30);				/* scan the display up top to bottom */
		lcd_cmd(0xB0);						/* send the auto write command */

		colptr = &l_display_array[y][0];	/* point to row of data in buffer */
		for (x=0; x < X_BYTES; x++)
		{
			do
			{
				dat=lcd_in_sta();	
			} 
			while((dat & 0x08) != 0x08);		/* wait till STA3=1 */
			lcd_out_dat(*colptr++);
		}
		do									/* poll loop for ready for auto reset command */
		{
			dat=lcd_in_sta();	
		} 
		while((dat & 0x08) != 0x08);		/* wait till STA2=1 */
		lcd_out_ctl(0xB2);					/* send the auto reset command */
	}
}
