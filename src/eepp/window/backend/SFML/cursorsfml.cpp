#include <eepp/window/backend/SFML/cursorsfml.hpp>

#ifdef EE_BACKEND_SFML_ACTIVE

namespace EE { namespace Window { namespace Backend { namespace SFML {

CursorSFML::CursorSFML( Texture * tex, const Vector2i& hotspot, const std::string& name, EE::Window::Window * window ) :
	Cursor( tex, hotspot, name, window )
{
}

CursorSFML::CursorSFML( Image * img, const Vector2i& hotspot, const std::string& name, EE::Window::Window * window ) :
	Cursor( img, hotspot, name, window )
{
}

CursorSFML::CursorSFML( const std::string& path, const Vector2i& hotspot, const std::string& name, EE::Window::Window * window ) :
	Cursor( path, hotspot, name, window )
{
}

void CursorSFML::Create() {
}

}}}}

#endif
