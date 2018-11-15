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
#include "gui/collision-grid.hpp"
#include "gui/collision-quad-tree.hpp"
#include "gui/direction-enum.hpp"
#include "interact/interaction-manager.hpp"
#include "map/level-enum.hpp"
#include "map/parser.hpp"
#include "map/transition.hpp"
#include "map/walk-sfx.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/timing.hpp"
#include "misc/vector-map.hpp"

#include <SFML/Graphics/Drawable.hpp>

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
        Map(const sf::FloatRect &, interact::InteractionManager &);
        virtual ~Map();

        Map(const Map &) = delete;
        Map(Map &&) = delete;
        Map & operator=(const Map &) = delete;
        Map & operator=(Map &&) = delete;

        void TransitionLevel(const Transition &);

        void Load(
            const Level::Enum LEVEL_TO_LOAD,
            const Level::Enum LEVEL_FROM,
            const bool IS_TEST_LOAD = false);

        bool MovePlayer(const gui::Direction::Enum);

        void MoveNonPlayers();

        void MoveNonPlayer(
            avatar::Model & npcAvatar,
            const float MOVE_AMOUNT,
            const sf::FloatRect & PLAYER_RECT_FOR_COLL_DET);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        Level::Enum WhichLevel() const { return level_; }

        void Update(const float TIME_ELAPSED);

        void SetPlayerWalkAnim(const gui::Direction::Enum, const bool);

        const avatar::Model & Player() const { return player_; }

        const NpcModelMap_t & NonPlayers() const { return nonPlayersPtrModelMap_; }

        void EntryAndExitLevels(
            std::vector<Level::Enum> & entryLevels, std::vector<Level::Enum> & exitLevels);

    private:
        bool DoesPlayerCollideAfterMove(
            const gui::Direction::Enum DIRECTION, const float MOVE_AMOUNT);

        const sf::Vector2f FindPlayerStartPos(
            const TransitionVec_t &, const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM);

        bool DetectCollisionWithExitTransitionAndHandle(
            const gui::Direction::Enum DIRECTION, const float MOVE_AMOUNT);

        void HandleExitTransition(const Transition &);

        void PlayTransitionSfx(
            const gui::sound_effect::MapTransition, const bool IS_DOOR_OPENING) const;

        void UpdateWalkSfx();

        void ResetNonPlayers();

        void AddNonPlayerAvatar(const game::NpcPtr_t);

        const sf::Sprite GetNpcDefaultPoseSprite(const game::NpcPtr_t) const;

        bool IsPosTooCloseToAvatar(
            const sf::Vector2f & POS_V,
            const avatar::Model & AVATAR,
            const float DISTANCE_TOO_CLOSE) const;

        bool IsPosTooCloseToAnyAvatars(const sf::Vector2f & POS_V) const;

        void FindLocationToPlaceAvatar(
            const FloatRectVec_t & WALK_RECTS,
            std::size_t & walkRectsIndex,
            sf::Vector2f & startingPosV);

        void StopWalkSfxIfValid();

        bool DoesRectCollideWithMap(const sf::FloatRect & RECT) const;
        bool DoesRectCollideWithMap_UsingAlgorithm_Naive(const sf::FloatRect & RECT) const;
        bool DoesRectCollideWithMap_UsingAlgorithm_Quad(const sf::FloatRect & RECT) const;
        bool DoesRectCollideWithMap_UsingAlgorithm_Grid(const sf::FloatRect & RECT) const;

        bool DoesAvatarAtRectCollideWithNPCs(
            const avatar::Model & AVATAR, const sf::FloatRect & AVATAR_RECT_FOR_COLL_DET) const;

        const sf::Vector2f
            MoveVector(const gui::Direction::Enum DIRECTION, const float MOVE_AMOUNT) const;

        const sf::FloatRect AvatarCenteredMapRect(const avatar::Model & AVATAR) const;

        // Both the images used to draw the map and the lpc avatar images used to draw the
        // avatars have empty/transparent borders.  Taken together they cause avatars to
        // collide with stuff on the map before they actually touch it.  So this is corrected here
        // by shrinking the size of the bounding rect used in collision detection.
        const sf::FloatRect
            ResizeAvatarRectForCollisionDetection(const sf::FloatRect & RECT_ORIG) const;

        const sf::FloatRect AvatarRectForCollisionDetection(const avatar::Model & AVATAR) const;

        const sf::FloatRect AvatarRectForCollisionDetectionAfterMove(
            const avatar::Model & AVATAR,
            const gui::Direction::Enum DIRECTION,
            const float MOVE_AMOUNT) const;

    private:
        static const float PLAYER_MOVE_DISTANCE_;
        static const float NONPLAYER_MOVE_DISTANCE_;

        const float WALK_SFX_VOLUME_RATIO_;

        MapDisplayUPtr_t mapDisplayUPtr_;
        interact::InteractionManager & interactionManager_;
        std::vector<sf::FloatRect> collisionVec_;
        gui::QuadTree quadTree_;
        gui::CollisionGrid collisionGrid_;
        mutable misc::TimeTrials collisionTimeTrials_;
        const std::size_t collisionNaiveIndex_;
        const std::size_t collisionQuadIndex_;
        const std::size_t collisionGridIndex_;
        TransitionVec_t transitionVec_;
        Level::Enum level_;

        // the texture of this Model's LPCView will stay in the TextureCache until the
        // AdventureStage is destructed and everything in the TextureCache is cleared.
        avatar::Model player_;

        // TEMP TODO - make sure this comment still applies and makes sense after CachedTextures
        // the textures of these Model's LPCViews should be removed from the TextureCache each time
        // this map is cleared before a new level loads.
        NpcModelMap_t nonPlayersPtrModelMap_;

        WalkRectMap_t walkRectVecMap_;
        WalkSfxRegionLayers walkSfxLayers_;
        gui::sound_effect::Enum walkSfx_;
        bool walkSfxIsWalking_;
        misc::IntervalTimer sfxTimer_;
    };

    using MapUPtr_t = std::unique_ptr<Map>;
    using MapPtr_t = misc::NotNull<Map *>;
    using MapPtrOpt_t = boost::optional<MapPtr_t>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAP_HPP_INCLUDED
