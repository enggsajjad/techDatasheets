//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:		ConfigParser.cpp
// Compiler:		Standard C++ compiler.
// Output Size:
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		28-Feb-2005 (for AES)     RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		ConfigParser class parses the configuration file.
//
//			Please refer to Application Note Documentation for more
//			information.
//
// Other Info:		
//=============================================================================


#include "ConfigParser.h"
#include "CreateException.h"
#include <stdlib.h>
#include <stdio.h>


//=============================================================================
// Constructor. Parses the configuration file.

ConfigParser::ConfigParser(char *filename) : 
	pageSizeM(0), memSizeM(0), keyCountM(0), crcFlashEnableM(false)
{
	bool keyDefined[3] = {false, false, false};
		
	keyM[0] = "010101010101010101010101010101010101";
	keyM[1] = "010101010101010101";
	keyM[2] = "010101010101010101";

	initialVectorM = "00000000000000000000000000000000";
	signatureM = "0000000000000000";

	// Read setup file
	FILE *inFile = fopen(filename, "r");

	if (!inFile)
		throw new CreateException(ERROR_FILE_NOT_FOUND, filename);

	// Read line by line until the whole file is read
	while (!feof(inFile))
	{
		char lvalue[256], rvalue[128];
		
		if (fscanf(inFile, "%s = %s", lvalue, rvalue) < 2)
			continue;

		for( char * t = lvalue; *t; ++t ) *t = toupper( *t );
		for( char * t = rvalue; *t; ++t ) *t = toupper( *t );

		// Try to find a match for the found parameter
		if (strstr(lvalue, "PAGE_SIZE"))
			sscanf(rvalue, "%d", &pageSizeM);
		else if (strstr(lvalue, "MEM_SIZE"))
			sscanf(rvalue, "%d", &memSizeM);
		else if (strstr(lvalue, "KEY"))
		{
			unsigned char index = lvalue[3] - '1';

			keyM[index] = rvalue;
			if (keyM[index].size() != (index == 0 ? 18 : 9) )
				throw new CreateException(ERROR_KEY, lvalue);

			keyDefined[index] = true;
		}
		else if (strstr(lvalue, "INITIAL_VECTOR"))
		{
			initialVectorM = rvalue;
			if (initialVectorM.size() != 16)
				throw new CreateException(ERROR_INITIAL_VECTOR, rvalue);
		}
		else if (strstr(lvalue, "SIGNATURE"))
		{
			signatureM = rvalue;
			if (signatureM.size() != 4)
				throw new CreateException(ERROR_SIGNATURE, rvalue);
		}
		else if (strstr(lvalue, "CRC_ENABLE"))
		{
			if (strstr(rvalue, "YES"))
				crcFlashEnableM = true;
			else if (strstr(rvalue, "NO"))
				crcFlashEnableM = false;
			else
				throw new CreateException(ERROR_CRC, rvalue);
		}
		else
		{
			strcat( lvalue, " = " );
			strcat( lvalue, rvalue );
			throw new CreateException(ERROR_CONFIGURATION_PARAMETER, lvalue);
		}
	}

	if (pageSizeM == 0)
		throw new CreateException(ERROR_PAGE_SIZE_REQUIRED, filename);

	// Count the final number of keys and check that there are no keys missing
	keyCountM = 0;
	for (int i = 0; i < 3; i++)
	{
		char *keyString = "KEYx";

		if (keyDefined[i])
		{
			if (keyCountM != i)
			{
				keyString[3] = i - 1 + '1';
				throw new CreateException(ERROR_KEY_NOT_DEFINED, keyString);
			}
			keyCountM++;
		}
	}
}


//=============================================================================
// Destructor

ConfigParser::~ConfigParser()
{
}
