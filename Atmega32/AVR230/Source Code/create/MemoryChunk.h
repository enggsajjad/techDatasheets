//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			MemoryChunk.h
// Compiler:		Microsoft Visual C++ 6.0
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		MemoryChunk is inherited from DataBuffer. Additionally,
//					DataBuffer is associated to a memory location using
//					'addressM' member.
//
// Other Info:		
//=============================================================================


#ifndef MEMORYCHUNK_H
#define MEMORYCHUNK_H

#include "DataBuffer.h"


class MemoryChunk : public DataBuffer  
{
public:
	MemoryChunk(int address, const DataBuffer& data, unsigned char emptyFill);
	MemoryChunk(unsigned char emptyFill = 0x00);
	MemoryChunk(const MemoryChunk& chunk);
	virtual ~MemoryChunk();

	MemoryChunk& operator =(const MemoryChunk& rvalue);
	bool in(unsigned int address);
	unsigned char operator[] (int address);

	unsigned int getStartAddress();
	unsigned int getEndAddress();

private:
	unsigned int addressM;
	unsigned char emptyFillM;
};

#endif	// MEMORYCHUNK_H
