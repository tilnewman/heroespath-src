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
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"
#include "sfutil/direction.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

namespace heroespath
{
namespace avatar
{
    const std::size_t Model::ID_INVALID_ { 0 };
    std::size_t Model::nextValidId_ { 1 };

    const float Model::NUM_BLINKS_TIME_WINDOW_SEC_ { 1.5f };
    const std::size_t Model::NUM_BLINKS_WITHIN_TIME_WINDOW_ { 3 };
    const float Model::TIME_BETWEEN_BLINK_MIN_SEC_ { 0.25f };
    const float Model::TIME_BETWEEN_BLINK_MAX_SEC_ { 4.0f };
    const float Model::TIME_BETWEEN_WALK_MIN_SEC_ { 3.0f };
    const float Model::TIME_BETWEEN_WALK_MAX_SEC_ { 5.0f };
    const float Model::WALK_TARGET_CLOSE_ENOUGH_ { 5.0f };
    const float Model::WALKING_INTO_DURATION_SEC_ { 0.5f };
    const float Model::NEXT_TO_PLAYER_PAUSE_DURATION_SEC_ { 6.0f };

    Model::Model(const Avatar::Enum AVATAR_ENUM)
        : view_(AVATAR_ENUM)
        , blinkTimerSec_(0.0f)
        , timeUntilNextBlinkSec_(RandomBlinkDelay())
        , blinkTimes_()
        , action_(Pose::Standing)
        , walkTimerSec_(0.0f)
        , timeUntilNextWalkSec_(RandomWalkDelay())
        , walkRects_()
        , walkTargetPosV_()
        , walkRectIndex_(0)
        , prevWalkDirection_(gui::Direction::Count)
        , walkingIntoTimerSec_(0.0f)
        , walkingIntoNpcPtrOpt_()
        , isNextToPlayer_(false)
        , mapPosV_(0.0f, 0.0f) // see Map::Load() and Map::Move() for where player's map pos is set
        , nextToPlayerPauseTimerSec_(0.0f)
        , id_(nextValidId_++)
    {}

    Model::Model(
        const Avatar::Enum AVATAR_ENUM,
        const std::vector<sf::FloatRect> & WALK_RECTS,
        const std::size_t CURRENT_WALK_RECT_INDEX,
        const sf::Vector2f & CENTERED_MAP_POS_V)
        : view_(AVATAR_ENUM)
        , blinkTimerSec_(0.0f)
        , timeUntilNextBlinkSec_(RandomBlinkDelay())
        , blinkTimes_()
        , action_(Pose::Standing)
        , walkTimerSec_(0.0f)
        , timeUntilNextWalkSec_(RandomWalkDelay())
        , walkRects_(WALK_RECTS)
        , walkTargetPosV_()
        , walkRectIndex_(CURRENT_WALK_RECT_INDEX)
        , prevWalkDirection_(gui::Direction::Count)
        , walkingIntoTimerSec_(0.0f)
        , walkingIntoNpcPtrOpt_()
        , isNextToPlayer_(false)
        , mapPosV_(CENTERED_MAP_POS_V)
        , nextToPlayerPauseTimerSec_(0.0f)
        , id_(nextValidId_++)
    {
        if (WALK_RECTS.empty())
        {
            M_HP_LOG_ERR(
                "Given an empty WALK_RECTS." + M_HP_VAR_STR(CURRENT_WALK_RECT_INDEX)
                + M_HP_VAR_STR(WALK_RECTS.size()));

            walkRectIndex_ = 0;
        }

        if (CURRENT_WALK_RECT_INDEX >= WALK_RECTS.size())
        {
            M_HP_LOG_ERR(
                "Given an out of bounds CURRENT_WALK_RECT_INDEX."
                + M_HP_VAR_STR(CURRENT_WALK_RECT_INDEX) + M_HP_VAR_STR(WALK_RECTS.size()));

            walkRectIndex_ = 0;
        }

        view_.SetCenteredPos(mapPosV_);
    }

    void Model::Update(const float TIME_ELAPSED)
    {
        UpdateAnimationAndStopIfNeeded(TIME_ELAPSED);
        UpdateBlinking(TIME_ELAPSED);

        if (isNextToPlayer_)
        {
            nextToPlayerPauseTimerSec_ += TIME_ELAPSED;

            if (nextToPlayerPauseTimerSec_ > NEXT_TO_PLAYER_PAUSE_DURATION_SEC_)
            {
                nextToPlayerPauseTimerSec_ = 0.0f;
                isNextToPlayer_ = false;
            }
        }

        if ((IsPlayer() == false) && (false == isNextToPlayer_))
        {
            UpdateWalkingAction(TIME_ELAPSED);
        }
    }

