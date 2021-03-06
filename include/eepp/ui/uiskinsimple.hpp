#ifndef EE_UICUISKINSIMPLE_HPP
#define EE_UICUISKINSIMPLE_HPP

#include <eepp/ui/base.hpp>
#include <eepp/ui/uiskin.hpp>

namespace EE { namespace Graphics {
class Drawable;
}}

namespace EE { namespace UI {

class EE_API UISkinSimple : public UISkin {
	public:
		static UISkinSimple * New( const std::string& name );

		UISkinSimple( const std::string& name );

		virtual ~UISkinSimple();

		virtual void draw( const Float& X, const Float& Y, const Float& Width, const Float& Height, const Uint32& Alpha, const Uint32& State );

		void setSkin( const Uint32& State );

		bool stateExists( const Uint32& state );

		UISkinSimple * clone( const std::string& NewName, const bool& CopyColorsState = true );

		virtual UISkin * clone();

		Sizei getSize( const Uint32& state );

		Rect getBorderSize( const Uint32 & state );
	protected:
		Drawable * 	mDrawable[ UISkinState::StateCount ];
		Color		mTempColor;

		void stateNormalToState( const Uint32& State );
};

}}

#endif
