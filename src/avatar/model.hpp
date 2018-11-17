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
#include "sfutil/size-and-scale.hpp"

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

        // use to create non-player (NPC) Models
        Model(
            const Avatar::Enum,
            const std::vector<sf::FloatRect> & WALK_RECTS,
            const std::size_t CURRENT_WALK_RECT_INDEX,
            const sf::Vector2f & MAP_POS_V);

        Model(const Model &) = default;
        Model(Model &&) = default;
        Model & operator=(const Model &) = default;
        Model & operator=(Model &&) = default;

        void Update(const float TIME_ELAPSED);

        bool IsPlayer() const { return walkRects_.empty(); }
        bool IsWalking() const { return (Pose::Walking == action_); }
        Pose::Enum WhichPose() const { return view_.WhichPose(); }
        gui::Direction::Enum Direction() const { return view_.Direction(); }
        const sf::Sprite & CurrentSprite() const { return view_.CurrentSprite(); }
        const sf::Sprite & DefaultPoseSprite() const { return view_.DefaultPoseSprite(); }
        const sf::Vector2f MapPos() const { return mapPosV_; }

        void MapPos(const sf::Vector2f & NEW_POS_V);
        void Move(const sf::Vector2f & MOVE_V);
        void SetWalkAnim(const gui::Direction::Enum, const bool WILL_START_OR_STOP);
        void StopWalking();
        void ChangeDirection();
        void MovingIntoSet(const game::NpcPtr_t);
        void MovingIntoReset();
        const game::NpcPtrOpt_t MovingIntoUpdate(const float TIME_ELAPSED);

        void SetIsNextToPlayer(
            const bool IS_NEXT_TO_PLAYER,
            const sf::Vector2f & PLAYER_POS_V,
            const bool WILL_FORCE_TURN_TO_FACE_PLAYER);

        // the pos given must be in centered map coordinates
        void TurnToFacePos(const sf::Vector2f &);

        static const std::size_t ID_INVALID_;
        std::size_t Id() const { return id_; }

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
        void UpdateSpritePos();

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
        sf::Vector2f mapPosV_;

        // comparing avatar::Models by their member variables won't work so we need a unique id
        // number to tell them apart, and there needs to be an invalid or not-an-id number as well.
        std::size_t id_;
        static std::size_t nextValidId_;
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
