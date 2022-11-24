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
 * 3. Feel free to modify the codes but we ask you to provide us with 
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
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////
 //				     	 Refer to the library manual for function documentation     			   //
 ////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _GHI_USBWIZ_HEADER
#define _GHI_USBWIZ_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "Types.h"
#include "ErrorCode.h"

////////////////////////////////////////////////////////////////////////////////////

#define GHI_USBWIZ_LIBRARY_VERSION 0x100

////	INCLUDE DEFINES	////

// for UpdateFirmware() and ReadDeviceInfo()...
#define	_INCLUDE_EXTRA_COMMANDS_SUPPORT_

#define	_INCLUDE_FAT_SYSTEM_SUPPORT_
#ifdef	_INCLUDE_FAT_SYSTEM_SUPPORT_
	#define	_INCLUDE_EXTENDED_FAT_SYSTEM_SUPPORT_
	#define	_INCLUDE_READWRITE_WRAPPERS_SUPPORT_
#endif

#define _INCLUDE_TIME_WORK_SUPPORT_

#define	_INCLUDE_STORAGE_SECTOR_SUPPORT_

#define _INCLUDE_HID_SUPPORT_
#define _INCLUDE_PRINTER_SUPPORT_
#define _INCLUDE_SERIAL_DEVICES_SUPPORT_

// Do not use unless necessary
//#define _INCLUDE_OLD_STORAGE_FUNCTIONS_WRAPPERS_SUPPORT_

////				////

#define SD_DEVICE							'S'
#define USB_DEVICE_PORT_0					'0'
#define USB_DEVICE_PORT_1					'1'

#define FILE_READ_MODE						'R'
#define FILE_WRITE_MODE						'W'
#define FILE_APPEND_MODE					'A'

#define FILE_HANDLE_0						0
#define FILE_HANDLE_1						1
#define FILE_HANDLE_2						2
#define FILE_HANDLE_3						3

#define SERIAL_CDC_DEVICE					'C'
#define SERIAL_SILABS_DEVICE				'S'
#define SERIAL_PROLIFIC1_DEVICE				'P'
#define SERIAL_PROLIFIC2_DEVICE				'R'
#define SERIAL_FTDI_DEVICE					'F'

//////////////////////////////////////////

// Get Library Version, this function uses BCD, if the return value is 0x205 the version is 2.05
int16 GHI_GetLibraryVersion(void);

// checks are returns the error code from USBwiz
int8 GHI_GetResult(void);

// This get usbwiz version
// the minor version number is BCD. ie: 0x10 = version number 10
int8 GHI_GetVersion(int8* major,int8 * BCDminor);

int8 GHI_SetUSBwizBaudRate(int16 bauderate);

#ifdef	_INCLUDE_EXTRA_COMMANDS_SUPPORT_
	// device info structure
	typedef struct
	{
		int16	vendor_id;
		int16	product_id;
		int8	number_of_interfaces;
		int8	interface1_class;		// only get the first interface
		int8 	interface1_subclass;
		int8	interface1_protocol;
	}DEVICE_INFO;
	int8 GHI_ReadDeviceInfo(int8 device, DEVICE_INFO* info);
	
	int8 GHI_UpdateFirmware(int8 device);
	
	int8 GHI_SoftwareReset(void);

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//								Storage And FAT System functions										 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef	_INCLUDE_FAT_SYSTEM_SUPPORT_
	int8 GHI_MountFATFileSystem(int8 device);
	int8 GHI_SwitchDevice(int8 device);
	
	int8 GHI_ChangeDirectory(int8* filename);
	int8 GHI_MakeDirectory(int8* filename);
	
	int8 GHI_OpenFile(int8 filehandle, int8 *filename, int8 openmode);
	int8 GHI_SendReadCommand(int8 filehandle, int32 desireddatasize, int8 filler);
	int8 GHI_SendWriteCommand(int8 filehandle, int32 desireddatasize);
	int8 GHI_GetReadAndWriteResult(int32 * actualdatasize);
	
	int8 GHI_CloseFile(int8 filehande);
	int8 GHI_DeleteFile(int8 *filename);
#endif

#ifdef _INCLUDE_STORAGE_SECTOR_SUPPORT_
	// use register media for read/write sector only
	int8 GHI_RegisterSD(void);
	int8 GHI_RegisterUSBMassStorage(int8 device);
	int8 GHI_ReadSector(int32 sectornum);
	int8 GHI_WriteSector(int32 sectornum);
	#ifndef	_INCLUDE_FAT_SYSTEM_SUPPORT_
		int8 GHI_SwitchDevice(int8 device);
	#endif
#endif

