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
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"

#include "game/game-data-file.hpp"

#include <string>


namespace game
{
    const float     BottomSymbol::DEFAULT_INVALID_DIMM_(-1.0f);//any value less than zero will work as a default value
    const float     BottomSymbol::DEFAULT_HORIZ_POS_(-1.0f);//see above
    const float     BottomSymbol::DEFAULT_VERT_POS_OFFSET_(20.0f);
    const sf::Color BottomSymbol::DEFAULT_COLOR_(sf::Color(255, 255, 255, 127));


    BottomSymbol::BottomSymbol(const float       VERT_SCALE,
                               const bool        WILL_INVERT_COLOR,
                               const float       SCREEN_WIDTH,
                               const float       SCREEN_HEIGHT,
                               const float       HORIZ_POS,
                               const float       VERT_POS_OFFSET,
                               const sf::Color & COLOR)
    :
        halfScreenWidth_(0.0f), //all members initialized in Setup() below
        screenHeight_   (0.0f),
        horizPos_       (0.0f),
        vertPosOffset_  (0.0f),
        sprite_         (),
        texture_        (),
        posTop_         (0.0f)
    {
        Setup(VERT_SCALE,
              WILL_INVERT_COLOR,
              SCREEN_WIDTH,
              SCREEN_HEIGHT,
              HORIZ_POS,
              VERT_POS_OFFSET,
              COLOR);
    }


    BottomSymbol::~BottomSymbol()
    {}


    void BottomSymbol::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        //don't use horizPos_ and center symbols if negative
        if (horizPos_ < 0.0f)
        {
            sprite_.setPosition(((halfScreenWidth_ - sprite_.getGlobalBounds().width) + 8.0f), posTop_);
            target.draw(sprite_, STATES);

            sprite_.setPosition((halfScreenWidth_ - 8.0f), posTop_);
            target.draw(sprite_, STATES);

            sprite_.setPosition(((halfScreenWidth_ - (sprite_.getGlobalBounds().width * 2.0f)) + 24.0f), posTop_);
            target.draw(sprite_, STATES);

            sprite_.setPosition(((halfScreenWidth_ + sprite_.getGlobalBounds().width) - 24.0f), posTop_);
            target.draw(sprite_, STATES);
        }
        else
        {
            float posX(horizPos_);
            sprite_.setPosition(posX, posTop_);
            target.draw(sprite_, STATES);

            posX += sprite_.getGlobalBounds().width;
            posX -= 16.0f;
            sprite_.setPosition(posX, posTop_);
            target.draw(sprite_, STATES);

            posX += sprite_.getGlobalBounds().width;
            posX -= 16.0f;
            sprite_.setPosition(posX, posTop_);
            target.draw(sprite_, STATES);

            posX += sprite_.getGlobalBounds().width;
            posX -= 16.0f;
            sprite_.setPosition(posX, posTop_);
            target.draw(sprite_, STATES);
        }
    }


    void BottomSymbol::Setup(const float       VERT_SCALE,
                             const bool        WILL_INVERT_COLOR,
                             const float       SCREEN_WIDTH,
                             const float       SCREEN_HEIGHT,
                             const float       HORIZ_POS,
                             const float       VERT_POS_OFFSET,
                             const sf::Color & COLOR)
    {
        sfml_util::LoadImageOrTexture<sf::Texture>(texture_,
            GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-symbol1"));

        texture_.setSmooth(true);

        if (WILL_INVERT_COLOR)
        {
            sfml_util::Invert(texture_);
        }

        sprite_.setTexture(texture_);

        float widthToUse(SCREEN_WIDTH);
        if (widthToUse < 0.0f)
        {
            widthToUse = sfml_util::Display::Instance()->GetWinWidth();
        }

        float heightToUse(SCREEN_HEIGHT);
        if (heightToUse < 0.0f)
        {
            heightToUse = sfml_util::Display::Instance()->GetWinHeight();
        }

        halfScreenWidth_ = (widthToUse * 0.5f);
        screenHeight_ = heightToUse;
        vertPosOffset_ = VERT_POS_OFFSET;
        horizPos_ = HORIZ_POS;
        sprite_.setColor(COLOR);
        const float SCALE(sfml_util::MapByRes(1.0f, 5.0f));
        sprite_.setScale(SCALE, SCALE * VERT_SCALE);
        posTop_ = (screenHeight_ - sprite_.getGlobalBounds().height) - vertPosOffset_;
        sprite_.setPosition(0.0f, posTop_);
    }

}
