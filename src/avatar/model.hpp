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
#ifndef HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
#define HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
//
// model.hpp
//
#include "avatar/pose-enum.hpp"

#include "sfml-util/direction-enum.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <vector>
#include <utility>


namespace heroespath
{
namespace avatar
{

    struct IView;
    using IViewUPtr_t = std::unique_ptr<IView>;


    //Responsible for all state and non-drawing behavior of an NPC.
    class Model
    {
    public:
        Model(
            IViewUPtr_t viewUPtr,
            const std::vector<sf::FloatRect> & WALK_RECTS = std::vector<sf::FloatRect>());

        void Update(const float TIME_ELAPSED);

        inline const IView & GetView() const { return * viewUPtr_; }

        void SetWalkAnim(const sfml_util::Direction::Enum, const bool WILL_START_OR_STOP);

        inline bool IsPlayer() const { return walkRects_.empty(); }

        void Move(const float AMOUNT);

        void StopWalking();

        void ChangeDirection();

        void MovingIntoSet(const std::size_t NON_PLAYER_INDEX);
        void MovingIntoReset();
        int MovingIntoUpdate(const float TIME_ELAPSED);

    private:
        float RandomBlinkDelay() const;
        float RandomWalkDelay() const;

        void ExtendTimeUntilNextBlinkIfNeeded(const float PREV_BLINK_DELAY);

        void UpdateAnimationAndStopIfNeeded(const float TIME_ELAPSED);

        void UpdateBlinking(const float TIME_ELAPSED);

        void UpdateWalkingAction(const float TIME_ELAPSED);

        std::size_t RandomWalkRectIndex() const;

        const sf::Vector2f RandomWalkTarget() const;

        sfml_util::Direction::Enum WalkDirection(
            const sfml_util::Direction::Enum DIRECTION_TO_MAINTAIN) const;

    private:
        static const float NUM_BLINKS_TIME_WINDOW_SEC_;
        static const std::size_t NUM_BLINKS_WITHIN_TIME_WINDOW_;
        static const float TIME_BETWEEN_BLINK_MIN_SEC_;
        static const float TIME_BETWEEN_BLINK_MAX_SEC_;
        static const float TIME_BETWEEN_WALK_MIN_SEC_;
        static const float TIME_BETWEEN_WALK_MAX_SEC_;
        static const float WALK_TARGET_CLOSE_ENOUGH_;
        static const int WALKING_INTO_INDEX_INVALID_;
        static const float WALKING_INTO_DURATION_SEC_;

        IViewUPtr_t viewUPtr_;
        float blinkTimerSec_;
        float timeUntilNextBlinkSec_;
        std::vector<float> blinkTimes_;
        Pose::Enum action_;
        float walkTimerSec_;
        float timeUntilNextWalkSec_;
        std::vector<sf::FloatRect> walkRects_;
        sf::Vector2f walkTargetPosV_;
        std::size_t walkRectIndex_;
        sf::Vector2f posV_;
        sfml_util::Direction::Enum prevWalkDirection_;
        float walkingIntoTimerSec_;
        int walkingIntoIndex_;
    };

}
}

#endif //HEROESPATH_AVATAR_MODEL_HPP_INCLUDED