#ifdef	_INCLUDE_EXTENDED_FAT_SYSTEM_SUPPORT_
	// To get media statistics
	void GHI_StartMediaStatistics (void);
	int8 GHI_GetResultMediaStatistics(int32 * size, int32 * free);
	
	// must use GHI_GetResult() after calling this function
	// which returns any errors
	void GHI_StartQformat(void);
	
	int8 GHI_InitGetFile(void);
	int8 GHI_GetNextFile(int8 * file_name, int8 * file_ext, int8 * attributes, int32 * size);
	
	int8 GHI_RemoveDirectory(int8* filename);
	
	// must use GHI_GetReadAndWriteResult(int32 * actualdatasize)
	// after successfully calling this function
	int8 GHI_SendReadWriteFileCommand(int8 readhandle, int8 writehandle, int32 size);

	int8 GHI_SendShadowWriteTwoFiles(int8 firstfilehandle, int8 secondfilehandle, int32 desireddatasize);
	int8 GHI_GetShadowWriteTwoFileResults(int8 *fresult1, int8 *fresult2, int32 *writtendata1, int32 *writtendata2);

	int8 GHI_SendShadowWriteThreeFiles(int8 firstfilehandle, int8 secondfilehandle, int8 thirdfilehandle, int32 desireddatasize);
	int8 GHI_GetShadowWriteThreeFileResults(int8 *fresult1, int8 *fresult2, int8 *fresult3, int32 *writtendata1, int32 *writtendata2, int32 *writtendata3);
 
	int8 GHI_SeekFile(int8 filehandle, int32 newposition);
	int8 GHI_GetPointerPosition(int8 filehandle, int32 *sector, int16 *positioninsector);

	int8 GHI_SplitFile(int8 sourcehandle, int8 desthandle1,  int8 desthandle2, int32 splitposition, int32 * actualdestsize1, int32 * actualdestsize2);
	int8 GHI_FlushFile(int8 filehandle); 
	
	int8 GHI_GetFileInfo(int8 *filename, int32 *size, int8 *attributes, int32 *TimeDate);
	int8 GHI_RenameFile(int8 *filename,int8 *newname);

	int8 GHI_SetFileSize(int8 filehandle,int32 newsize);
#endif

#ifdef _INCLUDE_READWRITE_WRAPPERS_SUPPORT_
	int8 GHI_ReadFile(int8 handle, int8 *buffer, int32 size, int8 filler, int32 *actualdatasize);
	int8 GHI_WriteFile(int8 handle, int8 *buffer, int32 size, int32 *actualdatasize);
#endif

#ifdef	_INCLUDE_TIME_WORK_SUPPORT_		
	int8 GHI_InitializeTime(int8 backup);
	int8 GHI_SetTime(int32 time);
	int8 GHI_GetTime(int32 * time);
	// This is a null terminated buffer that holds the time in a formatted way. It's size must be at least 22bytes
	int8 GHI_GetFormattedTime(int8 *buffer);

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

	int32 GHI_GetFATTimeStructure(int32 year, int32 month, int32 day, int32 hours, int32 minutes, int32 seconds);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   					     					USB Driver Functions                              				   //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef	_INCLUDE_HID_SUPPORT_
	// Human Interface Device only - Mouse, Keyboard, Joystick
	int8 GHI_RegisterHID(int8 device);
	// must call GHI_GetResult after successfully calling this function and reading the data
	int8 GHI_PrepareHIDReport(int8 device, int8 requested_size, int8 * actual_reportsize);
#endif

#ifdef	_INCLUDE_PRINTER_SUPPORT_
	int8 GHI_RegisterPrinter(int8 device);
	int8 GHI_ResetPrinter(int8 device);
	int8 GHI_GetPrinterStatus(int8 device, int8 * status);
	// must use GHI_GetResult() after successfully
	// calling this function which returns any errors
	int8 GHI_PrinterPrint(int8 device, int8 size);
#endif

#ifdef	_INCLUDE_SERIAL_DEVICES_SUPPORT_
	// Serial Devices ( CDC, FTDI, Prolific... )
	int8 GHI_RegisterSerialDevice(int8 device, int8 type, int32 baudrate);

	// must use GHI_GetResult() after successfully
	// calling these functions and reading/writing data
	// * GHI_GetResult() returns error codes
	int8 GHI_SerialWrite(int8 device, int8 size);
	int8 GHI_SerialRead(int8 device, int8 *sent_data_size);
#endif

#ifdef _INCLUDE_OLD_STORAGE_FUNCTIONS_WRAPPERS_SUPPORT_
	int8 GHI_BL_LoadFirmware(int8 drive);
	int8 GHI_GetResults(void);
	int8 GHI_SetARMBaudRate(int32 bauderate);
	int8 GHI_AttachStorageMedia(int8 device, int8 deviceorder, int8 LUN);
	int8 GHI_ReadSectorFromCurrentFileSystem(int32 sectornum);
	int8 GHI_WriteSectorFromCurrnetFileDydtem(int32 sectornum);
	int8 GHI_MountFileSystem(int8 filesystemorder, int8 device, int8 deviceorder);
	int8 GHI_SwitchFileSystem(int8 filesystemorder);
	int8 GHI_EnumurateUSBDevicetoRootHub(int8 usbport, int8 usbdevicehandle);
	int8 GHI_ReleaseUSBDeviceHandle(int8 usbdevicehandle);
	int8 GHI_RegisterMassStorageDevice(int8 usbdevicehandle, int8 massstoragedevicehandle, int8 *lastLUN);
	int8 InitializeFATMedia(int8 DriveLetter);
	int8 SwitchToFATMedia(int8 DriveLetter);
	int8 GHI_GetResultsQformat(void);
#endif

////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _GHI_USBWIZ_HEADER
