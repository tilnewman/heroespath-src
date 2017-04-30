//
// popup-buttons.cpp
//
#include "popup-buttons.hpp"

#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/static-sounds.hpp"

#include "heroespath/loop-manager.hpp"


namespace sfml_util
{

    PopupButton_Yes::PopupButton_Yes(const heroespath::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Yes", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Yes", POPUP_INFO))
    {}

    void PopupButton_Yes::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
        heroespath::LoopManager::Instance()->PopupWaitEnd(Response::Yes);
    }


    PopupButton_No::PopupButton_No(const heroespath::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_No", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("No", POPUP_INFO))
    {}

    void PopupButton_No::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
        heroespath::LoopManager::Instance()->PopupWaitEnd(Response::No);
    }


    PopupButton_Cancel::PopupButton_Cancel(const heroespath::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Cancel", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Cancel", POPUP_INFO))
    {}

    void PopupButton_Cancel::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
        heroespath::LoopManager::Instance()->PopupWaitEnd(Response::Cancel);
    }


    PopupButton_Continue::PopupButton_Continue(const heroespath::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Continue", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Continue", POPUP_INFO))
    {}

    void PopupButton_Continue::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
        heroespath::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
    }


    PopupButton_Okay::PopupButton_Okay(const heroespath::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Okay", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Okay", POPUP_INFO))
    {}

    void PopupButton_Okay::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
        heroespath::LoopManager::Instance()->PopupWaitEnd(Response::Okay);
    }


    PopupButton_Select::PopupButton_Select(const heroespath::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Select", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Select", POPUP_INFO)),
        selection_(-1)//any negative number will work here
    {}

    void PopupButton_Select::OnClick(const sf::Vector2f &)
    {
        if (selection_ >= 0)
        {
            SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
            heroespath::LoopManager::Instance()->PopupWaitEnd(Response::Select, static_cast<std::size_t>(selection_));
        }
    }

}
