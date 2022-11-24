#ifndef _LCD240x64_H
#define _LCD240x64_H

/* equate the LCD Controller control signals to their port assignments */
sbit LCD_CE = P3^0;					/* port bit for Chip Select */
sbit LCD_CD = P3^1;					/* port bit for Control/Data Select */
sbit LCD_WR = P3^2;					/* port bit for the Write Strobe */
sbit LCD_RD	= P3^3;					/* port bit for the Read Strobe */
sbit LCD_RST = P3^4;				/* port bit for Display Reset */
sbit LCD_FS = P3^5;					/* port bit for Font Select */

/* LCD screen and bitmap image array consants */
#define X_BYTES				30
#define Y_BYTES	      		64
#define SCRN_LEFT			0
#define SCRN_TOP			0
#define SCRN_RIGHT			239
#define SCRN_BOTTOM			63

/* LCD Global data arrays */
extern const unsigned char code l_mask_array[8];
extern unsigned char xdata l_display_array[Y_BYTES][X_BYTES];

/* LCD function prototype list */
extern void lcd_init(void);
extern void lcd_out_dat(char dat);
extern void lcd_out_ctl(char dat);
extern char lcd_in_dat(void);
extern char lcd_in_sta(void);
extern void lcd_busy_wait(void);
extern void lcd_cmd_2(char cmd,unsigned int parm);
extern void lcd_cmd_1(char cmd,unsigned char parm);
extern void lcd_cmd(char cmd);
extern void lcd_erase(void);
extern int lcd_test(char test);
extern void lcd_clear_area(unsigned char left,  unsigned char top,    
			               unsigned char right, unsigned char bottom);
extern void lcd_invert_area(unsigned char left,  unsigned char top,    
			                unsigned char right, unsigned char bottom);
extern void lcd_horz_line(unsigned char left, unsigned char right,
		                  unsigned char row);
extern void lcd_vert_line(unsigned char top, unsigned char bottom,
		                  unsigned char column);
extern void lcd_clr_horz_line(unsigned char left, unsigned char right,
		                      unsigned char row);
extern void lcd_clr_vert_line(unsigned char top, unsigned char bottom,
		                      unsigned char column);
extern void lcd_box(unsigned char left, unsigned char top,
                    unsigned char right, unsigned char bottom);
extern void lcd_clr_box(unsigned char left, unsigned char top,
                        unsigned char right, unsigned char bottom);
extern void lcd_glyph(unsigned char left, unsigned char top,
			          unsigned char width, unsigned char height,
			          unsigned char *glyph, unsigned char store_width);
extern void lcd_text(unsigned char left, unsigned char top, unsigned char font, char *str);
extern void lcd_update(unsigned char top, unsigned char bottom);

#endif
