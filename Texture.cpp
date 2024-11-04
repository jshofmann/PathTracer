#include "stdafx.h"

#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include "Texture.h"
#include "Debug.h"
#include "BMPReader.h"
#include "TGAReader.h"

// Note: callers should delete[] the returned pointer when done
static uint8_t* ReadFile( const char* filename, size_t& sizeInBytes )
{
	sizeInBytes = 0;

	FILE* file = fopen( filename, "r+b" );
	if( file == NULL )
	{
		debugf( "ReadFile: Failed to open %s due to error %d: %s\n", filename, errno, strerror( errno ) );
		return nullptr;
	}

	fseek( file, 0, SEEK_END );
	size_t length = ftell( file );
	fseek( file, 0, SEEK_SET );

	uint8_t* data = new uint8_t[ length ];
	if( data == nullptr )
	{
		fclose( file );
		return nullptr;
	}

	size_t bytesRead = fread( data, 1, length, file );
	if( bytesRead == 0 )
	{
		if( ferror( file ) != 0 )
		{
			debugf( "ReadFile: fread() failed, perhaps due to error %d: %s\n", errno, strerror( errno ) );
		}
	}
	else if( bytesRead < length )
	{
		debugf( "ReadFile: Read only %d of %d bytes, perhaps due to error %d: %s\n", bytesRead, length, errno, strerror( errno ) );
	}

	fclose( file );

	sizeInBytes = length;
	return data;
}

// Read in the given .dds, .tga, or .bmp file, decode the pixels if necessary,
// and return an ARGB8 array containing the image data. Does not support all
// DDS files - just 2D textures, no cubemaps, volumes, arrays, etc., and
// does not support all DXGI_FORMAT types either, just some of the uncompressed
// ones - ARGB8, XRGB8, etc. Note that callers will be expected to delete[]
// the returned memory.
static void* ReadTextureFile( const char* filename, uint16_t& width, uint16_t& height, uint16_t& bytesPerPixel, const uint8_t*& pixels )
{
	size_t size;
	uint8_t* file = ReadFile( filename, size );
	if( ( file == nullptr ) || ( size == 0 ) )
	{
		return nullptr;
	}

	// Peek at the first four bytes to identify the file type
	if( ( file[ 0 ] == 'D' ) && ( file[ 1 ] == 'D' ) && ( file[ 2 ] == 'S' ) )
	{
		// Probably a DDS file
	}
	else if( ( file[ 0 ] == 'B' ) && ( file[ 1 ] == 'M' ) && ( file[ 2 ] == ' ' ) )
	{
		// Probably a BMP file
		BMPReader reader;
		if( !reader.set( file, uint32_t( size ) ) )
		{
			delete[] file;
			return nullptr;
		}

		width = reader.getWidth();
		height = reader.getHeight();
		bytesPerPixel = reader.getBitCount() / 8; // bits per pixel -> bytes per pixel
		pixels = reader.getPixelData();

		return file;
	}
	else
	{
		// Assume a TGA file; they have no "magic number" or other way
		// to unambiguously identify the file contents as TGA-format pixels
		TGAReader reader;
		if( !reader.set( file, uint32_t( size ) ) )
		{
			delete[] file;
			return nullptr;
		}

		width = reader.getWidth();
		height = reader.getHeight();
		bytesPerPixel = reader.getBitsPerPixel() / 8; // bits per pixel -> bytes per pixel
		pixels = reader.getPixelData();

		return file;
	}

	delete[] file;

	return nullptr;
}

ImageTexture::ImageTexture( const char* filename )
{
	mFile = ReadTextureFile( filename, mWidth, mHeight, mBytesPerPixel, mPixels );
}

ImageTexture::~ImageTexture()
{
	if( mFile != nullptr )
		delete[] mFile;
}

vec3 ImageTexture::getValue( float u, float v, const vec3& p ) const
{
	size_t col = size_t( u * mWidth );
	size_t row = size_t( ( 1.0f - v ) * mHeight - 0.001f );

	// Implement a clamp-style texture address mode;
	// (u, v) values outside of [0, 1] will clamp to the nearest border texel
	if( row < 0 )
		row = 0;
	if( row >= mHeight )
		row = mHeight - 1;

	if( col < 0 )
		col = 0;
	if( col >= mWidth )
		col = mWidth - 1;

	uint32_t stride = mBytesPerPixel * mWidth;

	float r = mPixels[ stride * row + mBytesPerPixel * col     ] / 255.0f;
	float g = mPixels[ stride * row + mBytesPerPixel * col + 1 ] / 255.0f;
	float b = mPixels[ stride * row + mBytesPerPixel * col + 2 ] / 255.0f;

	return vec3( r, g, b );
}
