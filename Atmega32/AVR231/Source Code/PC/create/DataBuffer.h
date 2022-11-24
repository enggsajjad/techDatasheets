//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:		DataBuffer.h
// Compiler:		Standard C++ compiler
// Output Size:
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		21-Mar-2005 (modified for AES)  RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		DataBuffer class allows the user to 
//			dynamically manipulate
//			buffers of unsigned chars. The class is based on
//			basic_string template. The functionality is 
//			basically the
//			same as in basic_string, except some functionality for
//			conversions between hex-format strings and unsigned char
//			buffers are added.
//
// Other Info:		
//=============================================================================

#ifndef DATABUFFER_H
#define DATABUFFER_H

#include "ByteString.h"


class DataBuffer : public ByteString
{
public:
	DataBuffer();
	DataBuffer(const DataBuffer& buffer);
	DataBuffer(const char *hexString);
	DataBuffer(const char *hexString, size_t length);
	DataBuffer(const unsigned char *data, size_t length);
	DataBuffer(size_t size, unsigned char fill = 0);

	~DataBuffer();

	DataBuffer& operator =(const DataBuffer& buffer);
	DataBuffer& operator =(char *rvalue);
	DataBuffer& operator =(const ByteString& string);

	char *toHexString(unsigned int first = 0, int length = -1);
	char *toCHexString(unsigned int first = 0, int length = -1);

private:
	char *hexStringM;	// temporary buffer containing a hex presentation of
						// the data

	void deleteHexString();
	void newHexString(size_t length);
	void parse(const char *hexString, size_t length);
};

#endif // DATABUFFER_H
