/*****************************************************************
* File: flashStorageDriver.c
* 
* revision and date: 
*       1.0: 2002.11.04 JLL
*       1.1: 2002.11.21 JLL
*       1.2: 2003.07.03 RAA
*
* Author: JLL
*
* Description:
*        This file contains the definition of a redundant data set stored in flash.
*        The purpose is to store parameters in flash memory rather than in EEPROM.
*        By the use of selfprogramming the parameters can be updated. The parameters
*        are stored redundantly to be able to test their validity. The tests performed
*        inspect that certain locations are zero (Zero Control), that the redundant 
*        copies are all identical and that the checksum stored in the data set is 
*        correct. If any of the tests fails an error message is returned. 
*
*        In some instances the errors leading to failure of the Zero Control test and
*        the reduncy test can be recovered; this is done by AND'ing the data - assuming 
*        that the failure is due to early termination of the flash write cycle. Refer to
*        the application note for details.
*
*****************************************************************/
#include "defines.h"
#include "flashStorageDriver.h"

/* Define WRITE_COMPLETION_PROTECTION to program parameter and 
   write_complete flags in seperate write cycles*/
#define WRITE_COMPLETION_PROTECTION

/* Define EEPROM_PARAMETER_BACKUP to enable temporary storage of 
   parameter in eeprom while erasing the page*/
#define EEPROM_PARAMETER_BACKUP



#ifndef __AT90Mega128__
  #error "Warning: flashStorageDriver.h is configured for ATmega128"
#endif
#define BOOT_START_ADDRESS 0xF000         // Device dependent!!
#define FLASH_PAGE_SIZE  256              // Device dependent!!
#define FLASH_PARAMETER_LOCATION 0x000400

#define PARAM_SPACE ( (int) (FLASH_PAGE_SIZE / (DATASIZE + 1.0/8.0) ))  // One element takes DATASIZE + one 8th of a byte
#define BYTE_WRITTEN_FLAGS ( (int) (PARAM_SPACE+7) / 8)                 // One bit required per element, round up


__flash const struct _flashParameters{
	DATATYPE parameters[PARAM_SPACE]; // This contains the actual parameter data
	unsigned char elementsWrittenFlags[BYTE_WRITTEN_FLAGS]; // This contains bit flags for each parameter location
	unsigned char unused[FLASH_PAGE_SIZE - PARAM_SPACE*DATASIZE - BYTE_WRITTEN_FLAGS]; // Fill flash page
	} flashParameters @ FLASH_PARAMETER_LOCATION = {
	
		'A', 'V', 'R', 0x12345678,                         // "Factory settings", first element
	
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,   // The last 34 elements are
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,   // initialized to all 1-bits,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,   // i.e. unprogrammed/erased
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		0xFF, 0xFF, 0xFF, -1,      0xFF, 0xFF, 0xFF, -1,
		
		0xFE,                                              // First flag byte with LSB flag programmed
		
		0xFF, 0xFF, 0xFF, 0xFF,                            // The rest of the flag bytes are unprogrammed
		
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF                 // Unused bytes
	
	};

// HOW TO INITIALIZE:
//
// Calculate the sizes of the three arrays in 'flashParameters' manually.
// Refer to the macros 'PARAM_SPACE' and 'BYTE_WRITTEN_FLAGS' and round down to nearest integer.
// Fill the first element of 'parameter' with your startup factory settings for your parameters.
// Fill the rest with a representation of all '1' bits (e.g. 0xFF or -1).
// Set the first flag byte of 'elementsWrittenFlags' to 0xFE (i.e. LSB flag programmed).
// Set the rest of the flag bytes to 0xFF.
// Fill the 'unused' array with dummy bytes, e.g. 0xFF
//
// ALTERNATIVELY:
//
// If EEPROM backup is enabled (i.e. EEPROM_PARAMETER_BACKUP defined), you can omit the
// initializers for 'flashParameters'.
// Instead, set the 'eParameter' initializer to your factory settings and the 'eFlag' byte
// to TRUE. The application will then be forced to erase the flash page and store the
// 'eParameter' to the first element in 'flashParameters'. The application thinks that a
// RESET has occured in the middle of a write operation and recovers using the backup
// parameter in EEPROM. Verify that the EEPROM contents are correct after programming
// your AVR device.


