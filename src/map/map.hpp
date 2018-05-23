// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_MAP_HPP_INCLUDED
#define HEROESPATH_MAP_MAP_HPP_INCLUDED
//
// map.hpp
//
#include "avatar/model.hpp"
#include "interact/interaction-manager.hpp"
#include "map/level-enum.hpp"
#include "map/parser.hpp"
#include "map/transition.hpp"
#include "map/walk-sfx.hpp"
#include "misc/not-null.hpp"
#include "misc/timer.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/direction-enum.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace game
{
    class Npc;
    using NpcPtr_t = misc::NotNull<Npc *>;
} // namespace game
namespace map
{

    class MapDisplay;
    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;

    using NpcModelMap_t = misc::VectorMap<game::NpcPtr_t, avatar::Model>;

    // Responsible for all state and operation of a 2D map of the game world.
    class Map : public sf::Drawable
    {
    public:
        Map(const Map &) = delete;
        Map(Map &&) = delete;
        Map & operator=(const Map &) = delete;
        Map & operator=(Map &&) = delete;

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

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        Level::Enum WhichLevel() const { return level_; }

        void Update(const float TIME_ELAPSED);

        void SetPlayerWalkAnim(const sfml_util::Direction::Enum, const bool);

        const avatar::Model & Player() const { return player_; }

        const NpcModelMap_t & NonPlayers() const { return nonPlayers_; }

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

        void ResetNonPlayers();

        void AddNonPlayerAvatar(const game::NpcPtr_t);

        const sf::Vector2f PlayerSpriteCenterMapPos() const;

        const sf::Sprite GetNpcDefaultPoseSprite(const game::NpcPtr_t) const;

    private:
        static const float PLAYER_MOVE_DISTANCE_;
        static const float NONPLAYER_MOVE_DISTANCE_;

        const float WALK_SFX_VOLUME_RATIO_;

        MapDisplayUPtr_t mapDisplayUPtr_;
        interact::InteractionManager & interactionManager_;
        std::vector<sf::FloatRect> collisionVec_;
        TransitionVec_t transitionVec_;
        Level::Enum level_;

        // the texture of this Model's LPCView will stay in the TextureCache until the
        // AdventureStage is destructed and everything in the TextureCache is cleared.
        avatar::Model player_;

        // the textures of these Model's LPCViews should be removed from the TextureCache each time
        // this map is cleared before a new level loads.
        NpcModelMap_t nonPlayers_;

        WalkRectMap_t walkRectVecMap_;
        WalkSfxRegionLayers walkSfxLayers_;
        sfml_util::music::Enum walkMusicWhich_;
        bool walkMusicIsWalking_;
        misc::Timer sfxTimer_;
    };

    using MapUPtr_t = std::unique_ptr<Map>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAP_HPP_INCLUDED
