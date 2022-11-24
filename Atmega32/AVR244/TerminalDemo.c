/*****************************************************************************
*
* Copyright (C) 2003 Atmel Corporation
*
* File              : TerminalDemo.c
* Compiler          : IAR EWAAVR 2.28a
* Version           : 2.1          
* Last modified     : 18-07-2003 by LTA
*
* Support mail      : avr@atmel.com
*
*
* Description       : Terminal demo application
*                     
*
*
****************************************************************************/

#include "TerminalDriver.h"

unsigned char __flash menu[] = "Choice A\nChoice B\nChoice C\n";

void main()
{
    unsigned char ret;

    Term_Initialise();                            // Initialize terminal server


    /* Handle menu and print result */
    ret = Term_Handle_Menu( menu, 1, 10, 10, 1 );
    Term_Erase_Screen();
    Term_Send( 'A' + ret - 1 );        
        

    while(1)    
    {
        Term_Send( Term_Get() );                // Echo forever...
    }
}