#ifdef EEPROM_PARAMETER_BACKUP
__eeprom unsigned char eFlag = FALSE;
__eeprom DATATYPE eParameter;
#endif


unsigned char parameterIndex = 0;   //Index variable for flash parameters. Note that element indexing is used


/**************************************************************************
* Function: void flashStorageInit( void )
*
* Description:
*      Search for the beginning data in the flash storage and update the
*      global variable parameterIndex.
*
**************************************************************************/
#pragma location = "BOOT"
void flashStorageInit( void )
{
  unsigned char temp;
  unsigned char count;
  unsigned char __flash *pFlash;


#ifdef EEPROM_PARAMETER_BACKUP
  /*Inspect if the EEPROM back-up of the parameter is to be used*/
  if( eFlag == TRUE )   //Parameter should be restored from EEPROM
  {
    parameterIndex = PARAM_SPACE - 1;   //Trigger page erase
    flashStorageWrite( eParameter );    //Write parameter to parameter storage
                                        //Note that erase error handling is not implemented here
    return;
  }
#endif

  /*Find last used parameter location within page*/
  pFlash = (unsigned char __flash *) flashParameters.elementsWrittenFlags;

  //Find first byte in elementsWrittenFlags that differs from 0x00
  while( *pFlash == 0x00 )
  {
    pFlash++;
  };
  
  //Find bit that differ from one (bit which is not programmed), start looking from bit 7
  temp = 0x80;
  count = 7;
  do
  {
    if( !(*pFlash & temp) )
    {
      break;
    }
    count--;
    temp >>= 1;    //Left-shift one through the byte
  }while( temp );

  /*Identify first vacant element location*/
  parameterIndex = ((pFlash - flashParameters.elementsWrittenFlags) << 3) + count;
}


/**************************************************************************
* Function: unsigned long flashStorageRead( void )
*
* Description:
*      Read the data last written into the flash storage and returns it.
*
**************************************************************************/
#pragma location = "BOOT"
DATATYPE flashStorageRead( void )
{
	return flashParameters.parameters[ parameterIndex ];
}


