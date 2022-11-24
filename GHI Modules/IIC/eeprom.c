/***************************************************************************
 *                                                                          
 *                          E E P R O M     S O U R C E
 *
 *         written and designed by Oceanwave Consulting, Inc.            
 *         This file contains Oceanwave Consulting Library functions
 *         which are solely owned and copyrighted by Oceanwave Consulting Inc.   
 *         http://www.oceanwaveconsulting.com/                                     
 *         info@oceanwaveconsulting.com       jnc@oceanwaveconsulting.com                    
 *                                                                          
 ***************************************************************************
 *                                                                          
 * FILE: eeprom.C
 *                                                                          
 ****************************************************************************/
#define COMPILING_EEPROM
#include <stdio.h>
#include <stdlib.h>
#include <51/reg51.h>
#include "i2c.h"
#include "bool.h"
#include "eeprom.h"


static bool   EEPROM_Wait(void);
static bool   EEPROM_I2C_Write(unsigned int start_address);
static bool   EEPROM_I2C_Read(unsigned int start_address);

/******************************************************************************

                     E E P R O M     I N I T I A L I Z E

*******************************************************************************

DESCRIPTION: This routine initializes the EEPROM port pins.  The I2C bus should
             have already been initialized by now.

INPUT ARGS:  none

OUTPUT ARGS: none

RETURNS:     none

NOTES:       none

******************************************************************************/
void EEPROM_Initialize(void)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

   /**************************************************************************/
  EEPROM_DISABLE_WRITE();
  }

/******************************************************************************

                         E E P R O M     V E R I F Y

*******************************************************************************

DESCRIPTION: This routine verifies the EEPROM vs a buffer in local memory
             This is escentially the same as EEPROM_Read, except the incoming
             data is not stored

INPUT ARGS:  address (index value) into the EEPROM for this data
             pointer to the starting byte (first byte to verify)
             the length of the buffer to verify

OUTPUT ARGS: none

RETURNS:     EEPROM_OK or EEPROM_BAD

NOTES:       none

******************************************************************************/
eeprom_error_t EEPROM_Verify(const int start_address, const void const user_data[], size_t size)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  unsigned int  i;
  unsigned char X;

   /**************************************************************************/

  if (EEPROM_I2C_Read(start_address) == TRUE)
    {
    /*--- now verify the requested block of data ---*/
    i = 0;
    do {
       i++;
       X = I2C_Get_Byte((bool)(i == size));
       } while ((i < size) && (X == ((unsigned char *)user_data)[i-1]));

    if (I2C_Generate_Stop() == TRUE)
      if (i == size)
        return (EEPROM_ERROR_NONE);
    }

  return(EEPROM_ERROR_VERIFY);
  }  // function bEEPROM_Verify
/******************************************************************************

                           E E P R O M     W R I T E
                         (Oceanwave Library Function)
*******************************************************************************

DESCRIPTION: Writes a value to the I2C bus, The value can be any type
             ie. structure, char, int, long, double, or string. The size
             paramater determines how many bytes are written.  Typically,
             this will be the physical memory size of the type being written.

INPUT ARGS:  address (index value) into the EEPROM for this data
             pointer to the starting byte (first byte to verifty)
             the length of the buffer to verify

OUTPUT ARGS: none

RETURNS:     EEPROM_OK or EEPROM_BAD

NOTES:       none

******************************************************************************/
eeprom_error_t EEPROM_Write(const int start_address, const void const user_data[], size_t size)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  eeprom_error_t Ret_Val;
  unsigned int   i;

   /**************************************************************************/

  Ret_Val = EEPROM_ERROR_WRITE;

  /*--- send the address to the EEPROM ---*/
  if (EEPROM_I2C_Write(start_address) == TRUE)
    {
    /*--- write the actually data ---*/
    for (i=0;i < size;i++)
      if (I2C_Send_Byte(((unsigned char *)user_data)[i]) == TRUE)
        if (((i+1) < size) && (((start_address + i + 1) % EEPROM_BLOCK_SIZE) == 0))
          if (I2C_Generate_Stop() == TRUE)
            if (EEPROM_Wait() == TRUE)
              if (EEPROM_I2C_Write(start_address+i+1) != TRUE)
                break;

    /*--- end the write, and verify the data ---*/
    if (I2C_Generate_Stop() == TRUE)
      if ((EEPROM_Wait() == TRUE) && (i == size))
        Ret_Val = EEPROM_Verify(start_address,user_data,size);
    }

  EEPROM_DISABLE_WRITE();
  return(Ret_Val);
  }  // function bEEPROM_Write


