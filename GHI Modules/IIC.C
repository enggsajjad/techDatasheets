/************************************************************
  The all-software implementation of I2C protocol
*************************************************************/

#include <io51.h>

#define  uchar   unsigned char

#define  SDA     P1.0    /* microcontroller's I/O lines     */
#define  SCL     P1.1    /* assigned to I2C lines           */
#define  EEPROM  0xAE    /* slave address of AT24C02 EEPROM,
                            data direction bit = 0          */
#define  NO_ACK  0       /* lack of acknowledgment          */
#define  ACK     1       /* acknowledgment                  */

/************************************************************
  The functions enabling data transfer via I2C bus, when
  a single master exists
*************************************************************/

/* Issuing of START condition */

void start(void)
{
  SDA = SCL = 1;
  SDA = 0;
  _opc(0);         /* it places NOP instruction into */
  _opc(0);         /* executable code                */
  _opc(0);
  _opc(0);
  _opc(0);
  SCL = 0;
}

/* Issuing a STOP condition */

void stop(void)
{
  SDA = 0;
  SCL = 1;
  _opc(0);
  _opc(0);
  _opc(0);
  _opc(0);
  _opc(0);
  SDA = 1;
}

/* Clock pulse generation. The function returns data or
   acknowledgment bit */

bit clock(void)
{
bit level;         /* state of SDA line        */

  SCL = 1;
  _opc(0);
  while(!SCL);     /* if a pulse was stretched */
  _opc(0);
  _opc(0);
  _opc(0);
  level = SDA;
  _opc(0);
  _opc(0);
  SCL = 0;  
  return(level);
}

/* Writing a byte to a slave, with most significant bit
   first. The function returns acknowledgment bit. */

bit write(uchar byte)
{
uchar mask = 0x80;

  while(mask)
  {
    if (byte & mask)
      SDA = 1;
    else
      SDA = 0;
    clock();
    mask >>= 1;           /* next bit to send           */
  }
  SDA = 1;                /* releasing of the line      */
  return(clock());        /* a slave should acknowledge */
}

/* Reading byte from a slave, with most significant bit 
   first. The parameter indicates, whether to acknowledge
   (1) or not (0) */

uchar read(bit acknowledgment)
{
uchar mask = 0x80,
      byte = 0x00;

  while(mask)
  {
    if (clock())
      byte |= mask;
    mask >>= 1;           /* next bit to receive */
  }
  if (acknowledgment)
  {
    SDA = 0;
    clock();
    SDA = 1;
  }
  else
  {
    SDA = 1;
    clock();
  }
  return(byte);
}

/************************************************************
  Communication with AT24C02 EEPROM
*************************************************************/

/* Writing byte to a given address. The function returns
   status of operation */

bit EEPROM_byte_write(uchar address, uchar byte)
{
bit status;

  status = 0;               /* failure by default */
  start();
  if (!write(EEPROM))       /* write operation    */
   if (!write(address))     /* byte address       */
     if (!write(byte))
       status = 1;          /* success            */
  stop();
  return(status);
}

/* Writing a block of size bytes, starting from a given
   address. The function returns status of operation */

bit eeprom_page_write(uchar *block, uchar address, uchar size)
{
bit status;

  status = 1;                 /* success by default      */
  start();
  if (!write(EEPROM))         /* write operation         */
    if (!write(address))      /* initial address         */
      while(size--)
        if (write(*block++))  /* lack of acknowlegdment? */
        {
          status = 0;         /* failure                 */
          break; 
        }
  stop();
  return(status);    
}

/* Determination the READY/BUSY status of the EEPROM */

bit EEPROM_acknowledge_polling(void)
{
bit status;

  start();
  status = write(EEPROM);
  stop();
  return(status);     /* if 1, the write cycle is in progress */
}

/* Time delay = count1 * (6 * count2 + 12) + 7 machine cycles */

void delay(uchar count1, uchar count2)
{
uchar tmp;            /* temporary variable */

  while(count1--)
  {
    tmp = count2;
    while(tmp--);
  }
}

/* Waiting for completion of the write cycle */

void EEPROM_busy(void)
{
  while(EEPROM_acknowledge_polling())
    delay(1, 164);      /* about 1 msec */
}

/* Reading byte from a given address. The function returns
   status of operation */

bit EEPROM_byte_read(uchar *byte, uchar address)
{
bit status;

  status = 0;                      /* failure by default */
  start();
  if (!write(EEPROM))              /* write operation    */
    if (!write(address))           /* byte address       */
    {
      start();
      if (!write(EEPROM | 0x01))   /* read operation     */
      {
        *byte = read(NO_ACK);
        status = 1;                /* success            */
      }
    }
  stop();
  return(status);
}

/* Reading a block of size bytes, starting from a given
   address. The function returns status of operation */

bit EEPROM_sequential_read(uchar *block, uchar address, uchar size)
{
bit status;

  status = 0;                      /* failure by default */
  start();
  if (!write(EEPROM))              /* write operation    */
    if (!write(address))           /* initial address    */
    {
      start();
      if (!write(EEPROM | 0x01))   /* read operation     */
      {
        while(size--)
          *block++ = read(size ? ACK : NO_ACK);
        status = 1;                /* success            */
      }
    }
  stop();
  return(status);
}

void main(void)
{
}