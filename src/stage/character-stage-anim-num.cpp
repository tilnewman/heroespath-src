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
// character-stage-anim-num.cpp
//
#include "character-stage-anim-num.hpp"

#include "sfml-util/sfml-util.hpp"

#include <sstream>
#include <tuple>


namespace heroespath
{
namespace stage
{

    AnimNum::AnimNum(const stats::Trait_t             VALUE,
                     const stats::Traits::Enum        WHICH_STAT,
                     const float                      START_LEFT,
                     const float                      START_TOP,
                     const float                      TARGET_LEFT,
                     const float                      TARGET_TOP,
                     const sfml_util::gui::TextInfo & TEXT_INFO)
    :
        value_       (VALUE),
        whichStat_   (WHICH_STAT),
        ignoreMe_    (VALUE < 0),
        colorVal_    (100),
        startLeft_   (START_LEFT),
        startTop_    (START_TOP),
        distanceX_   (TARGET_LEFT - START_LEFT),
        distanceY_   (TARGET_TOP - START_TOP),
        color_       (sf::Color(colorVal_, colorVal_, colorVal_, colorVal_)),
        isDoneMoving_(false),
        isDoneFading_(false),
        willFade_    (true),
        fadeCounter_ (0.0f),
        isHeldDown_  (false),
        textInfo_    (TEXT_INFO),
        textObj_     (),
        sliderX_     (misc::random::Float(0.55f, 0.75f)),
        timerSec_    (0.0f),
        prevPosX_    (-1.0f) //any negative value will work here
    {
        textObj_.setPosition(START_LEFT, START_TOP);
        CreateNewTextRegion();
    }


    AnimNum::~AnimNum()
    {}


    void AnimNum::CreateNewTextRegion()
    {
        std::ostringstream ss;
        ss << value_;
        textInfo_.text = ss.str();

        if (textInfo_.text == "0")
        {
            textInfo_.color = sf::Color(255, 200, 200);
        }
        else
        {
            textInfo_.color = sf::Color::White;
        }

        sfml_util::gui::SetupText(textObj_, textInfo_);
    }


    bool AnimNum::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        if (false == isDoneMoving_)
        {
            //position
            const float SLIDERX_RATIO(sliderX_.Update(ELAPSED_TIME_SEC));
            textObj_.setPosition(startLeft_ + (SLIDERX_RATIO * distanceX_), startTop_ + (SLIDERX_RATIO * distanceY_));

            //size
            const float SCALE( sfml_util::Map(SLIDERX_RATIO, 0.0f, 1.0f, 0.6f, 1.0f) );
            textObj_.setScale(SCALE, SCALE);

            //color
            const float COLOR_VALF(static_cast<float>(colorVal_));
            color_.r = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            color_.g = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            color_.b = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            color_.a = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            sfml_util::SetTextColor(textObj_, color_);

            isDoneMoving_ = (SLIDERX_RATIO >= 0.99);
            return false;
        }
        else
        {
            if ((false == isDoneFading_) && (willFade_))
            {
                fadeCounter_ += ELAPSED_TIME_SEC * 600.0f;
                if (color_.a > 0)
                {
                    const sf::Int8 ALPHA(static_cast<sf::Int8>(fadeCounter_));
                    if (fadeCounter_ < 254.0f)
                    {
                        color_.a = static_cast<sf::Uint8>(255 - static_cast<int>(ALPHA));
                    }
                    else
                    {
                        isDoneFading_ = true;
                        color_.a = 0;
                    }

                    sfml_util::SetTextColor(textObj_, color_);
                    return false;
                }
                else
                {
                    isDoneFading_ = true;
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }


    bool AnimNum::MouseDown(const float POS_LEFT, const float POS_TOP)
    {
        if (textObj_.getGlobalBounds().contains(POS_LEFT, POS_TOP))
        {
            isHeldDown_ = true;
            return true;
        }
        else
        {
            return false;
        }
    }


    bool AnimNum::UpdateTimer(const float ELAPSED_TIME_SEC)
    {
        if (misc::IsRealClose(prevPosX_, textObj_.getPosition().x))
        {
            timerSec_ += ELAPSED_TIME_SEC;
        }
        else
        {
            timerSec_ = 0.0f;
        }

        prevPosX_ = textObj_.getPosition().x;

        return (timerSec_ >= 3.0f);
    }


    bool operator==(const AnimNum & L, const AnimNum & R)
    {
        if (((misc::IsRealClose(L.startLeft_, R.startLeft_)) &&
             (misc::IsRealClose(L.startTop_, R.startTop_)) &&
             (misc::IsRealClose(L.distanceX_, R.distanceX_)) &&
             (misc::IsRealClose(L.distanceY_, R.distanceY_)) &&
             (misc::IsRealClose(L.fadeCounter_, R.fadeCounter_)) &&
             (misc::IsRealClose(L.timerSec_, R.timerSec_)) &&
             (misc::IsRealClose(L.prevPosX_, R.prevPosX_)) &&
             (misc::IsRealClose(L.sliderX_.Current(), R.sliderX_.Current()))) == false)
        {
            return false;
        }

        return std::tie(L.value_,
                        L.whichStat_,
                        L.ignoreMe_,
                        L.colorVal_,
                        L.color_,
                        L.isDoneMoving_,
                        L.isDoneFading_,
                        L.willFade_,
                        L.isHeldDown_,
                        L.textInfo_)
            ==
            std::tie(R.value_,
                     R.whichStat_,
                     R.ignoreMe_,
                     R.colorVal_,
                     R.color_,
                     R.isDoneMoving_,
                     R.isDoneFading_,
                     R.willFade_,
                     R.isHeldDown_,
                     R.textInfo_);
    }

}
}
