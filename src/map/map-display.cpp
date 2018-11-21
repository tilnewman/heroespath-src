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
        , playerPosV_(0.0f, 0.0f)
        , playerPosOffsetV_(0.0f, 0.0f)
        , offScreenImageBelow_()
        , offScreenImageAnim_()
        , offScreenImageAvatar_()
        , offScreenImageAbove_()
        , npcShadowCachedTexture_(
              "media-images-avatar-shadow",
              (gui::ImageOpt::Smooth | gui::ImageOpt::ShadowMaskForShadowImage))
        , npcShadowSprite_(npcShadowCachedTexture_.Get())
        , animInfoVec_()
        , animUPtrVec_()
        , avatarSprites_()
        , avatarSpriteIndexAndMapRects_()
        , mapTileTextures_()
        , tileDraws_()
        , tileVertexVecBelow_()
        , tileVertexVecAbove_()
        , timeTrials_("MapDisplay", TimeRes::Micro, true, 20, 0.0)
        , timeTrialIndexRedDrawSub_(timeTrials_.AddCollecter("SubSection"))
    {
        avatarSprites_.reserve(64);
        avatarSpriteIndexAndMapRects_.reserve(64);
        tileDraws_.reserve(50000);
        tileVertexVecBelow_.reserve(64);
        tileVertexVecAbove_.reserve(64);
    }

    MapDisplay::~MapDisplay() { StopAnimMusic(); }

    void MapDisplay::Load(
        const map::Layout & MAP_LAYOUT,
        const sf::Vector2f & PLAYER_STARTING_POS_V,
        const MapAnimVec_t & ANIM_INFO_VEC)
    {
        timeTrials_.EndAllContests();

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

        StopAnimMusic();

        animInfoVec_ = ANIM_INFO_VEC;
        SetupAnimations();

        StartAnimMusic();
        UpdateAnimMusicVolume();

        AnalyzeLayers(MAP_LAYOUT);
        mapTileTextures_ = MAP_LAYOUT.texture_vec;

        ReDrawEverythingAfterOffScreenTileChange();
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
        target.draw(offScreenImageBelow_, states);
        target.draw(offScreenImageAvatar_, states);
        target.draw(offScreenImageAbove_, states);
        target.draw(offScreenImageAnim_, states);
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

        DrawAnimationsOffScreen();
        DrawAvatarsOffScreen();
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

    void MapDisplay::IncrementTileOffsetsInDirection(const gui::Direction::Enum DIRECTION)
    {
        const auto OFFSCREEN_TEXTURE_RECT_BEFORE { offScreenTileRectI_ };

        sf::Vector2f moveV(0.0f, 0.0f);

        switch (DIRECTION)
        {
            case gui::Direction::Up:
            {
                if (offScreenTileRectI_.top > 0)
                {
                    --offScreenTileRectI_.top;
                    moveV.y = tileSizeVI_.y;
                }
                break;
            }

            case gui::Direction::Down:
            {
                if (sfutil::Bottom(offScreenTileRectI_) < sfutil::Bottom(mapTileRectI_))
                {
                    ++offScreenTileRectI_.top;

                    if (tileSizeVI_.y > offScreenTextureRect_.top)
                    {
                        moveV.y = -offScreenTextureRect_.top;
                    }
                    else
                    {
                        moveV.y = -tileSizeVI_.y;
                    }
                }
                break;
            }

            case gui::Direction::Left:
            {
                if (offScreenTileRectI_.left > 0)
                {
                    --offScreenTileRectI_.left;
                    moveV.x = tileSizeVI_.x;
                }
                break;
            }

            case gui::Direction::Right:
            {
                if (sfutil::Right(offScreenTileRectI_) < sfutil::Right(mapTileRectI_))
                {
                    ++offScreenTileRectI_.left;

                    if (tileSizeVI_.x > offScreenTextureRect_.left)
                    {
                        moveV.x = -offScreenTextureRect_.left;
                    }
                    else
                    {
                        moveV.x = -tileSizeVI_.x;
                    }
                }
                break;
            }

            case gui::Direction::Count:
            default:
            {
                break;
            }
        }

        const auto OFFSCREEN_TEXTURE_RECT_AFTER { offScreenTileRectI_ };

        if (OFFSCREEN_TEXTURE_RECT_BEFORE != OFFSCREEN_TEXTURE_RECT_AFTER)
        {
            UpdateOffScreenMapPosOffset();
            MoveOffscreenTextureRects(moveV);
            ReDrawEverythingAfterOffScreenTileChange();
        }
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
            ReDrawEverythingAfterOffScreenTileChange();
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
            ReDrawEverythingAfterOffScreenTileChange();
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
            ReDrawEverythingAfterOffScreenTileChange();
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
            ReDrawEverythingAfterOffScreenTileChange();
            return true;
        }

        return false;
    }

    void MapDisplay::ReDrawEverythingAfterOffScreenTileChange()
    {
        ResetMapSubsections();
        DrawAvatarsOffScreen();
        DrawAnimationsOffScreen();
        DrawMapOffScreen();
    }

    void MapDisplay::DrawAvatarsOffScreen()
    {
        offScreenImageAvatar_.render_texture.clear(sf::Color::Transparent);

        avatarSpriteIndexAndMapRects_.clear();
        offScreenImageAvatar_.vertex_array.clear();

        // fill the indexes vector but only with indexes to avatars that are visible
        const auto AVATAR_SPRITE_COUNT { avatarSprites_.size() };
        for (std::size_t index(0); index < AVATAR_SPRITE_COUNT; ++index)
        {
            // avatar sprites.getPosition() is already in map coordinates
            const auto MAP_RECT_V { avatarSprites_.at(index).getGlobalBounds() };
            const auto OFFSCREEN_RECT { OffScreenRectFromMapRect(MAP_RECT_V) };

            if (offScreenTextureRect_.intersects(OFFSCREEN_RECT))
            {
                avatarSpriteIndexAndMapRects_.emplace_back(std::make_pair(index, MAP_RECT_V));
            }
        }

        if (avatarSpriteIndexAndMapRects_.empty())
        {
            return;
        }

        // sort indexes by draw order so that avatars in front will be drawn on top of those behind
        if (avatarSpriteIndexAndMapRects_.size() > 1)
        {
            std::sort(
                std::begin(avatarSpriteIndexAndMapRects_),
                std::end(avatarSpriteIndexAndMapRects_),
                [&](const auto PAIR_A, const auto PAIR_B) {
                    return (sfutil::Bottom(PAIR_A.second) < sfutil::Bottom(PAIR_B.second));
                });
        }

        // The avatar shadows and sprites are drawn to the same offscreen position even thought
        // their sizes might differ. This loop draws only the shadows so that none are drawn on top
        // of avatars. Since this loop will have both the pos and size of all sprites we also setup
        // the vertexes in this loop.
        for (const auto & INDEX_MAPRECT_PAIR : avatarSpriteIndexAndMapRects_)
        {
            auto & sprite { avatarSprites_.at(INDEX_MAPRECT_PAIR.first) };

            npcShadowSprite_.setScale(sprite.getScale());

            // we have to change the avatar's sprite's pos before drawing offscreen so save it here
            // so it can be restored after
            const auto MAP_POS_ORIG_V { sfutil::Position(INDEX_MAPRECT_PAIR.second) };
            const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(MAP_POS_ORIG_V) };

            npcShadowSprite_.setPosition(OFFSCREEN_POS_V);
            offScreenImageAvatar_.render_texture.draw(npcShadowSprite_);

            sprite.setPosition(OFFSCREEN_POS_V);
            // do not draw the avatar sprite here, see the loop after this one below

            const auto SHADOW_SIZE_V { sfutil::Size(npcShadowSprite_) };
            const auto AVATAR_SIZE_V { sfutil::Size(sprite) };

            const sf::Vector2f TOTAL_SIZE_V(
                std::max(SHADOW_SIZE_V.x, AVATAR_SIZE_V.x),
                std::max(SHADOW_SIZE_V.y, AVATAR_SIZE_V.y));

            const sf::FloatRect OFFSCREEN_RECT(OFFSCREEN_POS_V, TOTAL_SIZE_V);

            const auto ONSCREEN_POS_V { OnScreenPosFromMapPos(MAP_POS_ORIG_V) };
            const sf::FloatRect ONSCREEN_RECT(ONSCREEN_POS_V, TOTAL_SIZE_V);

            VertexArrayQuadAppend(
                offScreenImageAvatar_.vertex_array, ONSCREEN_RECT, OFFSCREEN_RECT);
        }

        // draw all the avatar sprites
        for (const auto & INDEX_MAPRECT_PAIR : avatarSpriteIndexAndMapRects_)
        {
            auto & sprite { avatarSprites_.at(INDEX_MAPRECT_PAIR.first) };
            offScreenImageAvatar_.render_texture.draw(sprite);
            sprite.setPosition(sfutil::Position(INDEX_MAPRECT_PAIR.second));
        }

        offScreenImageAvatar_.render_texture.display();
    }

    void MapDisplay::DrawAnimationsOffScreen()
    {
        offScreenImageAnim_.render_texture.clear(sf::Color::Transparent);
        offScreenImageAnim_.vertex_array.clear();

        for (auto & animUPtr : animUPtrVec_)
        {
            // the animation objects GetEntityRegion() is already in map coordinates
            // change the pos to offscreen coordinates before drawing but then set it back
            const auto MAP_RECT_ORIG_V { animUPtr->GetEntityRegion() };
            const auto OFFSCREEN_RECT_ORIG { OffScreenRectFromMapRect(MAP_RECT_ORIG_V) };

            if (offScreenTextureRect_.intersects(OFFSCREEN_RECT_ORIG) == false)
            {
                continue;
            }

            animUPtr->SetEntityPos(sfutil::Position(OFFSCREEN_RECT_ORIG));
            offScreenImageAnim_.render_texture.draw(*animUPtr);
            animUPtr->SetEntityPos(sfutil::Position(MAP_RECT_ORIG_V));

            // animations might be drawn outside the onScreenRect_ so prevent that here
            const auto OFFSCREEN_RECT_FINAL { sfutil::Intersection(
                OFFSCREEN_RECT_ORIG, offScreenTextureRect_) };

            const auto CLIP_MOVE_V { (
                sfutil::Position(OFFSCREEN_RECT_FINAL) - sfutil::Position(OFFSCREEN_RECT_ORIG)) };

            const auto ONSCREEN_POS_FINAL_V {
                sfutil::Position(OnScreenRectFromMapRect(MAP_RECT_ORIG_V)) + CLIP_MOVE_V
            };

            const sf::FloatRect ONSCREEN_RECT_FINAL(
                ONSCREEN_POS_FINAL_V, sfutil::Size(OFFSCREEN_RECT_FINAL));

            VertexArrayQuadAppend(
                offScreenImageAnim_.vertex_array, ONSCREEN_RECT_FINAL, OFFSCREEN_RECT_FINAL);
        }

        if (offScreenImageAnim_.vertex_array.getVertexCount() > 0)
        {
            offScreenImageAnim_.render_texture.display();
        }
    }

    void MapDisplay::ResetMapSubsections()
    {
        // reset the new way
        tileVertexVecBelow_.clear();
        tileVertexVecAbove_.clear();

        auto appendQuad = [&](sf::VertexArray & vertexArray, const TileDraw & TILE_DRAW) {
            const auto MAP_POS_V { sfutil::VectorMult(TILE_DRAW.tile_index_v, tileSizeVI_) };

            const auto OFFSCREEN_POS_V { OffScreenPosFromMapPos(sf::Vector2f { MAP_POS_V }) };

            const sf::FloatRect OFFSCREEN_RECT(OFFSCREEN_POS_V, sf::Vector2f { tileSizeVI_ });

            const sf::FloatRect TEXTURE_RECT { TILE_DRAW.texture_rect };

            vertexArray.append(
                sf::Vertex(sfutil::TopLeft(OFFSCREEN_RECT), sfutil::TopLeft(TEXTURE_RECT)));

            vertexArray.append(
                sf::Vertex(sfutil::TopRight(OFFSCREEN_RECT), sfutil::TopRight(TEXTURE_RECT)));

            vertexArray.append(
                sf::Vertex(sfutil::BottomRight(OFFSCREEN_RECT), sfutil::BottomRight(TEXTURE_RECT)));

            vertexArray.append(
                sf::Vertex(sfutil::BottomLeft(OFFSCREEN_RECT), sfutil::BottomLeft(TEXTURE_RECT)));
        };

        // this loop assumes tileDraws_ sorted by layer/layer_type/texture
        const sf::Texture * currentTexturePtr { nullptr };
        for (const auto & TILE_DRAW : tileDraws_)
        {
            // TODO CLENAUP turn into an contains() call
            if ((TILE_DRAW.tile_index_v.x < offScreenTileRectI_.left)
                || (TILE_DRAW.tile_index_v.x > sfutil::Right(offScreenTileRectI_))
                || (TILE_DRAW.tile_index_v.y < offScreenTileRectI_.top)
                || (TILE_DRAW.tile_index_v.y > sfutil::Bottom(offScreenTileRectI_)))
            {
                continue;
            }

            const auto IS_BELOW { (TILE_DRAW.layer_type == LayerType::Ground) };

            if (TILE_DRAW.texture_ptr != currentTexturePtr)
            {
                currentTexturePtr = TILE_DRAW.texture_ptr;

                sf::VertexArray vertexArray;
                vertexArray.setPrimitiveType(sf::Quads);

                tileVertexVecBelow_.emplace_back(std::make_pair(currentTexturePtr, vertexArray));
                tileVertexVecAbove_.emplace_back(std::make_pair(currentTexturePtr, vertexArray));
            }

            if (IS_BELOW)
            {
                appendQuad(tileVertexVecBelow_.back().second, TILE_DRAW);
            }
            else
            {
                appendQuad(tileVertexVecAbove_.back().second, TILE_DRAW);
            }
        }

        // std::ostringstream ss;
        // ss << "ResetMapSubsections Below:";
        // std::size_t count { 0 };
        // for (auto & textureVertexsPair : tileVertexVecBelow_)
        //{
        //    ss << "\n\t#" << ++count << " " << textureVertexsPair.first << " "
        //       << textureVertexsPair.first->getSize() << " with "
        //       << (textureVertexsPair.second.getVertexCount() / VERTS_PER_QUAD_)
        //       << " tile draws (%="
        //       << (textureVertexsPair.second.getVertexCount() % VERTS_PER_QUAD_) << ")";
        //}
        // M_HP_LOG_WRN(ss.str());
        //
        // ss.str("");
        // ss << "ResetMapSubsections Above:";
        // count = 0;
        // for (auto & textureVertexsPair : tileVertexVecAbove_)
        //{
        //    ss << "\n\t#" << ++count << " " << textureVertexsPair.first << " "
        //       << textureVertexsPair.first->getSize() << " with "
        //       << (textureVertexsPair.second.getVertexCount() / VERTS_PER_QUAD_)
        //       << " tile draws (%="
        //       << (textureVertexsPair.second.getVertexCount() % VERTS_PER_QUAD_) << ")";
        //}
        // M_HP_LOG_WRN(ss.str());
    }

    void MapDisplay::DrawMapOffScreen()
    {
        M_HP_SCOPED_TIME_TRIAL(timeTrials_, timeTrialIndexRedDrawSub_);

        offScreenImageBelow_.render_texture.clear(sf::Color::Transparent);
        offScreenImageAbove_.render_texture.clear(sf::Color::Transparent);

        // do not clear or add to the Below/Above vertex arrays because they are always the same

        sf::RenderStates renderStates { sf::RenderStates::Default };

        for (const auto & TEXTURE_VERTS_PAIR : tileVertexVecBelow_)
        {
            if (TEXTURE_VERTS_PAIR.second.getVertexCount() > 0)
            {
                renderStates.texture = TEXTURE_VERTS_PAIR.first;

                offScreenImageBelow_.render_texture.draw(
                    &TEXTURE_VERTS_PAIR.second[0],
                    TEXTURE_VERTS_PAIR.second.getVertexCount(),
                    sf::Quads,
                    renderStates);
            }
        }

        for (const auto & TEXTURE_VERTS_PAIR : tileVertexVecAbove_)
        {
            if (TEXTURE_VERTS_PAIR.second.getVertexCount() > 0)
            {
                renderStates.texture = TEXTURE_VERTS_PAIR.first;

                offScreenImageAbove_.render_texture.draw(
                    &TEXTURE_VERTS_PAIR.second[0],
                    TEXTURE_VERTS_PAIR.second.getVertexCount(),
                    sf::Quads,
                    renderStates);
            }
        }

        offScreenImageBelow_.render_texture.display();
        offScreenImageAbove_.render_texture.display();
    }

    void MapDisplay::MoveOffscreenTextureRects(const sf::Vector2f & MOVE_V)
    {
        sfutil::Move(offScreenTextureRect_, MOVE_V);

        offScreenImageBelow_.MoveTextureRects(MOVE_V);
        offScreenImageAnim_.MoveTextureRects(MOVE_V);
        offScreenImageAvatar_.MoveTextureRects(MOVE_V);
        offScreenImageAbove_.MoveTextureRects(MOVE_V);
    }

    void MapDisplay::SetupOffScreenTextures()
    {
        const auto WIDTH { static_cast<unsigned>(offScreenRectI_.width) };
        const auto HEIGHT { static_cast<unsigned>(offScreenRectI_.height) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageBelow_.render_texture.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'below' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAbove_.render_texture.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'above' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAnim_.render_texture.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'anim' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenImageAvatar_.render_texture.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'avatar' texture.");

        // set the initial position of what is drawn on-screen from the off-screen texture
        offScreenTextureRect_.left
            = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeVI_.x);

        offScreenTextureRect_.top = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeVI_.y);
        offScreenTextureRect_.width = onScreenRect_.width;
        offScreenTextureRect_.height = onScreenRect_.height;

        // the Below and Above VertexArrays always have the same single quad contents
        VertexArrayQuadAppend(
            offScreenImageBelow_.vertex_array, onScreenRect_, offScreenTextureRect_);

        VertexArrayQuadAppend(
            offScreenImageAbove_.vertex_array, onScreenRect_, offScreenTextureRect_);
    }

    void MapDisplay::MakeAndAppendTileDraw(
        const Layer & LAYER,
        const sf::Texture * TEXTURE_PTR,
        const int TEXTURE_TILE_NUMBER,
        const sf::Vector2i & TILE_INDEXES)
    {
        const auto TEXTURE_WIDTH { static_cast<int>(TEXTURE_PTR->getSize().x) };
        const auto TEXTURE_WIDTH_IN_TILES { TEXTURE_WIDTH / tileSizeVI_.x };

        const int TEXTURE_RECT_TILE_LEFT { (TEXTURE_TILE_NUMBER % TEXTURE_WIDTH_IN_TILES) };
        const int TEXTURE_RECT_TILE_TOP { (TEXTURE_TILE_NUMBER / TEXTURE_WIDTH_IN_TILES) };

        const int TEXTURE_RECT_LEFT { (TEXTURE_RECT_TILE_LEFT * tileSizeVI_.x) };
        const int TEXTURE_RECT_TOP { (TEXTURE_RECT_TILE_TOP * tileSizeVI_.y) };

        const auto TEXTURE_RECT { sf::IntRect(
            sf::Vector2i(TEXTURE_RECT_LEFT, TEXTURE_RECT_TOP), tileSizeVI_) };

        tileDraws_.emplace_back(
            TileDraw(LAYER.index, LAYER.type, TILE_INDEXES, TEXTURE_PTR, TEXTURE_RECT));
    }

    void MapDisplay::AnalyzeLayers(const map::Layout & LAYOUT)
    {
        tileDraws_.clear();

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

                    if ((TEXTURE_INDEX < LAYOUT.texture_vec.size()))
                    {
                        const sf::Texture * texturePtr { &LAYOUT.texture_vec[TEXTURE_INDEX].Get() };
                        const auto TEXTURE_TILE_NUMBER { (LAYER_TILE_NUMBER - FIRST_ID) };
                        MakeAndAppendTileDraw(LAYER, texturePtr, TEXTURE_TILE_NUMBER, TILE_INDEXES);
                    }
                }
            }
        }

        // other code in this class depends on the tileDraws_ being sorted by layer/texture
        std::sort(std::begin(tileDraws_), std::end(tileDraws_));

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

    const sf::FloatRect MapDisplay::VertexArrayQuadOnScreenPos(
        const sf::VertexArray & VERTEX_ARRAY, const std::size_t QUAD_INDEX) const
    {
        const auto FIRST_VERTEX_INDEX { QUAD_INDEX * VERTS_PER_QUAD_ };

        if ((FIRST_VERTEX_INDEX + (VERTS_PER_QUAD_ - 1)) >= VERTEX_ARRAY.getVertexCount())
        {
            return sf::FloatRect();
        }

        const auto WIDTH { (
            VERTEX_ARRAY[FIRST_VERTEX_INDEX + 1].position.x
            - VERTEX_ARRAY[FIRST_VERTEX_INDEX].position.x) };

        const auto HEIGHT { (
            VERTEX_ARRAY[FIRST_VERTEX_INDEX + 3].position.y
            - VERTEX_ARRAY[FIRST_VERTEX_INDEX].position.y) };

        return { VERTEX_ARRAY[FIRST_VERTEX_INDEX].position, sf::Vector2f(WIDTH, HEIGHT) };
    }

    const sf::FloatRect MapDisplay::VertexArrayQuadTextureCoords(
        const sf::VertexArray & VERTEX_ARRAY, const std::size_t QUAD_INDEX) const
    {
        const auto FIRST_VERTEX_INDEX { QUAD_INDEX * VERTS_PER_QUAD_ };

        if ((FIRST_VERTEX_INDEX + (VERTS_PER_QUAD_ - 1)) >= VERTEX_ARRAY.getVertexCount())
        {
            return sf::FloatRect();
        }

        const auto WIDTH { (
            VERTEX_ARRAY[FIRST_VERTEX_INDEX + 1].texCoords.x
            - VERTEX_ARRAY[FIRST_VERTEX_INDEX].texCoords.x) };

        const auto HEIGHT { (
            VERTEX_ARRAY[FIRST_VERTEX_INDEX + 3].texCoords.y
            - VERTEX_ARRAY[FIRST_VERTEX_INDEX].texCoords.y) };

        return { VERTEX_ARRAY[FIRST_VERTEX_INDEX].texCoords, sf::Vector2f(WIDTH, HEIGHT) };
    }

    void MapDisplay::VertexArrayQuadOnScreenPos(
        sf::Vertex quadVertexes[VERTS_PER_QUAD_], const sf::FloatRect & ONSCREEN_RECT)
    {
        quadVertexes[0].position = sfutil::TopLeft(ONSCREEN_RECT);
        quadVertexes[1].position = sfutil::TopRight(ONSCREEN_RECT);
        quadVertexes[2].position = sfutil::BottomRight(ONSCREEN_RECT);
        quadVertexes[3].position = sfutil::BottomLeft(ONSCREEN_RECT);
    }

    void MapDisplay::VertexArrayQuadTextureCoords(
        sf::Vertex quadVertexes[VERTS_PER_QUAD_], const sf::FloatRect & TEXTURE_RECT)
    {
        quadVertexes[0].texCoords = sfutil::TopLeft(TEXTURE_RECT);
        quadVertexes[1].texCoords = sfutil::TopRight(TEXTURE_RECT);
        quadVertexes[2].texCoords = sfutil::BottomRight(TEXTURE_RECT);
        quadVertexes[3].texCoords = sfutil::BottomLeft(TEXTURE_RECT);
    }

    void MapDisplay::VertexArrayQuadAppend(
        sf::VertexArray & vertexArray,
        const sf::FloatRect & ONSCREEN_RECT,
        const sf::FloatRect & TEXTURE_RECT)
    {
        const auto FIRST_VERTEX_INDEX { vertexArray.getVertexCount() };

        for (std::size_t count(0); count < VERTS_PER_QUAD_; ++count)
        {
            vertexArray.append(sf::Vertex());
        }

        VertexArrayQuadOnScreenPos(&vertexArray[FIRST_VERTEX_INDEX], ONSCREEN_RECT);
        VertexArrayQuadTextureCoords(&vertexArray[FIRST_VERTEX_INDEX], TEXTURE_RECT);
    }

} // namespace map
} // namespace heroespath
