// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
#define HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
//
// map-display.hpp
//
#include "gui/animation-base.hpp"
#include "gui/animation-factory.hpp"
#include "gui/cached-texture.hpp"
#include "gui/direction-enum.hpp"
#include "map/layer-type-enum.hpp"
#include "map/map-anim.hpp"
#include "map/map-tile-draw.hpp"
#include "map/three-stage-image-managers.hpp"
#include "map/tiles-panel.hpp"
#include "misc/timing.hpp"
#include "misc/vector-map.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace map
{
    struct Layout;
    struct Layer;

    // Encapsulates a tiled map, along with the player's position.
    class MapDisplay : public sf::Drawable
    {
    private:
        static constexpr int EXTRA_OFFSCREEN_TILE_COUNT_ = 2;
        static constexpr float ONSCREEN_WALK_PAD_RATIO_ = 0.2f;

    public:
        MapDisplay(const MapDisplay &) = delete;
        MapDisplay(MapDisplay &&) = delete;
        MapDisplay & operator=(const MapDisplay &) = delete;
        MapDisplay & operator=(MapDisplay &&) = delete;

        MapDisplay(const sf::FloatRect & REGION);
        virtual ~MapDisplay();

        void Load(
            const Layout & MAP_LAYOUT,
            const sf::Vector2f & PLAYER_STARTING_POS_V,
            const MapAnimVec_t &);

        bool Move(const gui::Direction::Enum, const float ADJUSTMENT);
        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        void Update(const float TIME_ELAPSED);
        void UpdateAnimMusicVolume();

        const sf::Vector2f PlayerPosMap() const { return playerPosV_ + playerPosOffsetV_; }
        const sf::Vector2f PlayerPosScreen() const { return OnScreenPosFromMapPos(PlayerPosMap()); }

        const sf::Vector2f MapSize() const;

        std::vector<sf::Sprite> & AvatarSprites() { return imageManagerAvatars_.AvatarSprites(); }

        void AppendVertexesForTileDrawQuad(
            std::vector<sf::Vertex> & vertexes, const TileDraw & TILE_DRAW) const;

        const sf::Vector2f OnScreenPosFromMapPos(const sf::Vector2f &) const;
        const sf::FloatRect OnScreenRectFromMapRect(const sf::FloatRect &) const;

        const sf::Vector2f OnScreenPosFromOffScreenPos(const sf::Vector2f &) const;
        const sf::FloatRect OnScreenRectFromOffScreenRect(const sf::FloatRect &) const;

        const sf::Vector2f OffScreenPosFromMapPos(const sf::Vector2f &) const;
        const sf::FloatRect OffScreenRectFromMapRect(const sf::FloatRect &) const;

        const sf::FloatRect OnScreenRect() const { return onScreenRect_; }

        const sf::FloatRect OffScreenTextureRect() const { return offScreenTextureRect_; }
        const sf::IntRect OffScreenTileRect() const { return offScreenTileRectI_; }

        const VertexVec_t SpriteRectToVertexVec(const sf::FloatRect & RECT) const;
        const sf::Vector2f VertexVecToSpritePos(const VertexVec_t & VERTEXES) const;
        const sf::FloatRect VertexVecToSpriteRect(const VertexVec_t & VERTEXES) const;

    private:
        // returns true if any animations changed and need to be re-drawn
        bool UpdateAnimations(const float TIME_ELAPSED);

        void UpdateOffScreenMapPosOffset();

        bool MoveUp(const float DISTANCE);
        bool MoveDown(const float DISTANCE);
        bool MoveLeft(const float DISTANCE);
        bool MoveRight(const float DISTANCE);

        void UpdateAndReDrawAfterMapTileChange();

        // this is the only place offScreenTextureRect_ is changed
        void MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V);

        const sf::IntRect CalcOffscreenRect(
            const sf::FloatRect & ONSCREEN_RECT, const sf::Vector2i & TILE_SIZE_V) const;

        const sf::IntRect CalcOffscreenTileSubRectForMapPos(
            const sf::IntRect & OFFSCREEN_RECT,
            const sf::Vector2f & MAP_POS_V,
            const sf::Vector2i & MAP_SIZE_IN_TILES_V) const;

        void SetupAnimations();
        void StartAnimMusic();
        void StopAnimMusic();
        float CalcAnimationVolume(const float DISTANCE_TO_PLAYER) const;

    private:
        // where the map is on screen in int pixels
        const sf::FloatRect onScreenRect_;

        // sub-rect of onScreenRect_ defining how close the player can get to the edge of the
        // map
        const sf::FloatRect onScreenRectInner_;

        // the full rect of both offScreenTextureAbove_ and offScreenTextureBelow_
        sf::IntRect offScreenRectI_;

        // the sub-rect of both offScreenTextureAbove_ and offScreenTextureBelow_ in tiles
        // these are the tiles from the map that are drawn offscreen
        sf::IntRect offScreenTileRectI_;

        // sub-rect of offScreenRectI_ that moves around as the player walks
        // defines what offscreen gets drawn onscreen
        // the size of this will always match the size of onScreenRect_
        sf::FloatRect offScreenTextureRect_;

        // the map pos of where the offscreen texture image starts
        // this is just a cached value to prevent repeated calculations
        sf::Vector2f offScreenMapOffsetPosV_;

        // the full rect of the entire map in map coordinates
        sf::IntRect mapRectI_;

        // the full rect of the entire map in map tiles (get the full map tile counts here)
        sf::IntRect mapTileRectI_;

        // the size of the tiles of the currently loaded map
        sf::Vector2i tileSizeVI_;

        // These distances are in offscreen coordinates.
        const float ANIM_SFX_DISTANCE_MIN_;
        const float ANIM_SFX_DISTANCE_MAX_;
        const float ANIM_SFX_VOLUME_MIN_RATIO_;

        // these two variables are in centered map coordinates
        sf::Vector2f playerPosV_;
        sf::Vector2f playerPosOffsetV_;

        MapAnimVec_t animInfoVec_;
        std::vector<gui::AnimationUPtr_t> animUPtrVec_;

        std::vector<sf::Texture> mapTileTextures_;

        ThreeStageImageManagerMapLayer imageManagerMapBelow_;
        ThreeStageImageManagerMapLayer imageManagerMapAbove_;
        ThreeStageImageManagerAvatarSprites imageManagerAvatars_;
        ThreeStageImageManagerAnimations imageManagerAnimations_;

        VertQuadArray_t backgroundColorVertexes_;
    };

    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
