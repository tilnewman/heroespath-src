//
// text-button.cpp
//
#include "text-button.hpp"

#include "sfml-util/gui/mouse-text-info.hpp"


namespace sfml_util
{
namespace gui
{

    float TextButton::CLICK_SHIFT_DEFAULT_(2.0f);


    TextButton::TextButton(const std::string & NAME)
    :
        GuiText    (std::string(NAME).append("_TextButton")),
        clickShift_(0)
    {}


    TextButton::TextButton( const std::string &   NAME,
                            const float           POS_LEFT,
                            const float           POS_TOP,
                            const MouseTextInfo & MOUSE_TEXT_INFO,
                            const float           CLICK_SHIFT )
    :
        GuiText    (std::string(NAME).append("_TextButton"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO),
        clickShift_(CLICK_SHIFT)
    {
        Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CLICK_SHIFT);
    }


    TextButton::~TextButton()
    {}


    void TextButton::Setup(const float           POS_LEFT,
                           const float           POS_TOP,
                           const MouseTextInfo & MOUSE_TEXT_INFO,
                           const float           CLICK_SHIFT)
    {
        GuiText::Setup(MOUSE_TEXT_INFO.up.text, POS_LEFT, POS_TOP, MOUSE_TEXT_INFO);
        clickShift_ = CLICK_SHIFT;
    }

}
}
