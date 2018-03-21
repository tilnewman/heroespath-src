///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_POPUP_POPUPSTAGECOMBATOVER_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGECOMBATOVER_HPP_INCLUDED
//
// popup-stage-combat-over.hpp
//
#include "popup/popup-stage-base.hpp"

#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Image Fade Popup Stage.
    class PopupStageCombatOver : public PopupStageBase
    {
    public:
        PopupStageCombatOver(const PopupStageCombatOver &) = delete;
        PopupStageCombatOver(const PopupStageCombatOver &&) = delete;
        PopupStageCombatOver & operator=(const PopupStageCombatOver &) = delete;
        PopupStageCombatOver & operator=(const PopupStageCombatOver &&) = delete;

    public:
        explicit PopupStageCombatOver(const PopupInfo &);
        virtual ~PopupStageCombatOver();

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

    private:
        sf::Texture bgTexture_;
        sf::Sprite bgSprite_;
        sfml_util::gui::TextRegionUPtr_t titleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGECOMBATOVER_HPP_INCLUDED
