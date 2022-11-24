// ReadPage.c
// Read PAGE_SIZE bytes at address into bufptr
// Starts reading at address 0 within the page
// Please refer to the application note titled: 
// "Programming Specification for Atmel's Configuration Memory E²PROMS AT17C65/128/256/512/010" 
// found at www.atmel.com for detailed device address decoding and page address formatting
 
#include "at17c.h"
 
extern void BitDelay(void);
extern unsigned char SendByte(unsigned char byte, unsigned char order);
extern unsigned char GetByte(unsigned char lastbyte);
extern void SendStartBit(void);
extern void SendStopBit(void);

void ReadPage(unsigned int address, unsigned char *bufptr) {

unsigned char i;
unsigned char addr1;
unsigned char addr2;
unsigned char addr3;
 
    PORTB &= 0xfb; // bring SER_EN low
    
    BitDelay();
    
    addr1 = (unsigned char)(address >> 9);
    addr2 = (unsigned char)(address >> 1);
    addr3 = (unsigned char)(address << 7);
        
    SendStartBit();
    SendByte(AT17C010 + WRITE,MSB_FIRST);  // send device address byte
    SendByte(addr1,MSB_FIRST);                 // 1st address byte
    SendByte(addr2,MSB_FIRST);                 // 2nd address byte
    SendByte(addr3,MSB_FIRST);                 // 3rd address byte
 
    SendStartBit();
    SendByte(AT17C010 + READ,MSB_FIRST);  // send device address byte with read bit
 
    for (i = 0; i < (PAGE_SIZE - 1); i++)
        bufptr[i] = GetByte(0);
            
    bufptr[(PAGE_SIZE - 1)] = GetByte(1); // 1 signals last byte of read sequence

    SendStopBit();

    PORTB |= 0x04; // bring SER_EN high
        
}
