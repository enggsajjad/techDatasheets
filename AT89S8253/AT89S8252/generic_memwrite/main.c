/*-----------------------------------------------------------------------------
MAIN.C

This program fills memory areas with stuff.
-----------------------------------------------------------------------------*/
#include <string.h>    // Include prototypes for memcpy function

unsigned char idata idata_buffer [16] _at_ 0xB0;
unsigned char data  data_buffer  [16] _at_ 0x60;
unsigned char pdata pdata_buffer [16] _at_ 0xFF60;
unsigned char xdata xdata_buffer [16] _at_ 0x0200;

void main (void)
{
/*---------------------------------------------------------
Fill idata_buffer with something.
---------------------------------------------------------*/
memcpy (idata_buffer, "IDATA Buffer....", sizeof (idata_buffer));

/*---------------------------------------------------------
Fill data_buffer with something.
---------------------------------------------------------*/
memcpy (data_buffer, "DATA Buffer......", sizeof (data_buffer));

/*---------------------------------------------------------
Fill pdata_buffer with something.
---------------------------------------------------------*/
memcpy (pdata_buffer, "PDATA Buffer -- NEVER USE :-)", sizeof (pdata_buffer));

/*---------------------------------------------------------
Fill xdata_buffer with something.
---------------------------------------------------------*/
memcpy (xdata_buffer, "XDATA Buffer....", sizeof (xdata_buffer));

/*---------------------------------------------------------
Delay Forever - Embedded Programs NEVER Return from Main!
---------------------------------------------------------*/
while (1);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

