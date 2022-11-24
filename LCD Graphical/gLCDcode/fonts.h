#ifndef _FONTS_H
#define _FONTS_H

struct FONT_DEF 
{
   unsigned char store_width;            /* glyph storage width in bytes */
   unsigned char glyph_height;  		 /* glyph height for storage */
   unsigned char code *glyph_table;      /* font table start address in memory */
   unsigned char fixed_width;            /* fixed width of glyphs. If zero */
                                         /* then use the width table. */
   unsigned char code *width_table; 	 /* variable width table start adress */
};

/* define the range if characters in the font tables */
#define ASCII_BOT	0x20
#define ASCII_TOP	0x7E

/* font definition tables for the three fonts */
extern struct FONT_DEF code fonts[];

/* glyph bitmap tables for the three fonts */ 
extern unsigned char code normal_glyph_table[];
extern unsigned char code bold_glyph_table[];
extern unsigned char code bigbold_glyph_table[];

/* width tables for the two small fonts */
extern unsigned char code normal_width_table[];
extern unsigned char code bold_width_table[];


#endif
