// PathTracer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <cstdint>
#include <cstdarg>

#include <commctrl.h>

#include "Main.h"
#include "Debug.h"
#include "PathTracer.h"
#include "ProgressBar.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE	gInstance = NULL;					// current instance
CHAR		szTitle[ MAX_LOADSTRING ];			// The title bar text
CHAR		szWindowClass[ MAX_LOADSTRING ];	// the main window class name
HWND		gHwnd = NULL;
HBITMAP		gBitmap = NULL;
BOOL		gContinue = TRUE;
HACCEL		gAccelTable = NULL;

static void AdjustWindowDimensions( uint16_t& width, uint16_t& height, DWORD dwWindowStyle )
{
	// Adjust the width and height to account for the title bar and borders
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width - 1;
	rect.bottom = height - 1;

	if( AdjustWindowRect( &rect, dwWindowStyle, FALSE ) == TRUE )
	{
		LONG windowWidth = rect.right - rect.left + 1;
		LONG windowHeight = rect.bottom - rect.top + 1;

//		debugf( "Win32Window::adjustWindowDimensions: Adjusting %d x %d to %d x %d\n", width, height, windowWidth, windowHeight );

		width = ( windowWidth < 0 ) ? 0 : ( windowWidth > 65535 ) ? 65535 : static_cast< uint16_t >( windowWidth );
		height = ( windowHeight < 0 ) ? 0 : ( windowHeight > 65535 ) ? 65535 : static_cast< uint16_t >( windowHeight );
	}
}

// Message handler for about box
static INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );

	switch( message )
	{
	case WM_INITDIALOG:
	{
		return INT_PTR( TRUE );
	}

	case WM_COMMAND:
	{
		if( LOWORD( wParam ) == IDOK || LOWORD( wParam ) == IDCANCEL )
		{
			EndDialog( hDlg, LOWORD( wParam ) );
			return INT_PTR( TRUE );
		}
	}
	break;

	} // switch( message )

	return INT_PTR( FALSE );
}

//
//	FUNCTION: WndProc( HWND, UINT, WPARAM, LPARAM )
//
//	PURPOSE: Processes messages for the main window.
//
//	WM_COMMAND	- process the application menu
//	WM_PAINT	- Paint the main window
//	WM_DESTROY	- post a quit message and return
//
//
static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
	{
		CREATESTRUCT* pCreate = reinterpret_cast< CREATESTRUCT* >( lParam );
		PathTracer* tracer = reinterpret_cast< PathTracer* >( pCreate->lpCreateParams );

		// Store the pointer in the instance data of the window so it can
		// be retrieved by using GetWindowLongPtr( hWnd, GWLP_USERDATA )
        SetWindowLongPtr( hWnd, GWLP_USERDATA, LONG_PTR( tracer ) );
	}
	break;

	case WM_COMMAND:
	{
		int wmId = LOWORD( wParam );

		// Parse the menu selections:
		switch( wmId )
		{
		case IDM_SAVE:
		{
			PathTracer* tracer = reinterpret_cast< PathTracer* >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );

			// !!! should open a file chooser here...
			tracer->saveImage( "image.tga" );
		}
		break;

		case IDM_ABOUT:
		{
			DialogBox( gInstance, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
		}
		break;

		case IDM_EXIT:
		{
			DestroyWindow( hWnd );
		}
		break;

		default: return DefWindowProc( hWnd, message, wParam, lParam );

		} // switch( wmId )
	}
	break;

	case WM_CLOSE:
	{
		DestroyWindow( hWnd );
		gContinue = FALSE;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint( hWnd, &ps );

		HDC hdcMem = CreateCompatibleDC( hdc );
		HGDIOBJ oldBitmap = SelectObject( hdcMem, gBitmap );

		BITMAP bitmap;
		GetObject( gBitmap, sizeof( BITMAP ), &bitmap );

		BitBlt( hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY );

		SelectObject( hdcMem, oldBitmap );
		DeleteDC( hdcMem );

		EndPaint( hWnd, &ps );
	}
	break;

	case WM_DESTROY:
	{
		PostQuitMessage( 0 );
		gContinue = FALSE;
	}
	break;

	default: return DefWindowProc( hWnd, message, wParam, lParam );

	} // switch( message )

	return 0;
}

//
//	FUNCTION: MyRegisterClass()
//
//	PURPOSE: Registers the window class.
//
static ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof( WNDCLASSEX );
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE( IDI_PATHTRACER ) );
	wcex.hCursor		= LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground	= HBRUSH( COLOR_WINDOW + 1 );
	wcex.lpszMenuName	= MAKEINTRESOURCE( IDC_PATHTRACER );
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

	return RegisterClassEx( &wcex );
}

