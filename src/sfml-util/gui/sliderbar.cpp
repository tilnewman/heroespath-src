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
// sliderbar.cpp
//
#include "sliderbar.hpp"

#include "misc/assertlogandthrow.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        SliderBar::SliderBar(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const SliderStyle & STYLE,
            const callback::ISliderBarCallbackHandlerPtrOpt_t CHANGE_HANDLER_PTR_OPT,
            const float INITIAL_VALUE)
            : GuiEntity(std::string(NAME).append("_SliderBar"), POS_LEFT, POS_TOP)
            , currentVal_(INITIAL_VALUE)
            , LENGTH_(LENGTH)
            , STYLE_(STYLE)
            , botOrLeftImage_(
                  std::string(NAME).append(
                      (STYLE.orientation == Orientation::Horiz) ? "SliderBar's_Left Arrow"
                                                                : "SliderBar's_Bottom Arrow"),
                  0.0f,
                  0.0f)
            , topOrRightImage_(
                  std::string(NAME).append(
                      (STYLE.orientation == Orientation::Horiz) ? "SliderBar's_Right Arrow"
                                                                : "SliderBar's_Top Arrow"),
                  0.0f,
                  0.0f)
            , barImage_(std::string(NAME).append("SliderBar's"), 0.0f, 0.0f)
            , padImage_(std::string(NAME).append("SliderBar's"), 0.0f, 0.0f)
            , changeHandlerPtrOpt_(CHANGE_HANDLER_PTR_OPT)
        {
            Setup();
        }

        void SliderBar::SetupEntityRegion()
        {
            // setup bounding rect for SliderBar as a Clickable object itself
            auto const POS_V{ GetEntityPos() };

            sf::FloatRect r;
            r.left = POS_V.x; // these values were already set in the constructor
            r.top = POS_V.y;
            if (STYLE_.orientation == Orientation::Horiz)
            {
                r.width = LENGTH_;
                r.height = botOrLeftImage_.GetUpSprite().getLocalBounds().height;
            }
            else
            {
                r.width = topOrRightImage_.GetUpSprite().getLocalBounds().width;
                r.height = LENGTH_;
            }

            SetEntityRegion(r);
        }

        void SliderBar::SetCurrentValue(const float NEW_VAL)
        {
            currentVal_ = NEW_VAL;
            SetPadPosition();
            OnChange(currentVal_);
        }

        void SliderBar::SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORDS)
        {
            const sf::Vector2f POS_V(GetEntityPos());

            // change from screen to slider coords
            if (STYLE_.orientation == Orientation::Horiz)
            {
                auto const MIN_SCREENX{ POS_V.x
                                        + botOrLeftImage_.GetUpSprite().getLocalBounds().width };

                auto const MAX_SCREENX{ (
                    POS_V.x + (LENGTH_ - topOrRightImage_.GetUpSprite().getLocalBounds().width)
                    - barImage_.GetUpSprite().getLocalBounds().height) };

                if (NEW_COORDS.x < MIN_SCREENX)
                {
                    currentVal_ = 0.0f;
                }
                else
                {
                    if (NEW_COORDS.x > MAX_SCREENX)
                    {
                        currentVal_ = 1.0f;
                    }
                    else
                    {
                        currentVal_ = (NEW_COORDS.x - MIN_SCREENX) / (MAX_SCREENX - MIN_SCREENX);
                    }
                }
            }
            else
            {
                auto const MIN_SCREENY{ POS_V.y
                                        + topOrRightImage_.GetUpSprite().getLocalBounds().height };

                auto const MAX_SCREENY{ (
                    POS_V.y + (LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height)
                    - barImage_.GetUpSprite().getLocalBounds().height) };

                if (NEW_COORDS.y < MIN_SCREENY)
                {
                    currentVal_ = 0.0f;
                }
                else
                {
                    if (NEW_COORDS.y > MAX_SCREENY)
                    {
                        currentVal_ = 1.0f;
                    }
                    else
                    {
                        currentVal_ = (NEW_COORDS.y - MIN_SCREENY) / (MAX_SCREENY - MIN_SCREENY);
                    }
                }
            }

            SetupAllPositions();
        }

        void SliderBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            target.draw(barImage_, states);
            target.draw(padImage_, states);
            target.draw(botOrLeftImage_, states);
            target.draw(topOrRightImage_, states);
        }

        void SliderBar::OnChange(const float)
        {
            if (changeHandlerPtrOpt_)
            {
                changeHandlerPtrOpt_->Obj().HandleCallback(this);
            }
        }

        void SliderBar::Setup()
        {
            auto const GE_PTR{ GuiElements::Instance() };

            if (STYLE_.isLineLarge)
            {
                barImage_.GetUpSprite()
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_HorizontalLineLarge());
            }
            else
            {
                barImage_.GetUpSprite()
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_HorizontalLineSmall());
            }

            if (STYLE_.orientation == Orientation::Horiz)
            {
                // pick the pad sprite
                padImage_.GetUpSprite()
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_PadHorizontalLarge());

                switch (STYLE_.brightness)
                {
                    case Brightness::Medium:
                    {
                        if (STYLE_.willLabelArrows)
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowMinusLeftMed());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowPlusRightMed());
                        }
                        else
                        {
                            botOrLeftImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowLeftMed());

                            topOrRightImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowRightMed());
                        }

                        break;
                    }
                    case Brightness::Dark:
                    {
                        if (STYLE_.willLabelArrows)
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowMinusLeftDark());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowPlusRightDark());
                        }
                        else
                        {
                            botOrLeftImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowLeftDark());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowRightDark());
                        }

                        break;
                    }

                    case Brightness::Bright: // default fallthrough
                    case Brightness::Count:
                    default:
                    {
                        if (STYLE_.willLabelArrows)
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowMinusLeftBright());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowPlusRightBright());
                        }
                        else
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowLeftBright());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowRightBright());
                        }

                        break;
                    } // end default
                } // end switch

                SetupAllPositions();

                // set the bar's length
                auto const TARGET_LENGTH{ LENGTH_
                                          - botOrLeftImage_.GetUpSprite().getLocalBounds().width
                                          + (LENGTH_ / 80.0f) };

                auto const CURRENT_LENGTH{ barImage_.GetUpSprite().getLocalBounds().width };
                auto const NEW_SCALE{ TARGET_LENGTH / CURRENT_LENGTH };
                barImage_.GetUpSprite().setScale(NEW_SCALE, 1.0f);

                // ensure the given length is long enough
                // const float MIN_LENGTH(botOrLeftImage_.GetUpSprite().getLocalBounds().width +
                // topOrRightImage_.GetUpSprite().getLocalBounds().width +
                // (padImage_.GetUpSprite().getLocalBounds().width * 2.0f));
                // M_ASSERT_OR_LOGANDTHROW_SS((LENGTH_ >= MIN_LENGTH), "SliderBar(horizontal) with
                // length=" << LENGTH_ << " is too short!  (min=" << MIN_LENGTH << ")");
            }
            else
            {
                // pick the pad sprite to use
                padImage_.GetUpSprite()
                    = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_PadVerticalLarge());

                // bar sprites are only horizontal so rotate if orientation is vertical
                barImage_.GetUpSprite().rotate(270.0f);

                switch (STYLE_.brightness)
                {
                    case Brightness::Medium:
                    {
                        if (STYLE_.willLabelArrows)
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowMinusDownMed());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowPlusUpMed());
                        }
                        else
                        {
                            botOrLeftImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowDownMed());

                            topOrRightImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowUpMed());
                        }

                        break;
                    }
                    case Brightness::Dark:
                    {
                        if (STYLE_.willLabelArrows)
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowMinusDownDark());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowPlusUpDark());
                        }
                        else
                        {
                            botOrLeftImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowDownDark());

                            topOrRightImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowUpDark());
                        }

                        break;
                    }

                    case Brightness::Bright: // default fallthrough
                    case Brightness::Count:
                    default:
                    {
                        if (STYLE_.willLabelArrows)
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowMinusDownBright());

                            topOrRightImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowPlusUpBright());
                        }
                        else
                        {
                            botOrLeftImage_.GetUpSprite() = sf::Sprite(
                                GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowDownBright());

                            topOrRightImage_.GetUpSprite()
                                = sf::Sprite(GE_PTR->GetTexture(), GE_PTR->GetRect_ArrowUpBright());
                        }

                        break;
                    } // end default
                } // end switch

                SetupAllPositions();

                // set the length of the bar sprite
                auto const TARGET_LENGTH{ LENGTH_
                                          - botOrLeftImage_.GetUpSprite().getLocalBounds().height
                                          + (LENGTH_ / 80.0f) };

                auto const CURRENT_LENGTH{ barImage_.GetUpSprite().getLocalBounds().width };
                auto const NEW_SCALE{ TARGET_LENGTH / CURRENT_LENGTH };
                barImage_.GetUpSprite().setScale(NEW_SCALE, 1.0f);

                // ensure given length is long enough
                // const float MIN_LENGTH((topOrRightImage_.GetUpSprite().getLocalBounds().height +
                // botOrLeftImage_.GetUpSprite().getLocalBounds().height +
                // (padImage_.GetUpSprite().getLocalBounds().height * 2.0f)));
                // M_ASSERT_OR_LOGANDTHROW_SS((LENGTH_ >= MIN_LENGTH), "SliderBar(vertical) with
                // length=" << LENGTH_ << " is too short!  (min = " << MIN_LENGTH << ")");
            } // end if (orientation)
        }

        void SliderBar::SetEntityRegions()
        {
            // set the GuiEntity regions, which should match the sprite_ global bounds
            topOrRightImage_.SetEntityRegion(topOrRightImage_.GetUpSprite().getGlobalBounds());
            botOrLeftImage_.SetEntityRegion(botOrLeftImage_.GetUpSprite().getGlobalBounds());
            barImage_.SetEntityRegion(barImage_.GetUpSprite().getGlobalBounds());
            padImage_.SetEntityRegion(padImage_.GetUpSprite().getGlobalBounds());
        }

        bool SliderBar::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION)
        {
            if (IsInEntityRegion(MOUSE_POS_V))
            {
                if (WHEEL_MOTION < 0.0f)
                {
                    currentVal_ -= 0.02f;
                }
                else
                {
                    currentVal_ += 0.02f;
                }

                SetupAllPositions();
                return true;
            }
            else
            {
                return false;
            }
        }

        // position sprites at either end of the bar
        void SliderBar::SetupAllPositions()
        {
            const sf::Vector2f POS_V(GetEntityPos());

            if (STYLE_.orientation == Orientation::Horiz)
            {
                botOrLeftImage_.GetUpSprite().setPosition(POS_V);

                topOrRightImage_.GetUpSprite().setPosition(
                    POS_V.x + LENGTH_ - topOrRightImage_.GetUpSprite().getLocalBounds().width,
                    POS_V.y);

                barImage_.GetUpSprite().setPosition(
                    POS_V.x + (botOrLeftImage_.GetUpSprite().getLocalBounds().width * 0.5f),
                    POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f)
                        - (barImage_.GetUpSprite().getLocalBounds().height * 0.5f));
            }
            else
            {
                botOrLeftImage_.GetUpSprite().setPosition(
                    POS_V.x,
                    POS_V.y + LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height);

                topOrRightImage_.GetUpSprite().setPosition(POS_V.x, POS_V.y);

                barImage_.GetUpSprite().setPosition(
                    POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f)
                        + (barImage_.GetUpSprite().getLocalBounds().height / 3.0f),
                    POS_V.y + (topOrRightImage_.GetUpSprite().getLocalBounds().height * 0.5f));

                barImage_.GetUpSprite().setPosition(
                    POS_V.x + (barImage_.GetUpSprite().getGlobalBounds().width * 0.5f),
                    POS_V.y + LENGTH_ - 10.0f);
            }

            SetupEntityRegion();
            SetPadPosition();
            SetEntityRegions();
            OnChange(currentVal_);
        }

        void SliderBar::SetPadPosition()
        {
            if (currentVal_ > 1.0f)
            {
                currentVal_ = 1.0f;
            }
            else
            {
                if (currentVal_ < 0.0f)
                {
                    currentVal_ = 0.0f;
                }
            }

            const sf::Vector2f POS_V(GetEntityPos());

            if (STYLE_.orientation == Orientation::Horiz)
            {
                auto const SLIDER_RANGE{ LENGTH_
                                         - (botOrLeftImage_.GetUpSprite().getLocalBounds().width
                                            + topOrRightImage_.GetUpSprite().getLocalBounds().width
                                            + padImage_.GetUpSprite().getLocalBounds().width
                                            - 8.0f) };

                // magic number 3 moves passed the shadow
                auto const SLIDER_POSX{ POS_V.x
                                        + botOrLeftImage_.GetUpSprite().getLocalBounds().width
                                        + (SLIDER_RANGE * currentVal_) - 3.0f };

                padImage_.SetEntityPos(
                    SLIDER_POSX,
                    POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f)
                        - (padImage_.GetUpSprite().getLocalBounds().height * 0.5f));
            }
            else
            {
                auto const SLIDER_RANGE{ LENGTH_
                                         - (topOrRightImage_.GetUpSprite().getLocalBounds().height
                                            + botOrLeftImage_.GetUpSprite().getLocalBounds().height
                                            + padImage_.GetUpSprite().getLocalBounds().height
                                            - 5.0f) };

                // magic number 1 moves passed the shadow
                auto const SLIDER_POSY{ POS_V.y
                                        + topOrRightImage_.GetUpSprite().getLocalBounds().height
                                        + (SLIDER_RANGE * currentVal_) - 1.0f };

                padImage_.SetEntityPos(
                    POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f)
                        - (padImage_.GetUpSprite().getLocalBounds().width * 0.5f),
                    SLIDER_POSY);
            }
        }

        bool SliderBar::MouseDown(const sf::Vector2f & MOUSE_POS_V)
        {
            if (botOrLeftImage_.MouseDown(MOUSE_POS_V))
            {
                if (STYLE_.orientation == Orientation::Horiz)
                {
                    SoundManager::Instance()->PlaySfx_TickOff();
                    currentVal_ -= 0.1f;
                }
                else
                {
                    SoundManager::Instance()->PlaySfx_TickOn();
                    currentVal_ += 0.1f;
                }

                SetupAllPositions();
                return true;
            }
            else
            {
                if (topOrRightImage_.MouseDown(MOUSE_POS_V))
                {
                    if (STYLE_.orientation == Orientation::Horiz)
                    {
                        SoundManager::Instance()->PlaySfx_TickOn();
                        currentVal_ += 0.1f;
                    }
                    else
                    {
                        SoundManager::Instance()->PlaySfx_TickOff();
                        currentVal_ -= 0.1f;
                    }

                    SetupAllPositions();
                    return true;
                }
                else
                {
                    if (padImage_.MouseDown(MOUSE_POS_V))
                    {
                        SoundManager::Instance()->PlaySfx_TickOn();
                        entityMouseState_ = MouseState::Down;
                    }
                    else
                    {
                        if (barImage_.MouseDown(MOUSE_POS_V))
                        {
                            if (STYLE_.orientation == Orientation::Horiz)
                            {
                                if (MOUSE_POS_V.x < padImage_.GetEntityRegion().left)
                                {
                                    SoundManager::Instance()->PlaySfx_TickOff();
                                    currentVal_ -= 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                                else
                                {
                                    if (MOUSE_POS_V.x > (padImage_.GetEntityRegion().left
                                                         + padImage_.GetEntityRegion().width))
                                    {
                                        SoundManager::Instance()->PlaySfx_TickOn();
                                        currentVal_ += 0.1f;
                                        SetupAllPositions();
                                        return true;
                                    }
                                }
                            }
                            else
                            {
                                if (MOUSE_POS_V.y < padImage_.GetEntityRegion().top)
                                {
                                    SoundManager::Instance()->PlaySfx_TickOff();
                                    currentVal_ -= 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                                else
                                {
                                    if (MOUSE_POS_V.y > (padImage_.GetEntityRegion().top
                                                         + padImage_.GetEntityRegion().height))
                                    {
                                        SoundManager::Instance()->PlaySfx_TickOn();
                                        currentVal_ += 0.1f;
                                        SetupAllPositions();
                                        return true;
                                    }
                                }
                            } // end else orientation
                        } // end if barEntity click
                    } // end if pad click
                } // end else topOrRightEntity click
            } // end else botOrLeftEntity click

            return false;
        }

        bool SliderBar::MouseUp(const sf::Vector2f & MOUSE_POS_V)
        {
            if (padImage_.MouseUp(MOUSE_POS_V))
            {
                SoundManager::Instance()->PlaySfx_TickOff();
                entityMouseState_ = MouseState::Up;
            }

            return false;
        }

        bool SliderBar::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
        {
            if (padImage_.IsHeldDown()) // represents the pad
            {
                SetCurrentValueFromScreenCoords(MOUSE_POS_V);
            }

            return false;
        }

        void SliderBar::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
            SetupAllPositions();
        }

        void SliderBar::MoveEntityPos(const float HORIZ, const float VERT)
        {
            GuiEntity::MoveEntityPos(HORIZ, VERT);
            SetupAllPositions();
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
