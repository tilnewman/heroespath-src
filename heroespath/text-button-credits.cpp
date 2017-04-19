//
// text-button-credits.cpp
//
#include "text-button-credits.hpp"

#include "sfml-util/gui/mouse-text-info.hpp"

#include "heroespath/loop-manager.hpp"


namespace heroespath
{

    CreditsButton::CreditsButton(const std::string & NAME)
    :
        TextButton(std::string(NAME).append("_CreditsButton"))
    {}


    CreditsButton::CreditsButton(const std::string &                   NAME,
                                 const float                           POS_LEFT,
                                 const float                           POS_TOP,
                                 const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                                 const float                           CLICK_SHIFT)
    :
        TextButton (std::string(NAME).append("_CreditsButton"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CLICK_SHIFT)
    {}


    CreditsButton::~CreditsButton()
    {}


    void CreditsButton::Setup(const float                           POS_LEFT,
                              const float                           POS_TOP,
                              const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                              const float                           CLICK_SHIFT)
    {
        TextButton::Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CLICK_SHIFT);
    }


    void CreditsButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Credits();
    }

}
