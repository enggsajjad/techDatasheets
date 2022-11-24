#include <avr/io.h>
#include "usart.h"

/* 25. May 2005 - adapted to avrlibc iom168.h version 1.17 */

void USART_Init(unsigned int baudrate) // UDRR Value not baudrate
{
    // Set baud rate
    UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char)baudrate;

    // Enable 2x speed
    UCSRA = (1<<U2X);

    // Enable receiver and transmitter
    UCSRB = (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<UDRIE);

    // Async. mode, 8N1
    UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
}


void Usart_Tx(char data)
{
    while (!(UCSRA & (1<<UDRE)));
    UDR = data;
}

char Usart_Rx(void)
{
    while (!(UCSRA & (1<<RXC)));
    return UDR;
}
