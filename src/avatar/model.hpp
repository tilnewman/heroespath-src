// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
#define HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
//
// model.hpp
//
#include "avatar/avatar-enum.hpp"
#include "avatar/lpc-view.hpp"
#include "avatar/pose-enum.hpp"
#include "gui/direction-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace heroespath
{
namespace game
{
    class Npc;
    using NpcPtr_t = misc::NotNull<Npc *>;
    using NpcPtrOpt_t = boost::optional<NpcPtr_t>;
} // namespace game
namespace avatar
{

    // Responsible for all state and non-drawing behavior of an NPC.
    class Model
    {
    public:
        // use to create player Models
        explicit Model(const Avatar::Enum);

        // use to create non-player Models
        Model(
            const Avatar::Enum,
            const std::vector<sf::FloatRect> & WALK_RECTS,
            const std::size_t CURRENT_WALK_RECT_INDEX,
            const sf::Vector2f & CURRENT_CENTERED_MAP_POS_V);

        void Update(const float TIME_ELAPSED);

        const LPCView & GetView() const { return view_; }

        void SetWalkAnim(const gui::Direction::Enum, const bool WILL_START_OR_STOP);

        bool IsPlayer() const { return walkRects_.empty(); }

        void MoveIfWalking(const float AMOUNT) { view_.MoveIfWalking(AMOUNT); }

        void StopWalking();

        void ChangeDirection();

        void MovingIntoSet(const game::NpcPtr_t);
        void MovingIntoReset();
        const game::NpcPtrOpt_t MovingIntoUpdate(const float TIME_ELAPSED);

        void SetIsNextToPlayer(
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

        bool IsWalking() const { return (Pose::Walking == action_); }

        // the pos given must be in centered map coordinates
        void TurnToFacePos(const sf::Vector2f &);

        // see comment below in LPCView about these coordinates
        const sf::Vector2f GetCenteredMapPos() const { return view_.GetCenteredMapPos(); }

        void SetCenteredMapPos(const sf::Vector2f & NEW_POS_V)
        {
            view_.SetCenteredMapPos(NEW_POS_V);
        }

    private:
        float RandomBlinkDelay() const;
        float RandomWalkDelay() const;

        void ExtendTimeUntilNextBlinkIfNeeded(const float PREV_BLINK_DELAY);

        void UpdateAnimationAndStopIfNeeded(const float TIME_ELAPSED);

        void UpdateBlinking(const float TIME_ELAPSED);

        void UpdateWalkingAction(const float TIME_ELAPSED);

        std::size_t RandomWalkRectIndex() const;

        const sf::Vector2f RandomWalkTarget() const;

        gui::Direction::Enum WalkDirection(const gui::Direction::Enum DIRECTION_TO_MAINTAIN) const;

    private:
        static const float NUM_BLINKS_TIME_WINDOW_SEC_;
        static const std::size_t NUM_BLINKS_WITHIN_TIME_WINDOW_;
        static const float TIME_BETWEEN_BLINK_MIN_SEC_;
        static const float TIME_BETWEEN_BLINK_MAX_SEC_;
        static const float TIME_BETWEEN_WALK_MIN_SEC_;
        static const float TIME_BETWEEN_WALK_MAX_SEC_;
        static const float WALK_TARGET_CLOSE_ENOUGH_;
        static const float WALKING_INTO_DURATION_SEC_;

        LPCView view_;
        float blinkTimerSec_;
        float timeUntilNextBlinkSec_;
        std::vector<float> blinkTimes_;
        Pose::Enum action_;
        float walkTimerSec_;
        float timeUntilNextWalkSec_;
        std::vector<sf::FloatRect> walkRects_;
        sf::Vector2f walkTargetPosV_;
        std::size_t walkRectIndex_;
        gui::Direction::Enum prevWalkDirection_;
        float walkingIntoTimerSec_;
        game::NpcPtrOpt_t walkingIntoNpcPtrOpt_;
        bool isNextToPlayer_;
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
