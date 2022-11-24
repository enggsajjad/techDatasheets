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


#include "Types.h"  



#define     LCD_RS              LATDbits.LATD6
#define     LCD_R_W             LATDbits.LATD5
#define     LCD_E               LATDbits.LATD4
#define     LCD_RS_TRIS         TRISDbits.TRISD6
#define     LCD_R_W_TRIS        TRISDbits.TRISD5
#define     LCD_E_TRIS          TRISDbits.TRISD4
#define     LCD_RS_OUT          LATDbits.LATD6
#define     LCD_R_W_OUT         LATDbits.LATD5
#define     LCD_D4              PORTDbits.RD0 
#define     LCD_D5              PORTDbits.RD1
#define     LCD_D6              PORTDbits.RD2
#define     LCD_D7              PORTDbits.RD3
#define    	LCD_D4_TRIS         TRISDbits.TRISD0 
#define     LCD_D5_TRIS         TRISDbits.TRISD1
#define     LCD_D6_TRIS         TRISDbits.TRISD2
#define     LCD_D7_TRIS         TRISDbits.TRISD3



void LCD_Init(void);

void LCD_Putc(int8);
void LCD_Putrs(const rom char*);
void LCD_Puts(const char*);

void LCD_Clear(void);
void LCD_CursorHome(void);
void LCD_CursorRight(void);
void LCD_CursorLeft(void);
void LCD_ScrollRight(void);
void LCD_ScrollLeft(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_UnderlineOn(void);
void LCD_UnderlineOff(void);
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);
void LCD_SetCursor(int8 row,int8 col);

