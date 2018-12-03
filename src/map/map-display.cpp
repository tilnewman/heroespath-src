// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// map-display.cpp
//
#include "map-display.hpp"

#include "avatar/lpc-view.hpp"
#include "gui/margins.hpp"
#include "gui/sound-manager.hpp"
#include "map/layer.hpp"
#include "map/layout.hpp"
#include "map/map-tile-optimizer.hpp"
#include "map/map.hpp"
#include "map/parser.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

#include <algorithm>
#include <exception>
#include <set>
#include <sstream>

namespace heroespath
{
namespace map
{

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
        const IndexVertsPairVec_t & DRAW_PAIRS)
    {
        // Below and Above always have the same single quad so no harm in setting it every time
        sfutil::SetVertexesForQuad(
            &offScreenToOnScreenQuadVerts[0],
            MAP_DISPLAY.OnScreenRect(),
            MAP_DISPLAY.OffScreenTextureRect());

        return WILL_DRAW_OFFSCREEN;
    }

    MapDisplay::MapDisplay(const sf::FloatRect & REGION)
        : onScreenRect_(REGION)
        , onScreenRectInner_(
              gui::Margins(sfutil::ScaleCopy(sfutil::Size(REGION), ONSCREEN_WALK_PAD_RATIO_))
                  .ApplyShrinkCopy(REGION))
        , offScreenRectI_()
        , offScreenTileRectI_()
        , offScreenTextureRect_()
        , offScreenMapOffsetPosV_()
        , mapRectI_()
        , mapTileRectI_()
        , tileSizeVI_()
        , ANIM_SFX_DISTANCE_MIN_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-distance-min"))
        , ANIM_SFX_DISTANCE_MAX_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-distance-max"))
        , ANIM_SFX_VOLUME_MIN_RATIO_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-min-volume-ratio"))
        , sourceToOffScreenDrawsPairsAvatar_()
        , sourceToOffScreenDrawsPairsAnim_()
        , offScreenImageAvatar_()
        , offScreenImageAnim_()
        , offScreenToOnScreenVertsAvatar_()
        , offScreenToOnScreenVertsAnim_()
        , willDrawOffScreenAvatar_(true)
        , willDrawOffScreenAnim_(true)
        , willDrawOnScreenAvatar_(true)
        , willDrawOnScreenAnim_(true)
        , playerPosV_(0.0f, 0.0f)
        , playerPosOffsetV_(0.0f, 0.0f)
        , animInfoVec_()
        , animUPtrVec_()
        , avatarSprites_()
        , npcShadowCachedTexture_(
              "media-images-avatar-shadow",
              (gui::ImageOpt::Smooth | gui::ImageOpt::ShadowMaskForShadowImage))
        , npcShadowSprite_(npcShadowCachedTexture_.Get(), sf::IntRect(0, 0, 56, 84))
        , mapTileTextures_()
        , imageManagerMapBelow_(mapTileTextures_)
        , imageManagerMapAbove_(mapTileTextures_)
    {}

    MapDisplay::~MapDisplay() { StopAnimMusic(); }

    void MapDisplay::Load(
        const map::Layout & MAP_LAYOUT,
        const sf::Vector2f & PLAYER_STARTING_POS_V,
        const MapAnimVec_t & ANIM_INFO_VEC)
    {
        mapTileTextures_.clear();

        // set player position in map coordinates
        playerPosOffsetV_ = sf::Vector2f(0.0f, 0.0f);
        playerPosV_ = PLAYER_STARTING_POS_V;

        // tile size
        tileSizeVI_ = MAP_LAYOUT.tile_size_v;

        // set map coordinates
        mapRectI_
            = sf::IntRect(sf::Vector2i(), sfutil::VectorMult(MAP_LAYOUT.tile_count_v, tileSizeVI_));

        mapTileRectI_ = sf::IntRect(sf::Vector2i(), MAP_LAYOUT.tile_count_v);

        // set offscreen coordinates
        offScreenRectI_ = CalcOffscreenRect(onScreenRect_, tileSizeVI_);

        offScreenTileRectI_ = CalcOffscreenTileSubRectForMapPos(
            offScreenRectI_, PlayerPosMap(), sfutil::Size(mapTileRectI_));

        UpdateOffScreenMapPosOffset();

        SetupOffScreenTextures();

        // set the initial position of what is drawn on-screen from the off-screen texture
        offScreenTextureRect_.left
            = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeVI_.x);

