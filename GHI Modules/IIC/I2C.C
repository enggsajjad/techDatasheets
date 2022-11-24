/***************************************************************************
 *                                                                          
 *                              I 2 C     S O U R C E
 *
 *         written and designed by Oceanwave Consulting, Inc.            
 *         This file contains Oceanwave Consulting Library functions
 *         which are solely owned and copyrighted by Oceanwave Consulting Inc.   
 *         http://www.oceanwaveconsulting.com/                                     
 *         info@oceanwaveconsulting.com       jnc@oceanwaveconsulting.com                    
 *                                                                          
 ***************************************************************************
 *                                                                          
 * FILE: i2c.C
 *                                                                          
 ****************************************************************************/
#define COMPILING_I2C
#include <stdio.h>
#include <stdlib.h>
#include <51/reg51.h>
#include "i2c.h"
#include "bool.h"

/******************************************************************************

                        I 2 C   G E N E R A T E   S T A R T
                           (Oceanwave Library Function)
*******************************************************************************

DESCRIPTION: A start condition occurs when the data line transitions from
             high to low while the clock line is high.  A repeated start 
             condition is valid, and used when a "read" is performed.  A read 
             must first write the address to the I2C device before the read 
             can commence.

INPUT ARGS:  none

OUTPUT ARGS: none

RETURNS:     none

NOTES:       data and clock lines are low at end of function call

******************************************************************************/
void I2C_Generate_Start(void)	
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

   /**************************************************************************/
  I2C_DIO = 1;                         //data has to be high before the clock 
                                       //is, this is the definition of a start
  I2C_CLK = 1;
  I2C_NOP();
  I2C_DIO = 0;                         //change the data while CLK is high
  I2C_NOP();
  I2C_CLK = 0;
  }
/******************************************************************************

                        I 2 C   G E N E R A T E   S T O P
                           (Oceanwave Library Function)
******************************************************************************* 

DESCRIPTION: A stop condition occurs when the data line transitions from
             low to high while the clock line is high.

INPUT ARGS:  none

OUTPUT ARGS: none

RETURNS:     If the MASTER cannot successfully regain control of the bus, 
             there is an error.  This routine will return TRUE on success
             and FALSE on a failure to regain the bus

NOTES:       data and clock lines are high at end of function call
             This particular routine is adapted from siemens apnote AP1625
******************************************************************************/
bit I2C_Generate_Stop(void)	
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  unsigned char retry;   // don't want to be here for ever, this is a retry counter
   /**************************************************************************/

  retry = 9;

  I2C_DIO = 1;
  while (retry--)
    if (!I2C_DIO)              // while the data line is low
      {                        // continue to pulse the clock
      I2C_CLK = 1;
      I2C_NOP();
      I2C_CLK = 0;
      }
    else  
      {                       // when the data line actually is high
      I2C_DIO = 0;            // we can now execute a valid STOP
      I2C_NOP();
      I2C_CLK = 1;
      I2C_NOP();
      I2C_DIO = 1;
      return(TRUE);
      }

  I2C_CLK = 1;
  return (FALSE);
  }
/******************************************************************************

                            I 2 C   S E N D   B Y T E
                           (Oceanwave Library Function)
******************************************************************************* 

DESCRIPTION: Get and Send bit manipulate data at the lowest level of the I2C
             bus.  For most applications, we actually will need to use whole 
             bytes.  This function with orchestrate sending a single byte to 
             the hardware, and then wait for the ACK bit indicating its 
             reception.

INPUT ARGS:  The single byte being sent to the physical hardware (X)

OUTPUT ARGS: none

RETURNS:     Returns the 9th bit generated by the hardware.  This is the "ACK"
             bit... commonly it should be '0' for an ACK.
             We invert it to make it positive logic
             TRUE = good transaction, FALSE = error

NOTES:       none

******************************************************************************/
bit I2C_Send_Byte(unsigned char byte_to_send)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  unsigned char index;  // bit position counter 0x80 -> 0x40 -> ... -> 0x01
  bit           ack;    // the acknowledge bit read from HW

   /**************************************************************************/
  for(index = 0x80; index != 0; index >>= 1)     // MSB first
    {
    if (byte_to_send & index)
      I2C_DIO = 1;
    else
      I2C_DIO = 0;

    I2C_CLK = 1;
    I2C_NOP();
    I2C_CLK = 0;
    }   

  /*--- now get the "ack" bit to make sure everything is good ---*/
  I2C_DIO = 1; // set for input
  I2C_NOP();

  I2C_CLK = 1;
  I2C_NOP();
  ack     = I2C_DIO;
  I2C_CLK = 0;

  return(ack == I2C_ACK);// returning ACK bit, an I2C ACK is a 0, so we'll
  }                      // change it to positive logic to make it easier
/******************************************************************************

                            I 2 C   G E T   B Y T E
                           (Oceanwave Library Function)
******************************************************************************* 

DESCRIPTION: Get and Send bit manipulate data at the lowest level of the I2C
             bus.  For most applications, we actually will need to use whole 
             bytes.  This function with orchestrate getting a single byte from
             the hardware, and then send the ACK bit indicating its reception.

INPUT ARGS:  The ACK bit being sent to the hardware.  Sending a NACK indicates 
             the end of a read cycle.

OUTPUT ARGS: none

RETURNS:     The byte read in from physical hardware

NOTES:       none

******************************************************************************/
unsigned char I2C_Get_Byte(bit ack)
  {
   /*---------------------------- literals ----------------------------------*/

   /*---------------------------- variables ---------------------------------*/

  unsigned char index;  // bit position counter 0x80 -> 0x40 -> ... -> 0x01
  unsigned char X;      // value being read from HW to return to caller

   /**************************************************************************/
  I2C_DIO = 1;   // set for a read
  X       = 0;
  for(index = 0x80; index != 0; index >>= 1)     // MSB first
    {
    I2C_NOP();
    I2C_CLK = 1; 
    if (I2C_DIO)
      X |= index;
    I2C_CLK = 0;
    }
 
  I2C_DIO = ack;    // ACK (0) or NACK (1) bit
  I2C_NOP();
  I2C_CLK = 1;
  I2C_NOP();
  I2C_CLK = 0;
      
  return(X);     
  }
