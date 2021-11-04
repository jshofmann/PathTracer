#pragma once

#include <stdint.h>

#include "TGASupport.h"

class TGAReader
{
public:
	TGAReader();

	// Assumes that data contains the contents of a .tga format file.
	// This function will not copy the data array given to it; this data
	// must remain valid until the TGAReader instance is destroyed.
	bool set( const void* data, uint32_t dataSizeInBytes );

	uint8_t getImageType( void ) const { return mHeader.ImageType; }

	uint16_t getWidth( void ) const { return mHeader.ImageWidth; }
	uint16_t getHeight( void ) const { return mHeader.ImageHeight; }
	uint8_t  getBitsPerPixel( void ) const { return mHeader.BitsPerPixel; }

	// If false, the origin is in the bottom left corner
	bool hasTopLeftOrigin( void ) const { return mHeader.ImageDescriptor == 0x20; }

	uint32_t getImageSize( void ) const { return mImageSize; }

	const uint8_t* getPixelData( void ) const { return mPixelData; }

private:
	TGAHeader		mHeader;
	uint32_t		mImageSize;
	const uint8_t*	mPixelData;

}; // class TGAReader
