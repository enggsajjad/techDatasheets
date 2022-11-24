//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			ComPort.h
// Compiler:		Microsoft Visual C++ 6.0
// Output Size:
// Based on work by:�E, VU
// Created:			27-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		Communications Port interface.
//
// Other Info:		
//=============================================================================

#ifndef COMPORT_H
#define COMPORT_H

//=============================================================================

#include <windows.h>

//=============================================================================
// ComPort declaration

class ComPort
{
public:
	ComPort();
	~ComPort();

	BOOL open(LPCTSTR port, DWORD baudRate, DWORD timeOut);

	BOOL setTimeOut(DWORD timeOut);
	BOOL flush();
	BOOL close();

	BOOL write(UCHAR *buffer, DWORD size);
	BOOL read(UCHAR *buffer, DWORD size);

private:	
	HANDLE commPortM;
};

#endif // COMPORT_H
