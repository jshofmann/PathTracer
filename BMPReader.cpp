#include "stdafx.h"

#include "BMPReader.h"

BMPReader::BMPReader()
	: mPixelData( NULL )
{
	memset( &mFileHeader, 0, sizeof( BitmapFileHeader ) );
	memset( &mInfoHeader, 0, sizeof( BitmapInfoHeader ) );
}

// Assumes that data contains the contents of a .bmp format file.
// This function will not copy the data array given to it; this data
// must remain valid until the BMPReader instance is destroyed.
bool BMPReader::set( const void* data, uint32_t dataSizeInBytes )
{
	// 54 == sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	// if data's size is smaller than that this cannot be a BMP file
	if( dataSizeInBytes < 54 )
		return false;

	// cast 'data' to something we can do pointer arithmetic on
	const uint8_t* bmpData = reinterpret_cast< const uint8_t* >( data );

	// 14: sizeof( BITMAPFILEHEADER )
	const uint8_t* infoHeader = bmpData + 14;

	memcpy( &mFileHeader, data, sizeof( BitmapFileHeader ) );
	memcpy( &mInfoHeader, infoHeader, sizeof( BitmapInfoHeader ) );

	mPixelData = bmpData + mFileHeader.bfOffBits;

	return true;
}

uint32_t BMPReader::getImageSize( void ) const
{
	// If BITMAPINFOHEADER::biSizeImage is negative, the sign bit is the
	// flag indicating that the origin is in the top-left corner.
	if( mInfoHeader.biSizeImage < 0 )
	{
		return uint32_t( -mInfoHeader.biSizeImage );
	}

	// If BITMAPINFOHEADER::biSizeImage is 0 (legal if biCompression == BI_RGB)
	// then calculate the image size from BITMAPFILEHEADER::bfSize and
	// BITMAPFILEHEADER::bfOffBits.
	if( mInfoHeader.biSizeImage == 0 )
	{
		return mFileHeader.bfSize - mFileHeader.bfOffBits;
	}

	return uint32_t( mInfoHeader.biSizeImage );
}
