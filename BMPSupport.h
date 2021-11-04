#pragma once

#include <stdint.h>

#if defined( COMPILER_MSVC )
	#pragma pack( push, p, 1 )
#elif defined( COMPILER_GCC )
	#pragma pack( push, 1 )
#elif defined( COMPILER_CLANG )
	#pragma pack( 1 )
#endif

	// Note: All values are specified in little-endian format

	// Adapted from https://msdn.microsoft.com/en-us/library/cc250415.aspx
	// Used by the BITMAPINFOHEADER::biCompression field.
	typedef enum
	{
		kBI_RGB			= 0,
		kBI_RLE8		= 1,
		kBI_RLE4		= 2,
		kBI_BITFIELDS	= 3,
		kBI_JPEG		= 4,
		kBI_PNG			= 5,
		kBI_CMYK		= 11,
		kBI_CMYKRLE8	= 12,
		kBI_CMYKRLE4	= 13
	}
	Compression;

	// Duplicate of the Windows RGBQUAD structure
	typedef struct _RGBQuad
	{
		uint8_t rgbBlue;
		uint8_t rgbGreen;
		uint8_t rgbRed;
		uint8_t rgbReserved;
	}
	RGBQuad;

	// Duplicate of the Windows BITMAPFILEHEADER structure
	class BitmapFileHeader
	{
	public:
		uint16_t	bfType;			// 'BM'
		uint32_t	bfSize;			// the size in bytes of the .bmp file
		uint16_t	bfReserved1;	// 0
		uint16_t	bfReserved2;	// 0
		uint32_t	bfOffBits;		// 54 (sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ))
	};

	// Duplicate of the Windows BITMAPINFOHEADER structure
	class BitmapInfoHeader
	{
	public:
		uint32_t	biSize;
		int32_t		biWidth;
		int32_t		biHeight;
		uint16_t	biPlanes;
		uint16_t	biBitCount;
		uint32_t	biCompression;
		int32_t		biSizeImage; // note: negative values indicate top-left origin (vs. bottom-left)
		int32_t		biXPelsPerMeter;
		int32_t		biYPelsPerMeter;
		uint32_t	biClrUsed;
		uint32_t	biClrImportant;
	};

	// Slightly modified duplicate of the Windows BITMAPINFO structure

	// Note: We can't just use a BITMAPINFO struct because GetDIBits() may want
	// to fill out the bmiColors array if bmiHeader.biBitCount is 1, 4, or 8.
	// BITMAPINFO declares its bmiColors array as [1]; if a palette is required
	// then using BITMAPINFO as-is will corrupt the stack when GetDIBits()
	// writes past the end of the BITMAPINFO struct.
	typedef struct _BitmapInfo
	{
		BitmapInfoHeader bmiHeader;
		RGBQuad          bmiColors[ 16 ]; // support all biBitCounts except 8
	}
	BitmapInfo;

#if defined( COMPILER_MSVC )
	#pragma pack( pop, p )
#elif defined( COMPILER_GCC )
	#pragma pack( pop )
#elif defined( COMPILER_CLANG )
	#pragma options align = reset
#endif
