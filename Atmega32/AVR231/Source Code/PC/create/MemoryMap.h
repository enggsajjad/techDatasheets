//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:		MemoryMap.h
// Compiler:		Standard C++ compiler.
// Output Size:
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		21-Mar-2005 (modified for AES)   RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		MemoryMap is a list (vector) of MemoryChunks 
//			that together
//			constitute the contents of a certain 
//			memory, e.g. AVR flash
//			memory contents. It is possible to access the MemoryMap
//			as an array, so the internal structure
//			 is that way hidden
//			from the user.
//
// Other Info:		
//=============================================================================


#ifndef MEMORYMAP_H
#define MEMORYMAP_H


#include "MemoryChunk.h"
#include "DataBuffer.h"
#include <vector>


typedef std::vector<MemoryChunk, std::allocator<MemoryChunk> > MemoryChunkVector;

class MemoryMap : private MemoryChunkVector
{
public:
	MemoryMap(unsigned char emptyFill = 0x00);
	MemoryMap(const char *filename, unsigned int start = 0x00000000,
		unsigned int end = 0xffffffff, unsigned char emptyFill = 0x00);

	virtual ~MemoryMap();

	bool in(unsigned int address);
	unsigned char operator [] (unsigned int address);
	unsigned char at(unsigned int address);
	void insert(MemoryChunk& chunk);
	void insert(unsigned int address, const DataBuffer& buffer);
	unsigned int getLowestAddress();
	unsigned int getHighestAddress();

private:
	unsigned char emptyFillM;			// byte that is used as the fill of
										// empty areas in the memory
	unsigned int loLimitM;				// Lowest address containing data
	unsigned int hiLimitM;				// Highest address containing data
};


#endif // MEMORYMAP_H