/**************************************************************************
* Function: void flashStorageWrite( DATATYPE flashData )
*
* Description:
*      Stores the incomming data in the circular flash storage. Erases the Flash page 
*      and start over if this is required to store the storage.
*
**************************************************************************/
#pragma location = "BOOT"
unsigned char flashStorageWrite( DATATYPE flashData )
{
  unsigned char eepromInterruptSetting;
  unsigned char temp;
  unsigned int tempData;
  unsigned char *tempPtr;
  unsigned char __flash *pFlash;
  unsigned int __flash *pFlashInt;


  /*Make sure that EEPROM write access is not ongoing*/
  eepromInterruptSetting = EECR & (1<<EERIE);    //Store interrupt mask for the eeprom
  EECR &= ~(1<<EERIE);                            //Disable EEPROM interrupt
  while(EECR & (1<<EEWE));                        //Wait for EEPROM to finish


  /*If end of parameter buffer reached - erase the buffer and start from the beginning*/
  if( parameterIndex == PARAM_SPACE - 1 )
  {
#ifdef EEPROM_PARAMETER_BACKUP
    if( !eFlag )                //If the eFlag is not already set
    {
      eParameter = flashData;
      eFlag = TRUE;             //Parameter stored in EEPROM is the valid
      while(EECR & (1<<EEWE));  //Wait for EEPROM to finish
    }
#endif


    //Erase parameter page and init parameterIndex;
    while( SPMCSR & (1<<SPMEN) );                 //Wait until SPM operation has completed
    _SPM_ERASE( FLASH_PARAMETER_LOCATION );       //Erase flash page
    parameterIndex = 0xFF;		     			  // When adding 1, this will be 0

    
    /*Verify that the page is erased*/
    pFlashInt = (unsigned int __flash*)FLASH_PARAMETER_LOCATION;
    temp = FLASH_PAGE_SIZE>>1;
    while( SPMCSR & (1<<SPMEN) );                 //Wait until SPM operation has completed
    __DataToR0ByteToSPMCR_SPM( 0, (unsigned char)(1<<RWWSRE)|(1<<SPMEN)); //Enable RWW part of flash

    do
    {
      if( *pFlashInt++ != 0xFFFF )
      {
          EECR |= eepromInterruptSetting;          //Restore EEPROM interrupt mask
          return FALSE;
      };
    }while(--temp);
  }
  
  
  /* Write the new data to the flash */

  // Get the byte address in Flash for the data to write
  parameterIndex++; // Move to next block, i.e. first unused block
  pFlash = (unsigned char __flash *) &(flashParameters.parameters[parameterIndex]);
  
  // Is the first byte at an odd address?
  temp = DATASIZE; //Number of bytes in data to write
  tempPtr = (unsigned char *) &flashData;
  
  if( (unsigned int) pFlash & 0x0001 ) // Odd address?
  {
  	pFlash--; // Move back to word boundary, i.e. even byte address
    tempData = (tempPtr[0] << 8) | 0x00FF; // Store first byte of data in high byte of Flash word
    _SPM_FILLTEMP( pFlash, tempData );
    temp--; // Decrease data byte count
    pFlash += 2; // Move ptrs to next word in flash
    tempPtr++;
  }
  
  while( temp >= 2 ) // Write as many words as possible
  {
  	tempData = tempPtr[0] | (tempPtr[1] << 8); // Combine bytes to word
    _SPM_FILLTEMP( pFlash, tempData );
    
    temp -= 2; // Two more bytes stored
    pFlash += 2; // Move ptrs
    tempPtr += 2;
  }
  
  if( temp > 0 ) // Still one single byte left ?
  {
  	tempData = 0xFF00 | tempPtr[0]; // Store last byte of data in low byte of Flash word
  	_SPM_FILLTEMP( pFlash, tempData );
  }
    
   
#ifdef WRITE_COMPLETION_PROTECTION
  //If WRITE_COMPLETION_PROTECTION is defined the data is written first and after the write is completed 
  // write-completion flag is written

  /*Write SPM buffer to flash*/
  _SPM_PAGEWRITE( FLASH_PARAMETER_LOCATION );
  while( SPMCSR & (1<<SPMEN) ); //Will not return until the flash write has completed
  __DataToR0ByteToSPMCR_SPM( 0, (unsigned char)(1<<RWWSRE)|(1<<SPMEN)); //Enable RWW part of flash
#endif

/*Point to the byte in elementsWrittenFlags corresponding to the first vacant parameter element*/
  pFlash = (unsigned char __flash *) flashParameters.elementsWrittenFlags;
  pFlash += parameterIndex>>3; // div by 8
  
/*Update the elementsWrittenFlags and store new value in SPM buffer*/
  //Load old elementWrittenFlags
  temp = *pFlash;           //Load old value
    
  //Update the elementsWrittenFlags
  if( temp == 0x00 )  // If current byte in elementsWrittenFlags are "full" move to next
  {
    pFlash++;
    temp = 0xFE; // Program 1st flag bit to 0
  }
  else
  {
	  temp <<= 1; // This will shift left and thus clear the next available flag bit
  }

  //If current byte of elementsWrittenFlags are odd the new value should be store in high word 
  // otherwise in low word. Bits not written to are masken out (setting them high)
  if( (unsigned int)pFlash & 0x0001 )         // if address is odd store in high word
  {
    pFlash--;
    tempData = (temp<<8) | 0x00FF;
  }
  else                                        //If address is even store in low word
  {
    tempData = 0xFF00 | temp;
  }

  _SPM_FILLTEMP( pFlash, tempData );


  /*Write SPM buffer to flash*/
  _SPM_PAGEWRITE( FLASH_PARAMETER_LOCATION );
  while( SPMCSR & (1<<SPMEN) ); //Will not return until the flash write has completed
  __DataToR0ByteToSPMCR_SPM( 0, (unsigned char)(1<<RWWSRE)|(1<<SPMEN)); //Enable RWW part of flash

#ifdef EEPROM_PARAMETER_BACKUP
    eFlag = FALSE;             //Parameter stored in EEPROM is NOT valid
#endif

  EECR |= eepromInterruptSetting;                            //Restore EEPROM interrupt mask
  
  return TRUE;
}