static void CopyBitmap( const PathTracer& tracer )
{
	uint16_t width, height;
	tracer.getDimensions( width, height );

	uint8_t bytesPerPixel = tracer.getBytesPerPixel();

	BITMAPINFO info;
	ZeroMemory( &info, sizeof( info ) );

	info.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = bytesPerPixel * 8;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0;
	info.bmiHeader.biXPelsPerMeter = 10;
	info.bmiHeader.biYPelsPerMeter = 10;
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biClrImportant = 0;

	info.bmiColors[ 0 ].rgbBlue = 0;
	info.bmiColors[ 0 ].rgbGreen = 0;
	info.bmiColors[ 0 ].rgbRed = 0;
	info.bmiColors[ 0 ].rgbReserved = 0;

	HDC dc = GetDC( gHwnd );

	const uint8_t* pixels = tracer.getPixels();

	void* bits;
	gBitmap = CreateDIBSection( dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0 );

	uint32_t pixelsSize = width * height * bytesPerPixel;
	memcpy( bits, pixels, pixelsSize );

	ReleaseDC( gHwnd, dc );
}

//
//	 FUNCTION: InitInstance( HINSTANCE, int )
//
//	 PURPOSE: Saves instance handle and creates main window
//
//	 COMMENTS:
//
//		  In this function, we save the instance handle in a global variable and
//		  create and display the main program window.
//
static BOOL InitInstance( HINSTANCE hInstance, int nCmdShow, PathTracer* tracer )
{
	gInstance = hInstance; // Store instance handle in our global variable

	uint16_t width, height;
	tracer->getDimensions( width, height );

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	AdjustWindowDimensions( width, height, dwStyle );

	gHwnd = CreateWindow(
		szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, 0,		// x, y
		width, height,
		NULL, NULL,				// hWndParent, hMenu
		hInstance,
		reinterpret_cast< LPVOID >( tracer ) );
	if( gHwnd == nullptr )
	{
		return FALSE;
	}

	ShowWindow( gHwnd, nCmdShow );
	UpdateWindow( gHwnd );

	return TRUE;
}

static bool PumpMessages( void )
{
	MSG msg;
	while( PeekMessage( &msg, gHwnd, 0, 0, PM_NOREMOVE ) )
	{
		// !?!? The BOOL that GetMessage() returns can be
		// TRUE, FALSE, or -1. Go Microsoft!
		BOOL hasMessage = GetMessage( &msg, gHwnd, 0, 0 );
		if( hasMessage != FALSE )
		{
			if( hasMessage == -1 )
			{
				return false;
			}
			else
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		if( !TranslateAccelerator( msg.hwnd, gAccelTable, &msg ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

	} // while( PeekMessage( &msg, gHwnd, 0, 0, PM_NOREMOVE ) )

	return true;
}

static void TracerCompleteCallback( const PathTracer& tracer, void* data )
{
	// Copy the results to the HBITMAP
	CopyBitmap( tracer );

	// Make sure that they're visible
	InvalidateRect( gHwnd, NULL, TRUE );

	if( data != nullptr )
	{
		reinterpret_cast< ProgressBar* >( data )->close();
	}
}

static void TracerProgressCallback( uint16_t step, void* data )
{
	if( data != nullptr )
	{
		reinterpret_cast< ProgressBar* >( data )->setPosition( step );
	}
}

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR lpCmdLine,
					   _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	// Initialize global strings
	LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadString( hInstance, IDC_PATHTRACER, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	// The common controls library must be initialized in order
	// to use a Windows progress bar.
	INITCOMMONCONTROLSEX init;
	memset( &init, 0, sizeof( INITCOMMONCONTROLSEX ) );
	init.dwSize = sizeof( INITCOMMONCONTROLSEX );
	init.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx( &init );

	PathTracer tracer;

//	if( !tracer.initialize( 1280, 720 ) )
	if( !tracer.initialize( 400, 200 ) )
	{
		return FALSE;
	}

	// Perform application initialization:
	if( !InitInstance( hInstance, nCmdShow, &tracer ) )
	{
		return FALSE;
	}

	gAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_PATHTRACER ) );

	tracer.startTrace();

	ProgressBar progress;
	progress.open( gHwnd, 100, 1, "PathTracer progress" );

	tracer.setProgressCallback( TracerProgressCallback, &progress );
	tracer.setCompleteCallback( TracerCompleteCallback, &progress );

	// Spawn the tracing threads
	std::atomic_uint32_t progressCounter( 0 );
	tracer.trace( progressCounter );

	// Main loop:
	while( gContinue )
	{
		if( !PumpMessages() )
		{
			break;
		}

	} // while( gContinue )

	return 0;
}
