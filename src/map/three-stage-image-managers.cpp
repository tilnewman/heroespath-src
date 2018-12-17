// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// three-stage-image-managers.cpp
//
#include "three-stage-image-managers.hpp"

#include "gui/animation.hpp"
#include "map/map-display.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"
#include "sfutil/distance.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"

namespace heroespath
{
namespace map
{
    ThreeStageImageManagerBase::ThreeStageImageManagerBase()
        : willDrawOffScreen_(true)
        , willDrawOnScreen_(true)
        , offScreenTexture_()
        , offScreenToOnScreenQuadVerts_()
        , sourceToOffScreenDrawPairs_()
    {}

    void ThreeStageImageManagerBase::SourceToOffScreen_Update(const MapDisplay & MAP_DISPLAY)
    {
        sourceToOffScreenDrawPairs_.clear();
        SourceToOffScreen_Update_Impl(MAP_DISPLAY, sourceToOffScreenDrawPairs_);
        willDrawOffScreen_ = (sourceToOffScreenDrawPairs_.empty() == false);
    }

    void ThreeStageImageManagerBase::SourceToOffScreen_Draw(const MapDisplay & MAP_DISPLAY)
    {
        if (willDrawOffScreen_)
        {
            offScreenTexture_.clear(sf::Color::Transparent);

            SourceToOffScreen_Draw_Impl(
                MAP_DISPLAY, sourceToOffScreenDrawPairs_, offScreenTexture_);

            offScreenTexture_.display();
        }
    }

    void ThreeStageImageManagerBase::OffScreenToOnScreen_Update(const MapDisplay & MAP_DISPLAY)
    {
        willDrawOnScreen_ = OffScreenToOnScreen_Update_Impl(
            MAP_DISPLAY,
            willDrawOffScreen_,
            offScreenToOnScreenQuadVerts_,
            sourceToOffScreenDrawPairs_);
    }

    void ThreeStageImageManagerBase::OffScreenToOnScreen_Draw(
        sf::RenderTarget & target, sf::RenderStates states) const
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

    void ThreeStageImageManagerBase::UpdateAllAndDrawOffScreen(const MapDisplay & MAP_DISPLAY)
    {
        SourceToOffScreen_Update(MAP_DISPLAY);
        SourceToOffScreen_Draw(MAP_DISPLAY);
        OffScreenToOnScreen_Update(MAP_DISPLAY);
    }

    void ThreeStageImageManagerBase::MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V)
    {
        sfutil::MoveVertexTexturesForQuad(&offScreenToOnScreenQuadVerts_[0], MOVE_V);
    }

