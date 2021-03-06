#ifndef EE_WINDOWCWINIMPL_HPP
#define EE_WINDOWCWINIMPL_HPP

#include <eepp/window/base.hpp>

#if EE_PLATFORM == EE_PLATFORM_WIN

#include <eepp/window/platformimpl.hpp>

namespace EE { namespace Window { namespace Platform {

using namespace EE::Window;

class WinImpl : public PlatformImpl {
	public:
		WinImpl( EE::Window::Window * window, eeWindowHandle handler );

		~WinImpl();

		void minimizeWindow();

		void maximizeWindow();

		bool isWindowMaximized();

		void hideWindow();

		void raiseWindow();

		void showWindow();

		void moveWindow( int left, int top );

		void setContext( eeWindowContex Context );

		Vector2i getPosition();

		void showMouseCursor();

		void hideMouseCursor();

		Cursor * createMouseCursor( Texture * tex, const Vector2i& hotspot, const std::string& name );

		Cursor * createMouseCursor( Image * img, const Vector2i& hotspot, const std::string& name );

		Cursor * createMouseCursor( const std::string& path, const Vector2i& hotspot, const std::string& name );

		void setMouseCursor( Cursor * cursor );

		void setSystemMouseCursor( EE_SYSTEM_CURSOR syscursor );

		void restoreCursor();

		eeWindowHandle getHandler() const;

		eeWindowContex getWindowContext();
	protected:
		eeWindowHandle	mHandler;
		void *			mCursorCurrent;
		bool			mCursorHidden;
};

}}}

#endif

#endif
