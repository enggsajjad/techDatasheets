//-----------------------------------------------------------------------------------
//              GHI Electronics, LLC
//               Copyright (c) 2008
//               All rights reserved
//-----------------------------------------------------------------------------------
/*
 * You can use this file if you agree to the following:
 *
 * 1. This header can't be changed under any condition.
 *    
 * 2. This is a free software and therefore is provided with NO warranty.
 * 
 * 3. Feel free to modify the code but we ask you to provide us with
 *	  any bugs reports so we can keep the code up to date.
 *
 * 4. This code may ONLY be used with GHI Electronics, LLC products.
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

#ifndef _uALFAT_LIB_HEADER
#define _uALFAT_LIB_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////
// use the optional wrappers functions, comment the next line to reduce the library code size
#define _USE_READWRITE_WRAPERS_
/////////////////////////////////////////////

#include "Types.h"
#include "ErrorCode.h"


// Boot Loader Functions
void GHI_SendLoadFirmwareFromSDCommand(void);
void GHI_SendLoadFirmwareFromUSBCommand(void);

// Library Functions
int16 GHI_GetLibraryVersion(void);
int8 GHI_GetResult(void);
int8 GHI_GetVersion(int8* major,int8 * BCDminor);
int8 GHI_InitializeSD(void);
int8 GHI_InitializeUSB(void);
int8 GHI_ChangeDirectory(int8* dirname);
int8 GHI_MakeDirectory( int8* dirname);
int8 GHI_OpenFile(int8 filehandle, int8 *filename, int8 openmode);
int8 GHI_SendReadCommand(int8 filehandle,int32 datasize, int8 filler);
int8 GHI_GetReadAndWriteResult(int32 * actualdatasize);
int8 GHI_SendWriteCommand( int8 filehandle,int32 datasize );
int8 GHI_CloseFile(int8 filehandle);
int8 GHI_FlushFile(int8 filehandle);
int8 GHI_DeleteFile(int8 *filename);
int8 GHI_SeekFile(int8 filehandle, int32 newposition);
int8 GHI_GetFileInfo(int8 * filename, int32 * size, int8 * attributes, int32 *date_time);
int8 GHI_InitGetFile(void);
int8 GHI_GetNextFile(int8 * filename,int8 * fileext, int8 * attributes, int32 * size);
void GHI_StartMediStatistics(void);
int8 GHI_GetResultsMediaStatistics(int32 * size ,int32 * free);
int8 GHI_uALFATPowerMode(int8 mode, int16 baud);
void GHI_uALFATHibernate(void);
int8 GHI_uALFATWake(void);

#ifdef _USE_READWRITE_WRAPERS_

int8 GHI_ReadFile(int8 handle, int8 *buffer, int32 size, int8 filler, int32 *actualdatasize);
int8 GHI_WriteFile(int8 handle, int8 *buffer, int32 size, int32 *actualdatasize);

#endif

// RTC
int8 GHI_InitTime(int8 backup);
int8 GHI_SetTime(int32 time);
int8 GHI_GetTime(int32 * time);
int8 GHI_GetFormattedTime(int8 *buffer);

////////////////////////////////////////////////////////
// FAT Time Structure, a 32bits variable
typedef struct
{
	int32 seconds2:5;	// seconds divided by 2
	int32 minutes:6;
	int32 hours:5;
	int32 day:5;		
	int32 month:4;
	int32 years_since_1980:7;
}FAT_Time_Structure;

// Since some compilers don't support unaligned bits, therefore, we implement
// this function to return the 32bits variable that holds FAT Time Structure.
// Note that year is the regular years count , seconds is the regular seconds count
int32 GetFATTimeStructure(int32 year, int32 month, int32 day, int32 hours, int32 minutes, int32 seconds);
//////////////////////////////////////////////////////////////	

#ifdef __cplusplus

}

#endif

#endif
