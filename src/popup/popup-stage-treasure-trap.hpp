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
#ifndef HEROESPATH_POPUP_POPUPSTAGETREASURETRAP_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGETREASURETRAP_HPP_INCLUDED
//
// popup-stage-treasure-trap.hpp
//
#include "popup/popup-stage-base.hpp"

#include "sfml-util/gui/text-region.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing a treasure trap popup window stage.
    class PopupStageTreasureTrap : public PopupStageBase
    {
    public:
        PopupStageTreasureTrap(const PopupStageTreasureTrap &) = delete;
        PopupStageTreasureTrap(const PopupStageTreasureTrap &&) = delete;
        PopupStageTreasureTrap & operator=(const PopupStageTreasureTrap &) = delete;
        PopupStageTreasureTrap & operator=(const PopupStageTreasureTrap &&) = delete;

    public:
        explicit PopupStageTreasureTrap(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageTreasureTrap();

        inline const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;

    private:
        void SetupTitleText();
        void SetupDescriptionText();
        void SetupAccentImage();

    private:
        sf::Texture trapTexture_;
        sf::Sprite trapSprite_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGETREASURETRAP_HPP_INCLUDED
