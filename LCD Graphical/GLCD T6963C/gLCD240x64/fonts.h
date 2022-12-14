#ifndef _FONTS_H
#define _FONTS_H

/* select desired fonts. (Simply comment out those not needed) */
#define EN_FIVE_DOT
#define EN_SIX_DOT
#define EN_SEVEN_DOT
#define EN_NINE_DOT
#define EN_TEN_DOT
#define EN_FIFTEEN_DOT
#define EN_EIGHTEEN_DOT

/* define number labels for the font selections */
typedef enum
{
#ifdef EN_FIVE_DOT
	FONT_FIVE_DOT,
#endif

#ifdef EN_SIX_DOT
	FONT_SIX_DOT,
#endif

#ifdef EN_SEVEN_DOT
	FONT_SEVEN_DOT,
#endif

#ifdef EN_NINE_DOT
	FONT_NINE_DOT,
#endif

#ifdef EN_TEN_DOT
	FONT_TEN_DOT,
#endif

#ifdef EN_FIFTEEN_DOT
	FONT_FIFTEEN_DOT,
#endif

#ifdef EN_EIGHTEEN_DOT
	FONT_EIGHTEEN_DOT,
#endif

	FONT_COUNT
} FONT_BASE;

struct FONT_DEF 
{
   unsigned char store_width;            /* glyph storage width in bytes */
   unsigned char glyph_height;  		 /* glyph height for storage */
   unsigned char code *glyph_table;      /* font table start address in memory */
   unsigned char fixed_width;            /* fixed width of glyphs. If zero */
                                         /* then use the width table. */
   unsigned char code *width_table; 	 /* variable width table start adress */
   unsigned char glyph_beg;			 	 /* start ascii offset in table */
   unsigned char glyph_end;				 /* end ascii offset in table */
   unsigned char glyph_def;				 /* code for undefined glyph code */
};

/* font definition tables for the three fonts */
extern struct FONT_DEF code fonts[FONT_COUNT];

/* glyph bitmap and width tables for the fonts */ 
#ifdef EN_FIVE_DOT
  extern unsigned char code five_dot_glyph_table[];
  extern unsigned char code five_dot_width_table[];
#endif

#ifdef EN_SIX_DOT
  extern unsigned char code six_dot_glyph_table[];
  extern unsigned char code six_dot_width_table[];
#endif

#ifdef EN_SEVEN_DOT
  extern unsigned char code seven_dot_glyph_table[];
  extern unsigned char code seven_dot_width_table[];
#endif

#ifdef EN_NINE_DOT
  extern unsigned char code nine_dot_glyph_table[];
#endif

#ifdef EN_TEN_DOT
  extern unsigned char code ten_dot_glyph_table[];
#endif

#ifdef EN_FIFTEEN_DOT
  extern unsigned char code fifteen_dot_glyph_table[];
  extern unsigned char code fifteen_dot_width_table[];
#endif

#ifdef EN_EIGHTEEN_DOT
  extern unsigned char code eighteen_dot_glyph_table[];
  extern unsigned char code eighteen_dot_width_table[];
#endif

#endif
