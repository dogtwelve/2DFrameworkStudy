#ifndef EE_UICUISLIDERBUTTON_HPP
#define EE_UICUISLIDERBUTTON_HPP

#include <eepp/ui/uicontrolanim.hpp>

namespace EE { namespace UI { namespace Private {

class EE_API UISliderButton : public UIControlAnim {
	public:
		static UISliderButton * New();

		UISliderButton();

		virtual ~UISliderButton();
	protected:
		virtual void onPositionChange();
};

}}}

#endif
