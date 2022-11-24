//=============================================================================
// Copyright Atmel Corporation 2003. All Rights Reserved.
//
// File:		AES.h
// Compiler:		Standard C++ compiler.
// Output Size:
// Based on work by:OE, VU
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		21-Feb-2005 (for AES)    RAA (Atmel Norway)	
//
// Support Mail:	avr@atmel.com
//
// Description:		Please refer to Application Note Documentation for more
//			information.
//
//=============================================================================

#ifndef AES_H
#define AES_H

#include "DataBuffer.h"

class Aes  
{
public:
	Aes(DataBuffer& initialVector, int keyCount);
	~Aes() {};

	void scheduleKey(DataBuffer key[3]);
	DataBuffer getK(int key, int index) const;
	void encrypt(unsigned char *buffer, unsigned char *chainBlock) const;
	void encryptBuffer(DataBuffer& buffer);

private:
	unsigned char chainBlockM[16];
	unsigned char keyTableM[16+8+8];
	int keyCountM;

	void removeParityBits( const unsigned char * withparity, 
			       unsigned char * withoutparity ) const;
	unsigned char getbit( const unsigned char * bits, unsigned int pos ) const;
	void setbit( unsigned char * bits, unsigned int pos, unsigned char value ) const;
	void checkParity( const unsigned char * bits ) const;
};

#endif // AES_H