    void Model::MapPos(const sf::Vector2f & NEW_POS_V)
    {
        mapPosV_ = NEW_POS_V;
        UpdateSpritePos();
    }

    void Model::Move(const sf::Vector2f & MOVE_V)
    {
        mapPosV_ += MOVE_V;
        UpdateSpritePos();
    }

    void Model::SetWalkAnim(const gui::Direction::Enum DIRECTION, const bool WILL_START)
    {
        view_.Set(((WILL_START) ? Pose::Walking : Pose::Standing), DIRECTION);
    }

    void Model::StopWalking()
    {
        SetWalkAnim(prevWalkDirection_, false);
        action_ = Pose::Standing;
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

    void Model::SetIsNextToPlayer(
        const bool IS_NEXT_TO_PLAYER,
        const sf::Vector2f & PLAYER_POS_V,
        const bool WILL_FORCE_TURN_TO_FACE_PLAYER)
    {
        if (WILL_FORCE_TURN_TO_FACE_PLAYER
            || ((isNextToPlayer_ == false) && (IS_NEXT_TO_PLAYER == true)))
        {
            TurnToFacePos(PLAYER_POS_V);
        }

        isNextToPlayer_ = IS_NEXT_TO_PLAYER;
    }

    void Model::TurnToFacePos(const sf::Vector2f & POS_V)
    {
        const auto DIRECTION { sfutil::DirectionFromAToB(mapPosV_, POS_V) };

        if ((Pose::Standing == action_) && (view_.Direction() != DIRECTION))
        {
            view_.Set(Pose::Standing, DIRECTION);
        }
    }

    bool Model::IsPosWithinWalkRects(const sf::Vector2f & POS_V) const
    {
        for (const auto & RECT : walkRects_)
        {
            if (sfutil::Contains(RECT, POS_V))
            {
                return true;
            }
        }

        return false;
    }

    float Model::RandomBlinkDelay() const
    {
        return misc::Random(TIME_BETWEEN_BLINK_MIN_SEC_, TIME_BETWEEN_BLINK_MAX_SEC_);
    }

    float Model::RandomWalkDelay() const
    {
        return misc::Random(TIME_BETWEEN_WALK_MIN_SEC_, TIME_BETWEEN_WALK_MAX_SEC_);
    }

    void Model::ExtendTimeUntilNextBlinkIfNeeded(const float PREV_BLINK_DELAY)
    {
        blinkTimes_.emplace_back(PREV_BLINK_DELAY);

        if (blinkTimes_.size() > NUM_BLINKS_TIME_WINDOW_SEC_)
        {
            blinkTimes_.erase(std::begin(blinkTimes_));

            const auto TOTAL_TIME { std::accumulate(
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
                const auto PREV_TIME_BEFORE_NEXT_BLINK_DELAY_SEC { timeUntilNextBlinkSec_ };
                timeUntilNextBlinkSec_ = RandomBlinkDelay();
                ExtendTimeUntilNextBlinkIfNeeded(PREV_TIME_BEFORE_NEXT_BLINK_DELAY_SEC);
            }
        }
    }

    void Model::UpdateWalkingAction(const float TIME_ELAPSED)
    {
        const auto WAS_STANDING { (Pose::Standing == action_) };

        if (WAS_STANDING)
        {
            walkTimerSec_ += TIME_ELAPSED;

            if (walkTimerSec_ > timeUntilNextWalkSec_)
            {
                walkTimerSec_ = 0.0f;
                walkRectIndex_ = RandomWalkRectIndex();
                walkTargetPosV_ = RandomWalkTarget();

                const auto NEW_DIRECTION { WalkDirection(gui::Direction::Count) };

                if (NEW_DIRECTION < gui::Direction::Count)
                {
                    action_ = Pose::Walking;
                    SetWalkAnim(NEW_DIRECTION, true);
                    prevWalkDirection_ = NEW_DIRECTION;
                }
            }
        }
        else
        {
            const auto NEW_DIRECTION { WalkDirection(prevWalkDirection_) };

            if (NEW_DIRECTION != prevWalkDirection_)
            {
                if (NEW_DIRECTION >= gui::Direction::Count)
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
        if (IsPlayer() || (walkRects_.size() <= 1))
        {
            return 0;
        }

        std::vector<std::size_t> possibleWalkRectIndexes_;
        for (std::size_t i(0); i < walkRects_.size(); ++i)
        {
            if (sfutil::Contains(walkRects_.at(i), mapPosV_))
            {
                possibleWalkRectIndexes_.emplace_back(i);
            }
        }

        if (possibleWalkRectIndexes_.empty())
        {
            M_HP_LOG_ERR(
                "Avatar/Model/NPC was not standing in any of the valid walk rects."
                + M_HP_VAR_STR(mapPosV_) + M_HP_VAR_STR(walkRects_.size()));

            return 0;
        }

        if (possibleWalkRectIndexes_.size() > 1)
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

        if (possibleWalkRectIndexes_.empty())
        {
            M_HP_LOG_ERR(
                "Avatar/Model/NPC has no valid walk rects after removing current walkRectIndex_."
                + M_HP_VAR_STR(walkRectIndex_) + M_HP_VAR_STR(mapPosV_)
                + M_HP_VAR_STR(walkRects_.size()));

            return walkRectIndex_;
        }

        return misc::RandomSelect(possibleWalkRectIndexes_);
    }

    const sf::Vector2f Model::RandomWalkTarget() const
    {
        if (IsPlayer())
        {
            return sf::Vector2f(0.0f, 0.0f);
        }

        if (walkRectIndex_ >= walkRects_.size())
        {
            M_HP_LOG_ERR(
                "walkRectIndex_ was out of bounds." + M_HP_VAR_STR(walkRectIndex_)
                + M_HP_VAR_STR(walkRects_.size()));

            return mapPosV_;
        }

        const auto RECT { walkRects_.at(walkRectIndex_) };

        const sf::Vector2f NEW_WALK_TARGET_POS_V(
            misc::Random(RECT.left, RECT.left + RECT.width),
            misc::Random(RECT.top, RECT.top + RECT.height));

        if (!sfutil::Contains(RECT, NEW_WALK_TARGET_POS_V))
        {
            M_HP_LOG_ERR(
                "NEW_WALK_TARGET_POS_V was not contained by the RECT it came from."
                + M_HP_VAR_STR(walkRectIndex_) + M_HP_VAR_STR(walkRects_.size())
                + M_HP_VAR_STR(NEW_WALK_TARGET_POS_V) + M_HP_VAR_STR(RECT));

            return mapPosV_;
        }

        return NEW_WALK_TARGET_POS_V;
    }

    gui::Direction::Enum
        Model::WalkDirection(const gui::Direction::Enum DIRECTION_TO_MAINTAIN) const
    {
        const auto CURRENT_POS_V { mapPosV_ };

        if (DIRECTION_TO_MAINTAIN >= gui::Direction::Count)
        {
            std::vector<gui::Direction::Enum> dirVec;

            if ((CURRENT_POS_V.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(gui::Direction::Up);
            }

            if ((walkTargetPosV_.y - CURRENT_POS_V.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(gui::Direction::Down);
            }

            if ((walkTargetPosV_.x - CURRENT_POS_V.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(gui::Direction::Right);
            }

            if ((CURRENT_POS_V.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                dirVec.emplace_back(gui::Direction::Left);
            }

            if (dirVec.empty())
            {
                return gui::Direction::Count;
            }
            else
            {
                return misc::RandomSelect(dirVec);
            }
        }
        else
        {
            if ((DIRECTION_TO_MAINTAIN == gui::Direction::Up)
                && ((CURRENT_POS_V.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return gui::Direction::Up;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == gui::Direction::Down)
                && ((walkTargetPosV_.y - CURRENT_POS_V.y) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return gui::Direction::Down;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == gui::Direction::Right)
                && ((walkTargetPosV_.x - CURRENT_POS_V.x) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return gui::Direction::Right;
            }
            else if (
                (DIRECTION_TO_MAINTAIN == gui::Direction::Left)
                && ((CURRENT_POS_V.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_))
            {
                return gui::Direction::Left;
            }
            else if ((CURRENT_POS_V.y - walkTargetPosV_.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return gui::Direction::Up;
            }
            else if ((walkTargetPosV_.y - CURRENT_POS_V.y) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return gui::Direction::Down;
            }
            else if ((walkTargetPosV_.x - CURRENT_POS_V.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return gui::Direction::Right;
            }
            else if ((CURRENT_POS_V.x - walkTargetPosV_.x) > WALK_TARGET_CLOSE_ENOUGH_)
            {
                return gui::Direction::Left;
            }
            else
            {
                return gui::Direction::Count;
            }
        }
    }

    void Model::UpdateSpritePos() { view_.SetCenteredPos(mapPosV_); }

} // namespace avatar
} // namespace heroespath
