// SerialPort.h: interface for the CSerialPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORT_H__82EB0852_FB46_11D3_982B_0060977D1CD3__INCLUDED_)
#define AFX_SERIALPORT_H__82EB0852_FB46_11D3_982B_0060977D1CD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CSerialPort
{
public:
	CSerialPort();
	~CSerialPort();

	// Serial port open command
	// ------------------------

	virtual BOOL Open(LPCTSTR strPort, DWORD dwBaudRate, DWORD dwTimeOut);

	// Overloaded io port commands
	// ---------------------------

	virtual BOOL SetTimeOut(DWORD dwTimeOut);
	virtual BOOL Flush();
	virtual BOOL Close();

	virtual BOOL Write(UCHAR *buffer, DWORD dwSize);
	virtual BOOL Read(UCHAR *buffer, DWORD dwSize);

private:	
	HANDLE m_hCommPort;
};

#endif // !defined(AFX_SERIALPORT_H__82EB0852_FB46_11D3_982B_0060977D1CD3__INCLUDED_)
