/***************************************************************************
 *                                                                          
 *                                   M A I N
 *
 *         written and designed by Oceanwave Consulting, Inc.            
 *         This file contains Oceanwave Consulting Library functions
 *         which are solely owned and copyrighted by Oceanwave Consulting Inc.   
 *         http://www.oceanwaveconsulting.com/                                     
 *         info@oceanwaveconsulting.com       jnc@oceanwaveconsulting.com                    
 *                                                                          
 ***************************************************************************
 *                                                                          
 * FILE: main.C
 *                                                                          
 ****************************************************************************/
#define COMPILING_MAIN
#include <stdio.h>
#include <stdlib.h>
#include <51/reg51.h>
#include <limits.h>
#include "i2c.h"
#include "eeprom.h"
#include "bool.h"
code char test_string[]="Taylor River Real Time and American Raisonance";
void main(void)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  unsigned int   addr; // address we want to write to in the EEPROM
  unsigned int   passes[4];
  eeprom_error_t return_value;
   /**************************************************************************/
  I2C_Initialize();
  EEPROM_Initialize();

  passes[0] = 0;
  passes[1] = 0;
  passes[2] = 0;
  passes[3] = 0;
  
  /*--- fill the EEPROM with our test string ---*/
  for (addr = 0; addr < EEPROM_SIZE; addr += sizeof(test_string))
    {     
    return_value = EEPROM_Write(addr,test_string,sizeof(test_string));
    /*--- this will count the number of good vs bad cycles ---*/
    /*--- ideally, passes[0] will be a high number, and    ---*/
    /*--- passes[1-3] will be zero                         ---*/
    passes[return_value]++;
    }

  /*--- set a break point here, and look at the "passes" array ---*/
  while (TRUE);
  } 
