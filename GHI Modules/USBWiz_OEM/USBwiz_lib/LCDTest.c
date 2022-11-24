/*
 * Copyright (c) 2006, GHI Electronics, LLC
 * All rights reserved. 
 *
 * You can use this file if you agree to the following
 *
 * 1. This header can't be changed under any condition
 *    
 * 2. This is free software and therefore is providede with NO warranty 
 * 
 * 3. Fell free to modify the codes but we ask you to provide us with 
 *	  any bugs reports so we can keep the code up to date
 *
 * 4. This code may ONLY be used with GHI Electronics, LLC products
 *
 * THIS SOFTWARE IS PROVIDED BY GHI ELECTRONICS, LLC ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL 
 * GHI ELECTRONICS, LLC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR ORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 *	Specs are subject to change without any notice
 */


#include "LCDTest.h"
#include <stdio.h>
#include "USBwiz_lib.h"
#include "GHI_HD44780_LCD.h"


int8 LCDTest(void)
{
	// initialze LCD
	LCD_Init();
	LCD_Clear();
	LCD_CursorHome();
	//
			
	LCD_Putrs(ROM_TYPE"USBwiz ROCKS!!!"); 		// print something
			
	printf(ROM_TYPE"Press S2 button to move the text to the left.\r"
				   "Press S4 button to move the text to the right.\r"
				   "Press S3 button to exit.\r");
	while(1)
	{
		if(PORTEbits.RE0)	// s2 is pressed , move left
		{
			while(PORTEbits.RE0);	// wait till the button releases
			LCD_ScrollLeft();	
		}
		else if(PORTEbits.RE2)	// s4 is pressed, move right
		{
			while(PORTEbits.RE2);	// wait till the button releases
			LCD_ScrollRight();
		}
		else if(PORTEbits.RE1)	// s3 is pressed, exit!
		{
			return ERROR_NO_ERROR;
		}
	}
	
}
