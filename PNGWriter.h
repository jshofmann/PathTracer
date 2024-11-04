#pragma once

#include <stdint.h>

namespace PNGWriter
{
	bool write( const uint8_t* pixels, uint16_t width, uint16_t height,
				uint8_t bytesPerPixel, const char* filename );
}
