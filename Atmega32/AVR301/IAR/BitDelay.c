// BitDelay.c
// for a 3.3v device min clock pulse width low and high is 4 us
// generate 2us delay for bit timing using NOP's
// 7.3728MHz crystal
           
#include "at17c.h"
 
void BitDelay(void) {

    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    
    return;
}
