#include <stdio.h>
#include "defines.h"
#include "flashStorageDriver.h"

//Declaration of functions
void UART0_Init( void );
int putchar( int data );


/**************************************************************************
* Function: __C_task void main( void )
*
* Description:
*      Illustrates how to use the flashStorageDriver.c functions to store
*      parameters in flash
*
**************************************************************************/

__C_task void main( void )
{
 
  DATATYPE testData;

  //Initialization
  UART0_Init();
  flashStorageInit();
  
  printf("\n\r***** Test Start *******\n\r");
  testData = flashStorageRead();
  
  /*The lines below could be added to ensure correct operation after power loss. This will ensure
    that if a parameter location was being programmed when the power loss occured it will be skipped*/
  flashStorageWrite( testData );  //Overwrites next location (in case it has already been tried written)
  flashStorageWrite( testData );  //Stores the parameter in a new guaranteed fresh location
  
  while(1)
  {
  	testData = flashStorageRead();
    flashStorageWrite( testData );
  }

}

/**************************************************************************
* Function: void UART0_Init( void )
*
* Description:
*      Initializes the UART0 so that status messages can be send to terminal
*
**************************************************************************/
void UART0_Init( void )
{
  #define SYSTEM_CLOCK = 3686400
  UBRR0L = 1;                   //115K £ 3.6864MHz
  UCSR0B |= (1<<TXEN0);
}

/**************************************************************************
* Function: int putchar ( int )
*
* Description:
*      Used by printf() to send chars to UART
*
**************************************************************************/
int putchar( int data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR0A & (1<<UDRE0)) )
		; 			                
	/* Start transmittion */
	UDR0 = (unsigned char)data;
	return 1;
}
