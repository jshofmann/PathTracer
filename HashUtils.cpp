#include "stdafx.h"

#include "HashUtils.h"

// This code is derived from the public domain CRC-32 v1.04 program
// written by Craig Bruce on 5-Dec-1994 and available at
// http://www.csbruce.com/~csbruce/software/crc32.c. His attribution:
//
// "Based on 'File Verification Using CRC' by Mark R. Nelson in Dr. Dobb's
//	Journal, May 1992, pp. 64-67.  This program DOES generate the same CRC
//	values as GZIP, PKZIP, and ZMODEM."
//
// Additional features are derived from the paper "A Painless Guide to CRC
// Error Detection Algorithms" by Ross N. Williams and code posted at
// http://www.ross.net/crc/download/crc_v3.txt.

#include "CRC32.inl"

// CalculateCRC32( const Byte* const data, const size_t length )
//
// Calculate and return the CRC-32 hash code for the given data.
//
//	-> data	  -- The byte array to generate the hash code for
//	-> length -- The size in bytes of the data array
// <-		  -- The CRC-32 hash code for the data array
uint32_t CalculateCRC32( const uint8_t* const data, const size_t length )
{
	uint32_t crc = 0xFFFFFFFF;

	for( size_t i = 0; i < length; ++i )
	{
		crc = ( crc >> 8 ) ^ CRC32Table[ ( crc ^ data[ i ] ) & 0xFF ];
	}

	return ( crc ^ 0xFFFFFFFF );
}
