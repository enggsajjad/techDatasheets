/********************************************************************************
	NAME 		: GLCD.H
	
	EXTENDED NAME	: 128x64 Graphic LCD management for KS0108 driver
	
	LAYER		: Application
	
	AUTHOR		: Stephane REY			  


REVISIONS :
 				
--------------------------------------------------------------------------------
DATE		VERSION	REVISOR DESCRIPTION				
--------------------------------------------------------------------------------
20.10.2002	V1.0 	SR	Initialization

--------------------------------------------------------------------------------
				
********************************************************************************/

/*******************************************************************************/
/* 			     DECLARATIONS / DEFINITIONS			       */
/*******************************************************************************/

/* EXTERN Function Prototype(s) */

void GLCD_LcdInit();	
void GLCD_ClearScreen ();
void GLCD_DisplayPicture (U8*);
void GLCD_Locate (U8, U8);
void GLCD_Rectangle (U8, U8, U8, U8);
void GLCD_Printf (U8*, FONT_DEF*);
void GLCD_DisplayValue (U32, U8, U8) ;
void GLCD_Circle(U8, U8, U8);

/* Extern definitions */

/* Hardware */
#define LCD_DI		P0_0  	
#define LCD_RW		P0_1  	
#define LCD_E		P0_2  	
#define LCD_CS1		P0_3  	
#define LCD_CS2		P0_4  	
#define LCD_RST		P0_5  	
#define LCD_DATA	P2    

/* LCD Registers */
#define X_ADRESS	0xB8	/* Adress base for Page 0 */
#define Y_ADRESS	0x40	/* Adress base for Y0	  */
#define START_LINE	0xC0	/* Adress base for line 0 */
#define DISPLAY_ON	0x3F	/* Turn display on	  */
#define DISPLAY_OFF	0x3E	/* Turn display off	  */

/* General use definitions */
#define RIGHT		0
#define LEFT		1
#define BUSY		0x80

