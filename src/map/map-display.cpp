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
#include "map/map.hpp"
#include "map/parser.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"
#include "sfutil/distance.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"

#include <algorithm>
#include <exception>
#include <set>
#include <sstream>

namespace heroespath
{
namespace map
{

    const std::array<sf::Vertex, MapDisplay::VERTS_PER_QUAD_> MapDisplay::EMPTY_QUAD_VERT_ARRAY_ {
        sf::Vertex(), sf::Vertex(), sf::Vertex(), sf::Vertex()
    };

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
        , sourceToOffScreenDrawsPairsBelow_()
        , sourceToOffScreenDrawPairsAvatar()
        , sourceToOffScreenDrawsPairsAbove_()
        , sourceToOffScreenDrawPairsAnim_()
        , offScreenImageBelow_()
        , offScreenImageAvatar_()
        , offScreenImageAbove_()
        , offScreenImageAnim_()
        , offScreenToOnScreenVertsBelow_()
        , offScreenToOnScreenVertsAvatar_()
        , offScreenToOnScreenVertsAbove_()
        , offScreenToOnScreenVertsAnim_()
        , playerPosV_(0.0f, 0.0f)
        , playerPosOffsetV_(0.0f, 0.0f)
        , animInfoVec_()
        , animUPtrVec_()
        , avatarSprites_()
        , npcShadowCachedTexture_(
              "media-images-avatar-shadow",
              (gui::ImageOpt::Smooth | gui::ImageOpt::ShadowMaskForShadowImage))
        , npcShadowSprite_(npcShadowCachedTexture_.Get())
        , mapTileTexturesOrig_()
        , mapTileTexturesNew_()
        , tileDrawsBelow_()
        , tileDrawsAbove_()
    {
        avatarSprites_.reserve(64);
        sourceToOffScreenDrawPairsAvatar.reserve(32);
        sourceToOffScreenDrawPairsAnim_.reserve(32);
        tileDrawsBelow_.reserve(20000);
        tileDrawsAbove_.reserve(20000);
        sourceToOffScreenDrawsPairsBelow_.reserve(1024);
        sourceToOffScreenDrawPairsAvatar.reserve(64);
        sourceToOffScreenDrawsPairsAbove_.reserve(1024);
        sourceToOffScreenDrawPairsAnim_.reserve(32);
        offScreenToOnScreenVertsBelow_.reserve(4096);
        offScreenToOnScreenVertsAvatar_.reserve(32);
        offScreenToOnScreenVertsAbove_.reserve(4096);
        offScreenToOnScreenVertsAnim_.reserve(32);
    }

    MapDisplay::~MapDisplay() { StopAnimMusic(); }

    void MapDisplay::Load(
        const map::Layout & MAP_LAYOUT,
        const sf::Vector2f & PLAYER_STARTING_POS_V,
        const MapAnimVec_t & ANIM_INFO_VEC)
    {
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

        mapTileTexturesNew_.clear();
        mapTileTexturesOrig_ = MAP_LAYOUT.texture_vec;

        PopulateTileDraws(MAP_LAYOUT);
        LogLayerAndTextureInfo();

        OptimizeTextures(tileDrawsBelow_);
        OptimizeTextures(tileDrawsAbove_);
        LogLayerAndTextureInfo();

        // TODO change this to a natural update system based on bool flags
        UpdateAndReDrawAll();
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
        OffScreenToOnScreen_Draw_MapBelow(target, states);
        OffScreenToOnScreen_Draw_Avatars(target, states);
        OffScreenToOnScreen_Draw_MapAbove(target, states);
        OffScreenToOnScreen_Draw_Animations(target, states);
    }

    void MapDisplay::Update(const float TIME_ELAPSED)
    {
        for (auto & animUPtr : animUPtrVec_)
        {
            if (misc::IsRealZero(TIME_ELAPSED) == false)
            {
                animUPtr->UpdateTime(TIME_ELAPSED);
            }
        }

        // TODO change this to a natural update system based on bool flags
        SourceToOffScreen_Update_Avatars();
        SourceToOffScreen_Draw_Avatars();
        OffScreenToOnScreen_Update_Avatars();

        SourceToOffScreen_Update_Animations();
        SourceToOffScreen_Draw_Animations();
        OffScreenToOnScreen_Update_Animations();
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

    void MapDisplay::SourceToOffScreen_Update_MapBelow()
    {
        SourceToOffScreen_Update_Map(tileDrawsBelow_, sourceToOffScreenDrawsPairsBelow_);
    }

    void MapDisplay::SourceToOffScreen_Update_Avatars()
    {
        // the map::Map class keeps the avatarSprites_ vector populated and up to date

        sourceToOffScreenDrawPairsAvatar.clear();

        // populate avatarToOffScreenDrawMap_ with sprite_indexes of only the visible sprites
        const auto AVATAR_SPRITE_COUNT { avatarSprites_.size() };
        for (std::size_t index(0); index < AVATAR_SPRITE_COUNT; ++index)
        {
            // avatar sprites.getPosition() is already in map coordinates
            const auto MAP_RECT { avatarSprites_.at(index).getGlobalBounds() };

            // this is not the actual offscreen rect, see below, this is only a temporary used to
            // find out if the avatar is visible and will be drawn
            const auto OFFSCREEN_RECT { OffScreenRectFromMapRect(MAP_RECT) };

            if (offScreenTextureRect_.intersects(OFFSCREEN_RECT))
            {
                // just need to add the index, the pos/texture coords set below
                sourceToOffScreenDrawPairsAvatar.emplace_back(
                    std::make_pair(index, EMPTY_QUAD_VERT_ARRAY_));

                // wait until the loop below to populate the shadow draws
            }
        }

        // no need to sort the shadow draws

        // sort the avatar draws so that avatars in front will be drawn on top of those behind
        if (sourceToOffScreenDrawPairsAvatar.size() > 1)
        {
            std::sort(
                std::begin(sourceToOffScreenDrawPairsAvatar),
                std::end(sourceToOffScreenDrawPairsAvatar),
                [&](const auto PAIR_A, const auto PAIR_B) {
                    return (
                        sfutil::Bottom(avatarSprites_.at(PAIR_A.first))
                        < sfutil::Bottom(avatarSprites_.at(PAIR_B.first)));
                });
        }

        // The avatar shadows and sprites are drawn to the same offscreen pos but they can have
        // different sizes so this loop calculates a combined size for each
        for (auto & drawPair : sourceToOffScreenDrawPairsAvatar)
        {
            const auto & SPRITE { avatarSprites_.at(drawPair.first) };
            const auto AVATAR_MAP_POS_V { SPRITE.getPosition() };
            const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(AVATAR_MAP_POS_V) };

            // source is a sprite so only use first vertex to hold the offscreen position
            drawPair.second[0].position = OFFSCREEN_POS_V;
        }
    }

    void MapDisplay::SourceToOffScreen_Update_MapAbove()
    {
        SourceToOffScreen_Update_Map(tileDrawsAbove_, sourceToOffScreenDrawsPairsAbove_);
    }

