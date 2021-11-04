#include "stdafx.h"

#include "BMPWriter.h"

using namespace gfc;

// Note: This function assumes that pixels have an XRGB8 or ARGB8 format
// pixelsSize is the size in bytes of the pixels array;
// pitch is the image pitch in bytes;
// width and height are measured in pixels
bool BMPWriter::write( const uint8_t* pixels, uint32_t pixelsSize, uint16_t width, uint16_t height, uint32_t pitch, ImageFormat format, const char* filename )
{
	AutoRef<File> file = gfcNew( File )( String( filename ) );

	AutoRef<OutputStream> stream = file->getOutputStream();
	if( stream == NULL )
	{
		gfcLog( CH_GRAPHICS, LV_ERROR, "BMPWriter::write: Could not open the file %s for writing!", filename );
		return false;
	}

	// .bmp files have a little-endian byte ordering
	stream->setEndianess( Endian::kLittleEndian );

	// 14: sizeof( BITMAPFILEHEADER )
	// 40: sizeof( BITMAPINFOHEADER )
	uint32_t headerSize = 14 + 40;
	uint32_t fileSize = headerSize + pixelsSize;

	gfcDebug( "BMPWriter::write: Writing a %d x %d texture occupying %d bytes to %s\n", width, height, pixelsSize, filename );

	// A .bmp file is a BITMAPFILEHEADER struct followed by a BITMAPINFOHEADER
	// and then the uncompressed pixel data

	// Write the BITMAPFILEHEADER contents

	stream->writeUInt16( 0x4D42 );		// bfType: 'BM', BMP magic number
	stream->writeUInt32( fileSize );	// bfSize: .bmp file size in bytes
	stream->writeUInt16( 0 );			// bfReserved1: unused and ignored
	stream->writeUInt16( 0 );			// bfReserved2: unused and ignored
	stream->writeUInt32( headerSize );	// bfOffBits: byte offset to the pixel data

	// Write the BITMAPINFOHEADER contents

	uint16_t bytesPerPixel = (uint16_t) ImageGetBytesPerPixel( format );
	uint16_t bitsPerPixel = bytesPerPixel * 8;

	stream->writeUInt32( 40 );				// biSize: sizeof( BITMAPINFOHEADER )
	stream->writeInt32( width );			// biWidth
	stream->writeInt32( height );			// biHeight
	stream->writeUInt16( 1 );				// biPlanes
	stream->writeUInt16( bitsPerPixel );	// biBitCount
	stream->writeUInt32( 0 );				// biCompression: 0 == BI_RGB
	stream->writeUInt32( pixelsSize );		// biSizeImage
	stream->writeInt32( 0 );				// biXPelsPerMeter
	stream->writeInt32( 0 );				// biYPelsPerMeter
	stream->writeUInt32( 0 );				// biClrUsed
	stream->writeUInt32( 0 );				// biClrImportant

	// Write the pixel data, flipping it vertically and byteswapping each pixel
	// Yes, this can be done more efficiently....

	for( int y = height - 1; y >= 0; --y )
	{
		for( int x = 0; x < width; ++x )
		{
			uint32_t index = y * pitch + x * bytesPerPixel;
			uint32_t pixel = *( (uint32_t*) &pixels[ index ] );

			stream->writeUInt32( pixel );
		}
	}

	stream->close();

	return true;
}
