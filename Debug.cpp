#include "stdafx.h"

#include <stdarg.h>
#include <stdio.h>

#include "Debug.h"

void vdebugf( const char* format, ... )
{
	static constexpr size_t kMaxMessageLength = 1024;
	char buf[ kMaxMessageLength ];

	va_list args;
	va_start( args, format );

	int written = vsnprintf_s( buf, sizeof( buf ), format, args );
	va_end( args );

	if( written >= 0 )
	{
		OutputDebugString( buf );
	}
}
