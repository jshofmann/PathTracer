#include "stdafx.h"

#include <commctrl.h>

#include "ProgressBar.h"

extern HINSTANCE gInstance;

ProgressBar::ProgressBar()
	: mProgressBar( NULL )
{
}

ProgressBar::~ProgressBar()
{
	close();
}

void ProgressBar::open( HWND parent, uint16_t range, uint16_t step, const char* title )
{
	if( mProgressBar != NULL )
	{
		close();
	}

	uint16_t parentWidth, parentHeight;

	RECT rect;
	if( GetWindowRect( parent, &rect ) )
	{
		parentWidth = static_cast< uint16_t >( rect.right - rect.left );
		parentHeight = static_cast< uint16_t >( rect.bottom - rect.top );
	}

	// The scroll bar dimensions are arbitrarily picked
	int barWidth = parentWidth / 4; // 25% of the width of the game window
	int barHeight = parentHeight / 10; // 10% of the height of the game window

	// Enforce a minimum dimension
	if( barWidth < 200 )
		barWidth = 200;

	if( barHeight < 70 )
		barHeight = 70;

	// Center the progress bar in the game window
	rect.left = ( parentWidth / 2 ) - ( barWidth / 2 );
	rect.top = ( parentHeight / 2 ) - ( barHeight / 2 );
	rect.right = rect.left + barWidth;
	rect.bottom = rect.top + barHeight;

	DWORD barStyle = WS_CHILD | WS_VISIBLE | PBS_SMOOTH;

	// Add a title bar to the window if a title is set
	if( title != NULL )
	{
		barStyle |= WS_CAPTION;
	}

	// Account for window titles, borders, etc in the progress bar's location
	AdjustWindowRect( &rect, barStyle, FALSE );

	mProgressBar = CreateWindow( PROGRESS_CLASS,
								 title,
								 barStyle,
								 rect.left, rect.top, barWidth, barHeight,
								 parent,
								 (HMENU) NULL,
								 gInstance,
								 NULL );
	if( mProgressBar == NULL )
	{
		return;
	}

	// Enfore the range

	// The progress bar should run from 0 to range with increments of step units
	SendMessage( mProgressBar, PBM_SETRANGE, 0, MAKELPARAM( 0, range ) );
	SendMessage( mProgressBar, PBM_SETSTEP, (WPARAM) step, 0 );
}

void ProgressBar::close( void )
{
	if( mProgressBar != NULL )
	{
		DestroyWindow( mProgressBar );
		mProgressBar = NULL;
	}
}

void ProgressBar::step( void )
{
	if( mProgressBar != NULL )
	{
		SendMessage( mProgressBar, PBM_STEPIT, 0, 0 );
	}
}

void ProgressBar::setPosition( uint16_t position )
{
	if( mProgressBar != NULL )
	{
		SendMessage( mProgressBar, PBM_SETPOS, (WPARAM) position, 0 );
	}
}
