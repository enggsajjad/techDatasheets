/*****************************************************************
* File: flashParamDriver.h
* 
* revision and date: 
*       1.0: 2002.11.04 JLL
*       1.1: 2003.07.04 RAA
*
* Author: JLL
*
* Description:
*        Header file for flashStorageDriver.c
*
*****************************************************************/
#ifndef FLASHPARAMDRIVER_H
#define FLASHPARAMDRIVER_H

struct _savedparams
{
  unsigned char text[3];
  signed long number;
};

#define DATATYPE struct _savedparams
#define DATASIZE sizeof( DATATYPE )

void flashStorageInit( void );
DATATYPE flashStorageRead( void );
unsigned char flashStorageWrite( DATATYPE flashData );

#endif
