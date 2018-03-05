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
#ifndef HEROESPATH_MAP_MAP_HPP_INCLUDED
#define HEROESPATH_MAP_MAP_HPP_INCLUDED
//
// map.hpp
//
#include "sfml-util/direction-enum.hpp"

#include "avatar/model.hpp"
#include "interact/interaction-manager.hpp"
#include "map/level-enum.hpp"
#include "map/parser.hpp"
#include "map/transition.hpp"
#include "map/walk-sfx.hpp"
#include "misc/timer.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace map
{

    class MapDisplay;
    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;

    // Responsible for all state and operation of a 2D map of the game world.
    class Map : public sf::Drawable
    {
        Map(const Map &) = delete;
        Map & operator=(const Map &) = delete;

    public:
        Map(const sf::FloatRect &, interact::InteractionManager &);
        virtual ~Map();

        void TransitionLevel(const Transition &);

        void Load(
            const Level::Enum LEVEL_TO_LOAD,
            const Level::Enum LEVEL_FROM,
            const bool IS_TEST_LOAD = false);

        bool MovePlayer(const sfml_util::Direction::Enum);

        void MoveNonPlayers();

        virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

        inline Level::Enum Level() const { return level_; }

        void Update(const float TIME_ELAPSED);

        void SetPlayerWalkAnim(const sfml_util::Direction::Enum, const bool);

        inline const avatar::Model & Player() const { return player_; }

        inline const std::vector<avatar::Model> & NonPlayers() const { return nonPlayers_; }

        void EntryAndExitLevels(
            std::vector<Level::Enum> & entryLevels, std::vector<Level::Enum> & exitLevels);

    private:
        bool DoesAdjPlayerPosCollide(const sfml_util::Direction::Enum DIR, const float ADJ);

        const sf::Vector2f FindStartPos(
            const TransitionVec_t &, const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM);

        bool CheckIfEnteringTransition(
            const sfml_util::Direction::Enum DIRECTION,
            const float ADJUSTMENT,
            Transition & transition) const;

        void HandleEnteringTransition(const Transition &);

        const sf::Vector2f CalcAdjPlayerPos(
            const sfml_util::Direction::Enum DIRECTION, const float ADJUSTMENT) const;

        void PlayTransitionSfx(
            const sfml_util::sound_effect::MapTransition, const bool IS_DOOR_OPENING) const;

        void UpdateWalkMusic();

    private:
        static const float PLAYER_MOVE_DISTANCE_;
        static const float NONPLAYER_MOVE_DISTANCE_;

        const float WALK_SFX_VOLUME_RATIO_;

        MapDisplayUPtr_t mapDisplayUPtr_;
        interact::InteractionManager & interactionManager_;
        std::vector<sf::FloatRect> collisionVec_;
        TransitionVec_t transitionVec_;
        Level::Enum level_;
        avatar::Model player_;
        std::vector<avatar::Model> nonPlayers_;
        WalkRectMap_t walkRectVecMap_;
        WalkSfxRegionLayers walkSfxLayers_;
        sfml_util::music::Enum walkMusicWhich_;
        bool walkMusicIsWalking_;
        misc::Timer sfxTimer_;
        sf::FloatRect interactionRect_;
    };

    using MapUPtr_t = std::unique_ptr<Map>;
}
}

#endif // HEROESPATH_MAP_MAP_HPP_INCLUDED
