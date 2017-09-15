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
//  pos-slider.cpp
//
#include "pos-slider.hpp"


namespace sfml_util
{
namespace sliders
{

    PosSlider::PosSlider(const sf::Vector2f & FROM_POS_V,
                         const sf::Vector2f & TO_POS_V,
                         const float          SLIDER_SPEED)
    :
        origFromPosV_    (FROM_POS_V),
        fromPosV_        (FROM_POS_V),
        toPosV_          (TO_POS_V),
        isFinishedMoving_(true),//will not start moving immediately
        movingDir_       (sfml_util::Moving::Still),
        slider_          (SLIDER_SPEED)
    {
        Setup(FROM_POS_V, TO_POS_V, SLIDER_SPEED);
    }


    PosSlider::~PosSlider()
    {}


    void PosSlider::Setup(const sf::Vector2f & FROM_POS_V,
                          const sf::Vector2f & TO_POS_V,
                          const float          SLIDER_SPEED)
    {
        fromPosV_ = FROM_POS_V;
        toPosV_ = TO_POS_V;
        origFromPosV_ = FROM_POS_V;
        slider_.Reset(SLIDER_SPEED);
        ResetSlidersAndStopMoving();
    }


    void PosSlider::Reset()
    {
        ResetSlidersAndStopMoving();
    }


    void PosSlider::StartMovingToward()
    {
        fromPosV_ = Position();
        ResetSlidersAndStartMoving(Moving::Toward);
    }


    void PosSlider::StartMovingAway()
    {
        toPosV_ = origFromPosV_;
        fromPosV_ = Position();
        ResetSlidersAndStartMoving(Moving::Away);
    }


    bool PosSlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isFinishedMoving_)
        {
            return false;
        }
        else
        {
            slider_.Update(ELAPSED_TIME_SECONDS);
            isFinishedMoving_ = slider_.GetIsDone();
            return true;
        }
    }


    const sf::Vector2f PosSlider::Position() const
    {
        auto const PROGRESS_RATIO{ ProgressRatio() };
        auto const POS_LEFT{ fromPosV_.x + ((toPosV_.x - fromPosV_.x) * PROGRESS_RATIO) };
        auto const POS_TOP{ fromPosV_.y + ((toPosV_.y - fromPosV_.y) * PROGRESS_RATIO) };
        return sf::Vector2f(POS_LEFT, POS_TOP);
    }


    void PosSlider::ResetSlidersAndStartMoving(const sfml_util::Moving::Enum MOVING_DIR)
    {
        slider_.Reset(slider_.GetSpd());
        movingDir_ = MOVING_DIR;
        isFinishedMoving_ = false;
    }


    void PosSlider::ResetSlidersAndStopMoving()
    {
        ResetSlidersAndStartMoving(Moving::Still);
        isFinishedMoving_ = true;
    }

}
}
