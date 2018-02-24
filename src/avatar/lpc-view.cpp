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
// lpc-view.cpp
//
#include "lpc-view.hpp"
#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"
#include "log/log-macros.hpp"
#include "sfml-util/loaders.hpp"


namespace heroespath
{
namespace avatar
{

    const int LPCView::CELL_SIZE_{ 64 };
    const float LPCView::FRAME_DURATION_SEC_WALK_{ 0.05f };
    const float LPCView::FRAME_DURATION_SEC_GIVETAKE_{ 0.1f };
    const float LPCView::FRAME_DURATION_SEC_SINGLEFRAME_{ 1.5f };
    const float LPCView::FRAME_DURATION_SEC_BLINK_MIN_{ 0.025f };
    const float LPCView::FRAME_DURATION_SEC_BLINK_MAX_{ 0.20f };


    LPCView::LPCView(const Avatar::Enum WHICH_AVATAR)
    :
        whichAvatar_(WHICH_AVATAR),
        texture_(),
        sprite_(),
        animation_(CreateAnimation(Pose::Standing, sfml_util::Direction::Right)),
        frameTimerSec_(0.0f),
        frameIndex_(0)
    {
        sfml_util::LoadTexture(texture_, Avatar::ImagePath(whichAvatar_));
        sprite_.setTexture(texture_, true);
        SetupSprite();
    }


    void LPCView::Set(
        const Pose::Enum POSE,
        const sfml_util::Direction::Enum DIRECTION)
    {
        if ((POSE != animation_.pose) || (DIRECTION != animation_.direction))
        {
            animation_ = CreateAnimation(POSE, DIRECTION);
            frameTimerSec_ = 0.0f;
            frameIndex_ = 0;
            SetupSprite();
        }
    }


    bool LPCView::Update(const float TIME_ELAPSED)
    {
        if (animation_.frame_duration > 0.0f)
        {
            frameTimerSec_ += TIME_ELAPSED;
            if (frameTimerSec_ > animation_.frame_duration)
            {
                frameTimerSec_ = 0.0f;

                if (++frameIndex_ >= animation_.frame_num_vec.size())
                {
                    frameIndex_ = 0;

                    if (false == animation_.will_loop)
                    {
                        return true;
                    }
                }
                else
                {
                    SetupSprite();
                }
            }
        }

        return false;
    }


    const FrameNumVec_t LPCView::FrameNumbers(
        const Pose::Enum POSE,
        const sfml_util::Direction::Enum DIRECTION) const
    {
        namespace su = sfml_util;

        switch (POSE)
        {
            case Pose::Standing:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:  { return { 9 }; }
                    case su::Direction::Right: { return { 25 }; }
                    case su::Direction::Up:    { return { 1 }; }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:                   { return { 17 }; }
                }
            }
            case Pose::Walking:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:  { return { 10,11,12,13,14,15,16,41 }; }
                    case su::Direction::Right: { return { 26,27,28,29,30,31,32,57 }; }
                    case su::Direction::Up:    { return { 2,3,4,1,6,7,8 }; }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:                   { return { 18,19,20,17,22,23,24 }; }
                }
            }
            case Pose::GiveTake:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:  { return { 42,43,44,45,46 }; }
                    case su::Direction::Right: { return { 58,59,60,61,62 }; }
                    case su::Direction::Up:    { return { 34,35,36,37,38 }; }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:                   { return { 50,51,52,53,54 }; }
                }
            }
            case Pose::Fight:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:  { return { 47 }; }
                    case su::Direction::Right: { return { 63 }; }
                    case su::Direction::Up:    { return { 39 }; }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:                   { return { 55 }; }
                }
            }
            case Pose::Cast:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:  { return { 48 }; }
                    case su::Direction::Right: { return { 64 }; }
                    case su::Direction::Up:    { return { 40 }; }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:                   { return { 56 }; }
                }
            }
            case Pose::Blink:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:  { return { 5 }; }
                    case su::Direction::Right: { return { 21 }; }
                    case su::Direction::Up:    { return { 1 }; }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:                   { return { 33 }; }
                }
            }
            case Pose::Dead:
            case Pose::Count:
            default:                           { return { 49 }; }
        }
    }


    const sf::IntRect LPCView::FrameRect(const FrameNum_t FRAME_NUM) const
    {
        auto const CELL_COUNT{ static_cast<FrameNum_t>(texture_.getSize().x) / CELL_SIZE_ };

        auto const FRAME_INDEX_X{ [&]()
            {
                auto const INDEX{ (FRAME_NUM % CELL_COUNT) - 1 };
                if (INDEX == -1)
                {
                    return CELL_COUNT - 1;
                }
                else
                {
                    return INDEX;
                }
            }() };

        auto const FRAME_INDEX_Y{ [&]()
            {
                auto const INDEX{ FRAME_NUM / CELL_COUNT };
                if ((FRAME_NUM % CELL_COUNT) == 0)
                {
                    return INDEX - 1;
                }
                else
                {
                    return INDEX;
                }
            }() };

        return sf::IntRect(
            FRAME_INDEX_X * CELL_SIZE_,
            FRAME_INDEX_Y * CELL_SIZE_,
            CELL_SIZE_,
            CELL_SIZE_);
    }


    const Animation LPCView::CreateAnimation(
        const Pose::Enum POSE,
        const sfml_util::Direction::Enum DIRECTION) const
    {
        return Animation(
            POSE,
            DIRECTION,
            FrameNumbers(POSE, DIRECTION),
            FrameDuration(POSE),
            (POSE == Pose::Walking));
    }


    float LPCView::FrameDuration(const Pose::Enum POSE) const
    {
        switch (POSE)
        {
            case Pose::Walking:  { return FRAME_DURATION_SEC_WALK_; }
            case Pose::GiveTake: { return FRAME_DURATION_SEC_GIVETAKE_; }
            case Pose::Cast:     { return FRAME_DURATION_SEC_SINGLEFRAME_; }
            case Pose::Blink:
            {
                return misc::random::Float(
                    FRAME_DURATION_SEC_BLINK_MIN_,
                    FRAME_DURATION_SEC_BLINK_MAX_);
            }
            case Pose::Fight:
            case Pose::Standing:
            case Pose::Dead:
            case Pose::Count:
            default: { return 0.0f; }
        }
    }


    void LPCView::SetupSprite()
    {
        sprite_.setTextureRect( FrameRect(animation_.frame_num_vec[frameIndex_]) );

        auto const SCALE{ Avatar::Scale(Avatar::Name(whichAvatar_)) };
        sprite_.setScale(SCALE, SCALE);
    }

}
}
