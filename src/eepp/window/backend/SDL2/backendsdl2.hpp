#ifndef EE_WINDOWCBACKENDSDL2_HPP
#define EE_WINDOWCBACKENDSDL2_HPP

#include <eepp/window/backend.hpp>
#include <eepp/window/backend/SDL2/base.hpp>

#ifdef EE_BACKEND_SDL2

#include <eepp/window/backend/SDL2/windowsdl2.hpp>

namespace EE { namespace Window { namespace Backend { namespace SDL2 {

class EE_API WindowBackendSDL2 : public WindowBackend {
	public:
		WindowBackendSDL2();
		
		~WindowBackendSDL2();
};

}}}}

#endif

#endif