        offScreenTextureRect_.top = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeVI_.y);
        offScreenTextureRect_.width = onScreenRect_.width;
        offScreenTextureRect_.height = onScreenRect_.height;

        StopAnimMusic();

        animInfoVec_ = ANIM_INFO_VEC;
        SetupAnimations();

        StartAnimMusic();
        UpdateAnimMusicVolume();

        TileDrawVec_t mapTileDrawsBelow;
        TileDrawVec_t mapTileDrawsAbove;
        MapTileOptimizer::Packet optimizerPacket(
            MAP_LAYOUT,
            sfutil::Size(mapTileRectI_),
            tileSizeVI_,
            mapTileTextures_,
            mapTileDrawsBelow,
            mapTileDrawsAbove);

        MapTileOptimizer::Optimize(optimizerPacket);

        imageManagerMapBelow_.Reset(sfutil::Size(offScreenRectI_), mapTileDrawsBelow);
        imageManagerMapAbove_.Reset(sfutil::Size(offScreenRectI_), mapTileDrawsAbove);

        UpdateAndReDrawAfterMapTileChange();
    }

    bool MapDisplay::Move(const gui::Direction::Enum DIRECTION, const float ADJUSTMENT)
    {
        switch (DIRECTION)
        {
            case gui::Direction::Up:
            {
                return MoveUp(ADJUSTMENT);
            }
            case gui::Direction::Down:
            {
                return MoveDown(ADJUSTMENT);
            }
            case gui::Direction::Left:
            {
                return MoveLeft(ADJUSTMENT);
            }
            case gui::Direction::Right:
            {
                return MoveRight(ADJUSTMENT);
            }
            case gui::Direction::Count:
            default:
            {
                return false;
            }
        }
    }

    void MapDisplay::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        imageManagerMapBelow_.OffScreenToOnScreen_Draw(target, states);
        OffScreenToOnScreen_Draw_Avatars(target, states);
        imageManagerMapAbove_.OffScreenToOnScreen_Draw(target, states);
        OffScreenToOnScreen_Draw_Animations(target, states);
    }

    void MapDisplay::Update(const float TIME_ELAPSED)
    {
        if (UpdateAnimations(TIME_ELAPSED))
        {
            SourceToOffScreen_Update_Animations();
            SourceToOffScreen_Draw_Animations();
            OffScreenToOnScreen_Update_Animations();
        }

        SourceToOffScreen_Update_Avatars();
        SourceToOffScreen_Draw_Avatars();
        OffScreenToOnScreen_Update_Avatars();
    }

    void MapDisplay::UpdateAnimMusicVolume()
    {
        for (const auto & ANIM_INFO : animInfoVec_)
        {
            if (ANIM_INFO.music_vec.empty() == false)
            {
                const auto ANIM_POS_V { sf::Vector2f(
                    ANIM_INFO.rect.left + (ANIM_INFO.rect.width * 0.5f),
                    ANIM_INFO.rect.top + (ANIM_INFO.rect.height * 0.5f)) };

                const auto DISTANCE_TO_PLAYER { sfutil::Distance(ANIM_POS_V, PlayerPosMap()) };

                gui::SoundManager::Instance()->MusicVolume(
                    ANIM_INFO.music_vec, CalcAnimationVolume(DISTANCE_TO_PLAYER));
            }
        }
    }

    bool MapDisplay::UpdateAnimations(const float TIME_ELAPSED)
    {
        auto didAnimationChange { false };
        for (auto & animUPtr : animUPtrVec_)
        {
            const auto FRAME_BEFORE { animUPtr->CurrentFrame() };
            animUPtr->UpdateTime(TIME_ELAPSED);
            const auto FRAME_AFTER { animUPtr->CurrentFrame() };

            if (FRAME_BEFORE != FRAME_AFTER)
            {
                didAnimationChange = true;
            }
        }

        return didAnimationChange;
    }

    const sf::Vector2f MapDisplay::MapSize() const
    {
        return sfutil::Size(sf::FloatRect(mapRectI_));
    }

    void MapDisplay::UpdateOffScreenMapPosOffset()
    {
        offScreenMapOffsetPosV_ = sf::Vector2f { sfutil::VectorMult(
            sfutil::Position(offScreenTileRectI_), tileSizeVI_) };
    }

    bool MapDisplay::MoveUp(const float DISTANCE)
    {
        // player is not pushing on the edges of the map
        // player will appear to move on screen but the surrounding map will hold still
        if (PlayerPosScreen().y > onScreenRectInner_.top)
        {
            playerPosOffsetV_.y -= DISTANCE;

            if (PlayerPosScreen().y < onScreenRectInner_.top)
            {
                playerPosOffsetV_.y += (onScreenRectInner_.top - PlayerPosScreen().y);
            }

            return true;
        }

        // player pushing/moving the map
        // player will appear to hold still while the surrounding map moves

        // move the offScreenTextureRect_ only
        if (((offScreenTextureRect_.top - DISTANCE) < 0.0f) == false)
        {
            playerPosV_.y -= DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(0.0f, -DISTANCE));
            return true;
        }

        // move the offScreenTextureRect_ and change which tiles are drawn
        if (offScreenTileRectI_.top > 0)
        {
            playerPosV_.y -= DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(0.0f, -DISTANCE));

            --offScreenTileRectI_.top;
            UpdateOffScreenMapPosOffset();
            MoveOffscreenTextureRects(sf::Vector2f(0.0f, tileSizeVI_.y));
            UpdateAndReDrawAfterMapTileChange();
            return true;
        }

        return false;
    }

    bool MapDisplay::MoveDown(const float DISTANCE)
    {
        // player is not pushing on the edges of the map
        // player will appear to move on screen but the surrounding map will hold still
        if (PlayerPosScreen().y < sfutil::Bottom(onScreenRectInner_))
        {
            playerPosOffsetV_.y += DISTANCE;

            if (PlayerPosScreen().y > sfutil::Bottom(onScreenRectInner_))
            {
                playerPosOffsetV_.y -= (PlayerPosScreen().y - sfutil::Bottom(onScreenRectInner_));
            }

            return true;
        }

        // player pushing/moving the map
        // player will appear to hold still while the surrounding map moves

        // move the offScreenTextureRect_ only
        if ((sfutil::Bottom(offScreenTextureRect_) + DISTANCE) > sfutil::Bottom(offScreenRectI_)
            == false)
        {
            playerPosV_.y += DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(0.0f, DISTANCE));
            return true;
        }

        // move the offScreenTextureRect_ and change which tiles are drawn
        if (sfutil::Bottom(offScreenTileRectI_) < sfutil::Bottom(mapTileRectI_))
        {
            playerPosV_.y += DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(0.0f, DISTANCE));

            ++offScreenTileRectI_.top;
            UpdateOffScreenMapPosOffset();
            MoveOffscreenTextureRects(sf::Vector2f(0.0f, -tileSizeVI_.y));
            UpdateAndReDrawAfterMapTileChange();
            return true;
        }

        return false;
    }

    bool MapDisplay::MoveLeft(const float DISTANCE)
    {
        // player is not pushing on the edges of the map
        // player will appear to move on screen but the surrounding map will hold still
        if (PlayerPosScreen().x > onScreenRectInner_.left)
        {
            playerPosOffsetV_.x -= DISTANCE;

            if (PlayerPosScreen().x < onScreenRectInner_.left)
            {
                playerPosOffsetV_.x += (onScreenRectInner_.left - PlayerPosScreen().x);
            }

            return true;
        }

        // player pushing/moving the map
        // player will appear to hold still while the surrounding map moves

        // move the offScreenTextureRect_ only
        if (((offScreenTextureRect_.left - DISTANCE) < 0.0f) == false)
        {
            playerPosV_.x -= DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(-DISTANCE, 0.0f));
            return true;
        }

        // move the offScreenTextureRect_ and change which tiles are drawn
        if (offScreenTileRectI_.left > 0)
        {
            playerPosV_.x -= DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(-DISTANCE, 0.0f));

            --offScreenTileRectI_.left;
            UpdateOffScreenMapPosOffset();
            MoveOffscreenTextureRects(sf::Vector2f(tileSizeVI_.x, 0.0f));
            UpdateAndReDrawAfterMapTileChange();
            return true;
        }

        return false;
    }

    bool MapDisplay::MoveRight(const float DISTANCE)
    {
        // player is not pushing on the edges of the map
        // player will appear to move on screen but the surrounding map will hold still
        if (PlayerPosScreen().x < sfutil::Right(onScreenRectInner_))
        {
            playerPosOffsetV_.x += DISTANCE;

            if (PlayerPosScreen().x > sfutil::Right(onScreenRectInner_))
            {
                playerPosOffsetV_.x += (PlayerPosScreen().x - sfutil::Right(onScreenRectInner_));
            }

            return true;
        }

        // player pushing/moving the map
        // player will appear to hold still while the surrounding map moves

        // move the offScreenTextureRect_ only
        if ((sfutil::Right(offScreenTextureRect_) + DISTANCE) > sfutil::Right(offScreenRectI_)
            == false)
        {
            playerPosV_.x += DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(DISTANCE, 0.0f));
            return true;
        }

        // move the offScreenTextureRect_ and change which tiles are drawn
        if (sfutil::Right(offScreenTileRectI_) < sfutil::Right(mapTileRectI_))
        {
            playerPosV_.x += DISTANCE;
            MoveOffscreenTextureRects(sf::Vector2f(DISTANCE, 0.0f));

            ++offScreenTileRectI_.left;
            UpdateOffScreenMapPosOffset();
            MoveOffscreenTextureRects(sf::Vector2f(-tileSizeVI_.x, 0.0f));
            UpdateAndReDrawAfterMapTileChange();
            return true;
        }

        return false;
    }

    void MapDisplay::SourceToOffScreen_Update_Avatars()
    {
        // the map::Map class keeps the avatarSprites_ vector populated and up to date

        sourceToOffScreenDrawsPairsAvatar_.clear();

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

            const auto AVATAR_OFFSCREEN_POS_V { OffScreenPosFromMapPos(
                sfutil::Position(AVATAR_MAP_RECT)) };

            npcShadowSprite_.setScale(AVATAR_SPRITE.getScale());
            const auto SHADOW_SIZE_V { sfutil::Size(npcShadowSprite_) };

            const sf::Vector2f COMBINED_SIZE_V(
                std::max(SHADOW_SIZE_V.x, AVATAR_SIZE_V.x),
                std::max(SHADOW_SIZE_V.y, AVATAR_SIZE_V.y));

            const sf::FloatRect COMBINED_OFFSCREEN_RECT(AVATAR_OFFSCREEN_POS_V, COMBINED_SIZE_V);

            if (offScreenTextureRect_.intersects(COMBINED_OFFSCREEN_RECT))
            {
                sourceToOffScreenDrawsPairsAvatar_.emplace_back(
                    std::make_pair(index, SpriteRectToVertexVec(COMBINED_OFFSCREEN_RECT)));
            }
        }

        // sort the avatar draws so that avatars in front will be drawn on top of those behind
        if (sourceToOffScreenDrawsPairsAvatar_.size() > 1)
        {
            std::sort(
                std::begin(sourceToOffScreenDrawsPairsAvatar_),
                std::end(sourceToOffScreenDrawsPairsAvatar_),
                [&](const auto PAIR_A, const auto PAIR_B) {
                    return (
                        sfutil::Bottom(avatarSprites_.at(PAIR_A.first))
                        < sfutil::Bottom(avatarSprites_.at(PAIR_B.first)));
                });
        }

        willDrawOffScreenAvatar_ = (sourceToOffScreenDrawsPairsAvatar_.empty() == false);
    }

    void MapDisplay::SourceToOffScreen_Update_Animations()
    {
        sourceToOffScreenDrawsPairsAnim_.clear();

        const auto ANIM_COUNT { animUPtrVec_.size() };
        for (std::size_t index(0); index < ANIM_COUNT; ++index)
        {
            // the animation objects GetEntityRegion() is already in map coordinates
            const auto MAP_RECT_ORIG_V { animUPtrVec_.at(index)->GetEntityRegion() };
            const auto OFFSCREEN_RECT { OffScreenRectFromMapRect(MAP_RECT_ORIG_V) };

            if (offScreenTextureRect_.intersects(OFFSCREEN_RECT))
            {
                sourceToOffScreenDrawsPairsAnim_.emplace_back(
                    std::make_pair(index, SpriteRectToVertexVec(OFFSCREEN_RECT)));
            }
        }

        willDrawOffScreenAnim_ = (sourceToOffScreenDrawsPairsAnim_.empty() == false);
    }

    void MapDisplay::SourceToOffScreen_Draw_Avatars()
    {
        if (false == willDrawOffScreenAvatar_)
        {
            return;
        }

        offScreenImageAvatar_.clear(sf::Color::Transparent);

        // draw all avatar shadows first
        for (const auto & INDEX_VERTEXES_PAIR : sourceToOffScreenDrawsPairsAvatar_)
        {
            const auto & AVATAR_SPRITE { avatarSprites_.at(INDEX_VERTEXES_PAIR.first) };

            const auto MAP_POS_V { AVATAR_SPRITE.getPosition() };
            const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(MAP_POS_V) };

            npcShadowSprite_.setScale(AVATAR_SPRITE.getScale());
            npcShadowSprite_.setPosition(OFFSCREEN_POS_V);

            offScreenImageAvatar_.draw(npcShadowSprite_);
        }

        // draw avatar sprites
        // set the sprite position to offscreen coordinates but remember to set it back after
        for (const auto & INDEX_VERTEXES_PAIR : sourceToOffScreenDrawsPairsAvatar_)
        {
            auto & sprite { avatarSprites_.at(INDEX_VERTEXES_PAIR.first) };

            const auto MAP_POS_V { sprite.getPosition() };
            const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(MAP_POS_V) };

            sprite.setPosition(OFFSCREEN_POS_V);
            offScreenImageAvatar_.draw(sprite);
            sprite.setPosition(MAP_POS_V);
        }

        offScreenImageAvatar_.display();
    }

    void MapDisplay::SourceToOffScreen_Draw_Animations()
    {
        if (false == willDrawOffScreenAnim_)
        {
            return;
        }

        offScreenImageAnim_.clear(sf::Color::Transparent);

        for (const auto & INDEX_VERTEXES_PAIR : sourceToOffScreenDrawsPairsAnim_)
        {
            auto & anim { *animUPtrVec_.at(INDEX_VERTEXES_PAIR.first) };

            const auto MAP_POS_V { anim.GetEntityPos() };
            const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(MAP_POS_V) };

            anim.SetEntityPos(OFFSCREEN_POS_V);
            offScreenImageAnim_.draw(anim);
            anim.SetEntityPos(MAP_POS_V);
        }

        offScreenImageAnim_.display();
    }

    void MapDisplay::OffScreenToOnScreen_Update_Avatars()
    {
        if (sourceToOffScreenDrawsPairsAvatar_.empty())
        {
            willDrawOnScreenAvatar_ = false;
        }
        else
        {
            willDrawOnScreenAvatar_ = true;

            sf::FloatRect minEnclosingOffScreenSourceRect;
            for (const auto & INDEX_VERTEXES_PAIR : sourceToOffScreenDrawsPairsAvatar_)
            {
                const auto OFFSCREEN_RECT { VertexVecToSpriteRect(INDEX_VERTEXES_PAIR.second) };

                if (minEnclosingOffScreenSourceRect.width > 0.0f)
                {
                    minEnclosingOffScreenSourceRect = sfutil::MinimallyEnclosing(
                        minEnclosingOffScreenSourceRect, OFFSCREEN_RECT);
                }
                else
                {
                    minEnclosingOffScreenSourceRect = OFFSCREEN_RECT;
                }
            }

            const auto VISIBLE_OFFSCREEN_RECT { sfutil::Intersection(
                minEnclosingOffScreenSourceRect, offScreenTextureRect_) };

            const sf::FloatRect ONSCREEN_RECT { OnScreenRectFromOffScreenRect(
                VISIBLE_OFFSCREEN_RECT) };

            sfutil::SetVertexesForQuad(
                &offScreenToOnScreenVertsAvatar_[0], ONSCREEN_RECT, VISIBLE_OFFSCREEN_RECT);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Update_Animations()
    {
        if (sourceToOffScreenDrawsPairsAnim_.empty())
        {
            willDrawOnScreenAnim_ = false;
        }
        else
        {
            willDrawOnScreenAnim_ = true;

            sf::FloatRect minEnclosingOffScreenSourceRect;
            for (const auto & INDEX_VERTEXES_PAIR : sourceToOffScreenDrawsPairsAnim_)
            {
                const auto OFFSCREEN_RECT { VertexVecToSpriteRect(INDEX_VERTEXES_PAIR.second) };

                if (minEnclosingOffScreenSourceRect.width > 0.0f)
                {
                    minEnclosingOffScreenSourceRect = sfutil::MinimallyEnclosing(
                        minEnclosingOffScreenSourceRect, OFFSCREEN_RECT);
                }
                else
                {
                    minEnclosingOffScreenSourceRect = OFFSCREEN_RECT;
                }
            }

            const auto VISIBLE_OFFSCREEN_RECT { sfutil::Intersection(
                minEnclosingOffScreenSourceRect, offScreenTextureRect_) };

            const sf::FloatRect ONSCREEN_RECT { OnScreenRectFromOffScreenRect(
                VISIBLE_OFFSCREEN_RECT) };

            sfutil::SetVertexesForQuad(
                &offScreenToOnScreenVertsAnim_[0], ONSCREEN_RECT, VISIBLE_OFFSCREEN_RECT);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw_Avatars(
        sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (willDrawOnScreenAvatar_)
        {
            OffScreenToOnScreen_Draw(
                offScreenImageAvatar_,
                &offScreenToOnScreenVertsAvatar_[0],
                offScreenToOnScreenVertsAvatar_.size(),
                target,
                states);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw_Animations(
        sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (willDrawOnScreenAnim_)
        {
            OffScreenToOnScreen_Draw(
                offScreenImageAnim_,
                &offScreenToOnScreenVertsAnim_[0],
                offScreenToOnScreenVertsAnim_.size(),
                target,
                states);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw(
        const sf::RenderTexture & RENDER_TEXTURE,
        const sf::Vertex VERTEXES[],
        const std::size_t VERTEX_COUNT,
        sf::RenderTarget & target,
        sf::RenderStates states) const
    {
        if (VERTEX_COUNT > 0)
        {
            states.texture = &RENDER_TEXTURE.getTexture();
            target.draw(&VERTEXES[0], VERTEX_COUNT, sf::Quads, states);
        }
    }

    void MapDisplay::UpdateAndReDrawAfterMapTileChange()
    {
        imageManagerMapBelow_.UpdateAllAndDrawOffScreen(*this);
        imageManagerMapAbove_.UpdateAllAndDrawOffScreen(*this);
    }

    void MapDisplay::MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V)
    {
        sfutil::Move(offScreenTextureRect_, MOVE_V);

        imageManagerMapBelow_.MoveOffscreenTextureRects(MOVE_V);
        imageManagerMapAbove_.MoveOffscreenTextureRects(MOVE_V);

        sfutil::MoveVertexTexturesForQuad(&offScreenToOnScreenVertsAvatar_[0], MOVE_V);
        sfutil::MoveVertexTexturesForQuad(&offScreenToOnScreenVertsAnim_[0], MOVE_V);
    }

    void MapDisplay::SetupOffScreenTextures()
    {
        const auto WIDTH { static_cast<unsigned>(offScreenRectI_.width) };
        const auto HEIGHT { static_cast<unsigned>(offScreenRectI_.height) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAnim_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'anim' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAvatar_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'avatar' texture.");
    }

    const sf::IntRect MapDisplay::CalcOffscreenRect(
        const sf::FloatRect & ONSCREEN_RECT, const sf::Vector2i & TILE_SIZE_V) const
    {
        const sf::Vector2i ONSCREEN_SIZE_IN_TILES_V(
            ((static_cast<int>(ONSCREEN_RECT.width) / TILE_SIZE_V.x) + 1),
            ((static_cast<int>(ONSCREEN_RECT.height) / TILE_SIZE_V.y) + 1));

        const auto OFFSCREEN_EXTRA_SIZE_V { (TILE_SIZE_V * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2)) };

        const auto OFFSCREEN_SIZE_V { sfutil::VectorMult(ONSCREEN_SIZE_IN_TILES_V, TILE_SIZE_V)
                                      + OFFSCREEN_EXTRA_SIZE_V };

        return sf::IntRect(sf::Vector2i(), OFFSCREEN_SIZE_V);
    }

    const sf::IntRect MapDisplay::CalcOffscreenTileSubRectForMapPos(
        const sf::IntRect & OFFSCREEN_RECT,
        const sf::Vector2f & MAP_POS_V,
        const sf::Vector2i & MAP_SIZE_IN_TILES_V) const
    {
        sf::IntRect tileRect;
        tileRect.width = std::min((OFFSCREEN_RECT.width / tileSizeVI_.x), MAP_SIZE_IN_TILES_V.x);
        tileRect.height = std::min((OFFSCREEN_RECT.height / tileSizeVI_.y), MAP_SIZE_IN_TILES_V.y);

        {
            const auto PLAYER_MAP_TILE_POS_X { static_cast<int>(MAP_POS_V.x) / tileSizeVI_.x };
            tileRect.left = std::max(0, (PLAYER_MAP_TILE_POS_X - (tileRect.width / 2)));

            if (sfutil::Right(tileRect) > MAP_SIZE_IN_TILES_V.x)
            {
                tileRect.left = std::max(0, (MAP_SIZE_IN_TILES_V.x - tileRect.width));
            }
        }

        {
            const auto PLAYER_MAP_TILE_POS_Y { static_cast<int>(MAP_POS_V.y) / tileSizeVI_.y };
            tileRect.top = std::max(0, (PLAYER_MAP_TILE_POS_Y - (tileRect.height / 2)));

            if (sfutil::Bottom(tileRect) > MAP_SIZE_IN_TILES_V.y)
            {
                tileRect.top = std::max(0, (MAP_SIZE_IN_TILES_V.y - tileRect.height));
            }
        }

        return tileRect;
    }

    const sf::Vector2f MapDisplay::OnScreenPosFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        return (
            (sfutil::Position(onScreenRect_) + OffScreenPosFromMapPos(MAP_POS_V))
            - sfutil::Position(offScreenTextureRect_));
    }

    const sf::FloatRect MapDisplay::OnScreenRectFromMapRect(const sf::FloatRect & MAP_RECT) const
    {
        return sf::FloatRect(
            OnScreenPosFromMapPos(sfutil::Position(MAP_RECT)), sfutil::Size(MAP_RECT));
    }

    const sf::Vector2f
        MapDisplay::OnScreenPosFromOffScreenPos(const sf::Vector2f & OFFSCREEN_POS_V) const
    {
        return (
            (sfutil::Position(onScreenRect_) + OFFSCREEN_POS_V)
            - sfutil::Position(offScreenTextureRect_));
    }

    const sf::FloatRect
        MapDisplay::OnScreenRectFromOffScreenRect(const sf::FloatRect & OFFSCREEN_RECT) const
    {
        return sf::FloatRect(
            OnScreenPosFromOffScreenPos(sfutil::Position(OFFSCREEN_RECT)),
            sfutil::Size(OFFSCREEN_RECT));
    }

    const sf::Vector2f MapDisplay::OffScreenPosFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        return (MAP_POS_V - offScreenMapOffsetPosV_);
    }

    const sf::FloatRect MapDisplay::OffScreenRectFromMapRect(const sf::FloatRect & MAP_RECT) const
    {
        return sf::FloatRect(
            OffScreenPosFromMapPos(sfutil::TopLeft(MAP_RECT)), sfutil::Size(MAP_RECT));
    }

    void MapDisplay::SetupAnimations()
    {
        animUPtrVec_.clear();

        for (const auto & ANIM_INFO : animInfoVec_)
        {
            animUPtrVec_.emplace_back(gui::AnimationFactory::Make(
                ANIM_INFO.which_anim,
                ANIM_INFO.rect,
                gui::Animations::TimePerFrameSec(ANIM_INFO.which_anim)));

            animUPtrVec_.back()->RandomVaryTimePerFrame();
        }
    }

    void MapDisplay::StartAnimMusic()
    {
        for (const auto & ANIM_INFO : animInfoVec_)
        {
            if (ANIM_INFO.music_vec.empty() == false)
            {
                gui::SoundManager::Instance()->MusicStart(ANIM_INFO.music_vec);
            }
        }
    }

    void MapDisplay::StopAnimMusic()
    {
        for (const auto & ANIM_INFO : animInfoVec_)
        {
            if (ANIM_INFO.music_vec.empty() == false)
            {
                gui::SoundManager::Instance()->MusicStop(ANIM_INFO.music_vec);
            }
        }
    }

    float MapDisplay::CalcAnimationVolume(const float DISTANCE_TO_PLAYER) const
    {
        const auto DIFF_DISTANCE { ANIM_SFX_DISTANCE_MAX_ - ANIM_SFX_DISTANCE_MIN_ };

        // Sounds coming from animations are gui::music and not gui::sound_effects,
        // but they are governed by the SoundEffectsVolume and not the MusicVolume.
        const auto MAX_VOLUME { gui::SoundManager::Instance()->SoundEffectVolume() };
        const auto MIN_VOLUME { MAX_VOLUME * ANIM_SFX_VOLUME_MIN_RATIO_ };
        const auto DIFF_VOLUME { MAX_VOLUME - MIN_VOLUME };

        if (DISTANCE_TO_PLAYER > ANIM_SFX_DISTANCE_MAX_)
        {
            return MIN_VOLUME;
        }
        else if (DISTANCE_TO_PLAYER < ANIM_SFX_DISTANCE_MIN_)
        {
            return MAX_VOLUME;
        }
        else
        {
            return MIN_VOLUME + ((1.0f - (DISTANCE_TO_PLAYER / DIFF_DISTANCE)) * DIFF_VOLUME);
        }
    }

    void MapDisplay::AppendVertexesForTileDrawQuad(
        std::vector<sf::Vertex> & vertexes, const TileDraw & TILE_DRAW) const
    {
        const auto MAP_POS_V { sfutil::VectorMult(TILE_DRAW.tile_index_v, tileSizeVI_) };
        const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(sf::Vector2f { MAP_POS_V }) };
        const sf::FloatRect OFFSCREEN_RECT(OFFSCREEN_POS_V, sf::Vector2f { tileSizeVI_ });
        const sf::FloatRect TEXTURE_RECT { TILE_DRAW.texture_rect };
        sfutil::AppendVertexesForQuad(vertexes, OFFSCREEN_RECT, TEXTURE_RECT);
    }

} // namespace map
} // namespace heroespath
