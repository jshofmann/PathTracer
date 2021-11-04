#pragma once

#include <stdint.h>

#include "BMPSupport.h"

class BMPReader
{
public:
	BMPReader();

	// Assumes that data contains the contents of a .bmp format file.
	// This function will not copy the data array given to it; this data
	// must remain valid until the BMPReader instance is destroyed.
	bool set( const void* data, uint32_t dataSizeInBytes );

	uint32_t getImageSize( void ) const;

	int getWidth( void ) const { return mInfoHeader.biWidth; }
	int getHeight( void ) const { return mInfoHeader.biHeight; }
	uint16_t getBitCount( void ) const { return mInfoHeader.biBitCount; }

	uint32_t getCompression( void ) const { return mInfoHeader.biCompression; }

	// If false, the origin is in the bottom left corner
	bool hasTopLeftOrigin( void ) const { return mInfoHeader.biSizeImage < 0; }

	const uint8_t* getPixelData( void ) const { return mPixelData; }

private:
	BitmapFileHeader	mFileHeader;
	BitmapInfoHeader	mInfoHeader;
	const uint8_t*		mPixelData;

}; // class BMPReader
