#pragma once

#include <cstdint>

class ProgressBar
{
public:
	ProgressBar();
	~ProgressBar();

	// Create and display a progress bar that goes from 0 to range
	// with increments of step units. If title != NULL then that string
	// will be displayed in the title bar of the progress bar window.
	void open( HWND parent, uint16_t range = 100, uint16_t step = 1, const char* title = NULL );
	void close( void );

	// Increment the bar by the step value given to open()
	void step( void );

	// Set the bar's display to 'position' units
	// position must be between 0 and the range value given to open()
	void setPosition( uint16_t position );

private:
	HWND mProgressBar;

}; // class ProgressBar
