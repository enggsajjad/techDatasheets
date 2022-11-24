// main
// use the atstk200 starter kit and external prototype board to program
// an Atmel AT17C010 device. Leds on PORTC are used for visual status indicators
// 4.7k ohm pull-up resistors are used on SDA and SCL

#include "at17c.h"

volatile unsigned char t0_timed_out;
volatile unsigned char t1_timed_out;
unsigned char wrbuf[PAGE_SIZE];
unsigned char rdbuf[PAGE_SIZE];

extern void WritePage(unsigned int address, unsigned char *bufptr);
extern void ReadPage(unsigned int address, unsigned char *bufptr);
void ProgramResetPolarity(unsigned char state);
unsigned char VerifyResetPolarity(void);
extern void Init(void);

void C_task main(void) {

unsigned int address = 0;
unsigned char i;

    Init();

    _SEI(); // enable interrupts

    // init test buffer
    for (i = 0; i < PAGE_SIZE; i++)
        wrbuf[i] = i;

    while (1) {
 
        PORTC = 0x00; // leds off
 
        for (address = 0; address < MAX_PAGES; address++) {
            WritePage(address,&wrbuf[0]);
            PORTC = address; // leds on
            ReadPage(address,&rdbuf[0]);
            
            // verify programmed page
            for (i = 0; i < PAGE_SIZE; i++) {
                if (wrbuf[i] != rdbuf[i]) {
                    PORTC = 0xff; // leds on
                }    
            }
            
            ProgramResetPolarity(0);
            if (VerifyResetPolarity()) {
                    PORTC = 0xaa;
            }    
          
            ProgramResetPolarity(0xff);
            if (!VerifyResetPolarity()) {
                    PORTC = 0x55;
            }    
            
            // clear verify buf
            for (i = 0; i < PAGE_SIZE; i++)
                rdbuf[i] = 0; 
        }    
        
    } // while (1)

} // main
