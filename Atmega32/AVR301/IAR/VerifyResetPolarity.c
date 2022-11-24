// VerifyResetPolarity.c
// 4 bytes are read from locations 0x20000 through 0x20003
// The bytes are verified to be of all the same value. If they are then the value is returned.
// return value = 0xff = active high reset and active low output enable.
// return value = 0x00 = active low reset and active high output enable.
// If they aren't then 0xaa is returned to signal an error condition.

#include "at17c.h"
 
extern void BitDelay(void);
extern unsigned char SendByte(unsigned char byte, unsigned char order);
extern unsigned char GetByte(unsigned char lastbyte);
extern void SendStartBit(void);
extern void SendStopBit(void);

unsigned char VerifyResetPolarity(void) {

unsigned char loc_1;
unsigned char loc_2;
unsigned char loc_3;
unsigned char loc_4;
unsigned char value;

    PORTB &= 0xfb; // bring SER_EN low
    BitDelay(); // for good measure

    SendStartBit();
    SendByte(AT17C010 + WRITE,MSB_FIRST);  // send device address byte
    SendByte(0x00,MSB_FIRST);              // 1st address byte ... most significant byte first
    SendByte(0x00,MSB_FIRST);              // 2nd address byte ... most significant byte first
    SendByte(0x02,MSB_FIRST);              // 3rd address byte ... most significant byte first
 
    SendStartBit();
    SendByte(AT17C010 + READ,MSB_FIRST);  // send device address byte with read
 
    loc_1 = GetByte(0);
    loc_2 = GetByte(0);
    loc_3 = GetByte(0);
    loc_4 = GetByte(1);
    SendStopBit();

    PORTB |= 0x04; // bring SER_EN high
    
    if ((loc_1 == loc_2) && (loc_2 == loc_3) && (loc_3 == loc_4))
        value = loc_1; // valid reset/oe polarity
    else
        value = 0xaa; // error
        
    return(value);
        
}
