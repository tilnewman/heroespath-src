// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_THREE_STAGE_IMAGE_MANAGERS_HPP_INCLUDED
#define HEROESPATH_MAP_THREE_STAGE_IMAGE_MANAGERS_HPP_INCLUDED
//
// three-stage-image-managers.hpp
//
#include "gui/cached-texture.hpp"
#include "map/map-anim.hpp"
#include "map/map-tile-draw.hpp"
#include "misc/vector-map.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace heroespath
{
namespace gui
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;
} // namespace gui
namespace map
{

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
        ThreeStageImageManagerBase();
        virtual ~ThreeStageImageManagerBase() = default;

        void SourceToOffScreen_Update(const MapDisplay & MAP_DISPLAY);
        void SourceToOffScreen_Draw(const MapDisplay & MAP_DISPLAY);
        void OffScreenToOnScreen_Update(const MapDisplay & MAP_DISPLAY);
        void OffScreenToOnScreen_Draw(sf::RenderTarget & target, sf::RenderStates states) const;
        void UpdateAllAndDrawOffScreen(const MapDisplay & MAP_DISPLAY);
        void MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V);

    protected:
        virtual void ResetBase(const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V);

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
        ThreeStageImageManagerMapLayer(const std::vector<sf::Texture> & MAP_TILE_TEXTURES);
        virtual ~ThreeStageImageManagerMapLayer() = default;

        void Reset(
            const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V, const TileDrawVec_t & MAP_TILE_DRAWS);

    protected:
        void SourceToOffScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs) final;

        void SourceToOffScreen_Draw_Impl(
            const MapDisplay &,
            const IndexVertsPairVec_t & DRAW_PAIRS,
            sf::RenderTexture & renderTexture) final;

        bool OffScreenToOnScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY,
            const bool WILL_DRAW_OFFSCREEN,
            VertQuadArray_t & offScreenToOnScreenQuadVerts,
            const IndexVertsPairVec_t & DRAW_PAIRS) final;

    private:
        const std::vector<sf::Texture> & mapTileTextures_;
        TileDrawVec_t mapTileDraws_;
    };

    class ThreeStageImageManagerAvatarSprites : public ThreeStageImageManagerBase
    {
    public:
        ThreeStageImageManagerAvatarSprites();

        virtual ~ThreeStageImageManagerAvatarSprites() = default;

        void Reset(const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V)
        {
            ResetBase(OFFSCREEN_TEXTURE_SIZE_V);
        }

        std::vector<sf::Sprite> & AvatarSprites() { return avatarSprites_; }

    protected:
        void SourceToOffScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs) final;

        void SourceToOffScreen_Draw_Impl(
            const MapDisplay &,
            const IndexVertsPairVec_t & DRAW_PAIRS,
            sf::RenderTexture & renderTexture) final;

        bool OffScreenToOnScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY,
            const bool WILL_DRAW_OFFSCREEN,
            VertQuadArray_t & offScreenToOnScreenQuadVerts,
            const IndexVertsPairVec_t & DRAW_PAIRS) final;

    private:
        std::vector<sf::Sprite> avatarSprites_;

        // the avatar shadow sprite has been created so that (1) it will match the opacity of all
        // the map shadows (2) it can be drawn at the same top/left position as an avatar sprite and
        // its position will be correct (3) it can/should be scaled to match the avatar sprite's
        // scale
        gui::CachedTexture npcShadowCachedTexture_;
        sf::Sprite npcShadowSprite_;
    };

    class ThreeStageImageManagerAnimations : public ThreeStageImageManagerBase
    {
    public:
        ThreeStageImageManagerAnimations(std::vector<gui::AnimationUPtr_t> & animUPtrVec);
        virtual ~ThreeStageImageManagerAnimations() = default;

        void Reset(const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V)
        {
            ResetBase(OFFSCREEN_TEXTURE_SIZE_V);
        }

    protected:
        void SourceToOffScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs) final;

        void SourceToOffScreen_Draw_Impl(
            const MapDisplay &,
            const IndexVertsPairVec_t & DRAW_PAIRS,
            sf::RenderTexture & renderTexture) final;

        bool OffScreenToOnScreen_Update_Impl(
            const MapDisplay & MAP_DISPLAY,
            const bool WILL_DRAW_OFFSCREEN,
            VertQuadArray_t & offScreenToOnScreenQuadVerts,
            const IndexVertsPairVec_t & DRAW_PAIRS) final;

    private:
        std::vector<gui::AnimationUPtr_t> & animUPtrVec_;
    };

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_THREE_STAGE_IMAGE_MANAGERS_HPP_INCLUDED
