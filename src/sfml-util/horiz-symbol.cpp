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
// horiz-symbol.cpp
//
#include "horiz-symbol.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "game/game-data-file.hpp"

namespace heroespath
{
namespace sfml_util
{

    const sf::Color BottomSymbol::DEFAULT_COLOR_{ sf::Color(255, 255, 255, 127) };

    BottomSymbol::BottomSymbol(
        const float VERT_SCALE,
        const bool WILL_INVERT_COLOR,
        const sf::Color & COLOR,
        const float VERT_OFFSET_RATIO)
        : sprite1_()
        , sprite2_()
        , sprite3_()
        , sprite4_()
        , texture_()
    {
        Setup(VERT_SCALE, WILL_INVERT_COLOR, COLOR, VERT_OFFSET_RATIO);
    }

    BottomSymbol::~BottomSymbol() {}

    void BottomSymbol::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sprite1_, states);
        target.draw(sprite2_, states);
        target.draw(sprite3_, states);
        target.draw(sprite4_, states);
    }

    void BottomSymbol::Setup(
        const float VERT_SCALE,
        const bool WILL_INVERT_COLOR,
        const sf::Color & COLOR,
        const float VERT_OFFSET_RATIO)
    {
        sfml_util::LoadTexture(
            texture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-symbol1"));

        if (WILL_INVERT_COLOR)
        {
            sfml_util::Invert(texture_);
        }

        sprite1_.setTexture(texture_);
        sprite2_.setTexture(texture_);
        sprite3_.setTexture(texture_);
        sprite4_.setTexture(texture_);

        sprite1_.setColor(COLOR);
        sprite2_.setColor(COLOR);
        sprite3_.setColor(COLOR);
        sprite4_.setColor(COLOR);

        auto const SCALE{ sfml_util::MapByRes(1.0f, 5.0f) };
        sprite1_.setScale(SCALE, SCALE * VERT_SCALE);
        sprite2_.setScale(SCALE, SCALE * VERT_SCALE);
        sprite3_.setScale(SCALE, SCALE * VERT_SCALE);
        sprite4_.setScale(SCALE, SCALE * VERT_SCALE);

        auto const VERT_OFFSET{ sprite1_.getGlobalBounds().height * VERT_OFFSET_RATIO };

        auto const TOP{ (sfml_util::Display::Instance()->GetWinHeight()
                         - sprite1_.getGlobalBounds().height)
                        + VERT_OFFSET };

        auto const PAD{ 8.0f };
        auto const THREE_PADS{ PAD * 3.0f };
        auto const HALF_SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() * 0.5f };

        sprite1_.setPosition(((HALF_SCREEN_WIDTH - sprite1_.getGlobalBounds().width) + PAD), TOP);

        sprite2_.setPosition((HALF_SCREEN_WIDTH - PAD), TOP);

        sprite3_.setPosition(
            ((HALF_SCREEN_WIDTH - (sprite1_.getGlobalBounds().width * 2.0f)) + THREE_PADS), TOP);

        sprite4_.setPosition(
            ((HALF_SCREEN_WIDTH + sprite1_.getGlobalBounds().width) - THREE_PADS), TOP);
    }

    float BottomSymbol::Bottom() const { return sfml_util::Display::Instance()->GetWinHeight(); }
} // namespace sfml_util
} // namespace heroespath
