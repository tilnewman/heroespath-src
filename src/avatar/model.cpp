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
#include "avatar/i-view.hpp"
#include "avatar/lpc-view.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

namespace heroespath
{
namespace avatar
{

    const float Model::NUM_BLINKS_TIME_WINDOW_SEC_{ 1.5f };
    const std::size_t Model::NUM_BLINKS_WITHIN_TIME_WINDOW_{ 3 };
    const float Model::TIME_BETWEEN_BLINK_MIN_SEC_{ 0.25f };
    const float Model::TIME_BETWEEN_BLINK_MAX_SEC_{ 4.0f };
    const float Model::TIME_BETWEEN_WALK_MIN_SEC_{ 1.0f };
    const float Model::TIME_BETWEEN_WALK_MAX_SEC_{ 2.0f };
    const float Model::WALK_TARGET_CLOSE_ENOUGH_{ 5.0f };
    const std::size_t Model::WALKING_INTO_INDEX_INVALID_{
        0
    }; // TODO fix by using something other than a size_t
    const float Model::WALKING_INTO_DURATION_SEC_{ 1.5f };

    Model::Model(IViewUPtr_t viewUPtr, const std::vector<sf::FloatRect> & WALK_RECTS)
        : viewUPtr_(std::move(viewUPtr))
        , blinkTimerSec_(0.0f)
        , timeUntilNextBlinkSec_(RandomBlinkDelay())
        , blinkTimes_()
        , action_(Pose::Standing)
        , walkTimerSec_(0.0f)
        , timeUntilNextWalkSec_(RandomWalkDelay())
        , walkRects_(WALK_RECTS)
        , walkTargetPosV_()
        , walkRectIndex_(RandomWalkRectIndex())
        , posV_(RandomWalkTarget())
        , prevWalkDirection_(sfml_util::Direction::Count)
        , walkingIntoTimerSec_(0.0f)
        , walkingIntoIndex_(WALKING_INTO_INDEX_INVALID_)
    {}

    void Model::Update(const float TIME_ELAPSED)
    {
        UpdateAnimationAndStopIfNeeded(TIME_ELAPSED);
        UpdateBlinking(TIME_ELAPSED);

        if (IsPlayer() == false)
        {
            UpdateWalkingAction(TIME_ELAPSED);
        }

        viewUPtr_->UpdatePos(posV_);

        // TODO finish implementation,
        // TODO change method of identifying moving into targets, std::size_t won't work...
        MovingIntoUpdate(TIME_ELAPSED);
    }

    void Model::SetWalkAnim(const sfml_util::Direction::Enum DIRECTION, const bool WILL_START)
    {
        viewUPtr_->Set(((WILL_START) ? Pose::Walking : Pose::Standing), DIRECTION);
    }

    void Model::Move(const float AMOUNT)
    {
        if (Pose::Walking == action_)
        {
            switch (viewUPtr_->Direction())
            {
                case sfml_util::Direction::Left:
                {
                    posV_.x -= AMOUNT;
                    break;
                }
                case sfml_util::Direction::Right:
                {
                    posV_.x += AMOUNT;
                    break;
                }
                case sfml_util::Direction::Up:
                {
                    posV_.y -= AMOUNT;
                    break;
                }
                case sfml_util::Direction::Down:
                case sfml_util::Direction::Count:
                default:
                {
                    posV_.y += AMOUNT;
                    break;
                }
            }

            viewUPtr_->UpdatePos(posV_);
        }
    }

    void Model::StopWalking()
    {
        SetWalkAnim(prevWalkDirection_, false);
        action_ = Pose::Standing;
    }

