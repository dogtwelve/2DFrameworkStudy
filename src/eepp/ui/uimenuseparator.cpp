#include <eepp/ui/uimenuseparator.hpp>
#include <eepp/graphics/subtexture.hpp>

namespace EE { namespace UI {

UIMenuSeparator * UIMenuSeparator::New() {
	return eeNew( UIMenuSeparator, () );
}

UIMenuSeparator::UIMenuSeparator() :
	UIWidget()
{
	applyDefaultTheme();
}

UIMenuSeparator::~UIMenuSeparator() {
}

Uint32 UIMenuSeparator::getType() const {
	return UI_TYPE_MENU_SEPARATOR;
}

bool UIMenuSeparator::isType( const Uint32& type ) const {
	return UIMenuSeparator::getType() == type ? true : UIControlAnim::isType( type );
}

void UIMenuSeparator::setTheme( UITheme * Theme ) {
	UIWidget::setTheme( Theme );
	setThemeSkin( Theme, "separator" );
	
	if ( NULL != getSkin() ) {
		setSize( Sizei( mSize.getWidth(), getSkinSize().getHeight() ) );
	}
}

}}
