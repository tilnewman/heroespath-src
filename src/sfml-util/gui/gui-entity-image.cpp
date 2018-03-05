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
// gui-entity-image.cpp
//
#include "gui-entity-image.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        GuiImage::GuiImage(
            const std::string & NAME,
            const sf::FloatRect & SCREEN_REGION,
            const sf::Sprite & SPRITE_UP,
            const sf::Sprite & SPRITE_DOWN,
            const sf::Sprite & SPRITE_OVER)
            : GuiEntity(std::string(NAME).append("_GuiImage"), SCREEN_REGION)
            , upSprite_(SPRITE_UP)
            , downSprite_(SPRITE_DOWN)
            , overSprite_(SPRITE_OVER)
        {
            Setup(SCREEN_REGION.left, SCREEN_REGION.top);
        }

        GuiImage::GuiImage(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const sf::Sprite & SPRITE_UP,
            const sf::Sprite & SPRITE_DOWN,
            const sf::Sprite & SPRITE_OVER)
            : GuiEntity(std::string(NAME).append("GuiImage"), POS_LEFT, POS_TOP)
            , upSprite_(SPRITE_UP)
            , downSprite_(SPRITE_DOWN)
            , overSprite_(SPRITE_OVER)
        {
            Setup(POS_LEFT, POS_TOP);
        }

        GuiImage::~GuiImage() {}

        void GuiImage::Setup(const float POS_LEFT, const float POS_TOP)
        {
            upSprite_.setPosition(POS_LEFT, POS_TOP);
            downSprite_.setPosition(POS_LEFT, POS_TOP);
            overSprite_.setPosition(POS_LEFT, POS_TOP);

            if (upSprite_.getTexture() != nullptr)
            {
                if (downSprite_.getTexture() == nullptr)
                {
                    downSprite_ = upSprite_;
                }

                if (overSprite_.getTexture() == nullptr)
                {
                    overSprite_ = upSprite_;
                }
            }

            SetEntityRegion(sf::FloatRect(
                POS_LEFT,
                POS_TOP,
                upSprite_.getLocalBounds().width,
                upSprite_.getLocalBounds().height));
        }

        void GuiImage::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (upSprite_.getTexture() == nullptr)
                return;

            if (MouseState::Up == entityMouseState_)
            {
                target.draw(upSprite_, states);
            }
            else
            {
                if (MouseState::Over == entityMouseState_)
                {
                    if (overSprite_.getTexture() == nullptr)
                    {
                        target.draw(upSprite_, states);
                    }
                    else
                    {
                        target.draw(overSprite_, states);
                    }
                }
                else
                {
                    if (downSprite_.getTexture() == nullptr)
                    {
                        target.draw(upSprite_, states);
                    }
                    else
                    {
                        target.draw(downSprite_, states);
                    }
                }
            }
        }

        void GuiImage::SpriteColorsSet(const sf::Color & NEW_COLOR)
        {
            upSprite_.setColor(NEW_COLOR);
            downSprite_.setColor(NEW_COLOR);
            overSprite_.setColor(NEW_COLOR);
        }

        void GuiImage::SpriteColorsReset()
        {
            upSprite_.setColor(sf::Color::White);
            downSprite_.setColor(sf::Color::White);
            overSprite_.setColor(sf::Color::White);
        }

        void GuiImage::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
            upSprite_.setPosition(POS_LEFT, POS_TOP);
            downSprite_.setPosition(POS_LEFT, POS_TOP);
            overSprite_.setPosition(POS_LEFT, POS_TOP);
        }

        void GuiImage::MoveEntityPos(const float HORIZ, const float VERT)
        {
            GuiEntity::MoveEntityPos(HORIZ, VERT);
            upSprite_.move(HORIZ, VERT);
            downSprite_.move(HORIZ, VERT);
            overSprite_.move(HORIZ, VERT);
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
