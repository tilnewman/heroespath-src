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
// knot-frame.cpp
//
#include "knot-frame.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "game/game-data-file.hpp"


namespace heroespath
{
namespace sfml_util
{
namespace gui
{

    const float KnotFrame::OUTER_WIDTH_RATIO_{ 0.4f };
    const float KnotFrame::INNER_WIDTH_RATIO_{ 0.6f };
    const float KnotFrame::FRAME_WIDTH_RATIO_{ 0.2f };


    KnotFrame::KnotFrame()
    :
        regionInner_(),
        regionOuter_(),
        size_(0.0f),
        color_(sf::Color::Black),
        quadVerts_(),
        texture_(),
        topLeftSprite_(),
        topRightSprite_(),
        botLeftSprite_(),
        botRightSprite_()
    {}


    KnotFrame::KnotFrame(
        const sf::FloatRect & INNER_REGION,
        const float SIZE,
        const sf::Color & COLOR)
    :
        regionInner_(),
        regionOuter_(),
        size_(0.0f),
        color_(sf::Color::Black),
        quadVerts_(),
        texture_(),
        topLeftSprite_(),
        topRightSprite_(),
        botLeftSprite_(),
        botRightSprite_()
    {
        Setup(INNER_REGION, SIZE, COLOR);
    }


    void KnotFrame::Setup(
        const sf::FloatRect & INNER_REGION,
        const float SIZE,
        const sf::Color & COLOR)
    {
        quadVerts_.clear();

        regionInner_ = INNER_REGION;
        size_ = SIZE;
        color_ = COLOR;

        sfml_util::LoadTexture(
            texture_,
            heroespath::game::GameDataFile::Instance()->
                GetMediaPath("media-images-misc-knot-corner"));

        topLeftSprite_.setTexture(texture_, true);
        topRightSprite_.setTexture(texture_, true);
        botLeftSprite_.setTexture(texture_, true);
        botRightSprite_.setTexture(texture_, true);

        topRightSprite_.setRotation(90.0f);
        botLeftSprite_.setRotation(-90.0f);
        botRightSprite_.setRotation(180.0f);

        topLeftSprite_.setColor(COLOR);
        topRightSprite_.setColor(COLOR);
        botLeftSprite_.setColor(COLOR);
        botRightSprite_.setColor(COLOR);

        auto const SCALE{ SIZE / topLeftSprite_.getLocalBounds().width };
        topLeftSprite_.setScale(SCALE, SCALE);
        topRightSprite_.setScale(SCALE, SCALE);
        botLeftSprite_.setScale(SCALE, SCALE);
        botRightSprite_.setScale(SCALE, SCALE);

        auto const LEFT{ INNER_REGION.left };
        auto const RIGHT{ LEFT + INNER_REGION.width + topLeftSprite_.getGlobalBounds().width };
        auto const TOP{ INNER_REGION.top };
        auto const BOTTOM{ TOP + INNER_REGION.height + topLeftSprite_.getGlobalBounds().height };

        topLeftSprite_.setPosition(LEFT - InnerSize(), TOP - InnerSize());
        topRightSprite_.setPosition(RIGHT - OuterSize(), TOP - InnerSize());
        botLeftSprite_.setPosition(LEFT - InnerSize(), BOTTOM - OuterSize() );
        botRightSprite_.setPosition(RIGHT - OuterSize(), BOTTOM - OuterSize());

        regionOuter_.left = topLeftSprite_.getPosition().x;
        regionOuter_.top = topLeftSprite_.getPosition().y;

        regionOuter_.width = 
            (topRightSprite_.getGlobalBounds().left + topRightSprite_.getGlobalBounds().width) -
                regionOuter_.left;

        regionOuter_.height =
            (botRightSprite_.getGlobalBounds().top + botRightSprite_.getGlobalBounds().height) -
                regionOuter_.top;

        //top bar
        auto const HORIZ_BAR_LEFT{
            topLeftSprite_.getGlobalBounds().left + topLeftSprite_.getGlobalBounds().width };

        auto const HORIZ_BAR_HEIGHT{ FrameSize() };

        auto const HORIZ_BAR_WIDTH{ topRightSprite_.getGlobalBounds().left - HORIZ_BAR_LEFT };

        if ((HORIZ_BAR_WIDTH < 1.0f) == false)
        {
            sfml_util::DrawQuad(
                sf::FloatRect(
                    HORIZ_BAR_LEFT,
                    topLeftSprite_.getGlobalBounds().top,
                    HORIZ_BAR_WIDTH,
                    HORIZ_BAR_HEIGHT),
                COLOR,
                COLOR,
                quadVerts_);

            //bottom bar
            auto const BOTTOM_HORIZ_BAR_TOP{
                botLeftSprite_.getGlobalBounds().top +
                botLeftSprite_.getGlobalBounds().height -
                HORIZ_BAR_HEIGHT };

            sfml_util::DrawQuad(
                sf::FloatRect(
                    HORIZ_BAR_LEFT,
                    BOTTOM_HORIZ_BAR_TOP,
                    HORIZ_BAR_WIDTH,
                    HORIZ_BAR_HEIGHT),
                COLOR,
                COLOR,
                quadVerts_);
        }

        //left bar
        auto const VERT_BAR_TOP{ 
            topLeftSprite_.getGlobalBounds().top + topLeftSprite_.getGlobalBounds().height };

        auto const VERT_BAR_HEIGHT{ botLeftSprite_.getGlobalBounds().top - VERT_BAR_TOP };
        auto const VERT_BAR_WIDTH{ HORIZ_BAR_HEIGHT };

        if ((VERT_BAR_HEIGHT < 1.0f) == false)
        {
            sfml_util::DrawQuad(
                sf::FloatRect(
                    topLeftSprite_.getGlobalBounds().left,
                    VERT_BAR_TOP,
                    VERT_BAR_WIDTH,
                    VERT_BAR_HEIGHT),
                COLOR,
                COLOR,
                quadVerts_);

            //right bar
            auto const RIGHT_VERT_BAR_LEFT{
                topRightSprite_.getGlobalBounds().left +
                topRightSprite_.getGlobalBounds().width -
                VERT_BAR_WIDTH };

            sfml_util::DrawQuad(
                sf::FloatRect(
                    RIGHT_VERT_BAR_LEFT,
                    VERT_BAR_TOP,
                    VERT_BAR_WIDTH,
                    VERT_BAR_HEIGHT),
                COLOR,
                COLOR,
                quadVerts_);
        }
    }


    void KnotFrame::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(topLeftSprite_, states);
        target.draw(topRightSprite_, states);
        target.draw(botLeftSprite_, states);
        target.draw(botRightSprite_, states);
        target.draw( & quadVerts_[0], quadVerts_.size(), sf::Quads, states);
    }

}
}
}
