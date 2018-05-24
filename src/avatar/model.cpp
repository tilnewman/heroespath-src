// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// model.cpp
//
#include "model.hpp"
#include "avatar/lpc-view.hpp"
#include "game/npc.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/sfml-util.hpp"

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
    const float Model::TIME_BETWEEN_WALK_MIN_SEC_{ 3.0f };
    const float Model::TIME_BETWEEN_WALK_MAX_SEC_{ 5.0f };
    const float Model::WALK_TARGET_CLOSE_ENOUGH_{ 5.0f };
    const float Model::WALKING_INTO_DURATION_SEC_{ 1.0f };

    Model::Model(const Avatar::Enum AVATAR_ENUM)
        : view_(
              AVATAR_ENUM,
              sf::Vector2f(
                  0.0f, 0.0f)) // see Map::Load() and Map::Move() for where player model pos is set
        , blinkTimerSec_(0.0f)
        , timeUntilNextBlinkSec_(RandomBlinkDelay())
        , blinkTimes_()
        , action_(Pose::Standing)
        , walkTimerSec_(0.0f)
        , timeUntilNextWalkSec_(RandomWalkDelay())
        , walkRects_()
        , walkTargetPosV_()
        , walkRectIndex_(0)
        , prevWalkDirection_(sfml_util::Direction::Count)
        , walkingIntoTimerSec_(0.0f)
        , walkingIntoNpcPtrOpt_(boost::none)
        , isNextToPlayer_(false)
    {}

    Model::Model(
        const Avatar::Enum AVATAR_ENUM,
        const std::vector<sf::FloatRect> & WALK_RECTS,
        const std::size_t CURRENT_WALK_RECT_INDEX,
        const sf::Vector2f & CURRENT_CENTERED_MAP_POS_V)
        : view_(AVATAR_ENUM, CURRENT_CENTERED_MAP_POS_V)
        , blinkTimerSec_(0.0f)
        , timeUntilNextBlinkSec_(RandomBlinkDelay())
        , blinkTimes_()
        , action_(Pose::Standing)
        , walkTimerSec_(0.0f)
        , timeUntilNextWalkSec_(RandomWalkDelay())
        , walkRects_(WALK_RECTS)
        , walkTargetPosV_()
        , walkRectIndex_(CURRENT_WALK_RECT_INDEX)
        , prevWalkDirection_(sfml_util::Direction::Count)
        , walkingIntoTimerSec_(0.0f)
        , walkingIntoNpcPtrOpt_(boost::none)
        , isNextToPlayer_(false)
    {}

    void Model::Update(const float TIME_ELAPSED)
    {
        UpdateAnimationAndStopIfNeeded(TIME_ELAPSED);
        UpdateBlinking(TIME_ELAPSED);

        if ((IsPlayer() == false) && (false == isNextToPlayer_))
        {
            UpdateWalkingAction(TIME_ELAPSED);
        }
    }

    void Model::SetWalkAnim(const sfml_util::Direction::Enum DIRECTION, const bool WILL_START)
    {
        view_.Set(((WILL_START) ? Pose::Walking : Pose::Standing), DIRECTION);
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

    void Model::MovingIntoSet(const game::NpcPtr_t NPC_PTR)
    {
        if (NPC_PTR != walkingIntoNpcPtrOpt_)
        {
            walkingIntoNpcPtrOpt_ = NPC_PTR;
            walkingIntoTimerSec_ = 0.0f;
        }
    }

    void Model::MovingIntoReset()
    {
        walkingIntoNpcPtrOpt_ = boost::none;
        walkingIntoTimerSec_ = 0.0f;
    }

    const game::NpcPtrOpt_t Model::MovingIntoUpdate(const float TIME_ELAPSED)
    {
        if (walkingIntoNpcPtrOpt_)
        {
            walkingIntoTimerSec_ += TIME_ELAPSED;

            if (walkingIntoTimerSec_ > WALKING_INTO_DURATION_SEC_)
            {
                walkingIntoTimerSec_ = 0.0f;
                return walkingIntoNpcPtrOpt_;
            }
        }

        return boost::none;
    }

    void Model::TurnToFacePos(const sf::Vector2f & POS_V)
    {
        auto const DIRECTION{ sfml_util::DirectionFromAToB(GetCenteredMapPos(), POS_V) };

        if ((Pose::Standing == action_) && (view_.Direction() != DIRECTION))
        {
            view_.Set(Pose::Standing, DIRECTION);
        }
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
        if (view_.Update(TIME_ELAPSED))
        {
            view_.Set(Pose::Standing, view_.Direction());
        }
    }

    void Model::UpdateBlinking(const float TIME_ELAPSED)
    {
        if (view_.WhichPose() == Pose::Standing)
        {
            blinkTimerSec_ += TIME_ELAPSED;
            if (blinkTimerSec_ > timeUntilNextBlinkSec_)
            {
                view_.Set(Pose::Blink, view_.Direction());
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
        if (IsPlayer() || (walkRects_.size() == 1))
        {
            return 0;
        }
        else
        {
            auto const CURRENT_POS_V{ GetCenteredMapPos() };

            std::vector<std::size_t> possibleWalkRectIndexes_;
            for (std::size_t i(0); i < walkRects_.size(); ++i)
            {
                if (walkRects_[i].contains(CURRENT_POS_V))
                {
                    possibleWalkRectIndexes_.emplace_back(i);
                }
            }

            if (possibleWalkRectIndexes_.empty())
            {
                return misc::random::SizeT(walkRects_.size() - 1);
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
            M_ASSERT_OR_LOGANDTHROW_SS(
                (walkRectIndex_ < walkRects_.size()),
                "avatar::Model::RandomWalkTarget() was called when walkRectIndex_ ("
                    << walkRectIndex_ << ") was out of bounds with walkRects vector of size="
                    << walkRects_.size() << ".");

            auto const RECT{ walkRects_[walkRectIndex_] };

            return sf::Vector2f(
                misc::random::Float(RECT.left, RECT.left + RECT.width),
                misc::random::Float(RECT.top, RECT.top + RECT.height));
        }
    }

    sfml_util::Direction::Enum
        Model::WalkDirection(const sfml_util::Direction::Enum DIRECTION_TO_MAINTAIN) const
    {
        auto const CURRENT_POS_V{ GetCenteredMapPos() };

        if (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Count)
        {
            std::vector<sfml_util::Direction::Enum> dirVec;

            if ((CURRENT_POS_V.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Up);
            }

            if ((walkTargetPosV_.y - CURRENT_POS_V.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Down);
            }

            if ((walkTargetPosV_.x - CURRENT_POS_V.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(sfml_util::Direction::Right);
            }

            if ((CURRENT_POS_V.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
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
                && ((CURRENT_POS_V.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Up;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Down)
                && ((walkTargetPosV_.y - CURRENT_POS_V.y) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Down;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Right)
                && ((walkTargetPosV_.x - CURRENT_POS_V.x) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Right;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == sfml_util::Direction::Left)
                && ((CURRENT_POS_V.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return sfml_util::Direction::Left;
            }
            else if ((CURRENT_POS_V.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Up;
            }
            else if ((walkTargetPosV_.y - CURRENT_POS_V.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Down;
            }
            else if ((walkTargetPosV_.x - CURRENT_POS_V.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return sfml_util::Direction::Right;
            }
            else if ((CURRENT_POS_V.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
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
