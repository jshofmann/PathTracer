#pragma once

#include <stdint.h>

class BMPWriter
{
public:
	// Note: This function assumes that pixels have an XRGB8 or ARGB8 format
	// pixelsSize is the size in bytes of the pixels array;
	// pitch is the image pitch in bytes;
	// width and height are measured in pixels
	static bool write( const uint8_t* pixels, uint32_t pixelsSize, uint16_t width, uint16_t height, uint32_t pitch, const char* filename );
};
