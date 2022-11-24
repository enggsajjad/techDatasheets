// SerialPort.cpp: implementation of the CSerialPort class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SerialPort.h"

#include <winbase.h>


CSerialPort::CSerialPort()
{
	m_hCommPort = INVALID_HANDLE_VALUE;
}


CSerialPort::~CSerialPort()
{
	if (m_hCommPort != INVALID_HANDLE_VALUE)
		CloseHandle (m_hCommPort);
}



// Opens the specified serial port 
BOOL CSerialPort::Open(LPCTSTR strPort, DWORD dwBaudRate, DWORD dwTimeOut)
{
	// Make sure that channel is closed
	if (m_hCommPort != INVALID_HANDLE_VALUE)
		return FALSE;
	
	// Open com port
	m_hCommPort = CreateFile(strPort, GENERIC_READ|GENERIC_WRITE, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	// Communication buffers
	if (! SetupComm(m_hCommPort, 1024, 1024))
	{
		CloseHandle(m_hCommPort);
		return FALSE;
	}

	// Set the serial communications device
	DCB dcb;
	ZeroMemory (&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = dwBaudRate;
	dcb.fBinary = 1;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if (!SetCommState(m_hCommPort, &dcb))
	{
		CloseHandle(m_hCommPort);
		return FALSE;
	}

	if (! SetTimeOut(dwTimeOut))
	{
		CloseHandle(m_hCommPort);
		return FALSE;
	}

	return TRUE;
}


BOOL CSerialPort::SetTimeOut(DWORD dwTimeOut)
{
	// Set timeouts
	COMMTIMEOUTS ctmo;
	ZeroMemory (&ctmo, sizeof(COMMTIMEOUTS));
	ctmo.ReadIntervalTimeout = dwTimeOut;
	ctmo.ReadTotalTimeoutMultiplier = dwTimeOut;
	ctmo.ReadTotalTimeoutConstant = dwTimeOut;

	return SetCommTimeouts(m_hCommPort, &ctmo);
}


// Flushes the input and output buffers of the serial port
BOOL CSerialPort::Flush()
{
	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	if (! PurgeComm(m_hCommPort, PURGE_TXCLEAR|PURGE_RXCLEAR))
		return FALSE;

	return TRUE;
}


// Closes the serial port
BOOL CSerialPort::Close()
{
	if (m_hCommPort != INVALID_HANDLE_VALUE)
		CloseHandle (m_hCommPort);

	m_hCommPort = INVALID_HANDLE_VALUE;

	return TRUE;
}



// Writes 'dwSize' bytes to the serial port
BOOL CSerialPort::Write(UCHAR *buffer, DWORD dwSize)
{
	// Return if comm port handler is not valid
	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;
	
	DWORD dwWritten;

	if (!WriteFile (m_hCommPort, buffer, dwSize, &dwWritten, NULL))
		return FALSE;

	if (dwWritten != dwSize)
		return FALSE;

	return TRUE;
}



// Reads 'dwSize' bytes from the serial port
BOOL CSerialPort::Read(UCHAR *buffer, DWORD dwSize)
{
	// Return if comm port handler is not valid
	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwRead;
	
	if (!ReadFile (m_hCommPort, buffer, dwSize, &dwRead, NULL))
		return FALSE;

	if (dwRead != dwSize)
		return FALSE;

	return TRUE;
}

