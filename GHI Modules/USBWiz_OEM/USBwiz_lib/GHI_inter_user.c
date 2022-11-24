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

#include "GHI_inter_user.h"
#include "types.h"

#ifdef _USE_GHI_INTR_USER_
//------------------------------------------
void GHI_ToggleWakePin(void)
{
}
//------------------------------------------
void GHI_Sleep(int16 ms)
{
	// this is fine for out PIC runing on 32 Mhz
	// you will have to change this loop according to your speed
	int32 x,z;
	for(z=0;z<ms;z++)
		for(x=0;x<500;x++);
}
//------------------------------------------
int8 GHI_OpenInterface(void)
{
	return UNKNOWN_ERROR;
	
	return NO_ERROR;
}
//------------------------------------------
int8 GHI_CloseInterface(void)
{
	return UNKNOWN_ERROR;
}
//------------------------------------------
int8 GHI_GetC(void)
{
}
//------------------------------------------
void GHI_PutC(int8 ch)
{
}
//------------------------------------------
void GHI_PutS(int8 * str)
{
}
//------------------------------------------
int8 GHI_DataIsReady(void)
{
}
//------------------------------------------
#endif
