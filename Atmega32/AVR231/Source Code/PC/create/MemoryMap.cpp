//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:		MemoryMap.cpp
// Compiler:		Standard C++ compiler.
// Output Size:
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		21-Mar-2005 (modified for AES)  RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		MemoryMap is a list (vector) of MemoryChunks 
//			that together
//			constitute the contents of a certain memory, e.g. AVR
//			flash memory contents. It is possible to access the
//			MemoryMap as an array, so the internal structure is that
//			way hidden from the user.
//
// Other Info:		
//=============================================================================


#include "MemoryMap.h"
#include "CreateException.h"


//=============================================================================
// Basic constructor

MemoryMap::MemoryMap(unsigned char emptyFill) :
	emptyFillM(emptyFill),
	loLimitM(0xffffffff),
	hiLimitM(0x00000000)
{
	// empty
}


//=============================================================================
// Constructor, that reads the contents from an Intel Hex formatted file.

MemoryMap::MemoryMap(const char *filename, unsigned int start,
		     unsigned int end, unsigned char emptyFill) :
	emptyFillM(emptyFill),
	loLimitM(0xffffffff),
	hiLimitM(0x00000000)
{
	if (!filename) return;

	FILE *inFile = fopen(filename, "r");

	if (!inFile)
		throw new CreateException(ERROR_FILE_NOT_FOUND, filename);

	int highAddress = 0;

	while (!feof(inFile))
	{
		char line[300];
		unsigned char length, type;
		int address;

		if (fscanf(inFile, ":%s\n", line) != 1)
			throw new CreateException(ERROR_HEX_FORMAT, filename);

		// Read the first 5 bytes from the current line of in_file (including
		// the ':') into length, addr and type.
		DataBuffer data(line);

		// Count checksum and make sure that the file is not damaged
		unsigned char chksum = 0;

		for (unsigned int i = 0; i < data.size(); i++)
			chksum += data[i];

		if (chksum != 0)
			throw new CreateException(ERROR_FILE_DAMAGED, filename);

		length = data[0];
		address = (data[1] << 8) | data[2];
		type = data[3];

		// Remove header and crc from the data
		data = data.substr(4, length);

		switch (type)
		{
			// Data Record (8-, 16-, 32-bit formats)
			case 0:
				try
				{
					insert(address + highAddress, data);
				}

				catch (CreateException *e)
				{
					if (e->getCode() == ERROR_OVERLAPPING_DATA)
					{
						ErrorCode code = e->getCode();
						delete e;

						throw new CreateException(code, filename);
					}
					else
						throw e;
				}
				break;

			// End of File Record (8-, 16-, 32-bit formats)
			case 1:
				return;

			// Extended Segment Address Record (16- or 32-bit formats)
			case 2:
				highAddress = (data[0] << 12) + (data[1] << 4);
				break;

			// Start Segment Address Record (16- or 32-bit formats)
			case 3:
				break;

			// Extended Linear Address Record (32-bit format)
			case 4:
				highAddress = (data[0] << 24) + (data[1] << 16);
				break;

			// Start Linear Address Record (32-bit format)
			case 5:
				break;

			// Unknown type
			default:
				throw new CreateException(ERROR_FILE_DAMAGED, filename);
		}
	}
}


//=============================================================================
// Destructor

MemoryMap::~MemoryMap()
{
	// empty
}


//=============================================================================
// Returns true, if some data resides in the indexed memory location

bool MemoryMap::in(unsigned int address)
{
	for (MemoryChunkVector::iterator i = begin(); i != end(); i++)
		if (i->in(address))
			return true;

	return false;
};


//=============================================================================
// Accesses memory by address index

unsigned char MemoryMap::operator [] (unsigned int address)
{
	for (MemoryChunkVector::iterator i = begin(); i != end(); i++)
		if (i->in(address))
			return (*i)[address];

	return emptyFillM;
}


//=============================================================================
// Same as 'operator []', except easier to use with pointers to objects
// i.e. in cases like MemoryMap->at().
// (otherwise user would need to (*MemoryMap)[] which looks clumsy).

unsigned char MemoryMap::at(unsigned int address)
{
	return (*this)[address];
}


//=============================================================================
// Insert a MemoryChunk to MemoryMap

void MemoryMap::insert(MemoryChunk& chunk)
{
	unsigned int address;

	for (address = chunk.getStartAddress();
		address <= chunk.getEndAddress(); address++)
		if (in(address))
			throw new CreateException(ERROR_OVERLAPPING_DATA, "");
	
	push_back(chunk);

	if (loLimitM > chunk.getStartAddress())
		loLimitM = chunk.getStartAddress();

	if (hiLimitM < chunk.getEndAddress())
		hiLimitM = chunk.getEndAddress();
}

//=============================================================================
// Insert a DataBuffer (associated with an address) to MemoryMap

void MemoryMap::insert(unsigned int address, const DataBuffer& buffer)
{
	MemoryChunk tmp(address, buffer, emptyFillM);
	insert(tmp);
}


//=============================================================================
// Find the lowest used address in the MemoryMap

unsigned int MemoryMap::getLowestAddress()
{
	return loLimitM;
}


//=============================================================================
// Find the highest used address in the MemoryMap

unsigned int MemoryMap::getHighestAddress()
{
	return hiLimitM;
}