/******************************************************************************

                            E E P R O M     R E A D
                         (Oceanwave Library Function)
*******************************************************************************

DESCRIPTION: Reads a value from the EEPROM bus the value can be any type
             ie. structure, char, int, long, double, or string. The size
             paramater determines how many bytes are read.  typically,
             this will be the size of the type being read.

INPUT ARGS:  address (index value) into the EEPROM for this data
             pointer to the starting byte (first byte to read)
             the length of the buffer to read

OUTPUT ARGS: the buffer pointed to will now contain a copy of the EEPROM's
             contents.  If an error occurs, the buffer's contents are
             indeterminent

RETURNS:     EEPROM_OK or EEPROM_BAD

NOTES:       none

******************************************************************************/
eeprom_error_t EEPROM_Read(const int start_address, void user_data[], size_t size)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  unsigned int i;

   /**************************************************************************/

  if (EEPROM_I2C_Read(start_address) == TRUE)
    {
    /*--- now fetch the request block of data ---*/
    for (i=0;i<size;i++)
      ((unsigned char *)user_data)[i] = I2C_Get_Byte((bool)(i == (size - 1)));

    if (I2C_Generate_Stop() == TRUE)
      return (EEPROM_ERROR_NONE);
    }

  return(EEPROM_ERROR_READ);
  }

/******************************************************************************

                           E E P R O M     W A I T
                         (Oceanwave Library Function)
*******************************************************************************

DESCRIPTION: This routine will wait until the EEPROM is ready, or a timeout
             happens, which ever happens first.

INPUT ARGS:  none

OUTPUT ARGS: none

RETURNS:     TRUE after a successful wait
             FALSE on timeout

NOTES:       you'll notice the comment out timer code below.  I've left it 
             there to show that you _should_ set a timeout in case the HW 
             fails, however thats beyond the scope of this demo.

             Many EEPROMs have a block-write wait time of 5 to 20mS.  Check your
             data sheet for the maximum wait time. I'd suggests doubling it, 
             and using it for your timeout value.

******************************************************************************/
static bool EEPROM_Wait(void)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

//  timer_t timer_start;
  bool not_busy;

   /**************************************************************************/

//  TimerStart = TIMER_Get_Milliseconds();

  EEPROM_DISABLE_WRITE();
  do {
     /*--- send a START ---*/
     I2C_Generate_Start();
     /*--- send the address byte (to see the bus busy or not) ---*/
     not_busy = I2C_Send_Byte(EEPROM_ADDR | I2C_READ);
     /*--- send a STOP ---*/
     I2C_Generate_Stop();
     } while ((not_busy == FALSE) /* && (TIMER_Diff_Time(TIMER_Get_Milliseconds(),timer_start) < 40)*/);

  return (not_busy);
  }
/******************************************************************************

                   E E P R O M     I 2 C     W R I T E

*******************************************************************************

DESCRIPTION: begin a write transaction between the EEPROM and the Micro, via I2C
             This includes the start, sending the device address, and the starting
             internal memory location 

INPUT ARGS:  the internal EEPROM memory address we want to access

OUTPUT ARGS: none

RETURNS:     TRUE if the transaction was good, FALSE otherwise

NOTES:       none

******************************************************************************/
static bool EEPROM_I2C_Write(unsigned int start_address)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  bool          ret_val;
  unsigned char bus_addr;
  unsigned char high_addr;
  unsigned char low_addr;

   /**************************************************************************/
  EEPROM_ENABLE_WRITE();

  /*--- first we have to set the address ---*/
  /*--- start a write transfer ---*/
  I2C_Generate_Start();

  bus_addr  = EEPROM_ADDR | I2C_WRITE;
  high_addr = (start_address >> 8) & 0x00FF;
  low_addr  = start_address & 0x00FF;

  if ((ret_val = I2C_Send_Byte(bus_addr)) == TRUE)
    {
    ret_val &= I2C_Send_Byte(high_addr);
    ret_val &= I2C_Send_Byte(low_addr);
    }

  return (ret_val);
  }
/******************************************************************************

                      E E P R O M     I 2 C     R E A D

*******************************************************************************

DESCRIPTION: begin a read transaction between the EEPROM and the Micro, via I2C
             This includes the start, sending the device address, and the starting
             internal memory location 

INPUT ARGS:  the internal EEPROM memory address we want to access

OUTPUT ARGS: none

RETURNS:     TRUE if the transaction was good, FALSE otherwise

NOTES:       none

******************************************************************************/
static bool EEPROM_I2C_Read(unsigned int start_address)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  bool ret_val;

   /**************************************************************************/

  ret_val = EEPROM_ERROR_READ;

  if ((ret_val = EEPROM_I2C_Write(start_address)) == TRUE)
    {
    /*--- send a STOP ---*/
    if (I2C_Generate_Stop() == TRUE)
      {
      EEPROM_DISABLE_WRITE();
      /*--- first we have to set the address ---*/
      /*--- start a write transfer ---*/
      I2C_Generate_Start();
      /*--- send the device address ---*/
      ret_val = I2C_Send_Byte(EEPROM_ADDR | I2C_READ);
      }
    /*--- setting it again won't hurt ---*/
    EEPROM_DISABLE_WRITE();
    }

  EEPROM_DISABLE_WRITE();
  return(ret_val);
  }
 

