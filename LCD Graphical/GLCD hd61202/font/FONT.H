/********************************************************************************
	NAME 		: FONT.H
	
	EXTENDED NAME	: Fonts for graphic LCD based on KS0108 or HD61202
	
	LAYER		: Application
	
	AUTHOR		: Stephane REY			  


REVISIONS :
 				
--------------------------------------------------------------------------------
DATE		VERSION	REVISOR DESCRIPTION				
--------------------------------------------------------------------------------
13.02.2003	V1.0 	SR	Initialization
--------------------------------------------------------------------------------
				
********************************************************************************/

/*******************************************************************************/
/* 			     DECLARATIONS / DEFINITIONS			       */
/*******************************************************************************/

/* EXTERN Function Prototype(s) */

/* Extern definitions */

typedef struct FONT_DEF 
{
	U8 u8Width;     	/* Character width for storage         */
	U8 u8Height;  		/* Character height for storage        */
	U8 *au8FontTable;       /* Font table start address in memory  */
} FONT_DEF;

extern code FONT_DEF Font_System3x6;
extern code FONT_DEF Font_System5x8;
extern code FONT_DEF Font_System7x8;
extern code FONT_DEF Font_Courrier8x12;

extern code U8 au8FontSystem3x6[];
extern code U8 au8FontSystem5x8[];
extern code U8 au8FontSystem7x8[];
extern code U8 au8FontCourrier8x12[];

