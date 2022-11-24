//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			ConfigParser.h
// Compiler:		Microsoft Visual C++ 6.0
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		ConfigParser class parses the configuration file.
//
//					Please refer to Application Note Documentation for more
//					information.
//
// Other Info:		
//=============================================================================


#ifndef CONFIGPARSER_h
#define CONFIGPARSER_h


#include "databuffer.h"


class ConfigParser  
{
private:
	int pageSizeM;				// Page size in target AVR (in bytes)
	int memSizeM;				// Size of the Application Section (in bytes)

	int keyCountM;				// Number of keys in DES encryption
	DataBuffer keyM[3];			// DES encryption keys

	DataBuffer initialVectorM;	// Initial Vector for CBC
	DataBuffer signatureM;		// Frame signature for valid frame verification

	bool crcFlashEnableM;		// Calculate CRC of the Application Section?

public:
	ConfigParser(char *filename);
	virtual ~ConfigParser();

	int getPageSize() { return pageSizeM; };
	int getMemSize() { return memSizeM; };
	int getKeyCount() { return keyCountM; };
	DataBuffer& getKey(int index) { return keyM[index]; };
	DataBuffer *getKeys() { return keyM; };
	
	DataBuffer& getInitialVector() { return initialVectorM; };
	DataBuffer& getSignature() { return signatureM; };
	bool getCRCFlashEnable() {return crcFlashEnableM; };
};

#endif // CONFIGPARSER_H
