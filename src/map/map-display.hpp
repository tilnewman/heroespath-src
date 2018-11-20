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
#include "map/layer.hpp"
#include "map/layout.hpp"
#include "map/map-anim.hpp"
#include "map/tiles-panel.hpp"
#include "misc/timing.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace map
{

    struct TileDraw
    {
        TileDraw(
            const std::size_t LAYER_INDEX,
            const LayerType::Enum LAYER_TYPE,
            const sf::Vector2i & TILE_INDEX_V,
            const sf::Texture * texturePtr,
            const sf::IntRect & TEXTURE_RECT)
            : layer_index(LAYER_INDEX)
            , layer_type(LAYER_TYPE)
            , tile_index_v(TILE_INDEX_V)
            , texture_ptr(texturePtr)
            , texture_rect(TEXTURE_RECT)
        {}

        TileDraw(const TileDraw &) = default;
        TileDraw(TileDraw &&) = default;
        TileDraw & operator=(const TileDraw &) = default;
        TileDraw & operator=(TileDraw &&) = default;

        std::size_t layer_index;
        LayerType::Enum layer_type;
        sf::Vector2i tile_index_v;
        const sf::Texture * texture_ptr;
        sf::IntRect texture_rect;
    };

    inline bool operator<(const TileDraw & LHS, const TileDraw & RHS)
    {
        return std::tie(
                   LHS.layer_index,
                   LHS.layer_type,
                   LHS.texture_ptr,
                   LHS.texture_rect,
                   LHS.tile_index_v)
            < std::tie(
                   RHS.layer_index,
                   RHS.layer_type,
                   RHS.texture_ptr,
                   RHS.texture_rect,
                   RHS.tile_index_v);
    }

    inline bool operator==(const TileDraw & LHS, const TileDraw & RHS)
    {
        return std::tie(
                   LHS.texture_rect,
                   LHS.tile_index_v,
                   LHS.layer_index,
                   LHS.texture_ptr,
                   LHS.layer_type)
            == std::tie(
                   RHS.texture_rect,
                   RHS.tile_index_v,
                   RHS.layer_index,
                   RHS.texture_ptr,
                   RHS.layer_type);
    }

    inline bool operator!=(const TileDraw & LHS, const TileDraw & RHS) { return !(LHS == RHS); }

    // Encapsulates a tiled map, along with the player's position.
    class MapDisplay : public sf::Drawable
    {
    public:
        MapDisplay(const MapDisplay &) = delete;
        MapDisplay(MapDisplay &&) = delete;
        MapDisplay & operator=(const MapDisplay &) = delete;
        MapDisplay & operator=(MapDisplay &&) = delete;

        MapDisplay(const sf::FloatRect & REGION);
        virtual ~MapDisplay();

        void Load(const sf::Vector2f & STARTING_POS_V, const MapAnimVec_t &);

        bool Move(const gui::Direction::Enum, const float ADJUSTMENT);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        const sf::Vector2f PlayerPosMap() const { return playerPosV_ + playerPosOffsetV_; }

        Layout & GetLayoutRef() { return layout_; }

        void Update(const float TIME_ELAPSED);
        void UpdateAnimMusicVolume();

        const sf::Vector2i MapSizeInMapCoordinatesi() const
        {
            return sf::Vector2i(
                (layout_.tile_count_v.x * layout_.tile_size_v.x),
                (layout_.tile_count_v.y * layout_.tile_size_v.y));
        }

        const sf::Vector2f MapSizeInMapCoordinatesf() const
        {
            return sf::Vector2f(MapSizeInMapCoordinatesi());
        }

        std::vector<sf::Sprite> & AvatarSprites() { return avatarSprites_; }

    private:
        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        void ReDraw();
        void DrawCharacterImages();
        void DrawAnimations();
        void DrawMapBelowAndAbove();

        void ResetMapSubsections();

        void UpdateOffscreenTextureCoords();
        void SetupOffScreenTextures();

        void AnalyzeLayers();

        const sf::IntRect CalcOffscreenRect(
            const sf::FloatRect & ONSCREEN_RECT, const sf::Vector2i & TILE_SIZE_V) const;

        const sf::IntRect CalcOffscreenTileSubRectForMapPos(
            const sf::IntRect & OFFSCREEN_RECT,
            const sf::Vector2f & MAP_POS_V,
            const sf::Vector2i & MAP_SIZE_IN_TILES_V) const;

        const sf::Vector2f PlayerPosScreen() const;

        enum class OffScreenType
        {
            Avatar,
            Animation,
            AboveOrBelow
        };

        const sf::Vector2f ScreenPosFromMapPos(const sf::Vector2f &) const;
        const sf::FloatRect ScreenRectFromMapRect(const sf::FloatRect &) const;
        const sf::Vector2f OffScreenPosFromMapPos(const sf::Vector2f &) const;
        const sf::FloatRect OffScreenRectFromMapRect(const sf::FloatRect &) const;

        void IncrementTileOffsetsInDirection(const gui::Direction::Enum);

        void SetupAnimations();

        void StartAnimMusic();
        void StopAnimMusic();

        float CalcAnimationVolume(const float DISTANCE_TO_PLAYER) const;

        const sf::FloatRect
            SingleQuadVertexArrayPosition(const sf::VertexArray & VERTEX_ARRAY) const;

        void SingleQuadVertexArrayPosition(
            sf::VertexArray & vertexArray, const sf::FloatRect & RECT);

        const sf::FloatRect
            SingleQuadVertexArrayTextureCoords(const sf::VertexArray & VERTEX_ARRAY) const;

        void SingleQuadVertexArrayTextureCoords(
            sf::VertexArray & vertexArray, const sf::FloatRect & RECT);

        const sf::FloatRect OffScreenVertexMapRect() const
        {
            return SingleQuadVertexArrayPosition(offScreenVertexArray_);
        }

        const sf::FloatRect OffScreenVertexTextureRect() const
        {
            return SingleQuadVertexArrayTextureCoords(offScreenVertexArray_);
        }

        void OffScreenVertexMapRect(const sf::FloatRect & RECT)
        {
            SingleQuadVertexArrayPosition(offScreenVertexArray_, RECT);
        }

        void OffScreenVertexTextureRect(const sf::FloatRect & RECT)
        {
            SingleQuadVertexArrayTextureCoords(offScreenVertexArray_, RECT);
        }

    public:
        // how many extra tiles to draw offscreen that are outside the visible map area
        static const int EXTRA_OFFSCREEN_TILE_COUNT_;
        static const std::size_t VERTS_PER_QUAD_;

    private:
        // where the map is on screen in int pixels
        const sf::FloatRect onScreenRect_;

        // sub-rect of onScreenRect_ defining how close the player can get to the edge of the map
        const sf::FloatRect onScreenRectInner_;

        // the full rect of both offScreenTextureAbove_ and offScreenTextureBelow_
        sf::IntRect offScreenRectI_;

        // the sub-rect of both offScreenTextureAbove_ and offScreenTextureBelow_ in tiles
        // these are the tiles from the map that are drawn offscreen
        sf::IntRect offScreenTileRectI_;

        // sub-rect of offScreenRectI_ that defines what from the offScreenTextures is drawn to
        // screen, and moves around the offScreenTextures as the player walks
        sf::FloatRect offScreenFollowRect_;

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

        Layout layout_;

        // these two variables are in centered map coordinates
        sf::Vector2f playerPosV_;
        sf::Vector2f playerPosOffsetV_;

        // verts that define what from offscreen is drawn onscreen
        sf::VertexArray offScreenVertexArray_;

        sf::RenderTexture offScreenTextureBelow_;
        sf::RenderTexture offScreenTextureAnim_;
        sf::RenderTexture offScreenTextureAvatar_;
        sf::RenderTexture offScreenTextureAbove_;

        bool willDrawTextureBelow_;
        bool willDrawTextureAnim_;
        bool willDrawTextureAvatar_;
        bool willDrawTextureAbove_;

        gui::CachedTexture npcShadowCachedTexture_;
        sf::Sprite npcShadowSprite_;

        MapAnimVec_t animInfoVec_;
        std::vector<gui::AnimationUPtr_t> animUPtrVec_;

        // MAP_ is responsible for keeping these populated and positioned in map coordinates every
        // frame before Update()/ReDraw() is called. This class is responsible or changing these
        // positions to offscreen coordinates before every draw.
        std::vector<sf::Sprite> avatarSprites_;
        std::vector<std::size_t> avatarSpriteIndexes_;

        std::vector<TileDraw> tileDraws_;
        std::vector<std::pair<const sf::Texture *, sf::VertexArray>> tileVertexVecBelow_;
        std::vector<std::pair<const sf::Texture *, sf::VertexArray>> tileVertexVecAbove_;

        // TEMP TODO REMOVE AFTER TESTING
        mutable misc::TimeTrials timeTrials_;
        const std::size_t timeTrialIndexRedDrawSub_;
    };

    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
