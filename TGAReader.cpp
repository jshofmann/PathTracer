#include "stdafx.h"

#include "TGAReader.h"

TGAReader::TGAReader()
	: mImageSize( 0 )
	, mPixelData( NULL )
{
	memset( &mHeader, 0, sizeof( TGAHeader ) );
}

// Assumes that data contains the contents of a .tga format file.
// This function will not copy the data array given to it; this data
// must remain valid until the TGAReader instance is destroyed.
bool TGAReader::set( const void* data, uint32_t dataSizeInBytes )
{
	// if data's size is smaller than the header then this cannot be a TGA file
	if( dataSizeInBytes < sizeof( TGAHeader ) )
		return false;

	memcpy( &mHeader, data, sizeof( TGAHeader ) );

	// !!! do some additional sanity checks on the header fields here?

	// Everything following the header is image data
	mImageSize = dataSizeInBytes - sizeof( TGAHeader );

	// cast 'data' to something we can do pointer arithmetic on
	mPixelData = reinterpret_cast< const uint8_t* >( data ) + sizeof( TGAHeader );

	return true;
}
