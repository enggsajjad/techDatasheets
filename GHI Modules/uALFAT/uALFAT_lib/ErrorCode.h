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

// ErrorCode.h

#ifndef _ERROR_CODE_
#define _ERROR_CODE_

#ifdef  __cplusplus

extern "C" {

#endif

#include "Types.h"

extern int8 fresult;

/////////////////////////////////////////////////////////////////////////////////////////////

#define ERROR_NO_ERROR				0x00

// Hardware Error codes
#define ERROR_READ_SECTOR			0x01
#define ERROR_WRITE_SECTOR			0x02
#define ERROR_ERASE_SECTOR			0x03
#define ERROR_SD_UNEXPECTED_VALUE	0x04
#define ERROR_MMC_INIT_TIMEOUT		0x05
#define ERROR_SET_BLOCK_SIZE_FAIL	0x06
#define ERROR_MMC_SEND_COMMAND_FAIL 0x07

// FAT_mount Error codes
#define ERROR_INVALID_BOOT_SECTOR		 0x10
#define ERROR_MBR_SIGNATURE_MISSMATCH    0x11
#define ERROR_BS_SIGNATURE_MISSMATCH     0x12
#define ERROR_SECTOR_SIZE_NOT_512        0x13
#define ERROR_FSINFO_SIGNATURE_MISSMATCH 0x14
#define ERROR_FAT12_NOT_SUPPORTED        0x15
#define ERROR_FAT16_NOT_SUPPORTED		 0x16
#define ERROR_FAT16_FAT32_NOT_SUPPORTED	 0x17

/// FAT functions
#define ERROR_CLUSTER_OVER_RANGE             0x21
#define ERROR_CLUSTER_UNDER_RANGE            0x22
#define ERROR_NEXT_CLUSTER_VALUE_OVER_RANGE  0x23
#define ERROR_NEXT_CLUSTER_VALUE_UNDER_RANGE 0x24
#define ERROR_NO_FREE_CLUSTERS               0x25

//FAT_filename Error codes
#define ERROR_FILE_NAME_FORBIDDEN_CHAR       0x31
#define ERROR_FILE_NAME_DIR_NAME_OVER_8      0x32
#define ERROR_FILE_NAME_DIR_EXTENSION_OVER_3 0x33
#define ERROR_FILE_NAME_FIRST_CHAR_ZERO      0x34
#define ERROR_MEDIA_FULL                     0x35

//FAT main functions Error Codes
#define DIR_ENT_FOUND                                   0x40
#define DIR_ENT_NOT_FOUND                               0x41
#define ERROR_FOLDER_IS_CORRUPTED_FIRST_CLUSTER         0x42
#define ERROR_FOLDER_IS_CORRUPTED_DIR_DOT_NOT_FOUND     0x43
#define ERROR_FOLDER_IS_CORRUPTED_DIR_DOTDOT_NOT_FOUND  0x44
#define ERROR_ROOT_DIRECTORY_IS_FULL                    0x45
#define ERROR_OPEN_FOLDER_FILE							0x46
#define ERROR_WRTIE_TO_READ_MODE_FILE					0x47
#define ERROR_SEEK_REQUIER_READ_MODE					0x48
#define ERROR_INVALID_SEEK_POINTER						0x49
#define ERROR_FOLDER_NOT_EMPTY							0x4A
#define ERROR_IS_NOT_FOLDER								0x4B
#define ERROR_READ_MODE_REQUIRED						0x4C
#define ERROR_END_OF_DIR_LIST							0x4D
#define ERROR_FILE_PARAMETERS							0x4E
#define ERROR_HANDLE_IN_USE								0x4F
#define ERROR_INVALID_HANDLE							0X4F
#define ERROR_ZERO_SIZE_FILE							0x50
#define ERROR_FILE_MODE									0x51
#define ERROR_FILE_IS_CORRUPTED							0x52
#define ERROR_POINTER_IS_OUT_OF_BOUNDS					0x53

// FAT Driver
#define ERROR_ATTACH_IDE_DEVICE_RESET_TIMEOUT			0x54
#define ERROR_ATTACH_IDE_DEVICE_IDENTIFY_TIMEOUT		0x55
#define ERROR_READ_IDE_SECTOR_TIMEOUT					0x56
#define FILE_UNKNOWN_DRIVE_LETTER						0x57

// Commander Error Code
#define COMMANDER_PASS							0x00
#define ERROR_COMMANDER_BAD_COMMAND             0x61
#define ERROR_COMMANDER_STR_LEN_TOO_LONG        0x62
#define ERROR_COMMANDER_NAME_NOT_VALID          0x63
#define ERROR_COMMANDER_NUMBER_INVALID          0x64
#define ERROR_COMMANDER_WRITE_PARTIAL_FAILURE   0x65
#define ERROR_COMMANDER_UNKNOWN_MEDIA_LETTER    0x66
#define ERROR_COMMANDER_FAILED_TO_OPEN_MEDIA    0x67
#define ERROR_COMMANDER_INCORRECT_CMD_PARAMETER 0x68
#define ERROR_CHECK_SUM							0x69

#define USBD													 0x70
#define ERROR_USBD_NO_ENOUGH_PIPES							USBD+0x01
#define ERROR_USBD_HANDLE_INUSE 							USBD+0x02
#define ERROR_USBD_INCORRECT_DESCRIPTOR 					USBD+0x03
#define ERROR_USBD_NONCONTROL_TRANSFER_FUNCTION 			USBD+0x04
#define ERROR_USBD_DATA_SIZE_IS_BIG_FOR_ENDPOINT			USBD+0x05
#define ERROR_USBD_TIMEOUT									USBD+0x06
#define ERROR_USBD_CONTROL_TRANSFER_REQUIERED				USBD+0x07
#define ERROR_USBD_NACK 									USBD+0x08
#define ERROR_USBD_HANDLE_CORRUPTED 						USBD+0x09
#define ERROR_USBD_DESCRIPTOR_CORRUPTED 					USBD+0x0A
#define ERROR_DESCRIPTOR_NOT_FOUND 							USBD+0x0B
#define ERROR_USB_HUB_NOT_FOUND 							USBD+0x0C
#define ERROR_HCD_USB_DEVICE_NOT_CONNECTED					USBD+0x0D

// MassStorage driver
#define ERROR_BOMS														   0x80
#define ERROR_BOMS_CSW_COMMAND_FAILD							ERROR_BOMS+0x01
#define ERROR_BOMS_CSW_STATUS_PHASE_ERROR						ERROR_BOMS+0x02
#define ERROR_BOMS_CSW											ERROR_BOMS+0x03
#define ERROR_BOMS_WORNG_LUN_NUMBER								ERROR_BOMS+0x04
#define ERROR_BOMS_WORNG_CSW_SIGNATURE							ERROR_BOMS+0x05
#define ERROR_BOMS_WORNG_TAG_MISSMATCHED						ERROR_BOMS+0x06
#define ERROR_USB_MASSSTORAGE_COMMAD_FAILED						ERROR_BOMS_CSW_COMMAND_FAILD

#define MASSSTORAGE																0xA0
#define ERROR_USB_MASS_STORAGE_DEVICE_NOT_READY						MASSSTORAGE
#define ERROR_USB_MASSSTORAGE_PROTOCOL_NOT_SUPPORTED				MASSSTORAGE+0x01
#define ERROR_USB_MASSSTORAGE_SUBCLASS_NOT_SUPPORTED				MASSSTORAGE+0x02
#define ERROR_SPC_INVALID_SENSE										MASSSTORAGE+0x03
#define ERROR_SPC_NO_ASC_ASCQ										MASSSTORAGE+0x04
#define ERROR_USB_MASSSTORAGE_NOT_FOUND								MASSSTORAGE+0x05

#define ERROR_HCD_INIT_FAIL			0x90
#define ERROR_HCD_INDEFINED_CASE    0x91

#define	HCD						0xB0
#define ERROR_HCD_BUSY      HCD+0x01
#define ERROR_HCD_BADREQ    HCD+0x02
#define ERROR_HCD_UNDEF     HCD+0x03
#define ERROR_HCD_NAK       HCD+0x04
#define ERROR_HCD_STALL     HCD+0x05
#define ERROR_HCD_TOGERR    HCD+0x06
#define ERROR_HCD_WRONGPID  HCD+0x07
#define ERROR_HCD_BADBC     HCD+0x08
#define ERROR_HCD_PIDERR    HCD+0x09
#define ERROR_HCD_PKTERR    HCD+0x0A
#define ERROR_HCD_CRCERR    HCD+0x0B
#define ERROR_HCD_KERR      HCD+0x0C
#define ERROR_HCD_JERR      HCD+0x0D
#define ERROR_HCD_TIMEOUT	HCD+0x0E
#define ERROR_HCD_BABBLE    HCD+0x0F

// Long name errors
#define ERROR_NAME_TOO_LONG						0xC0
#define ERROR_LEAD_CHAR_INVALID					0xC1
#define ERROR_TRAIL_CHAR_INVALID				0xC2
#define ERROR_PARSER_INVALID					0xC3
#define ERROR_PARSER_SUCC						0xC4
#define ERROR_MAX_NAMES							0xC5
#define ERROR_NON_ASCII_FOUND					0xC6
#define ERROR_LONG_NAME_BUFFER_NOT_FOUND		0xC7


#define ERROR_FLASH_NOT_BLANK				0xD1
#define ERROR_VERIFY						0xD2
#define ERROR_INTERNAL						0xD3
#define ERROR_CHECKSUM						0xD4
#define ERROR_INVALID_FIRMWARE				0xD7
#define ERROR_BR_COMMAND					0xD8
#define ERROR_FILE_IS_EMPTY					0xDA
#define ERROR_FILE_NOT_FOUND				0xDB
#define ERROR_UNKNOWN_COMMAND				0xDE
#define DEVICE_NOT_FOUND					0xE5

#define ERROR_UNEXPECTED_VALUE				0xF0
#define ERROR_INVALID_COMMAND				0xFD

#define CheckResult() if(fresult != ERROR_NO_ERROR) return fresult

#ifdef  __cplusplus

}

#endif

#endif
