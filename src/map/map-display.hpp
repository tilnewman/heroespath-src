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
#include "map/tiles-panel.hpp"
#include "misc/timing.hpp"
#include "misc/vector-map.hpp"
#include "sfutil/display.hpp"
#include "sfutil/distance.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"
#include "sfutil/vertex.hpp"

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

    using VertexVec_t = std::vector<sf::Vertex>;
    using VertQuadArray_t = std::array<sf::Vertex, sfutil::VERTS_PER_QUAD>;

    // combines a source texture with a collection of vert quads
    using IndexVertsPair_t = std::pair<std::size_t, VertexVec_t>;
    using IndexVertsPairVec_t = std::vector<IndexVertsPair_t>;

    class MapDisplay;

    // Respobsible for a source image, an offscreen RenderTexture where that soure image is drawn
    // to, and for drawing from that offscreen image to the screen.
    class ThreeStageImageManagerBase
    {
    public:
        ThreeStageImageManagerBase()
            : willDrawOffScreen_(true)
            , willDrawOnScreen_(true)
            , offScreenTexture_()
            , offScreenToOnScreenQuadVerts_()
            , sourceToOffScreenDrawPairs_()
        {}

        virtual ~ThreeStageImageManagerBase() = default;

        void SourceToOffScreen_Update(const MapDisplay & MAP_DISPLAY)
        {
            sourceToOffScreenDrawPairs_.clear();
            SourceToOffScreen_Update_Impl(MAP_DISPLAY, sourceToOffScreenDrawPairs_);
            willDrawOffScreen_ = (sourceToOffScreenDrawPairs_.empty() == false);
        }

        void SourceToOffScreen_Draw(const MapDisplay & MAP_DISPLAY)
        {
            if (willDrawOffScreen_)
            {
                offScreenTexture_.clear(sf::Color::Transparent);

                SourceToOffScreen_Draw_Impl(
                    MAP_DISPLAY, sourceToOffScreenDrawPairs_, offScreenTexture_);

                offScreenTexture_.display();
            }
        }

        void OffScreenToOnScreen_Update(const MapDisplay & MAP_DISPLAY)
        {
            willDrawOnScreen_ = OffScreenToOnScreen_Update_Impl(
                MAP_DISPLAY,
                willDrawOffScreen_,
                offScreenToOnScreenQuadVerts_,
                sourceToOffScreenDrawPairs_);
        }

        void OffScreenToOnScreen_Draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (willDrawOnScreen_)
            {
                states.texture = &offScreenTexture_.getTexture();

                target.draw(
                    &offScreenToOnScreenQuadVerts_[0],
                    offScreenToOnScreenQuadVerts_.size(),
                    sf::Quads,
                    states);
            }
        }

        void UpdateAllAndDrawOffScreen(const MapDisplay & MAP_DISPLAY)
        {
            SourceToOffScreen_Update(MAP_DISPLAY);
            SourceToOffScreen_Draw(MAP_DISPLAY);
            OffScreenToOnScreen_Update(MAP_DISPLAY);
        }

        void MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V)
        {
            sfutil::MoveVertexTexturesForQuad(&offScreenToOnScreenQuadVerts_[0], MOVE_V);
        }

    protected:
        virtual void ResetBase(const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V)
        {
            willDrawOffScreen_ = true;
            willDrawOnScreen_ = true;
            sourceToOffScreenDrawPairs_.clear();

            const auto OFFSCREEN_TEXTURE_WIDTH { static_cast<unsigned>(
                OFFSCREEN_TEXTURE_SIZE_V.x) };
            const auto OFFSCREEN_TEXTURE_HEIGHT { static_cast<unsigned>(
                OFFSCREEN_TEXTURE_SIZE_V.y) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                offScreenTexture_.create(OFFSCREEN_TEXTURE_WIDTH, OFFSCREEN_TEXTURE_HEIGHT),
                "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                    << OFFSCREEN_TEXTURE_WIDTH << "x" << OFFSCREEN_TEXTURE_HEIGHT
                    << ") for 'below' texture.");
        }

        virtual void SourceToOffScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs)
            = 0;

        virtual void SourceToOffScreen_Draw_Impl(
            const MapDisplay & MAP_DISPLAY,
            const IndexVertsPairVec_t & DRAW_PAIRS,
            sf::RenderTexture & renderTexture)
            = 0;

        virtual bool OffScreenToOnScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY,
            const bool WILL_DRAW_OFFSCREEN,
            VertQuadArray_t & offScreenToOnScreenQuadVerts,
            const IndexVertsPairVec_t & DRAW_PAIRS)
            = 0;

    private:
        bool willDrawOffScreen_;
        bool willDrawOnScreen_;
        sf::RenderTexture offScreenTexture_;
        VertQuadArray_t offScreenToOnScreenQuadVerts_;
        IndexVertsPairVec_t sourceToOffScreenDrawPairs_;
    };

    class ThreeStageImageManagerMapLayer : public ThreeStageImageManagerBase
    {
    public:
        ThreeStageImageManagerMapLayer(const std::vector<sf::Texture> & MAP_TILE_TEXTURES)
            : ThreeStageImageManagerBase()
            , mapTileTextures_(MAP_TILE_TEXTURES)
            , mapTileDraws_()
        {}

        virtual ~ThreeStageImageManagerMapLayer() = default;

        void Reset(
            const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V, const TileDrawVec_t & MAP_TILE_DRAWS)
        {
            ResetBase(OFFSCREEN_TEXTURE_SIZE_V);
            mapTileDraws_ = MAP_TILE_DRAWS;
        }

    protected:
        void SourceToOffScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs) final;

        void SourceToOffScreen_Draw_Impl(
            const MapDisplay &,
            const IndexVertsPairVec_t & DRAW_PAIRS,
            sf::RenderTexture & renderTexture) final
        {
            sf::RenderStates renderStates { sf::RenderStates::Default };

            for (const auto & INDEX_VERTS_PAIR : DRAW_PAIRS)
            {
                if (INDEX_VERTS_PAIR.second.empty() == false)
                {
                    renderStates.texture = &mapTileTextures_.at(INDEX_VERTS_PAIR.first);

                    renderTexture.draw(
                        &INDEX_VERTS_PAIR.second[0],
                        INDEX_VERTS_PAIR.second.size(),
                        sf::Quads,
                        renderStates);
                }
            }
        }

        bool OffScreenToOnScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY,
            const bool WILL_DRAW_OFFSCREEN,
            VertQuadArray_t & offScreenToOnScreenQuadVerts,
            const IndexVertsPairVec_t & DRAW_PAIRS) final;

    private:
        const std::vector<sf::Texture> & mapTileTextures_;
        TileDrawVec_t mapTileDraws_;
    };

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

        std::vector<sf::Sprite> & AvatarSprites() { return avatarSprites_; }

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

    private:
        // returns true if any animations changed and need to be re-drawn
        bool UpdateAnimations(const float TIME_ELAPSED);

        void UpdateOffScreenMapPosOffset();

        bool MoveUp(const float DISTANCE);
        bool MoveDown(const float DISTANCE);
        bool MoveLeft(const float DISTANCE);
        bool MoveRight(const float DISTANCE);

        // these functions set which sprites/textures will be drawn to offscreen
        void SourceToOffScreen_Update_Avatars();
        void SourceToOffScreen_Update_Animations();

        // these functions draw from source sprite/textures to offscreen
        void SourceToOffScreen_Draw_Avatars();
        void SourceToOffScreen_Draw_Animations();

        // these functions set what from offscreen will be drawn onscreen
        void OffScreenToOnScreen_Update_Avatars();
        void OffScreenToOnScreen_Update_Animations();

        void OffScreenToOnScreen_Draw_Avatars(
            sf::RenderTarget & target, sf::RenderStates states) const;

        void OffScreenToOnScreen_Draw_Animations(
            sf::RenderTarget & target, sf::RenderStates states) const;

        void OffScreenToOnScreen_Draw(
            const sf::RenderTexture & RENDER_TEXTURE,
            const sf::Vertex VERTEXES[],
            const std::size_t VERTEX_COUNT,
            sf::RenderTarget & target,
            sf::RenderStates states) const;

        void UpdateAndReDrawAfterMapTileChange();

        // this is the only place offScreenTextureRect_ is changed
        void MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V);

        void SetupOffScreenTextures();

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

        const VertexVec_t SpriteRectToVertexVec(const sf::FloatRect & RECT) const
        {
            return VertexVec_t(1, sf::Vertex(sfutil::Position(RECT), sfutil::Size(RECT)));
        }

        const sf::Vector2f VertexVecToSpritePos(const VertexVec_t & VERTEXES) const
        {
            if (VERTEXES.empty())
            {
                return sf::Vector2f();
            }
            else
            {
                return VERTEXES[0].position;
            }
        }

        const sf::FloatRect VertexVecToSpriteRect(const VertexVec_t & VERTEXES) const
        {
            if (VERTEXES.empty())
            {
                return sf::FloatRect();
            }
            else
            {
                return sf::FloatRect(VERTEXES[0].position, VERTEXES[0].texCoords);
            }
        }

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

        IndexVertsPairVec_t sourceToOffScreenDrawsPairsAvatar_;
        IndexVertsPairVec_t sourceToOffScreenDrawsPairsAnim_;

        // All of these share offScreenTextureRect_
        sf::RenderTexture offScreenImageAvatar_;
        sf::RenderTexture offScreenImageAnim_;

        VertQuadArray_t offScreenToOnScreenVertsAvatar_;
        VertQuadArray_t offScreenToOnScreenVertsAnim_;

        bool willDrawOffScreenAvatar_;
        bool willDrawOffScreenAnim_;

        bool willDrawOnScreenAvatar_;
        bool willDrawOnScreenAnim_;

        // these two variables are in centered map coordinates
        sf::Vector2f playerPosV_;
        sf::Vector2f playerPosOffsetV_;

        MapAnimVec_t animInfoVec_;
        std::vector<gui::AnimationUPtr_t> animUPtrVec_;

        std::vector<sf::Sprite> avatarSprites_;

        // the avatar shadow sprite has been created so that (1) it will match the opacity of all
        // the map shadows (2) it can be drawn at the same top/left position as an avatar sprite and
        // its position will be correct (3) it can/should be scaled to match the avatar sprite's
        // scale
        gui::CachedTexture npcShadowCachedTexture_;
        sf::Sprite npcShadowSprite_;

        std::vector<sf::Texture> mapTileTextures_;

        ThreeStageImageManagerMapLayer imageManagerMapBelow_;
        ThreeStageImageManagerMapLayer imageManagerMapAbove_;
    };

    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
