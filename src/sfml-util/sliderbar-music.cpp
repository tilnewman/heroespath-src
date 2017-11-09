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
        const std::string &        NAME,
        const float                POS_LEFT,
        const float                POS_TOP,
        const float                LENGTH,
        const gui::SliderStyle &   STYLE,
        const gui::MouseTextInfo & THREE_TEXT_INFOS,
        const float                INITIAL_VALUE,
        const float                RELATIVE_LABEL_POS_LEFT,
        const float                RELATIVE_LABEL_POS_TOP)
    :
        SliderBarLabeled(
            std::string(NAME).append("_SliderBarLabeled_Music"),
            POS_LEFT,
            POS_TOP,
            LENGTH,
            STYLE,
            THREE_TEXT_INFOS,
            INITIAL_VALUE,
            RELATIVE_LABEL_POS_LEFT,
            RELATIVE_LABEL_POS_TOP)
    {
        SetCurrentValue( (sfml_util::SoundManager::Instance()->MusicVolume() / 100.0f));
    }


    SliderBarLabeled_Music::~SliderBarLabeled_Music()
    {}
    

    void SliderBarLabeled_Music::OnChange(const float NEW_VALUE_PARAM)
    {
        //set both current volume and saved volume in SoundManager
        auto const NEW_VALUE_RANGE_CORRECTED{ NEW_VALUE_PARAM * 100.0f };
        sfml_util::SoundManager::Instance()->MusicVolumeSet(NEW_VALUE_RANGE_CORRECTED);

        auto const NEW_VALUE_RANGE_CORRECTED_INT{ static_cast<int>(NEW_VALUE_RANGE_CORRECTED) };
        gui::TextInfo textInfo(threeTextInfos_.up);
        if (NEW_VALUE_RANGE_CORRECTED_INT == 0)
        {
            textInfo = threeTextInfos_.down;
        }
        else
        {
            if (NEW_VALUE_RANGE_CORRECTED_INT == 100)
            {
                textInfo = threeTextInfos_.over;
            }
        }

        std::ostringstream ss;

        if (NEW_VALUE_RANGE_CORRECTED_INT == 0)
        {
            ss << "MUTE";
            textInfo.text = ss.str();
            textInfo.fontPtr = FontManager::Instance()->Font_Typical();
        }
        else
        {
            ss << NEW_VALUE_RANGE_CORRECTED_INT;
            textInfo.text = ss.str();
            textInfo.fontPtr = FontManager::Instance()->Font_NumbersDefault1();
        }

        sf::FloatRect r(textRegion_.GetEntityRegion());
        r.width = 0.0f;
        r.height = 0.0f;
        textRegion_.Setup(textInfo, r);
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
            auto const SLIDER_RANGE{
                LENGTH_ - (botOrLeftImage_.GetUpSprite().getLocalBounds().width +
                    topOrRightImage_.GetUpSprite().getLocalBounds().width +
                    padImage_.GetUpSprite().getLocalBounds().width -
                    8.0f) };

            //magic number 3 moves passed the shadow
            auto const SLIDER_POSX{
                POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width +
                    (SLIDER_RANGE * (1.0f - currentVal_)) -
                    3.0f };

            padImage_.GetUpSprite().setPosition(
                SLIDER_POSX,
                POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f) -
                (padImage_.GetUpSprite().getLocalBounds().height * 0.5f));
        }
        else
        {
            auto const SLIDER_RANGE{
                LENGTH_ - (topOrRightImage_.GetUpSprite().getLocalBounds().height +
                    botOrLeftImage_.GetUpSprite().getLocalBounds().height +
                    padImage_.GetUpSprite().getLocalBounds().height -
                    5.0f) };

            //magic number 1 moves passed the shadow
            auto const SLIDER_POSY{
                POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height +
                    (SLIDER_RANGE * (1.0f - currentVal_)) -
                    1.0f };

            padImage_.GetUpSprite().setPosition(
                POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f) -
                    (padImage_.GetUpSprite().getLocalBounds().width * 0.5f),
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
                                if (MOUSE_POS_V.x >
                                    (padImage_.GetEntityRegion().left + padImage_.GetEntityRegion().width))
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
                                if (MOUSE_POS_V.y >
                                    (padImage_.GetEntityRegion().top + padImage_.GetEntityRegion().height))
                                {
                                    SoundManager::Instance()->PlaySfx_TickOff();
                                    currentVal_ -= 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                            }
                        }//end else orientation
                    }//end if barEntity click
                }//end if pad click
            }//end else topOrRightEntity click
        }//end else botOrLeftEntity click

        return false;
    }


    void SliderBarLabeled_Music::SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORDS)
    {
        const sf::Vector2f POS_V(GetEntityPos());

        //change from screen to slider coords
        if (STYLE_.orientation == Orientation::Horiz)
        {
            auto const MIN_SCREENX{
                POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width };

            auto const MAX_SCREENX{
                (POS_V.x + (LENGTH_ - topOrRightImage_.GetUpSprite().getLocalBounds().width) -
                    barImage_.GetUpSprite().getLocalBounds().height) };

            if (NEW_COORDS.x < MIN_SCREENX)
            {
                currentVal_ = 1.0f;
            }
            else
                if (NEW_COORDS.x > MAX_SCREENX)
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
            auto const MIN_SCREENY{
                POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height };

            auto const MAX_SCREENY{
                (POS_V.y + (LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height) -
                    barImage_.GetUpSprite().getLocalBounds().height) };

            if (NEW_COORDS.y < MIN_SCREENY)
            {
                currentVal_ = 1.0f;
            }
            else
                if (NEW_COORDS.y > MAX_SCREENY)
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

}
}
