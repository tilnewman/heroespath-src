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
// model.cpp
//
#include "model.hpp"
#include "npc/i-view.hpp"
#include "misc/random.hpp"

#include <numeric>


namespace heroespath
{
namespace npc
{

    const float Model::TIME_BETWEEN_BLINK_MIN_SEC_{ 0.25f };
    const float Model::TIME_BETWEEN_BLINK_MAX_SEC_{ 4.0f };
    const float Model::NUM_BLINKS_TIME_WINDOW_SEC_{ 1.5f };
    const std::size_t Model::NUM_BLINKS_WITHIN_TIME_WINDOW_{ 3 };


    Model::Model(IViewUPtr_t viewUPtr)
    :
        viewUPtr_(std::move(viewUPtr)),
        blinkTimerSec_(0.0f),
        timeBeforeNextBlinkSec_(RandomBlinkDelay()),
        blinkTimes_()
    {}


    void Model::Update(const float TIME_ELAPSED)
    {
        if (viewUPtr_->Update(TIME_ELAPSED))
        {
            viewUPtr_->Set(Pose::Standing, viewUPtr_->Direction());
        }

        if (viewUPtr_->Pose() == Pose::Standing)
        {
            blinkTimerSec_ += TIME_ELAPSED;
            if (blinkTimerSec_ > timeBeforeNextBlinkSec_)
            {
                viewUPtr_->Set(Pose::Blink, viewUPtr_->Direction());
                blinkTimerSec_ = 0.0f;
                auto const PREV_TIME_BEFORE_NEXT_BLINK_DELAY_SEC{ timeBeforeNextBlinkSec_ };
                timeBeforeNextBlinkSec_ = RandomBlinkDelay();
                ExtendTimeBeforeNextBlinkIfNeeded(PREV_TIME_BEFORE_NEXT_BLINK_DELAY_SEC);
            }
        }
    }


    void Model::WalkAnim(const sfml_util::Direction::Enum DIRECTION, const bool WILL_START)
    {
        viewUPtr_->Set(((WILL_START) ? Pose::Walking : Pose::Standing), DIRECTION);
    }


    float Model::RandomBlinkDelay() const
    {
        return misc::random::Float(TIME_BETWEEN_BLINK_MIN_SEC_, TIME_BETWEEN_BLINK_MAX_SEC_);
    }


    void Model::ExtendTimeBeforeNextBlinkIfNeeded(const float PREV_BLINK_DELAY)
    {
        blinkTimes_.push_back(PREV_BLINK_DELAY);

        if (blinkTimes_.size() > NUM_BLINKS_TIME_WINDOW_SEC_)
        {
            blinkTimes_.erase( std::begin(blinkTimes_) );

            auto const TOTAL_TIME{
                std::accumulate(std::begin(blinkTimes_), std::end(blinkTimes_), 0.0f) };

            if (TOTAL_TIME < NUM_BLINKS_TIME_WINDOW_SEC_)
            {
                timeBeforeNextBlinkSec_ += NUM_BLINKS_TIME_WINDOW_SEC_;
            }
        }
    }

}
}
