#include "stdafx.h"

#include <stdio.h>

#include "TGAWriter.h"
#include "TGASupport.h"

bool TGAWriter::write( const uint8_t* pixels, uint16_t width, uint16_t height, uint8_t bytesPerPixel, const char* filename )
{
	if( ( pixels == NULL ) || ( filename == NULL ) )
	{
		return false;
	}

	FILE* file = fopen( filename, "wb" );
	if( file == NULL )
	{
		OutputDebugString( "fopen failed" );
		return false;
	}

	uint16_t zero = 0;

	fputc( 0, file );										// TGAHeader::IDLength
	fputc( 0, file );										// TGAHeader::ColormapType
	fputc( TGAHeader::IMAGE_TYPE_UNCOMPRESSED_RGB, file );	// TGAHeader::ImageType
	fwrite( &zero, sizeof( uint16_t ), 1, file );			// TGAHeader::ColormapOrigin
	fwrite( &zero, sizeof( uint16_t ), 1, file );			// TGAHeader::ColormapLength
	fputc( 0, file );										// TGAHeader::ColormapWidth
	fwrite( &zero, sizeof( uint16_t ), 1, file );			// TGAHeader::XOrigin
	fwrite( &zero, sizeof( uint16_t ), 1, file );			// TGAHeader::YOrigin
	fwrite( &width, sizeof( uint16_t ), 1, file );			// TGAHeader::ImageWidth
	fwrite( &height, sizeof( uint16_t ), 1, file );			// TGAHeader::ImageHeight
	fputc( bytesPerPixel * 8, file );						// TGAHeader::BitsPerPixel
//	fputc( 0x20, file );									// TGAHeader::ImageDescriptor (top left)
	fputc( 0, file );										// TGAHeader::ImageDescriptor (lower left)

	int size = width * height * bytesPerPixel;
	fwrite( pixels, size, 1, file );

	fclose( file );

	return true;
}
