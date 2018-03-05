// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// popup-stage-system-error.cpp
//
#include "popup-stage-system-error.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace popup
{

    PopupStageSystemError::PopupStageSystemError(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , bgTexture_()
        , bgSprite_()
    {}

    PopupStageSystemError::~PopupStageSystemError() {}

    void PopupStageSystemError::Setup()
    {
        PopupStageBase::Setup();

        sfml_util::LoadTexture(
            bgTexture_, game::GameDataFile::Instance()->GetMediaPath("media-images-misc-error"));

        sfml_util::Mask(bgTexture_, sf::Color::White);
        bgSprite_.setTexture(bgTexture_, true);
        bgSprite_.setColor(sf::Color(255, 255, 255, 32));

        auto const HORIZ_RESCALE{ textRegion_.width / static_cast<float>(bgTexture_.getSize().x) };

        bgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (bgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE{ textRegion_.height / static_cast<float>(bgTexture_.getSize().y) };

            bgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f))
                                - (bgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP{ (textRegion_.top + (textRegion_.height * 0.5f))
                               - (bgSprite_.getGlobalBounds().height * 0.5f) };

        bgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);
    }

    void PopupStageSystemError::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);
        target.draw(bgSprite_, STATES);
        Stage::Draw(target, STATES);
    }
}
}
