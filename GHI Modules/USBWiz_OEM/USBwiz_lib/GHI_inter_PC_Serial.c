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

#include "GHI_inter_PC_Serial.h"
#include "types.h"

#ifdef _USE_GHI_INTR_PC_SERIAL_

#include <stdio.h>
#include <string.h>
HANDLE hCom;

unsigned int nPortNumber=4;
unsigned nBaudRate=9600;

DWORD nOutputBufferSize = 16;
DWORD nInputBufferSize = 4096;
BOOL bState = FALSE;

//-----------------------------------
unsigned char SetReadingTimeout(unsigned long dwMillisec);
//-----------------------------------------------------
//this Function returns tthe size of read data.
unsigned int GHI_GetBuffer(void * data,unsigned int  SizeOfData)
{
	BOOL  bResult = FALSE;
 DWORD SizeOfReadData;
 if(bState)
 {
  //PurgeComm(hCom,PURGE_RXCLEAR);   //clear buffer before reading
  while( !bResult)
  {
   bResult =  ReadFile(hCom,data,SizeOfData,&SizeOfReadData,NULL);
   }
  return SizeOfReadData;///////Done
  }
 else
     return (0);////Disconnected
}
//-----------------------------------------------------------------------------
//this Function returns the size of written data.
unsigned int GHI_PutBuffer(const void * data, unsigned int SizeOfData)
{
 BOOL bResult = FALSE;
 DWORD SizeOfWrittenData;
	COMSTAT comstat;
	unsigned long error_codes;

 if(bState)
 {
  //PurgeComm(hCom,PURGE_TXCLEAR);   //clear buffer before reading
  bResult = WriteFile(hCom,data,SizeOfData,&SizeOfWrittenData,NULL);
  if(SizeOfData!=SizeOfWrittenData)
	  while(1);
  if(!bResult)
     return (0); //Error writing
  else
  {

	  do{
		  ClearCommError(hCom, &error_codes, &comstat);
	  }while(comstat.cbOutQue);
	  return 1;
  }
  }
  else
      return (0); //Disconnected
}
//----------------------------------------------------------------------------


//------------------------------------------
void GHI_ToggleWakePin(void)
{
}
//------------------------------------------
void GHI_Sleep(int16 ms)
{
	// this is fine for out PIC runing on 32 Mhz
	// you will have to change this loop according to your speed
	Sleep(ms);
}
//------------------------------------------
int8 GHI_OpenInterface(void)
{
	  DCB dcb;
  COMMTIMEOUTS Commtimeouts;
  BOOL fSuccess;
  char pcCommPort[6];
  if (hCom != NULL && hCom != INVALID_HANDLE_VALUE )
     return TRUE;     /// already in use

  
  if(sprintf(pcCommPort,"COM%i",nPortNumber)== 0) return FALSE;
  printf("Openning port %s\r\n",pcCommPort);

  hCom = CreateFile( pcCommPort,
                     GENERIC_READ | GENERIC_WRITE,
                     0,    // comm devices must be opened w/exclusive-access
                     NULL, // no security attributes
                     OPEN_EXISTING, // comm devices must use OPEN_EXISTING
                     0,    // not overlapped I/O
                     NULL  // hTemplate must be NULL for comm devices
                     );

  if (hCom == INVALID_HANDLE_VALUE)
      return FALSE;     // error creating file

  // We will build on the current configuration, and skip setting the size
  // of the input and output buffers with SetupComm.

  fSuccess = GetCommState(hCom, &dcb);

  if (!fSuccess) return FALSE;  // error Getting  serial port settings


  dcb.BaudRate = nBaudRate;     // set the baud rate
  dcb.ByteSize = 8;             // data size, xmit, and rcv
  dcb.Parity = NOPARITY;        // no parity bit
  dcb.StopBits = ONESTOPBIT;    // one stop bit
    
  dcb.fDtrControl = DTR_CONTROL_DISABLE;
  //dcb.fRtsControl = RTS_CONTROL_DISABLE;
  dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	
  fSuccess = SetCommState(hCom, &dcb);
  if (!fSuccess) return FALSE; // error setting serial port
  fSuccess = SetupComm(hCom,nInputBufferSize,nOutputBufferSize);
  PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT |
                       PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

  if(GetCommTimeouts(hCom,&Commtimeouts))
  {
   Commtimeouts.ReadTotalTimeoutConstant = 5000;
   Commtimeouts.ReadIntervalTimeout=100;
   Commtimeouts.ReadTotalTimeoutMultiplier = 0;
   if(!SetCommTimeouts(hCom,&Commtimeouts))return FALSE;
   }
   else return FALSE;

  bState = TRUE;
  SetReadingTimeout(6000);
  
  return TRUE;

	
	
 	
}
//------------------------------------------
int8 GHI_CloseInterface(void)
{
	BOOL bstate;
	if(hCom == NULL  || hCom == INVALID_HANDLE_VALUE )
         if( bState == FALSE )
            return TRUE; //already closed
         else
             return FALSE;//error closing the file
	else{
      PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT |
                       PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
     bstate = CloseHandle(hCom) ;
     hCom = NULL;
     bState  = FALSE;
     if(bstate)return TRUE;// successfully closed
     else return FALSE;    // error closing the file
     }
}
//------------------------------------------
int8 GHI_GetC(void)
{
	
	unsigned char ch;
	
	GHI_GetBuffer(&ch,1);
	return ch;
}
//------------------------------------------
void GHI_PutC(int8 ch)
{
	
	GHI_PutBuffer(&ch,1);
	
}
//------------------------------------------
void GHI_PutS(int8 * str)
{
	GHI_PutBuffer(str,strlen(str));
}
//------------------------------------------
int8 GHI_DataIsReady(void)
{
	COMSTAT comstat;
	unsigned long error_codes;
	
	if(!ClearCommError(hCom, &error_codes, &comstat))
		return 0;
	if(error_codes)
		return 0;
	if(comstat.cbInQue)
		return 1;
	return 0;
}
//------------------------------------------
unsigned char SetReadingTimeout(unsigned long dwMillisec)
{
 COMMTIMEOUTS Commtimeouts;
 if(GetCommTimeouts(hCom,&Commtimeouts))
 {
  Commtimeouts.ReadTotalTimeoutConstant = dwMillisec ;
  return SetCommTimeouts(hCom,&Commtimeouts);
  }else return FALSE;
}
//----------------------------------------------------
#endif
