#ifndef EE_GRAPHICSCGLOBALTEXTUREATLAS_HPP
#define EE_GRAPHICSCGLOBALTEXTUREATLAS_HPP

#include <eepp/graphics/base.hpp>
#include <eepp/graphics/textureatlas.hpp>

namespace EE { namespace Graphics {

/** @brief Any SubTexture that doesn't belong to an specific TextureAtlas ( a real texture atlas texture ), goes here.
	This is useful to auto release the SubTextures.
*/
class EE_API GlobalTextureAtlas : public TextureAtlas {
	SINGLETON_DECLARE_HEADERS(GlobalTextureAtlas)

	public:
		~GlobalTextureAtlas();
	protected:
		GlobalTextureAtlas();
};

}}

#endif
