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
// line.cpp
//
#include "line.hpp"

#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/sfml-util.hpp"

#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        Line::Line(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const std::size_t LENGTH,
            Orientation::Enum ORIENTATION,
            const Side::Enum SIDE,
            const bool WILL_CAP_ENDS)
            : GuiEntity(std::string(NAME).append("_Line"), POS_LEFT, POS_TOP)
            , length_(LENGTH)
            , ORIENTATION_(ORIENTATION)
            , SIDE_(SIDE)
            , WILL_CAP_ENDS_(WILL_CAP_ENDS)
            , pixelsOfMiddleToUse_(0)
            , middleCount_(0)
            , middleSprite_()
            , endTopOrLeftSprite_()
            , endBotOrRightSprite_()
            , finalSprite_()
            , offScreenTexture_()
        {
            Setup(POS_LEFT, POS_TOP, LENGTH);
            GuiEntity::SetWillAcceptFocus(false);
        }

        Line::Line(
            const std::string & NAME,
            Orientation::Enum ORIENTATION,
            const Side::Enum SIDE,
            const bool WILL_CAP_ENDS)
            : GuiEntity(NAME, 0.0f, 0.0f)
            , // position and length values not yet known, see Setup()
            length_(0)
            , ORIENTATION_(ORIENTATION)
            , SIDE_(SIDE)
            , WILL_CAP_ENDS_(WILL_CAP_ENDS)
            , pixelsOfMiddleToUse_(0)
            , middleCount_(0)
            , middleSprite_()
            , endTopOrLeftSprite_()
            , endBotOrRightSprite_()
            , finalSprite_()
            , offScreenTexture_()
        {
            GuiEntity::SetWillAcceptFocus(false);
        }

        void Line::Setup(const float POS_LEFT, const float POS_TOP, const std::size_t LENGTH)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (LENGTH > 0),
                "sfml_util::gui::Line(\"" << entityName_
                                          << "\")::Setup() was given a length <= zero!");

            length_ = LENGTH;
            SetEntityPos(POS_LEFT, POS_TOP);

            SetupBaseSprites();
            SetupMiddleSprites();
            SetupOffScreenTexture();
        }

        void Line::SetupBaseSprites()
        {
            auto const GE_PTR{ GuiElements::Instance() };

            // which spirtes to use, and the length calculations
            if (ORIENTATION_ == Orientation::Horiz)
            {
                if (SIDE_ == Side::Top)
                {
                    middleSprite_ = sf::Sprite(
                        GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallHorizontalTop());
                }
                else
                {
                    middleSprite_ = sf::Sprite(
                        GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallHorizontalBot());
                }

                endTopOrLeftSprite_
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallEndLeft());
                endBotOrRightSprite_
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallEndRight());
            }
            else
            {
                if (SIDE_ == Side::Left)
                {
                    middleSprite_
                        = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallVerticalLeft());
                }
                else
                {
                    middleSprite_ = sf::Sprite(
                        GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallVerticalRight());
                }

                endTopOrLeftSprite_
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallEndTop());
                endBotOrRightSprite_
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_LineSmallEndBot());
            }
        }

        void Line::SetupMiddleSprites()
        {
            if (ORIENTATION_ == Orientation::Horiz)
            {
                const std::size_t END_CAPS_WIDTH(
                    (WILL_CAP_ENDS_) ? static_cast<std::size_t>(
                                           endTopOrLeftSprite_.getLocalBounds().width
                                           + endBotOrRightSprite_.getLocalBounds().width)
                                     : 0);
                if (length_ <= END_CAPS_WIDTH)
                {
                    // if less than the minimum, then the length will be END_CAPS_WIDTH
                    pixelsOfMiddleToUse_ = 0;
                    middleCount_ = 0;
                }
                else
                {
                    const std::size_t LENGTH_BETWEEN_ENDS(length_ - END_CAPS_WIDTH);
                    const std::size_t MIDDLE_SPRITE_WIDTH(
                        static_cast<std::size_t>(middleSprite_.getLocalBounds().width));
                    if (LENGTH_BETWEEN_ENDS < MIDDLE_SPRITE_WIDTH)
                    {
                        middleCount_ = 0;
                        pixelsOfMiddleToUse_ = LENGTH_BETWEEN_ENDS;
                    }
                    else
                    {
                        middleCount_ = LENGTH_BETWEEN_ENDS / MIDDLE_SPRITE_WIDTH;
                        pixelsOfMiddleToUse_ = LENGTH_BETWEEN_ENDS % MIDDLE_SPRITE_WIDTH;
                    }
                }
            }
            else
            {
                const std::size_t END_CAPS_HEIGHT(
                    (WILL_CAP_ENDS_) ? static_cast<std::size_t>(
                                           endTopOrLeftSprite_.getLocalBounds().height
                                           + endBotOrRightSprite_.getLocalBounds().height)
                                     : 0);
                if (length_ <= END_CAPS_HEIGHT)
                {
                    // if less than the minimum, then the length will be END_CAPS_HEIGHT
                    pixelsOfMiddleToUse_ = 0;
                    middleCount_ = 0;
                }
                else
                {
                    const std::size_t LENGTH_BETWEEN_ENDS(length_ - END_CAPS_HEIGHT);
                    const std::size_t MIDDLE_SPRITE_HEIGHT(
                        static_cast<std::size_t>(middleSprite_.getLocalBounds().height));
                    if (LENGTH_BETWEEN_ENDS < MIDDLE_SPRITE_HEIGHT)
                    {
                        middleCount_ = 0;
                        pixelsOfMiddleToUse_ = LENGTH_BETWEEN_ENDS;
                    }
                    else
                    {
                        middleCount_ = LENGTH_BETWEEN_ENDS / MIDDLE_SPRITE_HEIGHT;
                        pixelsOfMiddleToUse_ = LENGTH_BETWEEN_ENDS % MIDDLE_SPRITE_HEIGHT;
                    }
                }
            }
        }

        void Line::SetupOffScreenTexture()
        {
            unsigned int width{ 0 };
            unsigned int height{ 0 };

            if (ORIENTATION_ == Orientation::Horiz)
            {
                // create
                auto createWidthToUse(static_cast<unsigned int>(length_));
                if ((0 == middleCount_) && (0 == pixelsOfMiddleToUse_) && (WILL_CAP_ENDS_))
                {
                    createWidthToUse = static_cast<unsigned int>(
                        endTopOrLeftSprite_.getLocalBounds().width
                        + endBotOrRightSprite_.getLocalBounds().width);
                }

                width = createWidthToUse;
                height = static_cast<unsigned int>(endTopOrLeftSprite_.getLocalBounds().height);
                offScreenTexture_.create(width, height);
                offScreenTexture_.clear(sf::Color::Transparent);

                float posX(0.0);

                if (WILL_CAP_ENDS_)
                {
                    endTopOrLeftSprite_.setPosition(0.0f, 0.0f);
                    offScreenTexture_.draw(endTopOrLeftSprite_);
                    posX += endTopOrLeftSprite_.getLocalBounds().width;
                }

                // draw as many middle segments as needed
                const float INITIAL_POS_X(posX);
                for (std::size_t i(0); i < middleCount_; ++i)
                {
                    middleSprite_.setPosition(
                        INITIAL_POS_X
                            + (static_cast<float>(i) * middleSprite_.getLocalBounds().width),
                        0.0f);
                    offScreenTexture_.draw(middleSprite_);
                    posX += middleSprite_.getLocalBounds().width;
                }

                // draw as many middle pixels as needed
                // const float ORIG_MIDDLE_LEN(middleSprite_.getLocalBounds().width);
                if (pixelsOfMiddleToUse_ > 0)
                {
                    const sf::IntRect ORIG_RECT(middleSprite_.getTextureRect());
                    middleSprite_.setTextureRect(sf::IntRect(
                        ORIG_RECT.left,
                        ORIG_RECT.top,
                        static_cast<int>(pixelsOfMiddleToUse_),
                        ORIG_RECT.height));
                    middleSprite_.setPosition(posX, 0.0f);
                    offScreenTexture_.draw(middleSprite_);
                    posX += static_cast<float>(pixelsOfMiddleToUse_);
                }

                if (WILL_CAP_ENDS_)
                {
                    endBotOrRightSprite_.setPosition(posX, 0.0f);
                    offScreenTexture_.draw(endBotOrRightSprite_);
                    // posX += endBotOrRightSprite_.getLocalBounds().width;//posX is not used again
                    // so this addition is pointless
                }

                sf::FloatRect r(GetEntityRegion());
                r.width = static_cast<float>(length_);
                r.height = endTopOrLeftSprite_.getLocalBounds().height;
                SetEntityRegion(r);
            }
            else
            {
                // create off-screen texture
                auto createHeightToUse(static_cast<unsigned int>(length_));
                if ((0 == middleCount_) && (0 == pixelsOfMiddleToUse_) && (WILL_CAP_ENDS_))
                {
                    createHeightToUse = static_cast<unsigned int>(
                        endTopOrLeftSprite_.getLocalBounds().height
                        + endBotOrRightSprite_.getLocalBounds().height);
                }

                width = static_cast<unsigned int>(endTopOrLeftSprite_.getLocalBounds().width);
                height = createHeightToUse;

                offScreenTexture_.create(width, height);
                offScreenTexture_.clear(sf::Color::Transparent);

                float posY(0.0);

                if (WILL_CAP_ENDS_)
                {
                    endTopOrLeftSprite_.setPosition(0.0f, 0.0f);
                    offScreenTexture_.draw(endTopOrLeftSprite_);
                    posY += endTopOrLeftSprite_.getLocalBounds().height;
                }

                // draw as many middle segments as needed
                const float INITIAL_POS_Y(posY);
                for (std::size_t i(0); i < middleCount_; ++i)
                {
                    middleSprite_.setPosition(
                        0.0f,
                        INITIAL_POS_Y
                            + (static_cast<float>(i) * middleSprite_.getLocalBounds().height));
                    offScreenTexture_.draw(middleSprite_);
                    posY += middleSprite_.getLocalBounds().height;
                }

                // draw as many middle pixels as needed
                if (pixelsOfMiddleToUse_ > 0)
                {
                    const sf::IntRect ORIG_RECT(middleSprite_.getTextureRect());
                    middleSprite_.setTextureRect(sf::IntRect(
                        ORIG_RECT.left,
                        ORIG_RECT.top,
                        ORIG_RECT.width,
                        static_cast<int>(pixelsOfMiddleToUse_)));
                    middleSprite_.setPosition(0.0f, posY);
                    offScreenTexture_.draw(middleSprite_);
                    posY += static_cast<float>(pixelsOfMiddleToUse_);
                }

                if (WILL_CAP_ENDS_)
                {
                    endBotOrRightSprite_.setPosition(0.0f, posY);
                    offScreenTexture_.draw(endBotOrRightSprite_);
                    // posY += endBotOrRightSprite_.getLocalBounds().height;//posY is not used again
                    // so this addition is pointless
                }

                sf::FloatRect r(GetEntityRegion());
                r.width = endTopOrLeftSprite_.getLocalBounds().width;
                r.height = static_cast<float>(length_);
                SetEntityRegion(r);
            }

            // finalize the off-screen texture and setup the final sprite used in the draw() calls
            offScreenTexture_.display();
            finalSprite_.setTexture(offScreenTexture_.getTexture());
            finalSprite_.setTextureRect(
                sf::IntRect(0, 0, static_cast<int>(width), static_cast<int>(height)));
            finalSprite_.setPosition(GetEntityPos());
        }

        void Line::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            target.draw(finalSprite_, states);
        }

        void Line::OnColorChange() { finalSprite_.setColor(GetEntityColorForeground()); }

        void Line::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
            finalSprite_.setPosition(POS_LEFT, POS_TOP);
        }

        void Line::MoveEntityPos(const float HORIZ, const float VERT)
        {
            GuiEntity::MoveEntityPos(HORIZ, VERT);
            finalSprite_.move(HORIZ, VERT);
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
