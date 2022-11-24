//=============================================================================
// Copyright Atmel Corporation 2003. All Rights Reserved.
//
// File:		CreateException.h
// Compiler:		Standard C++ compiler.
// Output Size:
// Created:		4-Feb-2003	JP (Atmel Finland)
// Modified:		21-Mar-2005 (modified for AES)   RAA (Atmel Norway)
//
// Support Mail:	avr@atmel.com
//
// Description:		Routines for exception generation.
//
// Other Info:		
//=============================================================================

#ifndef CREATEEXCEPTION_H
#define CREATEEXCEPTION_H

typedef enum
{
	ERROR_OK,
	ERROR_FILE_NOT_FOUND,
	ERROR_CONFIGURATION_PARAMETER,
	ERROR_HEX_FORMAT,
	ERROR_FILE_DAMAGED,
	ERROR_KEY,
	ERROR_KEY_PARITY,
	ERROR_INITIAL_VECTOR,
	ERROR_SIGNATURE,
	ERROR_OUT_OF_MEMORY,
	ERROR_CREATE_FILE,
	ERROR_OVERLAPPING_DATA,
	ERROR_UNKNOWN_ARGUMENT,
	ERROR_CONFIG_NOT_DEFINED,
	ERROR_PAGE_SIZE_REQUIRED,
	ERROR_CRC,
	ERROR_KEY_NOT_DEFINED,
	ERROR_NUM
} ErrorCode;


class CreateException
{
	ErrorCode codeM;			// Error code number
	char *reasonM;				// String describing the reason for exception

public:
	CreateException(ErrorCode code, const char *offender);
	virtual ~CreateException();
	char *getReason();
	ErrorCode getCode();
};


#endif // CREATEEXCEPTION_H

