/***************************************************************************
 *
 *                            I 2 C     H E A D E R
 *                                                                          
 *         written and designed by Oceanwave Consulting, Inc.            
 *         This file contains Oceanwave Consulting Library functions
 *         which are solely owned and copyrighted by Oceanwave Consulting Inc.   
 *         http://www.oceanwaveconsulting.com/                                     
 *         info@oceanwaveconsulting.com       jnc@oceanwaveconsulting.com                    
 *                                                                          
 ***************************************************************************
 *                                                                          
 * FILE: i2c.h
 *                                                                          
 * This file contains the prototypes for the drivers of the low level I2C bus
 *                                                                          
 *                                                                          
 ****************************************************************************/
#ifndef _I2C_H_
#define _I2C_H_

#include <intrins.h>

/*--- HW deffinitions ---*/
sbit I2C_CLK = P1^6;
sbit I2C_DIO = P1^7;

/*--- part of the I2C protocol ---*/
#define I2C_NACK    1
#define I2C_ACK     0
#define I2C_WRITE   0x00
#define I2C_READ    0x01

/*--- prototypes ---*/
#define       I2C_NOP()                
#define       I2C_Initialize()         I2C_Generate_Stop()
void          I2C_Generate_Start(void);
bit           I2C_Generate_Stop(void);
bit           I2C_Send_Byte(unsigned char byte_to_send);
unsigned char I2C_Get_Byte(bit ack);

#endif
