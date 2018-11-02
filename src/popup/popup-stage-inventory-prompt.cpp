// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-inventory-prompt.hpp
//
#include "popup-stage-inventory-prompt.hpp"

#include "gui/sound-manager.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    PopupStageInventoryPrompt::PopupStageInventoryPrompt(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
    {}

    PopupStageInventoryPrompt::~PopupStageInventoryPrompt() = default;

    void PopupStageInventoryPrompt::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);
        StageBase::draw(target, states);
    }

    bool PopupStageInventoryPrompt::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::I) && popupInfo_.WillIncludeItems())
        {
            PlayValidKeypressSoundEffect();

            RemovePopup(ResponseTypes::Select, PopupInfo::ContentNum_Item());

            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::C)
        {
            PlayValidKeypressSoundEffect();

            RemovePopup(ResponseTypes::Select, PopupInfo::ContentNum_Coins());

            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::G)
        {
            PlayValidKeypressSoundEffect();

            RemovePopup(ResponseTypes::Select, PopupInfo::ContentNum_Gems());

            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::M)
        {
            PlayValidKeypressSoundEffect();

            RemovePopup(ResponseTypes::Select, PopupInfo::ContentNum_MeteorShards());

            return true;
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }
} // namespace popup
} // namespace heroespath
