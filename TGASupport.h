#pragma once

#include <stdint.h>

#if defined( COMPILER_MSVC )
	#pragma pack( push, p, 1 )
#elif defined( COMPILER_GCC )
	#pragma pack( push, 1 )
#elif defined( COMPILER_CLANG )
	#pragma pack( 1 )
#endif

// as defined by http://en.wikipedia.org/wiki/Truevision_TGA
// All values are specified in little-endian format
class TGAHeader
{
public:
	enum
	{
		IMAGE_TYPE_NO_IMAGE_DATA                 = 0,
		IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED     = 1,
		IMAGE_TYPE_UNCOMPRESSED_RGB              = 2,
		IMAGE_TYPE_UNCOMPRESSED_GREYSCALE        = 3,
		IMAGE_TYPE_RLE_COLOR_MAPPED              = 9,
		IMAGE_TYPE_RLE_RGB                       = 10,
		IMAGE_TYPE_COMPRESSED_GREYSCALE          = 11,
		IMAGE_TYPE_COMPRESSED_COLOR_MAPPED       = 32,
		IMAGE_TYPE_COMPRESSED_COLOR_MAPPED_4PASS = 33
	};

	TGAHeader()
		: IDLength( 0 )
		, ColormapType( 0 )
		, ImageType( IMAGE_TYPE_NO_IMAGE_DATA )
		, ColormapOrigin( 0 )
		, ColormapLength( 0 )
		, ColormapWidth( 0 )
		, XOrigin( 0 )
		, YOrigin( 0 )
		, ImageWidth( 0 )
		, ImageHeight( 0 )
		, BitsPerPixel( 0 )
		, ImageDescriptor( 0x20 )
	{
	}

	uint8_t		IDLength;			// 0
	uint8_t		ColormapType;		// 0 (no colormap included)
	uint8_t		ImageType;			// From the above enum
	uint16_t	ColormapOrigin;		// 0
	uint16_t	ColormapLength; 	// 0
	uint8_t		ColormapWidth;		// 0
	uint16_t	XOrigin;			// pixels, usually 0
	uint16_t	YOrigin;			// pixels, usually 0
	uint16_t	ImageWidth;			// pixels
	uint16_t	ImageHeight;		// pixels
	uint8_t		BitsPerPixel;
	uint8_t		ImageDescriptor;	// 0x20 defines a top-left origin

}; // class TGAHeader

#if defined( COMPILER_MSVC )
	#pragma pack( pop, p )
#elif defined( COMPILER_GCC )
	#pragma pack( pop )
#elif defined( COMPILER_CLANG )
	#pragma options align = reset
#endif
