/***************************************************************************
 *                                                                          
 *                          E E P R O M     H E A D E R
 *
 *         written and designed by Oceanwave Consulting, Inc.            
 *         This file contains Oceanwave Consulting Library functions
 *         which are solely owned and copyrighted by Oceanwave Consulting Inc.   
 *         http://www.oceanwaveconsulting.com/                                     
 *         info@oceanwaveconsulting.com       jnc@oceanwaveconsulting.com                    
 *                                                                          
 ***************************************************************************
 *                                                                          
 * FILE: eeprom.h
 *                                                                          
 ****************************************************************************/
#ifndef _EEPROM_H_
#define _EEPROM_H_

typedef enum {
              EEPROM_ERROR_NONE, 
              EEPROM_ERROR_READ, 
              EEPROM_ERROR_WRITE, 
              EEPROM_ERROR_VERIFY
             } eeprom_error_t;

/*--- HW definitions ---*/
sbit EEPROM_WP = P1^5;
#define EEPROM_ENABLE_WRITE()     (EEPROM_WP = 0)
#define EEPROM_DISABLE_WRITE()    (EEPROM_WP = 1)

/*--- what address is our EEPROM at? ---*/
#define EEPROM_ADDR        0xA0

/*--- what type of EEPROM are we using? ---*/
/*--- please note my way of determining the size of your EEPROM      ---*/
/*--- based on part number only works from a 24C08 to a 24C512.      ---*/
/*--- If you use these defines in your code, you may have to change  ---*/
/*--- them based on your EEPROM type                                 ---*/
#define EEPROM_TYPE        256 // 24C256
#define EEPROM_BLOCK_SIZE  64
#define EEPROM_Kbits       EEPROM_TYPE
#define EEPROM_Kbytes      (EEPROM_Kbits / 8)
#define EEPROM_SIZE        (unsigned int)(EEPROM_Kbytes * 1024)

#define EEPROM_FIRST_BYTE  0x0000
#define EEPROM_LAST_BYTE   ((unsigned int)EEPROM_SIZE - 1)

void           EEPROM_Initialize(void);
eeprom_error_t EEPROM_Read(const int start_address, void user_data[], size_t size);
eeprom_error_t EEPROM_Write(const int start_address, const void const user_data[], size_t size);
eeprom_error_t EEPROM_Verify(const int start_address, const void const user_data[], size_t size);
#endif