    void MapDisplay::SourceToOffScreen_Update_Animations()
    {
        sourceToOffScreenDrawPairsAnim_.clear();

        const auto ANIM_COUNT { animUPtrVec_.size() };
        for (std::size_t index(0); index < ANIM_COUNT; ++index)
        {
            const auto & ANIM { *animUPtrVec_.at(index) };

            // the animation objects GetEntityRegion() is already in map coordinates
            // change the pos to offscreen coordinates before drawing but then set it back
            const auto MAP_RECT_ORIG_V { ANIM.GetEntityRegion() };
            const auto OFFSCREEN_RECT { OffScreenRectFromMapRect(MAP_RECT_ORIG_V) };

            if (offScreenTextureRect_.intersects(OFFSCREEN_RECT) == false)
            {
                continue;
            }

            // since the source is a sprite we use the first vertex to hold the offscreen position
            sourceToOffScreenDrawPairsAnim_.emplace_back(
                std::make_pair(index, EMPTY_QUAD_VERT_ARRAY_));

            sourceToOffScreenDrawPairsAnim_.back().second[0].position
                = sfutil::Position(OFFSCREEN_RECT);
        }
    }

    void MapDisplay::SourceToOffScreen_Update_Map(
        const std::vector<TileDraw> & TILE_DRAWS, DrawsPairVec_t & drawsPairs)
    {
        drawsPairs.clear();

        TimeCount_t drawCounter { 0 };
        TimeCount_t transitionCounter { 0 };

        std::size_t currentTextureIndex { MapTileTextureCount() };
        for (const auto & TILE_DRAW : TILE_DRAWS)
        {
            if ((TILE_DRAW.tile_index_v.x < offScreenTileRectI_.left)
                || (TILE_DRAW.tile_index_v.x > sfutil::Right(offScreenTileRectI_))
                || (TILE_DRAW.tile_index_v.y < offScreenTileRectI_.top)
                || (TILE_DRAW.tile_index_v.y > sfutil::Bottom(offScreenTileRectI_)))
            {
                continue;
            }

            ++drawCounter;

            if (TILE_DRAW.texture_index != currentTextureIndex)
            {
                ++transitionCounter;

                currentTextureIndex = TILE_DRAW.texture_index;

                drawsPairs.emplace_back(
                    std::make_pair(currentTextureIndex, std::vector<sf::Vertex>()));
            }

            QuadAppend(drawsPairs.back().second, TILE_DRAW);
        }
    }

    void MapDisplay::SourceToOffScreen_Draw_MapBelow()
    {
        SourceToOffScreen_Draw_Map(offScreenImageBelow_, sourceToOffScreenDrawsPairsBelow_);
    }

    void MapDisplay::SourceToOffScreen_Draw_Avatars()
    {
        offScreenImageAvatar_.clear(sf::Color::Transparent);

        // draw all avatar shadows first
        for (const auto & DRAW_PAIR : sourceToOffScreenDrawPairsAvatar)
        {
            const auto & AVATAR_SPRITE { avatarSprites_.at(DRAW_PAIR.first) };
            npcShadowSprite_.setScale(AVATAR_SPRITE.getScale());
            npcShadowSprite_.setPosition(DRAW_PAIR.second[0].position);
            offScreenImageAvatar_.draw(npcShadowSprite_);
        }

        // draw avatar sprites, remember to set the position back to map coordinates after
        for (const auto & DRAW_PAIR : sourceToOffScreenDrawPairsAvatar)
        {
            auto & sprite { avatarSprites_.at(DRAW_PAIR.first) };
            const auto AVATAR_MAP_POS_V { sprite.getPosition() };
            sprite.setPosition(DRAW_PAIR.second[0].position);
            offScreenImageAvatar_.draw(sprite);
            sprite.setPosition(AVATAR_MAP_POS_V);
        }

        offScreenImageAvatar_.display();
    }

    void MapDisplay::SourceToOffScreen_Draw_MapAbove()
    {
        SourceToOffScreen_Draw_Map(offScreenImageAbove_, sourceToOffScreenDrawsPairsAbove_);
    }

    void MapDisplay::SourceToOffScreen_Draw_Animations()
    {
        offScreenImageAnim_.clear(sf::Color::Transparent);

        bool wereAnyAnimsDrawn { false };
        for (const auto & DRAW_PAIR : sourceToOffScreenDrawPairsAnim_)
        {
            auto & anim { *animUPtrVec_.at(DRAW_PAIR.first) };

            const auto MAP_RECT { anim.GetEntityRegion() };
            const auto OFFSCREEN_RECT_ORIG { OffScreenRectFromMapRect(MAP_RECT) };

            if (offScreenTextureRect_.intersects(OFFSCREEN_RECT_ORIG) == false)
            {
                continue;
            }

            wereAnyAnimsDrawn = true;
            anim.SetEntityPos(sfutil::Position(OFFSCREEN_RECT_ORIG));
            offScreenImageAnim_.draw(anim);
            anim.SetEntityPos(sfutil::Position(MAP_RECT));
        }

        if (wereAnyAnimsDrawn)
        {
            offScreenImageAnim_.display();
        }
    }

    void MapDisplay::SourceToOffScreen_Draw_Map(
        sf::RenderTexture & renderTexture, const DrawsPairVec_t & DRAWS_PAIRS)
    {
        renderTexture.clear(sf::Color::Transparent);

        sf::RenderStates renderStates { sf::RenderStates::Default };

        for (const auto & DRAWS_PAIR : DRAWS_PAIRS)
        {
            if (DRAWS_PAIR.second.empty() == false)
            {
                renderStates.texture = &MapTileTexture(DRAWS_PAIR.first);

                renderTexture.draw(
                    &DRAWS_PAIR.second[0], DRAWS_PAIR.second.size(), sf::Quads, renderStates);
            }
        }

        renderTexture.display();
    }

