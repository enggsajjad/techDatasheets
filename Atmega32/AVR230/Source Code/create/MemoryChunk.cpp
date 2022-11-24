//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			MemoryChunk.cpp
// Compiler:		Microsoft Visual C++ 6.0
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		MemoryChunk is inherited from DataBuffer. Additionally,
//					there is an association to a memory location using
//					'addressM' member.
//
// Other Info:		
//=============================================================================


#include "MemoryChunk.h"


//=============================================================================
// Basic constructor

MemoryChunk::MemoryChunk(unsigned char emptyFill) : addressM(0),
	emptyFillM(emptyFill), DataBuffer()
{
	// empty
}


//=============================================================================
// Constructor, that associates a DataBuffer to an address.

MemoryChunk::MemoryChunk(int address, const DataBuffer& data,
	unsigned char emptyFill) : addressM(address), emptyFillM(emptyFill),
	DataBuffer(data)
{
	// empty
}


//=============================================================================
// Copy constructor

MemoryChunk::MemoryChunk(const MemoryChunk& chunk) : DataBuffer(chunk), 
	addressM(chunk.addressM)
{
	// empty
}


//=============================================================================
// Destructor

MemoryChunk::~MemoryChunk()
{
	// empty
}


//=============================================================================
// Assignment, rvalue of type MemoryChunk. Just makes a copy of rvalue.

MemoryChunk& MemoryChunk::operator =(const MemoryChunk& rvalue)
{
	if (this != &rvalue)
	{
		DataBuffer::operator = (rvalue);
		addressM = rvalue.addressM;
		emptyFillM = rvalue.emptyFillM;
	}

	return *this;
}


//=============================================================================
// Returns true, if the address is inside the MemoryChunk area.

bool MemoryChunk::in(unsigned int address)
{
	return (address >= getStartAddress()) && (address <= getEndAddress());
}


//=============================================================================
// Get a byte from the MemoryChunk, if the address is inside the MemoryChunk.
// If MemoryChunk is 'missed', returns 'emptyFillM'.

unsigned char MemoryChunk::operator [] (int address)
{
	return in(address) ?
		DataBuffer::operator [](address - addressM) : emptyFillM;
}


//=============================================================================
// Returns the start address of the chunk

unsigned int MemoryChunk::getStartAddress()
{
	return addressM;
}


//=============================================================================
// Returns the end address of the chunk

unsigned int MemoryChunk::getEndAddress()
{
	return addressM + size() - 1;
}