    void Model::ChangeDirection()
    {
        std::vector<sfml_util::Direction::Enum> dirVec{ sfml_util::Direction::Left,
                                                        sfml_util::Direction::Right,
                                                        sfml_util::Direction::Up,
                                                        sfml_util::Direction::Down };

        dirVec.erase(
            std::remove(std::begin(dirVec), std::end(dirVec), GetView().Direction()),
            std::end(dirVec));

        auto const NEW_DIRECTION{ misc::Vector::SelectRandom(dirVec) };

        auto const RECT{ walkRects_[walkRectIndex_] };

        switch (NEW_DIRECTION)
        {
            case sfml_util::Direction::Left:
            {
                walkTargetPosV_.x = RECT.left;
                break;
            }
            case sfml_util::Direction::Right:
            {
                walkTargetPosV_.x = RECT.left + RECT.width;
                break;
            }
            case sfml_util::Direction::Up:
            {
                walkTargetPosV_.y = RECT.top;
                break;
            }
            case sfml_util::Direction::Down:
            case sfml_util::Direction::Count:
            default:
            {
                walkTargetPosV_.y = RECT.top + RECT.height;
                break;
            }
        }

        SetWalkAnim(NEW_DIRECTION, true);
        prevWalkDirection_ = NEW_DIRECTION;
    }

    void Model::MovingIntoSet(const std::size_t NON_PLAYER_INDEX)
    {
        if (NON_PLAYER_INDEX != walkingIntoIndex_)
        {
            walkingIntoIndex_ = NON_PLAYER_INDEX;
            walkingIntoTimerSec_ = 0.0f;
        }
    }

    void Model::MovingIntoReset()
    {
        walkingIntoIndex_ = WALKING_INTO_INDEX_INVALID_;
        walkingIntoTimerSec_ = 0.0f;
    }

    std::size_t Model::MovingIntoUpdate(const float TIME_ELAPSED)
    {
        if (walkingIntoIndex_ != WALKING_INTO_INDEX_INVALID_)
        {
            walkingIntoTimerSec_ += TIME_ELAPSED;

            if (walkingIntoTimerSec_ > WALKING_INTO_DURATION_SEC_)
            {
                walkingIntoTimerSec_ = 0.0f;
                return walkingIntoIndex_;
            }
        }

        return WALKING_INTO_INDEX_INVALID_;
    }

    float Model::RandomBlinkDelay() const
    {
        return misc::random::Float(TIME_BETWEEN_BLINK_MIN_SEC_, TIME_BETWEEN_BLINK_MAX_SEC_);
    }

    float Model::RandomWalkDelay() const
    {
        return misc::random::Float(TIME_BETWEEN_WALK_MIN_SEC_, TIME_BETWEEN_WALK_MAX_SEC_);
    }

    void Model::ExtendTimeUntilNextBlinkIfNeeded(const float PREV_BLINK_DELAY)
    {
        blinkTimes_.emplace_back(PREV_BLINK_DELAY);

        if (blinkTimes_.size() > NUM_BLINKS_TIME_WINDOW_SEC_)
        {
            blinkTimes_.erase(std::begin(blinkTimes_));

            auto const TOTAL_TIME{ std::accumulate(
                std::begin(blinkTimes_), std::end(blinkTimes_), 0.0f) };

            if (TOTAL_TIME < NUM_BLINKS_TIME_WINDOW_SEC_)
            {
                timeUntilNextBlinkSec_ += NUM_BLINKS_TIME_WINDOW_SEC_;
            }
        }
    }

    void Model::UpdateAnimationAndStopIfNeeded(const float TIME_ELAPSED)
    {
        if (viewUPtr_->Update(TIME_ELAPSED))
        {
            viewUPtr_->Set(Pose::Standing, viewUPtr_->Direction());
        }
    }

    void Model::UpdateBlinking(const float TIME_ELAPSED)
    {
        if (viewUPtr_->Pose() == Pose::Standing)
        {
            blinkTimerSec_ += TIME_ELAPSED;
            if (blinkTimerSec_ > timeUntilNextBlinkSec_)
            {
                viewUPtr_->Set(Pose::Blink, viewUPtr_->Direction());
                blinkTimerSec_ = 0.0f;
                auto const PREV_TIME_BEFORE_NEXT_BLINK_DELAY_SEC{ timeUntilNextBlinkSec_ };
                timeUntilNextBlinkSec_ = RandomBlinkDelay();
                ExtendTimeUntilNextBlinkIfNeeded(PREV_TIME_BEFORE_NEXT_BLINK_DELAY_SEC);
            }
        }
    }