    void MapDisplay::OffScreenToOnScreen_Update_MapBelow()
    {
        // Below and Above always have the same single quad
        if (offScreenToOnScreenVertsBelow_.empty())
        {
            QuadAppend(offScreenToOnScreenVertsBelow_, onScreenRect_, offScreenTextureRect_);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Update_Avatars()
    {
        offScreenToOnScreenVertsAvatar_.clear();

        for (const auto & DRAW_PAIR : sourceToOffScreenDrawPairsAvatar)
        {
            const auto & SPRITE { avatarSprites_.at(DRAW_PAIR.first) };

            const auto AVATAR_MAP_POS_V { SPRITE.getPosition() };
            const auto AVATAR_SIZE_ORIG_V { sfutil::Size(SPRITE) };

            const auto OFFSCREEN_POS_ORIG_V { DRAW_PAIR.second[0].position };

            const sf::FloatRect OFFSCREEN_RECT_ORIG(OFFSCREEN_POS_ORIG_V, AVATAR_SIZE_ORIG_V);

            npcShadowSprite_.setScale(SPRITE.getScale());
            const auto SHADOW_SIZE_V { sfutil::Size(npcShadowSprite_) };

            const sf::Vector2f COMBINED_SIZE_V(
                std::max(SHADOW_SIZE_V.x, AVATAR_SIZE_ORIG_V.x),
                std::max(SHADOW_SIZE_V.y, AVATAR_SIZE_ORIG_V.y));

            const sf::FloatRect OFFSCREEN_RECT_GROWN_TO_FIT_SHADOW(
                OFFSCREEN_POS_ORIG_V, COMBINED_SIZE_V);

            const auto OFFSCREEN_RECT_INTERSECTION { sfutil::Intersection(
                OFFSCREEN_RECT_GROWN_TO_FIT_SHADOW, offScreenTextureRect_) };

            const auto INTERSECTION_POS_MOVE_V { (
                sfutil::Position(OFFSCREEN_RECT_INTERSECTION)
                - sfutil::Position(OFFSCREEN_RECT_GROWN_TO_FIT_SHADOW)) };

            const auto ONSCREEN_POS_V { OnScreenPosFromMapPos(AVATAR_MAP_POS_V)
                                        + INTERSECTION_POS_MOVE_V };

            const sf::FloatRect ONSCREEN_RECT(
                ONSCREEN_POS_V, sfutil::Size(OFFSCREEN_RECT_INTERSECTION));

            QuadAppend(offScreenToOnScreenVertsAvatar_, ONSCREEN_RECT, OFFSCREEN_RECT_INTERSECTION);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Update_MapAbove()
    {
        // Below and Above always have the same single quad
        if (offScreenToOnScreenVertsAbove_.empty())
        {
            QuadAppend(offScreenToOnScreenVertsAbove_, onScreenRect_, offScreenTextureRect_);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Update_Animations()
    {
        offScreenToOnScreenVertsAnim_.clear();

        for (const auto & DRAW_PAIR : sourceToOffScreenDrawPairsAnim_)
        {
            auto & anim { *animUPtrVec_.at(DRAW_PAIR.first) };

            const auto MAP_RECT { anim.GetEntityRegion() };

            const sf::FloatRect OFFSCREEN_RECT_ORIG(
                DRAW_PAIR.second[0].position, sfutil::Size(MAP_RECT));

            const auto OFFSCREEN_RECT_INTERSECTION { sfutil::Intersection(
                OFFSCREEN_RECT_ORIG, offScreenTextureRect_) };

            const auto INTERSECTION_POS_MOVE_V { (
                sfutil::Position(OFFSCREEN_RECT_INTERSECTION)
                - sfutil::Position(OFFSCREEN_RECT_ORIG)) };

            const auto ONSCREEN_POS_FINAL_V { OnScreenPosFromMapPos(sfutil::Position(MAP_RECT))
                                              + INTERSECTION_POS_MOVE_V };

            const sf::FloatRect ONSCREEN_RECT(
                ONSCREEN_POS_FINAL_V, sfutil::Size(OFFSCREEN_RECT_INTERSECTION));

            QuadAppend(offScreenToOnScreenVertsAnim_, ONSCREEN_RECT, OFFSCREEN_RECT_INTERSECTION);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw_MapBelow(
        sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (offScreenToOnScreenVertsBelow_.empty() == false)
        {
            OffScreenToOnScreen_Draw(
                offScreenImageBelow_,
                &offScreenToOnScreenVertsBelow_[0],
                offScreenToOnScreenVertsBelow_.size(),
                target,
                states);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw_Avatars(
        sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (offScreenToOnScreenVertsAvatar_.empty() == false)
        {
            OffScreenToOnScreen_Draw(
                offScreenImageAvatar_,
                &offScreenToOnScreenVertsAvatar_[0],
                offScreenToOnScreenVertsAvatar_.size(),
                target,
                states);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw_MapAbove(
        sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (offScreenToOnScreenVertsAbove_.empty() == false)
        {
            OffScreenToOnScreen_Draw(
                offScreenImageAbove_,
                &offScreenToOnScreenVertsAbove_[0],
                offScreenToOnScreenVertsAbove_.size(),
                target,
                states);
        }
    }

    void MapDisplay::OffScreenToOnScreen_Draw_Animations(
        sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (offScreenToOnScreenVertsAnim_.empty() == false)
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

    void MapDisplay::UpdateAndReDrawAll()
    {
        SourceToOffScreen_Update_MapBelow();
        SourceToOffScreen_Draw_MapBelow();
        OffScreenToOnScreen_Update_MapBelow();

        SourceToOffScreen_Update_Avatars();
        SourceToOffScreen_Draw_Avatars();
        OffScreenToOnScreen_Update_Avatars();

        SourceToOffScreen_Update_MapAbove();
        SourceToOffScreen_Draw_MapAbove();
        OffScreenToOnScreen_Update_MapAbove();

        SourceToOffScreen_Update_Animations();
        SourceToOffScreen_Draw_Animations();
        OffScreenToOnScreen_Update_Animations();
    }

    void MapDisplay::UpdateAndReDrawAfterMapTileChange()
    {
        SourceToOffScreen_Update_MapBelow();
        SourceToOffScreen_Draw_MapBelow();
        OffScreenToOnScreen_Update_MapBelow();

        SourceToOffScreen_Update_MapAbove();
        SourceToOffScreen_Draw_MapAbove();
        OffScreenToOnScreen_Update_MapAbove();
    }

    void MapDisplay::MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V)
    {
        sfutil::Move(offScreenTextureRect_, MOVE_V);

        MoveTextureRects(offScreenToOnScreenVertsBelow_, MOVE_V);
        MoveTextureRects(offScreenToOnScreenVertsAvatar_, MOVE_V);
        MoveTextureRects(offScreenToOnScreenVertsAbove_, MOVE_V);
        MoveTextureRects(offScreenToOnScreenVertsAnim_, MOVE_V);
    }

    void MapDisplay::SetupOffScreenTextures()
    {
        const auto WIDTH { static_cast<unsigned>(offScreenRectI_.width) };
        const auto HEIGHT { static_cast<unsigned>(offScreenRectI_.height) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageBelow_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'below' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAbove_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'above' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAnim_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'anim' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAvatar_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'avatar' texture.");
    }

    void MapDisplay::MakeAndAppendTileDraw(
        const Layer & LAYER,
        const std::size_t TEXTURE_INDEX,
        const int TEXTURE_WIDTH,
        const int TEXTURE_TILE_NUMBER,
        const sf::Vector2i & TILE_INDEXES)
    {
        const auto TEXTURE_WIDTH_IN_TILES { TEXTURE_WIDTH / tileSizeVI_.x };

        const int TEXTURE_RECT_TILE_LEFT { (TEXTURE_TILE_NUMBER % TEXTURE_WIDTH_IN_TILES) };
        const int TEXTURE_RECT_TILE_TOP { (TEXTURE_TILE_NUMBER / TEXTURE_WIDTH_IN_TILES) };

        const int TEXTURE_RECT_LEFT { (TEXTURE_RECT_TILE_LEFT * tileSizeVI_.x) };
        const int TEXTURE_RECT_TOP { (TEXTURE_RECT_TILE_TOP * tileSizeVI_.y) };

        const auto TEXTURE_RECT { sf::IntRect(
            sf::Vector2i(TEXTURE_RECT_LEFT, TEXTURE_RECT_TOP), tileSizeVI_) };

        if (LAYER.type == LayerType::Ground)
        {
            tileDrawsBelow_.emplace_back(
                TileDraw(LAYER.index, TILE_INDEXES, TEXTURE_INDEX, TEXTURE_RECT));
        }
        else
        {
            tileDrawsAbove_.emplace_back(
                TileDraw(LAYER.index, TILE_INDEXES, TEXTURE_INDEX, TEXTURE_RECT));
        }
    }

    void MapDisplay::PopulateTileDraws(const map::Layout & LAYOUT)
    {
        tileDrawsBelow_.clear();
        tileDrawsAbove_.clear();

        for (const auto & LAYER : LAYOUT.layer_vec)
        {
            for (int tileX(0); tileX < mapTileRectI_.width; ++tileX)
            {
                for (int tileY(0); tileY < mapTileRectI_.height; ++tileY)
                {
                    const auto TILE_INDEXES { sf::Vector2i(tileX, tileY) };
                    const auto LAYER_TILE_NUMBER { LAYOUT.LayerTileNumber(LAYER, TILE_INDEXES) };

                    const auto [FIRST_ID, TEXTURE_INDEX]
                        = LAYOUT.FindTileTextureInfo(LAYER_TILE_NUMBER);

                    if (TEXTURE_INDEX >= LAYOUT.texture_vec.size())
                    {
                        continue;
                    }

                    const sf::Texture & TEXTURE_REF { LAYOUT.texture_vec[TEXTURE_INDEX].Get() };
                    const auto TEXTURE_WIDTH { static_cast<int>(TEXTURE_REF.getSize().x) };
                    const auto TEXTURE_TILE_NUMBER { (LAYER_TILE_NUMBER - FIRST_ID) };

                    MakeAndAppendTileDraw(
                        LAYER, TEXTURE_INDEX, TEXTURE_WIDTH, TEXTURE_TILE_NUMBER, TILE_INDEXES);
                }
            }
        }

        // other code in this class depends on the tileDraws_ being sorted by layer/texture
        std::sort(std::begin(tileDrawsBelow_), std::end(tileDrawsBelow_));
        std::sort(std::begin(tileDrawsAbove_), std::end(tileDrawsAbove_));

        // const auto COUNT_ORIG { tileDraws_.size() };
        //
        // tileDraws_.erase(
        //    std::unique(std::begin(tileDraws_), std::end(tileDraws_)), std::end(tileDraws_));
        //
        // const auto COUNT { tileDraws_.size() };
        //
        // M_HP_LOG_WRN(
        //    "\t" << COUNT << "  TileDraws (" << (COUNT_ORIG - COUNT)
        //         << " duplicates removed) (tiles_panel_count=" << layout_.tiles_panel_vec.size()
        //         << ")(map_tile_count=" << (mapTileRectI_.width * mapTileRectI_.height)
        //         << ", layer_count=" << layout_.layer_vec.size() << ", total_tiles_count="
        //         << (static_cast<std::size_t>(mapTileRectI_.width * mapTileRectI_.height)
        //             * layout_.layer_vec.size())
        //         << ")");
        //
        // misc::VectorMap<
        //    const sf::Texture *,
        //    std::pair<std::vector<sf::IntRect>, std::vector<sf::Vector2i>>>
        //    textureMap;
        //
        // for (const auto & TILE_DRAW : tileDraws_)
        //{
        //    M_HP_ASSERT_OR_LOG_AND_THROW(
        //        ((TILE_DRAW.texture_rect.width == tileSizeVI_.x)
        //         && (TILE_DRAW.texture_rect.height == tileSizeVI_.y)),
        //        "TILE_DRAW.texture_rect=" << TILE_DRAW.texture_rect
        //                                  << " had an invalid size that should have been "
        //                                  << tileSizeVI_ << ".");
        //
        //    if (((TILE_DRAW.texture_rect.left % tileSizeVI_.x) != 0)
        //        || ((TILE_DRAW.texture_rect.top % tileSizeVI_.y) != 0))
        //    {
        //        M_HP_LOG_WRN("Invalid texture rect position=" << TILE_DRAW.texture_rect);
        //    }
        //
        //    textureMap[TILE_DRAW.texture_ptr].first.emplace_back(TILE_DRAW.texture_rect);
        //    textureMap[TILE_DRAW.texture_ptr].second.emplace_back(TILE_DRAW.tile_index_v);
        //}
        //
        // std::size_t unique { 0 };
        // std::size_t sum { 0 };
        // std::ostringstream ss;
        // std::size_t count { 0 };
        // for (auto & infoPair : textureMap)
        //{
        //    ss << "\n\t#" << ++count << " Texture size=" << infoPair.first->getSize()
        //       << ", map_tile_count=" << infoPair.second.second.size();
        //
        //    std::sort(std::begin(infoPair.second.first), std::end(infoPair.second.first));
        //    std::sort(std::begin(infoPair.second.second), std::end(infoPair.second.second));
        //
        //    infoPair.second.second.erase(
        //        std::unique(std::begin(infoPair.second.second), std::end(infoPair.second.second)),
        //        std::end(infoPair.second.second));
        //
        //    ss << " (" << infoPair.second.second.size() << " unique) ("
        //       << (mapTileRectI_.width * mapTileRectI_.height)
        //       << " total), coords_count=" << infoPair.second.first.size();
        //
        //    infoPair.second.first.erase(
        //        std::unique(std::begin(infoPair.second.first), std::end(infoPair.second.first)),
        //        std::end(infoPair.second.first));
        //
        //    const auto UNIQUE { infoPair.second.first.size() };
        //    unique += UNIQUE;
        //
        //    ss << " (" << UNIQUE << " unique)";
        //
        //    const auto TOTAL { (
        //        (infoPair.first->getSize().x / static_cast<unsigned>(tileSizeVI_.x))
        //        * (infoPair.first->getSize().y / static_cast<unsigned>(tileSizeVI_.y))) };
        //    sum += TOTAL;
        //
        //    const auto RATIO_OF_COVERAGE { (
        //        static_cast<double>(UNIQUE) / static_cast<double>(TOTAL)) };
        //
        //    ss << " (" << TOTAL << " total)";
        //
        //    ss << "(" << std::setprecision(3) << (RATIO_OF_COVERAGE * 100.0) << "% coverage)";
        //}
        //
        // ss << "\n\tTotals: unique=" << unique << ", sum=" << sum << ", " << std::setprecision(3)
        //   << ((static_cast<double>(unique) / static_cast<double>(sum)) * 100.0) << "%)";
        //
        // M_HP_LOG_WRN(
        //    "Texture Summary (tile_size=" << tileSizeVI_ << ")(tile_counts="
        //                                  << sfutil::Size(mapTileRectI_) << "):" << ss.str());
        //
        // misc::VectorMap<sf::Vector2i, std::size_t> posCountMap;
        // for (const auto & TILE_DRAW : tileDraws_)
        //{
        //    posCountMap[TILE_DRAW.tile_index_v]++;
        //}
        //
        // misc::VectorMap<std::size_t, std::vector<sf::Vector2i>> countPosMap;
        // for (const auto & POS_COUNT_PAIR : posCountMap)
        //{
        //    countPosMap[POS_COUNT_PAIR.second].emplace_back(POS_COUNT_PAIR.first);
        //}
        //
        // ss.str("");
        // ss << "Position Draw Counts (" << countPosMap.Size() << "):";
        // for (const auto & COUNT_POS_PAIR : countPosMap)
        //{
        //    ss << "\n\t" << COUNT_POS_PAIR.first << "\t" << COUNT_POS_PAIR.second.size();
        //}
        //
        // M_HP_LOG_WRN(ss.str());
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

    void MapDisplay::QuadAppend(
        std::vector<sf::Vertex> & vertexes,
        const sf::FloatRect & POSITION_RECT,
        const sf::FloatRect & TEXTURE_RECT) const
    {
        vertexes.emplace_back(
            sf::Vertex(sfutil::TopLeft(POSITION_RECT), sfutil::TopLeft(TEXTURE_RECT)));

        vertexes.emplace_back(
            sf::Vertex(sfutil::TopRight(POSITION_RECT), sfutil::TopRight(TEXTURE_RECT)));

        vertexes.emplace_back(
            sf::Vertex(sfutil::BottomRight(POSITION_RECT), sfutil::BottomRight(TEXTURE_RECT)));

        vertexes.emplace_back(
            sf::Vertex(sfutil::BottomLeft(POSITION_RECT), sfutil::BottomLeft(TEXTURE_RECT)));
    }

    void
        MapDisplay::QuadAppend(std::vector<sf::Vertex> & vertexes, const TileDraw & TILE_DRAW) const
    {
        const auto MAP_POS_V { sfutil::VectorMult(TILE_DRAW.tile_index_v, tileSizeVI_) };
        const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(sf::Vector2f { MAP_POS_V }) };
        const sf::FloatRect OFFSCREEN_RECT(OFFSCREEN_POS_V, sf::Vector2f { tileSizeVI_ });
        const sf::FloatRect TEXTURE_RECT { TILE_DRAW.texture_rect };
        QuadAppend(vertexes, OFFSCREEN_RECT, TEXTURE_RECT);
    }

    void MapDisplay::LogLayerAndTextureInfo()
    {
        auto tileTextureAreaPixels = [&](const sf::Texture & TEXTURE) {
            return static_cast<std::size_t>(TEXTURE.getSize().x * TEXTURE.getSize().y);
        };

        auto tileTextureAreaTiles = [&](const sf::Texture & TEXTURE) {
            return static_cast<std::size_t>(
                (static_cast<int>(TEXTURE.getSize().x) / tileSizeVI_.x)
                * (static_cast<int>(TEXTURE.getSize().y) / tileSizeVI_.y));
        };

        std::size_t totalTextureCountPixels { 0 };
        std::size_t totalTextureCountTiles { 0 };
        for (std::size_t index(0); index < MapTileTextureCount(); ++index)
        {
            const auto & TEXTURE { MapTileTexture(index) };
            totalTextureCountPixels += tileTextureAreaPixels(TEXTURE);
            totalTextureCountTiles += tileTextureAreaTiles(TEXTURE);
        }

        misc::VectorMap<std::pair<std::size_t, sf::IntRect>, int> uniqueImageMap;
        misc::VectorMap<std::size_t, int> uniqueLayerCountMapper;
        misc::VectorMap<sf::Vector2i, int> uniqueMapTileCountMap;
        for (const auto & TILE_DRAW : tileDrawsBelow_)
        {
            uniqueMapTileCountMap[TILE_DRAW.tile_index_v]++;
            uniqueLayerCountMapper[TILE_DRAW.layer_index]++;
            uniqueImageMap[std::make_pair(TILE_DRAW.texture_index, TILE_DRAW.texture_rect)]++;
        }

        for (const auto & TILE_DRAW : tileDrawsAbove_)
        {
            uniqueMapTileCountMap[TILE_DRAW.tile_index_v]++;
            uniqueLayerCountMapper[TILE_DRAW.layer_index]++;
            uniqueImageMap[std::make_pair(TILE_DRAW.texture_index, TILE_DRAW.texture_rect)]++;
        }

        auto makePercentString
            = [&](const auto NUMBER_A, const auto NUMBER_B, const std::string & DESCRIPTION) {
                  std::ostringstream pss;
                  const auto NUMBER_A_D { static_cast<double>(NUMBER_A) };
                  const auto NUMBER_B_D { static_cast<double>(NUMBER_B) };
                  if (misc::IsRealZero(NUMBER_B_D))
                  {
                      pss << "(div_by_zero_error%)";
                  }
                  else
                  {
                      const auto PERCENT { static_cast<int>((NUMBER_A_D / NUMBER_B_D) * 100.0) };
                      pss << "(" << PERCENT << "% " << DESCRIPTION << ")";
                  }
                  return pss.str();
              };

        const auto LAYER_COUNT { uniqueLayerCountMapper.Size() };
        const auto TILES_PER_LAYER_COUNT { (mapTileRectI_.width * mapTileRectI_.height) };

        const auto TOTAL_TILES_IN_MAP { LAYER_COUNT
                                        * static_cast<std::size_t>(TILES_PER_LAYER_COUNT) };

        const auto TOTAL_UNIQUE_IMAGE_COUNT { uniqueImageMap.Size() };
        const auto TOTAL_DRAW_COUNT { (tileDrawsBelow_.size() + tileDrawsAbove_.size()) };

        std::ostringstream ssPre;
        ssPre << "\nAll Layers/Texture info:  map_size=" << mapTileRectI_
              << ", tiles_per_layer=" << TILES_PER_LAYER_COUNT << ", layer_count=" << LAYER_COUNT
              << ", total_tiles=" << TOTAL_TILES_IN_MAP << ", total_draws=" << TOTAL_DRAW_COUNT
              << makePercentString(
                     TOTAL_DRAW_COUNT,
                     TOTAL_TILES_IN_MAP,
                     "of all possible tiles are actually drawn")
              << ", total_unique_draws=" << uniqueMapTileCountMap.Size()
              << makePercentString(
                     uniqueMapTileCountMap.Size(),
                     TOTAL_DRAW_COUNT,
                     "of all draws are to unique tiles (inverse overlap)")
              << ", texture_count=" << MapTileTextureCount()
              << ", total_images=" << totalTextureCountTiles
              << ", unique_images_drawn=" << TOTAL_UNIQUE_IMAGE_COUNT
              << makePercentString(
                     TOTAL_UNIQUE_IMAGE_COUNT,
                     totalTextureCountTiles,
                     "of all possible images are actually drawn")
              << ", each unique image drawn an average of "
              << (TOTAL_DRAW_COUNT / TOTAL_UNIQUE_IMAGE_COUNT) << " times.";

        M_HP_LOG_WRN(ssPre.str());

        auto logLayerAndTextureInfo = [&](const std::string & NAME,
                                          const std::vector<TileDraw> & DRAWS) {
            misc::VectorMap<std::size_t, int> uniqueLayerCountMap;
            misc::VectorMap<std::size_t, int> uniqueTextureCountMap;
            misc::VectorMap<sf::Vector2i, int> uniqueTilePosMap;

            struct TextureTrans
            {
                std::size_t index = 0;
                std::size_t draw_count = 0;
                misc::VectorMap<sf::IntRect, int> unique_image_map;
            };

            struct LayerTran
            {
                std::size_t index = 0;
                misc::VectorMap<sf::Vector2i, int> unique_tile_map;
                std::vector<TextureTrans> textures;
            };

            std::vector<LayerTran> layerTrans;

            for (const auto & TILE_DRAW : DRAWS)
            {
                if ((layerTrans.empty()) || (layerTrans.back().index != TILE_DRAW.layer_index))
                {
                    layerTrans.emplace_back(LayerTran());
                    layerTrans.back().index = TILE_DRAW.layer_index;

                    uniqueLayerCountMap[TILE_DRAW.layer_index]++;
                }

                if ((layerTrans.back().textures.empty())
                    || (layerTrans.back().textures.back().index != TILE_DRAW.texture_index))
                {
                    layerTrans.back().textures.emplace_back(TextureTrans());
                    layerTrans.back().textures.back().index = TILE_DRAW.texture_index;

                    uniqueTextureCountMap[TILE_DRAW.texture_index]++;
                }

                layerTrans.back().textures.back().unique_image_map[TILE_DRAW.texture_rect]++;
                layerTrans.back().textures.back().draw_count++;
                layerTrans.back().unique_tile_map[TILE_DRAW.tile_index_v]++;
                uniqueTilePosMap[TILE_DRAW.tile_index_v]++;
            }

            std::size_t uniqueTextureArea { 0 };
            for (const auto & INDEX_COUNT_PAIR : uniqueTextureCountMap)
            {
                uniqueTextureArea += tileTextureAreaTiles(MapTileTexture(INDEX_COUNT_PAIR.first));
            }

            std::size_t overlappingDrawsCount { 0 };
            for (const auto & TILEPOS_COUNT_PAIR : uniqueTilePosMap)
            {
                if (TILEPOS_COUNT_PAIR.second > 1)
                {
                    overlappingDrawsCount
                        += static_cast<std::size_t>(TILEPOS_COUNT_PAIR.second - 1);
                }
            }

            std::size_t totalUniqueImageCount { 0 };
            std::size_t textureTransitionCount { 0 };
            std::size_t prevTextureIndex { MapTileTextureCount() };
            for (auto & LAYER_INFO : layerTrans)
            {
                for (const auto & TEXTURE_INFO : LAYER_INFO.textures)
                {
                    if (TEXTURE_INFO.index != prevTextureIndex)
                    {
                        ++textureTransitionCount;
                        prevTextureIndex = TEXTURE_INFO.index;
                        totalUniqueImageCount += TEXTURE_INFO.unique_image_map.Size();
                    }
                }
            }
            --textureTransitionCount;

            const auto LAYER_TRANSITION_COUNT { layerTrans.size() };

            std::size_t textureTransitionsThatCanBeEliminatedWithReOrdering { 0 };
            if (LAYER_TRANSITION_COUNT > 1)
            {
                for (std::size_t i(0); i < (LAYER_TRANSITION_COUNT - 1); ++i)
                {
                    const auto & CURR_LAYER_INFO { layerTrans.at(i) };
                    const auto & NEXT_LAYER_INFO { layerTrans.at(i + 1) };

                    const auto CURR_LAST_TEXTURE_INDEX { CURR_LAYER_INFO.textures.back().index };
                    const auto NEXT_FIRST_TEXTURE_INDEX { NEXT_LAYER_INFO.textures.front().index };

                    if (CURR_LAST_TEXTURE_INDEX != NEXT_FIRST_TEXTURE_INDEX)
                    {
                        std::vector<std::size_t> indexes;

                        for (const auto & CURR_TEXTURE_INFO : CURR_LAYER_INFO.textures)
                        {
                            indexes.emplace_back(CURR_TEXTURE_INFO.index);
                        }

                        for (const auto & NEXT_TEXTURE_INFO : NEXT_LAYER_INFO.textures)
                        {
                            indexes.emplace_back(NEXT_TEXTURE_INFO.index);
                        }

                        std::sort(std::begin(indexes), std::end(indexes));

                        if (std::adjacent_find(std::begin(indexes), std::end(indexes))
                            != std::end(indexes))
                        {
                            ++textureTransitionsThatCanBeEliminatedWithReOrdering;
                        }
                    }
                }
            }

            const auto TOTAL_TEXTURES_COUNT { MapTileTextureCount() };
            const auto TOTAL_TILE_POS_IN_MAP { (mapTileRectI_.width * mapTileRectI_.height) };

            std::ostringstream ss;

            auto numberRatioToString = [&](const std::string & NUMBERS_NAME,
                                           const auto NUMBER_A,
                                           const auto NUMBER_B,
                                           const std::string & PERCENT_DESCRIPTION) {
                ss << "\n\t" << std::setw(24) << std::right << NUMBERS_NAME << "=" << std::setw(4)
                   << NUMBER_A << "/" << std::setw(4) << NUMBER_B;
                const auto NUMBER_A_D { static_cast<double>(NUMBER_A) };
                const auto NUMBER_B_D { static_cast<double>(NUMBER_B) };
                if (misc::IsRealZero(NUMBER_B_D) == false)
                {
                    const auto PERCENT { static_cast<int>((NUMBER_A_D / NUMBER_B_D) * 100.0) };
                    ss << "\t(" << PERCENT << "% " << PERCENT_DESCRIPTION << ")";
                }
                return ss.str();
            };

            ss << "\nLayer/Texture Info for \"" << NAME
               << "\", layer_count=" << uniqueLayerCountMap.Size()
               << ", texture_transition_optimizations="
               << textureTransitionsThatCanBeEliminatedWithReOrdering;

            numberRatioToString(
                "tiles_unique2total",
                uniqueTilePosMap.Size(),
                TOTAL_TILE_POS_IN_MAP,
                "of the map was drawn");

            numberRatioToString(
                "draws_unique2total",
                overlappingDrawsCount,
                uniqueTilePosMap.Size(),
                "of the draws overlapped");

            numberRatioToString(
                "textures_unique2total",
                uniqueTextureCountMap.Size(),
                TOTAL_TEXTURES_COUNT,
                "of ALL textures were used");

            numberRatioToString(
                "(images)_unique2total",
                totalUniqueImageCount,
                uniqueTextureArea,
                "of all used textures were actually drawn");

            numberRatioToString(
                "textures_trans2unique", textureTransitionCount, uniqueTextureCountMap.Size(), "");

            for (std::size_t layerTransIndex(0); layerTransIndex < layerTrans.size();
                 ++layerTransIndex)
            {
                const auto & LAYER_INFO { layerTrans.at(layerTransIndex) };
                std::size_t layerDrawCount { 0 };
                std::size_t layerUniqueImageCount { 0 };
                misc::VectorMap<std::size_t, int> layerUniqueTextureMap;
                for (const auto & TEXTURE_INFO : LAYER_INFO.textures)
                {
                    layerUniqueTextureMap[TEXTURE_INFO.index]++;
                    layerDrawCount += TEXTURE_INFO.draw_count;
                    layerUniqueImageCount += TEXTURE_INFO.unique_image_map.Size();
                }

                // std::size_t overlappingDrawCount { 0 };
                // for (const auto & TILE_COUNT_PAIR : LAYER_INFO.unique_tile_map)
                //{
                //    if (TILE_COUNT_PAIR.second > 1)
                //    {
                //        overlappingDrawCount += (TILE_COUNT_PAIR.second - 1);
                //    }
                //}

                // std::size_t layerUniqueTextureArea { 0 };
                // for (const auto & INDEX_COUNT_PAIR : layerUniqueTextureMap)
                //{
                //    layerUniqueTextureArea
                //        += tileTextureArea(TEXTURE_PTR_VEC.at(INDEX_COUNT_PAIR.first).Get());
                //}

                ss << "\n\t\tLayer #" << LAYER_INFO.index << " " << layerUniqueTextureMap.Size()
                   << "(";

                for (const auto & TEXTUREINDEX_COUNT_PAIR : layerUniqueTextureMap)
                {
                    ss << TEXTUREINDEX_COUNT_PAIR.first << ",";
                }

                ss << "), images="
                   << layerUniqueImageCount

                   // << ", unique_draws=" << LAYER_INFO.unique_tile_map.Size()
                   // << ", overlapping_draws=" << overlappingDrawCount
                   << ", draws=" << layerDrawCount;

                if (layerTransIndex > 0)
                {
                    const auto & PREV_LAYER_INFO { layerTrans.at(layerTransIndex - 1) };

                    std::size_t prevLayerOverlappingDrawsCount { 0 };
                    for (const auto & CURR_TILE_COUNT_PAIR : LAYER_INFO.unique_tile_map)
                    {
                        const auto & CURR_TILE { CURR_TILE_COUNT_PAIR.first };
                        for (const auto & PREV_TILE_COUNT_PAIR : PREV_LAYER_INFO.unique_tile_map)
                        {
                            const auto & PREV_TILE { PREV_TILE_COUNT_PAIR.first };

                            if (CURR_TILE == PREV_TILE)
                            {
                                ++prevLayerOverlappingDrawsCount;
                            }
                        }
                    }

                    ss << ", overlapping_draws=" << prevLayerOverlappingDrawsCount;
                }
            }

            M_HP_LOG_WRN(ss.str());
        };

        logLayerAndTextureInfo("BELOW", tileDrawsBelow_);
        logLayerAndTextureInfo("ABOVE", tileDrawsAbove_);
    }

    void MapDisplay::MoveTextureRects(VertexVec_t & vertexes, const sf::Vector2f & MOVE_V) const
    {
        for (auto & vertex : vertexes)
        {
            vertex.texCoords += MOVE_V;
        }
    }

    void MapDisplay::OptimizeTextures(std::vector<TileDraw> & tileDraws)
    {
        if (tileDraws.empty() || mapTileTexturesOrig_.empty())
        {
            return;
        }

        // calculate the number of tiles that can fit in each new texture
        const sf::Vector2<std::size_t> TILE_SIZE_T_V { tileSizeVI_ };

        auto tileCountOfImageWithPixelDimmension = [&](const std::size_t PIXEL_DIMMENSION) {
            const std::size_t TILE_COUNT_HORIZ { (PIXEL_DIMMENSION / TILE_SIZE_T_V.x) };
            const std::size_t TILE_CUONT_VERT { (PIXEL_DIMMENSION / TILE_SIZE_T_V.y) };
            return (TILE_COUNT_HORIZ * TILE_CUONT_VERT);
        };

        const std::size_t NEW_TEXTURE_MAX_DIMMENSION { 1024 };

        const std::size_t MAX_TILES_PER_NEW_TEXTURE { tileCountOfImageWithPixelDimmension(
            NEW_TEXTURE_MAX_DIMMENSION) };

        auto transferTileDrawsToNewTexture =
            [&](std::vector<std::vector<std::size_t>> & tileDrawIndexVecsToTransfer) {
                const auto TILE_DRAW_INDEXES_COUNT_ORIG { tileDrawIndexVecsToTransfer.size() };

                if (TILE_DRAW_INDEXES_COUNT_ORIG == 0)
                {
                    return;
                }

                const auto PIXEL_DIMMENSION = [&]() {
                    if (TILE_DRAW_INDEXES_COUNT_ORIG < MAX_TILES_PER_NEW_TEXTURE)
                    {
                        std::size_t powerOfTwo { 64 };

                        while ((powerOfTwo < NEW_TEXTURE_MAX_DIMMENSION)
                               && (tileCountOfImageWithPixelDimmension(powerOfTwo)
                                   < TILE_DRAW_INDEXES_COUNT_ORIG))
                        {
                            powerOfTwo <<= 1;
                        }

                        return powerOfTwo;
                    }
                    else
                    {
                        return NEW_TEXTURE_MAX_DIMMENSION;
                    }
                }();

                sf::RenderTexture renderTexture;

                renderTexture.create(
                    static_cast<unsigned>(PIXEL_DIMMENSION),
                    static_cast<unsigned>(PIXEL_DIMMENSION));

                renderTexture.clear(sf::Color::Transparent);

                std::vector<sf::Vertex> copyQuadvertexes;

                const auto TRANSFER_COUNT { std::min(
                    TILE_DRAW_INDEXES_COUNT_ORIG, MAX_TILES_PER_NEW_TEXTURE) };

                const std::size_t MAX_COL_COUNT { (PIXEL_DIMMENSION / TILE_SIZE_T_V.x) };
                const std::size_t MAX_ROW_COUNT { (PIXEL_DIMMENSION / TILE_SIZE_T_V.y) };

                sf::RenderStates renderStates { sf::RenderStates::Default };

                std::size_t transferVecIndex(0);
                for (std::size_t row(0);
                     ((row < MAX_ROW_COUNT) && (transferVecIndex < TRANSFER_COUNT));
                     ++row)
                {
                    for (std::size_t col(0);
                         ((col < MAX_COL_COUNT) && (transferVecIndex < TRANSFER_COUNT));
                         ++col)
                    {
                        // copy/draw the image from the old texture to the new texture
                        const sf::Vector2<std::size_t> DEST_POS_V(
                            (col * TILE_SIZE_T_V.x), (row * TILE_SIZE_T_V.y));

                        const sf::FloatRect DEST_POS_RECT(
                            sf::Vector2f { DEST_POS_V }, sf::Vector2f { TILE_SIZE_T_V });

                        if (tileDrawIndexVecsToTransfer.at(transferVecIndex).empty())
                        {
                            M_HP_LOG_ERR(
                                "****** tileDrawIndexVecToTransfer was empty!  transferVecIndex="
                                << transferVecIndex);
                        }

                        // all the TileDraws in tileDrawIndexVecsToTransfer[transferVecIndex] have
                        // the same texture_index and texture_rect
                        const auto & FIRST_TILE_DRAW_TO_TRNANSER { tileDraws.at(
                            tileDrawIndexVecsToTransfer.at(transferVecIndex).front()) };

                        const std::size_t SOURCE_TEXTURE_INDEX {
                            FIRST_TILE_DRAW_TO_TRNANSER.texture_index
                        };
                        const sf::FloatRect SOURCE_TEXTURE_RECT {
                            FIRST_TILE_DRAW_TO_TRNANSER.texture_rect
                        };

                        copyQuadvertexes.clear();
                        QuadAppend(copyQuadvertexes, DEST_POS_RECT, SOURCE_TEXTURE_RECT);

                        renderStates.texture = &mapTileTexturesOrig_.at(SOURCE_TEXTURE_INDEX).Get();

                        renderTexture.draw(
                            &copyQuadvertexes[0], copyQuadvertexes.size(), sf::Quads, renderStates);

                        // update all TileDraws with new source texture information
                        for (const auto & TILE_DRAW_TO_UPDATE_INDEX :
                             tileDrawIndexVecsToTransfer.at(transferVecIndex))
                        {
                            auto & tileDrawToUpdate { tileDraws.at(TILE_DRAW_TO_UPDATE_INDEX) };
                            tileDrawToUpdate.texture_index = mapTileTexturesNew_.size();
                            tileDrawToUpdate.texture_rect = sf::IntRect { DEST_POS_RECT };
                        }

                        ++transferVecIndex;
                    }
                }

                if (TRANSFER_COUNT != transferVecIndex)
                {
                    M_HP_LOG_ERR(
                        "****** TRANSFER_COUNT(" << TRANSFER_COUNT << ") != transferVecIndex("
                                                 << transferVecIndex << ")");
                }

                renderTexture.display();

                mapTileTexturesNew_.emplace_back(sf::Texture());
                mapTileTexturesNew_.back().loadFromImage(renderTexture.getTexture().copyToImage());

                tileDrawIndexVecsToTransfer.erase(
                    std::begin(tileDrawIndexVecsToTransfer),
                    std::begin(tileDrawIndexVecsToTransfer)
                        + static_cast<std::ptrdiff_t>(TRANSFER_COUNT));

                M_HP_LOG_WRN(
                    "New optimized texture #" << (mapTileTexturesNew_.size() - 1)
                                              << " created: " << renderTexture.getSize()
                                              << " holding " << TRANSFER_COUNT << " tiles.");
            };

        // create a minimally complete collection of unique layer/texture/rects/TileDrawIndexes
        using RectTileDrawIndexesMap_t = misc::VectorMap<sf::IntRect, std::vector<std::size_t>>;
        using TextureRectsMap_t = misc::VectorMap<std::size_t, RectTileDrawIndexesMap_t>;
        using LayerTexturesMap_t = misc::VectorMap<std::size_t, TextureRectsMap_t>;

        LayerTexturesMap_t layerTilesMap;

        bool isFirstUnique { true };
        for (std::size_t index(0); index < tileDraws.size(); ++index)
        {
            const auto & DRAW { tileDraws.at(index) };

            layerTilesMap[DRAW.layer_index][DRAW.texture_index][DRAW.texture_rect].emplace_back(
                index);

            if (isFirstUnique)
            {
                isFirstUnique = false;
                layerTilesMap.Reserve(32);
                layerTilesMap.Front().second.Reserve(32);
                layerTilesMap.Front().second.Front().second.Reserve(1024);
            }
        }

        // create an collection of TileDraw indexes that share texture_index and texture_rect,
        // in draw order, to be transfered to a new texture
        std::vector<std::vector<std::size_t>> tileDrawIndexVecsToTransferOrig;
        tileDrawIndexVecsToTransferOrig.reserve(tileDraws.size());

        std::size_t tileDrawConfirmCount { 0 };
        for (const auto & LAYER_TEXTURES_PAIR : layerTilesMap)
        {
            for (const auto & TEXTURE_RECTS_PAIR : LAYER_TEXTURES_PAIR.second)
            {
                for (const auto & RECT_INDEXES_PAIR : TEXTURE_RECTS_PAIR.second)
                {
                    tileDrawIndexVecsToTransferOrig.emplace_back(RECT_INDEXES_PAIR.second);
                    tileDrawConfirmCount += RECT_INDEXES_PAIR.second.size();
                }
            }
        }

        if (tileDraws.size() != tileDrawConfirmCount)
        {
            M_HP_LOG_ERR(
                "****** tileDraws.size()(" << tileDraws.size() << ") != tileDrawConfirmCount("
                                           << tileDrawConfirmCount << ")");
        }

        // eliminate duplicates that will end up on the same texture
        using ImagePair_t = std::pair<std::size_t, sf::IntRect>;
        misc::VectorMap<ImagePair_t, std::size_t> alreadyTransferedImageToUniqueIndexMap;

        std::vector<std::vector<std::size_t>> tileDrawIndexVecsToTransferUnique;

        std::size_t duplicateDetectionCount { 0 };
        std::size_t duplicateImageCount { 0 };
        std::size_t newTextureImageCounter { 0 };
        for (const auto & TILE_DRAW_INDEXES_ORIG : tileDrawIndexVecsToTransferOrig)
        {
            if (MAX_TILES_PER_NEW_TEXTURE >= ++newTextureImageCounter)
            {
                alreadyTransferedImageToUniqueIndexMap.Clear();
            }

            const auto & REFERENCE_TILE_DRAW { tileDraws.at(TILE_DRAW_INDEXES_ORIG.front()) };

            const auto IMAGE_PAIR { std::make_pair(
                REFERENCE_TILE_DRAW.texture_index, REFERENCE_TILE_DRAW.texture_rect) };

            if (alreadyTransferedImageToUniqueIndexMap.Exists(IMAGE_PAIR))
            {
                auto & tileDrawIndexesToTransferUnique { tileDrawIndexVecsToTransferUnique.at(
                    alreadyTransferedImageToUniqueIndexMap[IMAGE_PAIR]) };

                if (tileDrawIndexesToTransferUnique.empty() == false)
                {
                    tileDrawIndexesToTransferUnique.insert(
                        std::end(tileDrawIndexesToTransferUnique),
                        std::begin(TILE_DRAW_INDEXES_ORIG),
                        std::end(TILE_DRAW_INDEXES_ORIG));

                    duplicateImageCount += tileDrawIndexesToTransferUnique.size();
                    ++duplicateDetectionCount;
                }
            }
            else
            {
                alreadyTransferedImageToUniqueIndexMap[IMAGE_PAIR]
                    = tileDrawIndexVecsToTransferUnique.size();

                tileDrawIndexVecsToTransferUnique.emplace_back(TILE_DRAW_INDEXES_ORIG);
            }
        }

        M_HP_LOG_WRN(
            "\n\t*** " << duplicateDetectionCount << " duplicate detections totaling "
                       << duplicateImageCount);

        // transfer all TileDraws to newly created textures
        while (tileDrawIndexVecsToTransferUnique.empty() == false)
        {
            transferTileDrawsToNewTexture(tileDrawIndexVecsToTransferUnique);
        }

        const auto TILE_DRAW_COUNT_BEFORE { tileDraws.size() };

        // re-sort after transfer to ensure overlapping draws still work/look right
        std::sort(std::begin(tileDraws), std::end(tileDraws));

        tileDraws.erase(
            std::unique(std::begin(tileDraws), std::end(tileDraws)), std::end(tileDraws));

        const auto TILE_DRAW_COUNT_AFTER { tileDraws.size() };

        if (TILE_DRAW_COUNT_BEFORE != TILE_DRAW_COUNT_AFTER)
        {
            M_HP_LOG_WRN(
                "\n\t\t****** After optimization tile draw count went from "
                << TILE_DRAW_COUNT_BEFORE << " to " << TILE_DRAW_COUNT_AFTER);
        }
    }

} // namespace map
} // namespace heroespath
