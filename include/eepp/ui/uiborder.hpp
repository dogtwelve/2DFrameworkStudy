#ifndef EE_UICUIBORDER_HPP
#define EE_UICUIBORDER_HPP

#include <eepp/ui/base.hpp>

namespace EE { namespace UI {

class EE_API UIBorder {
	public:
		static UIBorder * New();

		UIBorder();

		const Color& getColor() const;

		UIBorder * setColor( const Color& Col );

		const unsigned int& getWidth() const;

		UIBorder * setWidth( const unsigned int& width );

		void draw( Rectf R, const Float& alpha, const int& corners, const bool& clipping );
	protected:
		Color			mColor;
		unsigned int	mWidth;
};

}}

#endif