    void Model::UpdateWalkingAction(const float TIME_ELAPSED)
    {
        auto const WAS_STANDING{ (Pose::Standing == action_) };

        if (WAS_STANDING)
        {
            walkTimerSec_ += TIME_ELAPSED;

            if (walkTimerSec_ > timeUntilNextWalkSec_)
            {
                walkTimerSec_ = 0.0f;
                walkRectIndex_ = RandomWalkRectIndex();
                walkTargetPosV_ = RandomWalkTarget();

                auto const NEW_DIRECTION{ WalkDirection(sfml_util::Direction::Count) };

                if (NEW_DIRECTION != sfml_util::Direction::Count)
                {
                    action_ = Pose::Walking;
                    SetWalkAnim(NEW_DIRECTION, true);
                    prevWalkDirection_ = NEW_DIRECTION;
                }
            }
        }
        else
        {
            auto const NEW_DIRECTION{ WalkDirection(prevWalkDirection_) };

            if (NEW_DIRECTION != prevWalkDirection_)
            {
                if (NEW_DIRECTION == sfml_util::Direction::Count)
                {
                    StopWalking();
                }
                else
                {
                    SetWalkAnim(NEW_DIRECTION, true);
                }

                prevWalkDirection_ = NEW_DIRECTION;
            }
        }
    }

    std::size_t Model::RandomWalkRectIndex() const
    {
        if (walkRects_.size() <= 1)
        {
            return 0;
        }
        else
        {
            std::vector<std::size_t> possibleWalkRectIndexes_;
            for (std::size_t i(0); i < walkRects_.size(); ++i)
            {
                if (walkRects_[i].contains(posV_))
                {
                    possibleWalkRectIndexes_.emplace_back(i);
                }
            }

            if (possibleWalkRectIndexes_.empty())
            {
                return static_cast<std::size_t>(
                    misc::random::Int(0, static_cast<int>(walkRects_.size())));
            }
            else if (possibleWalkRectIndexes_.size() > 1)
            {
                // if there is more than one option, then remove the current index
                // from the list of possibilities
                possibleWalkRectIndexes_.erase(
                    std::remove(
                        std::begin(possibleWalkRectIndexes_),
                        std::end(possibleWalkRectIndexes_),
                        walkRectIndex_),
                    std::end(possibleWalkRectIndexes_));
            }

            return misc::Vector::SelectRandom(possibleWalkRectIndexes_);
        }
    }

    const sf::Vector2f Model::RandomWalkTarget() const
    {
        if (IsPlayer())
        {
            return sf::Vector2f(0.0f, 0.0f);
        }
        else
        {
            auto const RECT{ walkRects_[walkRectIndex_] };

            return sf::Vector2f(
                misc::random::Float(RECT.left, RECT.left + RECT.width),
                misc::random::Float(RECT.top, RECT.top + RECT.height));
        }
    }

    sfml_util::Direction::Enum
        Model::WalkDirection(const sfml_util::Direction::Enum DIRECTION_TO_MAINTAIN) const
    {
        if (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Count)
        {
            std::vector<sfml_util::Direction::Enum> dirVec;

            if ((posV_.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Up);
            }

            if ((walkTargetPosV_.y - posV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Down);
            }

            if ((walkTargetPosV_.x - posV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Right);
            }

            if ((posV_.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Left);
            }

            if (dirVec.empty())
            {
                return sfml_util::Direction::Count;
            }
            else
            {
                return misc::Vector::SelectRandom(dirVec);
            }
        }
        else
        {
            if ((DIRECTION_TO_MAINTAIN == sfml_util::Direction::Up)
                && ((posV_.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Up;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Down)
                && ((walkTargetPosV_.y - posV_.y) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Down;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Right)
                && ((walkTargetPosV_.x - posV_.x) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Right;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Left)
                && ((posV_.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Left;
            }
            else if ((posV_.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Up;
            }
            else if ((walkTargetPosV_.y - posV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Down;
            }
            else if ((walkTargetPosV_.x - posV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Right;
            }
            else if ((posV_.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Left;
            }
            else
            {
                return sfml_util::Direction::Count;
            }
        }
    }
} // namespace avatar
} // namespace heroespath
