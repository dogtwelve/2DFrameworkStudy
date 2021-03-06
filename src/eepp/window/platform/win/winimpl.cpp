#include <eepp/config.hpp>

#if EE_PLATFORM == EE_PLATFORM_WIN

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <windows.h>
#undef RGB

static BOOL WIN_showWindow( HWND hWnd, int nCmdShow ) {
	return ShowWindow( hWnd, nCmdShow );
}

#include <eepp/window/platform/win/winimpl.hpp>
#include <eepp/window/window.hpp>
#include <eepp/window/platform/win/cursorwin.hpp>

namespace EE { namespace Window { namespace Platform {

static HCURSOR SYS_CURSORS[ SYS_CURSOR_COUNT ] = {0};

static HCURSOR GetLoadCursor( const EE_SYSTEM_CURSOR& cursor, LPCSTR syscur ) {
	if ( 0 == SYS_CURSORS[ cursor ] ) {
		SYS_CURSORS[ cursor ] = LoadCursor( NULL, syscur );
	}

	return SYS_CURSORS[ cursor ];
}

WinImpl::WinImpl( EE::Window::Window * window, eeWindowHandle handler ) :
	PlatformImpl( window ),
	mHandler( handler ),
	mCursorCurrent( NULL ),
	mCursorHidden( false )
{
}

WinImpl::~WinImpl() {
}

void WinImpl::minimizeWindow() {
	WIN_showWindow( mHandler, SW_MINIMIZE );
}

void WinImpl::maximizeWindow() {
	WIN_showWindow( mHandler, SW_MAXIMIZE );
}

bool WinImpl::isWindowMaximized() {
	return 0 != IsZoomed( mHandler );
}

void WinImpl::hideWindow() {
	WIN_showWindow( mHandler, SW_HIDE );
}

void WinImpl::raiseWindow() {
	HWND top;

	if ( !mWindow->isWindowed() )
		top = HWND_TOPMOST;
	else
		top = HWND_NOTOPMOST;

	SetWindowPos( mHandler, top, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE) );
}

void WinImpl::showWindow() {
	WIN_showWindow( mHandler, SW_SHOW );
}

void WinImpl::moveWindow( int left, int top ) {
	SetWindowPos( mHandler, NULL, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
}

void WinImpl::setContext( eeWindowContex Context ) {
	wglMakeCurrent( (HDC)GetDC( mHandler ), (HGLRC)Context );
}

Vector2i WinImpl::getPosition() {
	RECT r;
	GetWindowRect( mHandler, &r );
	return Vector2i( r.left, r.top );
}

void WinImpl::showMouseCursor() {
	mCursorHidden = false;

	if ( !mCursorCurrent ) {
		setSystemMouseCursor( SYS_CURSOR_ARROW );
	} else {
		SetCursor( (HCURSOR)mCursorCurrent );
		SetClassLong( getHandler(), GCL_HCURSOR, (DWORD)mCursorCurrent );

		POINT p;
		GetCursorPos( &p );
		SetCursorPos( p.x, p.y );
	}
}

void WinImpl::hideMouseCursor() {
	if ( mCursorHidden )
		return;

	mCursorHidden = true;

	PostMessage( mHandler, WM_SETCURSOR, 0, 0 );
}

Cursor * WinImpl::createMouseCursor( Texture * tex, const Vector2i& hotspot, const std::string& name ) {
	return eeNew( CursorWin, ( tex, hotspot, name, mWindow ) );
}

Cursor * WinImpl::createMouseCursor( Image * img, const Vector2i& hotspot, const std::string& name ) {
	return eeNew( CursorWin, ( img, hotspot, name, mWindow ) );
}

Cursor * WinImpl::createMouseCursor( const std::string& path, const Vector2i& hotspot, const std::string& name ) {
	return eeNew( CursorWin, ( path, hotspot, name, mWindow ) );
}

void WinImpl::setMouseCursor( Cursor * cursor ) {
	mCursorCurrent = reinterpret_cast<CursorWin*> ( cursor )->getCursor();

	if ( !mCursorHidden ) {
		SetCursor( (HCURSOR)mCursorCurrent );
		SetClassLong( getHandler(), GCL_HCURSOR, (DWORD)mCursorCurrent );

		POINT p;
		GetCursorPos( &p );
		SetCursorPos( p.x, p.y );
	}
}

void WinImpl::setSystemMouseCursor( EE_SYSTEM_CURSOR syscursor ) {
	HCURSOR mc;

	switch ( syscursor ) {
		case SYS_CURSOR_ARROW:
			mc = GetLoadCursor(SYS_CURSOR_ARROW, IDC_ARROW); break;
		case SYS_CURSOR_WAIT:
			mc = GetLoadCursor(SYS_CURSOR_WAIT, IDC_WAIT); break;
		case SYS_CURSOR_CROSSHAIR:
			mc = GetLoadCursor(SYS_CURSOR_CROSSHAIR, IDC_CROSS); break;
		case SYS_CURSOR_IBEAM:
			mc = GetLoadCursor(SYS_CURSOR_IBEAM, IDC_IBEAM); break;
		case SYS_CURSOR_WAITARROW:
			mc = GetLoadCursor(SYS_CURSOR_WAITARROW, IDC_WAIT); break;
		case SYS_CURSOR_SIZENWSE:
			mc = GetLoadCursor(SYS_CURSOR_SIZENWSE, IDC_SIZENWSE); break;
		case SYS_CURSOR_SIZENESW:
			mc = GetLoadCursor(SYS_CURSOR_SIZENESW, IDC_SIZENESW); break;
		case SYS_CURSOR_SIZEWE:
			mc = GetLoadCursor(SYS_CURSOR_SIZEWE, IDC_SIZEWE); break;
		case SYS_CURSOR_SIZENS:
			mc = GetLoadCursor(SYS_CURSOR_SIZENS, IDC_SIZENS); break;
		case SYS_CURSOR_SIZEALL:
			mc = GetLoadCursor(SYS_CURSOR_SIZEALL, IDC_SIZEALL); break;
		case SYS_CURSOR_NO:
			mc = GetLoadCursor(SYS_CURSOR_NO, IDC_NO); break;
		case SYS_CURSOR_HAND:
			mc = GetLoadCursor(SYS_CURSOR_HAND, IDC_HAND); break;
		default:
			return;
	}

	mCursorCurrent = mc;

	if ( !mCursorHidden ) {
		SetCursor( mc );
		SetClassLong( getHandler(), GCL_HCURSOR, (DWORD)mc );

		POINT p;
		GetCursorPos( &p );
		SetCursorPos( p.x, p.y );
	}
}

void WinImpl::restoreCursor() {
	if ( !mCursorHidden ) {
		showMouseCursor();
	} else {
		hideMouseCursor();
	}
}

eeWindowHandle WinImpl::getHandler() const {
	return mHandler;
}

eeWindowContex WinImpl::getWindowContext() {
	return wglGetCurrentContext();
}

}}}

#endif