    void ThreeStageImageManagerBase::ResetBase(const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V)
    {
        willDrawOffScreen_ = true;
        willDrawOnScreen_ = true;
        sourceToOffScreenDrawPairs_.clear();

        const auto OFFSCREEN_TEXTURE_WIDTH { static_cast<unsigned>(OFFSCREEN_TEXTURE_SIZE_V.x) };
        const auto OFFSCREEN_TEXTURE_HEIGHT { static_cast<unsigned>(OFFSCREEN_TEXTURE_SIZE_V.y) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTexture_.create(OFFSCREEN_TEXTURE_WIDTH, OFFSCREEN_TEXTURE_HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << OFFSCREEN_TEXTURE_WIDTH << "x" << OFFSCREEN_TEXTURE_HEIGHT
                << ") for 'below' texture.");
    }

    ThreeStageImageManagerMapLayer::ThreeStageImageManagerMapLayer(
        const std::vector<sf::Texture> & MAP_TILE_TEXTURES)
        : ThreeStageImageManagerBase()
        , mapTileTextures_(MAP_TILE_TEXTURES)
        , mapTileDraws_()
    {}

    void ThreeStageImageManagerMapLayer::Reset(
        const sf::Vector2i & OFFSCREEN_TEXTURE_SIZE_V, const TileDrawVec_t & MAP_TILE_DRAWS)
    {
        ResetBase(OFFSCREEN_TEXTURE_SIZE_V);
        mapTileDraws_ = MAP_TILE_DRAWS;
    }

    void ThreeStageImageManagerMapLayer::SourceToOffScreen_Draw_Impl(
        const MapDisplay &,
        const IndexVertsPairVec_t & DRAW_PAIRS,
        sf::RenderTexture & renderTexture)
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

    void ThreeStageImageManagerMapLayer::SourceToOffScreen_Update_Impl(
        const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs)
    {
        const auto OFFSCREEN_TILE_RECT { MAP_DISPLAY.OffScreenTileRect() };

        std::size_t currentTextureIndex { mapTileTextures_.size() };
        for (const auto & TILE_DRAW : mapTileDraws_)
        {
            if ((TILE_DRAW.tile_index_v.x < OFFSCREEN_TILE_RECT.left)
                || (TILE_DRAW.tile_index_v.x > sfutil::Right(OFFSCREEN_TILE_RECT))
                || (TILE_DRAW.tile_index_v.y < OFFSCREEN_TILE_RECT.top)
                || (TILE_DRAW.tile_index_v.y > sfutil::Bottom(OFFSCREEN_TILE_RECT)))
            {
                continue;
            }

            if (TILE_DRAW.texture_index != currentTextureIndex)
            {
                currentTextureIndex = TILE_DRAW.texture_index;

                drawPairs.emplace_back(
                    std::make_pair(currentTextureIndex, std::vector<sf::Vertex>()));
            }

            MAP_DISPLAY.AppendVertexesForTileDrawQuad(drawPairs.back().second, TILE_DRAW);
        }
    }

    bool ThreeStageImageManagerMapLayer::OffScreenToOnScreen_Update_Impl(
        const MapDisplay & MAP_DISPLAY,
        const bool WILL_DRAW_OFFSCREEN,
        VertQuadArray_t & offScreenToOnScreenQuadVerts,
        const IndexVertsPairVec_t &)
    {
        // Below and Above always have the same single quad so no harm in setting it every time
        sfutil::SetVertexesForQuad(
            &offScreenToOnScreenQuadVerts[0],
            MAP_DISPLAY.OnScreenRect(),
            MAP_DISPLAY.OffScreenTextureRect());

        return WILL_DRAW_OFFSCREEN;
    }

    ThreeStageImageManagerAvatarSprites::ThreeStageImageManagerAvatarSprites()
        : avatarSprites_()
        , npcShadowCachedTexture_(
              "media-image-avatar-shadow",
              (gui::ImageOpt::Smooth | gui::ImageOpt::ShadowMaskForShadowImage))
        , npcShadowSprite_(npcShadowCachedTexture_.Get(), sf::IntRect(0, 0, 56, 84))
    {}

    void ThreeStageImageManagerAvatarSprites::SourceToOffScreen_Update_Impl(
        const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs)
    {
        // the map::Map class keeps the avatarSprites_ vector populated and up to date

        // populate sourceToOffScreenDrawsPairsAvatar_ with only the visible avatars, and the
        // shadows make the visible rect of each avatar bigger so be sure to set the shadow sprite's
        // scale to match each avatar and get the full size of their sprites combined
        const auto AVATAR_SPRITE_COUNT { avatarSprites_.size() };
        for (std::size_t index(0); index < AVATAR_SPRITE_COUNT; ++index)
        {
            const auto & AVATAR_SPRITE { avatarSprites_.at(index) };

            // avatar sprites.getPosition() is already in map coordinates
            const auto AVATAR_MAP_RECT { AVATAR_SPRITE.getGlobalBounds() };

            const auto AVATAR_SIZE_V { sfutil::Size(AVATAR_MAP_RECT) };

            const auto AVATAR_OFFSCREEN_POS_V { MAP_DISPLAY.OffScreenPosFromMapPos(
                sfutil::Position(AVATAR_MAP_RECT)) };

            npcShadowSprite_.setScale(AVATAR_SPRITE.getScale());
            const auto SHADOW_SIZE_V { sfutil::Size(npcShadowSprite_) };

            const sf::Vector2f COMBINED_SIZE_V(
                std::max(SHADOW_SIZE_V.x, AVATAR_SIZE_V.x),
                std::max(SHADOW_SIZE_V.y, AVATAR_SIZE_V.y));

            const sf::FloatRect COMBINED_OFFSCREEN_RECT(AVATAR_OFFSCREEN_POS_V, COMBINED_SIZE_V);

            if (MAP_DISPLAY.OffScreenTextureRect().intersects(COMBINED_OFFSCREEN_RECT))
            {
                drawPairs.emplace_back(std::make_pair(
                    index, MAP_DISPLAY.SpriteRectToVertexVec(COMBINED_OFFSCREEN_RECT)));
            }
        }

        // sort the avatar draws so that avatars in front will be drawn on top of those behind
        if (drawPairs.size() > 1)
        {
            std::sort(
                std::begin(drawPairs),
                std::end(drawPairs),
                [&](const auto PAIR_A, const auto PAIR_B) {
                    return (
                        sfutil::Bottom(avatarSprites_.at(PAIR_A.first))
                        < sfutil::Bottom(avatarSprites_.at(PAIR_B.first)));
                });
        }
    }

    void ThreeStageImageManagerAvatarSprites::SourceToOffScreen_Draw_Impl(
        const MapDisplay & MAP_DISPLAY,
        const IndexVertsPairVec_t & DRAW_PAIRS,
        sf::RenderTexture & renderTexture)
    {
        // draw all avatar shadows first
        for (const auto & INDEX_VERTEXES_PAIR : DRAW_PAIRS)
        {
            const auto & AVATAR_SPRITE { avatarSprites_.at(INDEX_VERTEXES_PAIR.first) };

            const auto MAP_POS_V { AVATAR_SPRITE.getPosition() };
            const auto OFFSCREEN_POS_V { MAP_DISPLAY.OffScreenPosFromMapPos(MAP_POS_V) };

            npcShadowSprite_.setScale(AVATAR_SPRITE.getScale());
            npcShadowSprite_.setPosition(OFFSCREEN_POS_V);

            renderTexture.draw(npcShadowSprite_);
        }

        // draw avatar sprites
        // set the sprite position to offscreen coordinates but remember to set it back after
        for (const auto & INDEX_VERTEXES_PAIR : DRAW_PAIRS)
        {
            auto & sprite { avatarSprites_.at(INDEX_VERTEXES_PAIR.first) };

            const auto MAP_POS_V { sprite.getPosition() };
            const auto OFFSCREEN_POS_V { MAP_DISPLAY.OffScreenPosFromMapPos(MAP_POS_V) };

            sprite.setPosition(OFFSCREEN_POS_V);
            renderTexture.draw(sprite);
            sprite.setPosition(MAP_POS_V);
        }
    }

    bool ThreeStageImageManagerAvatarSprites::OffScreenToOnScreen_Update_Impl(
        const MapDisplay & MAP_DISPLAY,
        const bool,
        VertQuadArray_t & offScreenToOnScreenQuadVerts,
        const IndexVertsPairVec_t & DRAW_PAIRS)
    {
        if (DRAW_PAIRS.empty())
        {
            return false;
        }

        sf::FloatRect minEnclosingOffScreenSourceRect;
        for (const auto & INDEX_VERTEXES_PAIR : DRAW_PAIRS)
        {
            const auto OFFSCREEN_RECT { MAP_DISPLAY.VertexVecToSpriteRect(
                INDEX_VERTEXES_PAIR.second) };

            if (minEnclosingOffScreenSourceRect.width > 0.0f)
            {
                minEnclosingOffScreenSourceRect
                    = sfutil::MinimallyEnclosing(minEnclosingOffScreenSourceRect, OFFSCREEN_RECT);
            }
            else
            {
                minEnclosingOffScreenSourceRect = OFFSCREEN_RECT;
            }
        }

        const auto VISIBLE_OFFSCREEN_RECT { sfutil::Intersection(
            minEnclosingOffScreenSourceRect, MAP_DISPLAY.OffScreenTextureRect()) };

        const sf::FloatRect ONSCREEN_RECT { MAP_DISPLAY.OnScreenRectFromOffScreenRect(
            VISIBLE_OFFSCREEN_RECT) };

        sfutil::SetVertexesForQuad(
            &offScreenToOnScreenQuadVerts[0], ONSCREEN_RECT, VISIBLE_OFFSCREEN_RECT);

        return true;
    }

    ThreeStageImageManagerAnimations::ThreeStageImageManagerAnimations(
        std::vector<gui::AnimationUPtr_t> & animUPtrVec)
        : animUPtrVec_(animUPtrVec)
    {}

    void ThreeStageImageManagerAnimations::SourceToOffScreen_Update_Impl(
        const MapDisplay & MAP_DISPLAY, IndexVertsPairVec_t & drawPairs)
    {
        const auto ANIM_COUNT { animUPtrVec_.size() };
        for (std::size_t index(0); index < ANIM_COUNT; ++index)
        {
            // the animation objects GetEntityRegion() is already in map coordinates
            const auto MAP_RECT_ORIG_V { animUPtrVec_.at(index)->GetEntityRegion() };
            const auto OFFSCREEN_RECT { MAP_DISPLAY.OffScreenRectFromMapRect(MAP_RECT_ORIG_V) };

            if (MAP_DISPLAY.OffScreenTextureRect().intersects(OFFSCREEN_RECT))
            {
                drawPairs.emplace_back(
                    std::make_pair(index, MAP_DISPLAY.SpriteRectToVertexVec(OFFSCREEN_RECT)));
            }
        }
    }

    void ThreeStageImageManagerAnimations::SourceToOffScreen_Draw_Impl(
        const MapDisplay & MAP_DISPLAY,
        const IndexVertsPairVec_t & DRAW_PAIRS,
        sf::RenderTexture & renderTexture)
    {
        for (const auto & INDEX_VERTEXES_PAIR : DRAW_PAIRS)
        {
            auto & anim { *animUPtrVec_.at(INDEX_VERTEXES_PAIR.first) };

            const auto MAP_POS_V { anim.GetEntityPos() };
            const auto OFFSCREEN_POS_V { MAP_DISPLAY.OffScreenPosFromMapPos(MAP_POS_V) };

            anim.SetEntityPos(OFFSCREEN_POS_V);
            renderTexture.draw(anim);
            anim.SetEntityPos(MAP_POS_V);
        }
    }

    bool ThreeStageImageManagerAnimations::OffScreenToOnScreen_Update_Impl(
        const MapDisplay & MAP_DISPLAY,
        const bool,
        VertQuadArray_t & offScreenToOnScreenQuadVerts,
        const IndexVertsPairVec_t & DRAW_PAIRS)
    {
        if (DRAW_PAIRS.empty())
        {
            return false;
        }

        sf::FloatRect minEnclosingOffScreenSourceRect;
        for (const auto & INDEX_VERTEXES_PAIR : DRAW_PAIRS)
        {
            const auto OFFSCREEN_RECT { MAP_DISPLAY.VertexVecToSpriteRect(
                INDEX_VERTEXES_PAIR.second) };

            if (minEnclosingOffScreenSourceRect.width > 0.0f)
            {
                minEnclosingOffScreenSourceRect
                    = sfutil::MinimallyEnclosing(minEnclosingOffScreenSourceRect, OFFSCREEN_RECT);
            }
            else
            {
                minEnclosingOffScreenSourceRect = OFFSCREEN_RECT;
            }
        }

        const auto VISIBLE_OFFSCREEN_RECT { sfutil::Intersection(
            minEnclosingOffScreenSourceRect, MAP_DISPLAY.OffScreenTextureRect()) };

        const sf::FloatRect ONSCREEN_RECT { MAP_DISPLAY.OnScreenRectFromOffScreenRect(
            VISIBLE_OFFSCREEN_RECT) };

        sfutil::SetVertexesForQuad(
            &offScreenToOnScreenQuadVerts[0], ONSCREEN_RECT, VISIBLE_OFFSCREEN_RECT);

        return true;
    }

} // namespace map
} // namespace heroespath
