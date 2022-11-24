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


#include "GHI_HD44780_LCD.h"
#include "delays.h"


#define     CLR_DISP        0b00000001 //Clear display
#define     CUR_HOME        0b00000010    //Move cursor home and clear screen memory
#define     CUR_RIGHT       0b00010100    //Move cursor one to right
#define     CUR_LEFT        0b00010000    //Move cursor one to left
#define     SCROLL_RIGHT    0b00011100    //Scroll entire screen right one space
#define     SCROLL_LEFT     0b00011000    //Scroll entire screen left one space
#define     DISP_ON         0b00001100    //Turn visible LCD on
#define     DISP_OFF        0b00001000    //Turn visible LCD off
#define     UNDERLINE_ON    0b00001110    //Turn on underlined cursor
#define     UNDERLINE_OFF   0b00001100    //Turn off underlined cursor
#define     BLINKCUR_ON     0b00001101    //Turn on blinking box cursor
#define     BLINKCUR_OFF    0b00001100    //Turn off blinking box cursor
#define     DUALCUR_ON      0b00001111    //Turn on blinking box and underline cursor
#define     DUALCUR_OFF     0b00001100    //Turn off blinking box and underine cursor
#define     SET_CURSOR      0b10000000    //SET_CURSOR + X : Sets cursor position to X
#define     ENTRY_INC       0b00000110 //
#define     DD_RAM_ADDR     0b10000000 //
#define     DD_RAM_ADDR2    0b11000000 //
#define     CG_RAM_ADDR     0b01000000 //

void LCD_DelayMs(int16 x);
void LCD_SendCmd(int8 d);

//Initializes the 4-bit parallel interface to the HD44780
void LCD_Init(void)
{
	LCD_RS_TRIS = 0;
	LCD_R_W_TRIS = 0;
	LCD_E_TRIS = 0;
	
	LCD_RS_OUT = 0;               
	LCD_R_W_OUT = 0;

	LCD_D7_TRIS = 0;
	LCD_D6_TRIS = 0;
	LCD_D5_TRIS = 0;
	LCD_D4_TRIS = 0;
	
	// 4 bit data communication
	LCD_DelayMs(50);
	LCD_D7 = 0;
	LCD_D6 = 0;
	LCD_D5 = 1;
	LCD_D4 = 1;
	
	LCD_E = 1; 
	LCD_E = 0;
	
	LCD_DelayMs(5);
	LCD_DelayMs(50);
	LCD_D7 = 0;
	LCD_D6 = 0;
	LCD_D5 = 1;
	LCD_D4 = 1;
	
	LCD_E = 1; 
	LCD_E = 0;
	
	LCD_DelayMs(1);
	LCD_DelayMs(50);
	LCD_D7 = 0;
	LCD_D6 = 0;
	LCD_D5 = 1;
	LCD_D4 = 1;
	
	LCD_E = 1; 
	LCD_E = 0;
	
	LCD_DelayMs(5);
	LCD_DelayMs(50);
	LCD_D7 = 0;
	LCD_D6 = 0;
	LCD_D5 = 1;
	LCD_D4 = 0;
	
	LCD_E = 1; 
	LCD_E = 0;
	
	LCD_SendCmd(DISP_ON);
    LCD_SendCmd(CLR_DISP);

  
}    

void LCD_wait(void){
    unsigned char i = 1;
    unsigned char of=0;	
	LCD_D7_TRIS = 1;
	LCD_D6_TRIS = 1;
	LCD_D5_TRIS = 1;
	LCD_D4_TRIS = 1;

    LCD_R_W_OUT = 1; //Tell LCD to output status
    LCD_RS_OUT = 0;              

    while(i == 1)    {
        LCD_E = 1; 
        i = LCD_D7; //Read data bit 7 - Busy Flag
        LCD_E = 0;
    
        LCD_E = 1; 
		LCD_E = 0; //Toggle LCD_E to get the second four bits of the status byte - but we don't care
		
		of++;
		if(!of) LCD_Init();
    }
    	
	LCD_D7_TRIS = 0;
	LCD_D6_TRIS = 0;
	LCD_D5_TRIS = 0;
	LCD_D4_TRIS = 0;
}

//Sends an ASCII character to the LCD
void LCD_Putc(int8 c){
	int8 _c;
    LCD_wait();
    
    LCD_R_W_OUT = 0; //set LCD to write
    LCD_RS_OUT = 1; //set LCD to data mode
    
	_c = c>>4;
	LCD_D7 = (_c>>3)&1;
	LCD_D6 = (_c>>2)&1;
	LCD_D5 = (_c>>1)&1;
	LCD_D4 = (_c)&1;
    LCD_E = 1; LCD_E = 0; //Toggle the Enable Pin

	LCD_D7 = (c>>3)&1;
	LCD_D6 = (c>>2)&1;
	LCD_D5 = (c>>1)&1;
	LCD_D4 = (c)&1;
    LCD_E = 1; LCD_E = 0;
    LCD_DelayMs(1);
}

//Sends an LCD command
void LCD_SendCmd(int8 d){
	int8 _d;
	LCD_wait();
	
	LCD_R_W_OUT = 0; //set LCD to write
	
	_d = d>>4;
	LCD_D7 = (_d>>3)&1;
	LCD_D6 = (_d>>2)&1;
	LCD_D5 = (_d>>1)&1;
	LCD_D4 = (_d)&1;
		LCD_E = 1; LCD_E = 0;
	
	LCD_D7 = (d>>3)&1;
	LCD_D6 = (d>>2)&1;
	LCD_D5 = (d>>1)&1;
	LCD_D4 = (d)&1;
	LCD_E = 1; LCD_E = 0;	
    LCD_DelayMs(1);
}

void LCD_Putrs(const rom char * s){
	while(*s)
		LCD_Putc(*s++);
}
void LCD_Puts(const char * s){
	while(*s)
		LCD_Putc(*s++);
}

void LCD_DelayMs(int16 x){
	while(x--)
		Delay1KTCYx(8);
}
void  LCD_Clear(void) {
	LCD_SendCmd(CLR_DISP);
}

void  LCD_CursorHome(void) {
	LCD_SendCmd(CUR_HOME);
}
void  LCD_CursorRight(void) {
	LCD_SendCmd(CUR_RIGHT);
}

void  LCD_CursorLeft(void) {
	LCD_SendCmd(CUR_LEFT);
}
void  LCD_ScrollRight(void) {
	LCD_SendCmd(SCROLL_RIGHT);
}

void  LCD_ScrollLeft(void) {
	LCD_SendCmd(SCROLL_LEFT);
}
void  LCD_DisplayOn(void) {
	LCD_SendCmd(DISP_ON);
}
void  LCD_DisplayOff(void) {
	LCD_SendCmd(DISP_OFF);
}
void  LCD_UnderlineOn(void) {
	LCD_SendCmd(UNDERLINE_ON);
}
void  LCD_UnderlineOff(void) {
	LCD_SendCmd(UNDERLINE_OFF);
}
void  LCD_BlinkOn(void) {
	LCD_SendCmd(BLINKCUR_ON);
}
void  LCD_BlinkOff(void) {
	LCD_SendCmd(BLINKCUR_OFF);
}
void  LCD_SetCursor(int8 row,int8 col) {
	LCD_SendCmd(SET_CURSOR | row<<6 | col);
}

