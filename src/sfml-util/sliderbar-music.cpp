// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sliderbar-music.cpp
//
#include "sliderbar-music.hpp"

#include "sfml-util/sound-manager.hpp"

#include "log/log-macros.hpp"

#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    SliderBarLabeled_Music::SliderBarLabeled_Music(
        const std::string & NAME,
        const float POS_LEFT,
        const float POS_TOP,
        const float LENGTH,
        const gui::SliderStyle & STYLE,
        const gui::MouseTextInfo & THREE_TEXT_INFOS_HOLDER,
        const float INITIAL_VALUE,
        const float RELATIVE_LABEL_POS_LEFT,
        const float RELATIVE_LABEL_POS_TOP)
        : SliderBarLabeled(
              std::string(NAME).append("_SliderBarLabeled_Music"),
              POS_LEFT,
              POS_TOP,
              LENGTH,
              STYLE,
              THREE_TEXT_INFOS_HOLDER,
              INITIAL_VALUE,
              RELATIVE_LABEL_POS_LEFT,
              RELATIVE_LABEL_POS_TOP)
    {
        SetCurrentValue((sfml_util::SoundManager::Instance()->MusicVolume() / 100.0f));
    }

    SliderBarLabeled_Music::~SliderBarLabeled_Music() = default;

    void SliderBarLabeled_Music::OnChange(const float NEW_VALUE_PARAM)
    {
        // set both current volume and saved volume in SoundManager
        auto const NEW_VALUE_RANGE_CORRECTED{ NEW_VALUE_PARAM * 100.0f };
        sfml_util::SoundManager::Instance()->MusicVolumeSet(NEW_VALUE_RANGE_CORRECTED);

        auto const NEW_VALUE_RANGE_CORRECTED_INT{ static_cast<int>(NEW_VALUE_RANGE_CORRECTED) };

        gui::TextInfo textInfo{ GetTextInfoFromSliderValue(NEW_VALUE_RANGE_CORRECTED_INT) };

        std::ostringstream ss;

        if (NEW_VALUE_RANGE_CORRECTED_INT == 0)
        {
            ss << "MUTE";

            textInfo.fontPtrOpt = FontManager::Instance()->Font_Typical();
        }
        else
        {
            ss << NEW_VALUE_RANGE_CORRECTED_INT;
            textInfo.fontPtrOpt = FontManager::Instance()->Font_NumbersDefault1();
        }

        textInfo.text = ss.str();

        sf::FloatRect textInfoRect(textRegion_.GetEntityRegion());
        textInfoRect.width = 0.0f;
        textInfoRect.height = 0.0f;

        textRegion_.Setup(textInfo, textInfoRect);
    }

    void SliderBarLabeled_Music::SetPadPosition()
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

        auto const POS_V{ GetEntityPos() };

        if (STYLE_.orientation == Orientation::Horiz)
        {
            auto const SLIDER_RANGE{ LENGTH_
                                     - (botOrLeftImage_.GetUpSprite().getLocalBounds().width
                                        + topOrRightImage_.GetUpSprite().getLocalBounds().width
                                        + padImage_.GetUpSprite().getLocalBounds().width - 8.0f) };

            // magic number 3 moves passed the shadow
            auto const SLIDER_POSX{ POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width
                                    + (SLIDER_RANGE * (1.0f - currentVal_)) - 3.0f };

            padImage_.GetUpSprite().setPosition(
                SLIDER_POSX,
                POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f)
                    - (padImage_.GetUpSprite().getLocalBounds().height * 0.5f));
        }
        else
        {
            auto const SLIDER_RANGE{ LENGTH_
                                     - (topOrRightImage_.GetUpSprite().getLocalBounds().height
                                        + botOrLeftImage_.GetUpSprite().getLocalBounds().height
                                        + padImage_.GetUpSprite().getLocalBounds().height - 5.0f) };

            // magic number 1 moves passed the shadow
            auto const SLIDER_POSY{ POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height
                                    + (SLIDER_RANGE * (1.0f - currentVal_)) - 1.0f };

            padImage_.GetUpSprite().setPosition(
                POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f)
                    - (padImage_.GetUpSprite().getLocalBounds().width * 0.5f),
                SLIDER_POSY);
        }
    }

    bool SliderBarLabeled_Music::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (botOrLeftImage_.MouseDown(MOUSE_POS_V))
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
            if (topOrRightImage_.MouseDown(MOUSE_POS_V))
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
                                SoundManager::Instance()->PlaySfx_TickOn();
                                currentVal_ += 0.1f;
                                SetupAllPositions();
                                return true;
                            }
                            else
                            {
                                if (MOUSE_POS_V.x > (padImage_.GetEntityRegion().left
                                                     + padImage_.GetEntityRegion().width))
                                {
                                    SoundManager::Instance()->PlaySfx_TickOff();
                                    currentVal_ -= 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            if (MOUSE_POS_V.y < padImage_.GetEntityRegion().top)
                            {
                                SoundManager::Instance()->PlaySfx_TickOn();
                                currentVal_ += 0.1f;
                                SetupAllPositions();
                                return true;
                            }
                            else
                            {
                                if (MOUSE_POS_V.y > (padImage_.GetEntityRegion().top
                                                     + padImage_.GetEntityRegion().height))
                                {
                                    SoundManager::Instance()->PlaySfx_TickOff();
                                    currentVal_ -= 0.1f;
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

    void SliderBarLabeled_Music::SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORDS)
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
                currentVal_ = 1.0f;
            }
            else if (NEW_COORDS.x > MAX_SCREENX)
            {
                currentVal_ = 0.0f;
            }
            else
            {
                currentVal_ = (NEW_COORDS.x - MIN_SCREENX) / (MAX_SCREENX - MIN_SCREENX);
                currentVal_ = 1.0f - currentVal_;
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
                currentVal_ = 1.0f;
            }
            else if (NEW_COORDS.y > MAX_SCREENY)
            {
                currentVal_ = 0.0f;
            }
            else
            {
                currentVal_ = (NEW_COORDS.y - MIN_SCREENY) / (MAX_SCREENY - MIN_SCREENY);
                currentVal_ = 1.0f - currentVal_;
            }
        }

        SetupAllPositions();
    }
} // namespace sfml_util
} // namespace heroespath
