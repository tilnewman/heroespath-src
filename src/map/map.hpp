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
#include <tuple>
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

    using FloatRectVec_t = std::vector<sf::FloatRect>;

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

        // does not check for collision with map elements because NPCs stay within their walk bounds
        void MoveNonPlayers();

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        Level::Enum WhichLevel() const { return level_; }

        void Update(const float TIME_ELAPSED);

        void SetPlayerWalkAnim(const gui::Direction::Enum, const bool);

        void EntryAndExitLevels(
            std::vector<Level::Enum> & entryLevels, std::vector<Level::Enum> & exitLevels);

    private:
        // returns true on collision
        bool UpdatePlayerToNonPlayerCollisionStatus(const sf::FloatRect & MOVED_PLAYER_RECT);

        const sf::Vector2f FindPlayerStartPos(
            const TransitionVec_t &, const Level::Enum LEVEL_TO_LOAD, const Level::Enum LEVEL_FROM);

        void HandleExitTransition(const Transition &);

        void PlayTransitionSfx(
            const gui::sound_effect::MapTransition, const bool IS_DOOR_OPENING) const;

        void UpdateWalkSfx();

        void ResetNonPlayers();

        void AddNonPlayerAvatar(const game::NpcPtr_t);

        const sf::Sprite GetNpcDefaultPoseSprite(const game::NpcPtr_t) const;

        const std::tuple<bool, std::size_t, sf::Vector2f>
            PickRandomLocationToPlaceAvatar(const FloatRectVec_t & WALK_RECTS) const;

        void StopWalkSfxIfValid();

        bool DoesRectCollideWithMap(const sf::FloatRect & RECT) const;
        bool DoesRectCollideWithMap_UsingAlgorithm_Naive(const sf::FloatRect & RECT) const;
        bool DoesRectCollideWithMap_UsingAlgorithm_Quad(const sf::FloatRect & RECT) const;
        bool DoesRectCollideWithMap_UsingAlgorithm_Grid(const sf::FloatRect & RECT) const;

        const sf::Vector2f
            MoveVector(const gui::Direction::Enum DIRECTION, const float MOVE_AMOUNT) const;

        // Both the images used to draw the map and the lpc avatar images used to draw the
        // avatars have empty/transparent borders.  Taken together they cause avatars to
        // collide with stuff on the map before they actually touch it.  So this is corrected here
        // by shrinking the size of the bounding rect used in collision detection.
        const sf::FloatRect
            AdjustRectForCollisionDetectionWithMap(const sf::FloatRect & RECT) const;

        // Avatars can walk in front and behind each other so only the bottom of the rect counts
        // when detecting this kind of collision
        const sf::FloatRect
            AdjustRectForCollisionDetectionWithNonPlayers(const sf::FloatRect & RECT) const;

        void SetupAvatarSpritesForDrawing();

        bool DoesAvatarAtRectCollideWithAnyNonPlayers(
            const std::size_t AVATAR_ID,
            const sf::FloatRect & RECT_ADJ_FOR_COLL_DET_WITH_NPCS) const;

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

        // TODO - make sure this comment still applies and makes sense after CachedTextures
        // the textures of these Model's LPCViews should be removed from the TextureCache each time
        // this map is cleared before a new level loads.
        NpcModelMap_t nonPlayersPtrModelMap_;

        WalkRectMap_t walkRectVecMap_;
        WalkSfxRegionLayers walkSfxLayers_;
        gui::sound_effect::Enum walkSfx_;
        bool walkSfxIsWalking_;
        misc::IntervalTimer sfxTimer_;

        const float AVATAR_TO_AVATAR_COLLISION_DETECTION_RECT_HEIGHT_;
    };

    using MapUPtr_t = std::unique_ptr<Map>;
    using MapPtr_t = misc::NotNull<Map *>;
    using MapPtrOpt_t = boost::optional<MapPtr_t>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAP_HPP_INCLUDED
