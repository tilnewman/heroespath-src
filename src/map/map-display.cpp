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

    const int MapDisplay::EXTRA_OFFSCREEN_TILE_COUNT_ { 2 };
    const std::size_t MapDisplay::VERTS_PER_QUAD_ { 4 };

    MapDisplay::MapDisplay(const sf::FloatRect & REGION)
        : onScreenRect_(REGION)
        , onScreenRectInner_(gui::Margins(sfutil::MapByRes(50.0f, 500.0f)).ApplyShrinkCopy(REGION))
        , offScreenRectI_()
        , offScreenTileRectI_()
        , offScreenFollowRect_()
        , mapRectI_()
        , mapTileRectI_()
        , tileSizeVI_()
        , ANIM_SFX_DISTANCE_MIN_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-distance-min"))
        , ANIM_SFX_DISTANCE_MAX_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-distance-max"))
        , ANIM_SFX_VOLUME_MIN_RATIO_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-min-volume-ratio"))
        , layout_()
        , playerPosV_(0.0f, 0.0f)
        , playerPosOffsetV_(0.0f, 0.0f)
        , offScreenVertexArray_(sf::Quads, VERTS_PER_QUAD_)
        , offScreenTextureBelow_()
        , offScreenTextureAnim_()
        , offScreenTextureAvatar_()
        , offScreenTextureAbove_()
        , npcShadowCachedTexture_(
              "media-images-avatar-shadow",
              (gui::ImageOpt::Smooth | gui::ImageOpt::ShadowMaskForShadowImage))
        , npcShadowSprite_(npcShadowCachedTexture_.Get())
        , animInfoVec_()
        , animUPtrVec_()
        , avatarSprites_()
        , avatarSpriteIndexes_()
        , tileDraws_()
        , tileVertexVecBelow_()
        , tileVertexVecAbove_()
        , timeTrials_("MapDisplay", TimeRes::Micro, true, 20, 0.0)
        , timeTrialIndexRedDrawSub_(timeTrials_.AddCollecter("SubSection"))
    {
        avatarSprites_.reserve(64);
        avatarSpriteIndexes_.reserve(64);
        tileDraws_.reserve(50000);
        tileVertexVecBelow_.reserve(64);
        tileVertexVecAbove_.reserve(64);
    }

    MapDisplay::~MapDisplay() { StopAnimMusic(); }

    void MapDisplay::Load(const sf::Vector2f & STARTING_POS_V, const MapAnimVec_t & ANIM_INFO_VEC)
    {
        timeTrials_.EndAllContests();

        // set player position in map coordinates
        playerPosOffsetV_ = sf::Vector2f(0.0f, 0.0f);
        playerPosV_ = STARTING_POS_V;

        // tile size
        tileSizeVI_ = layout_.tile_size_v;

        // set map coordinates
        mapRectI_
            = sf::IntRect(sf::Vector2i(), sfutil::VectorMult(layout_.tile_count_v, tileSizeVI_));

        mapTileRectI_ = sf::IntRect(sf::Vector2i(), layout_.tile_count_v);

        // set offscreen coordinates
        offScreenRectI_ = CalcOffscreenRect(onScreenRect_, tileSizeVI_);

        offScreenTileRectI_ = CalcOffscreenTileSubRectForMapPos(
            offScreenRectI_, PlayerPosMap(), sfutil::Size(mapTileRectI_));

        SetupOffScreenTextures();

        StopAnimMusic();

        animInfoVec_ = ANIM_INFO_VEC;
        SetupAnimations();

        StartAnimMusic();
        UpdateAnimMusicVolume();

        AnalyzeLayers();
        ResetMapSubsections();
        ReDraw();
    }

    bool MapDisplay::Move(const gui::Direction::Enum DIR, const float ADJUSTMENT)
    {
        switch (DIR)
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
        states.texture = &offScreenTextureBelow_.getTexture();
        target.draw(offScreenVertexArray_, states);

        states.texture = &offScreenTextureAnim_.getTexture();
        target.draw(offScreenVertexArray_, states);

        states.texture = &offScreenTextureAvatar_.getTexture();
        target.draw(offScreenVertexArray_, states);

        states.texture = &offScreenTextureAbove_.getTexture();
        target.draw(offScreenVertexArray_, states);

        states.texture = nullptr;
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

        DrawAnimations();
        DrawCharacterImages();
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

    bool MapDisplay::MoveUp(const float ADJUSTMENT)
    {
        // move within the view before moving the view
        if (PlayerPosScreen().y > onScreenRectInner_.top)
        {
            playerPosOffsetV_.y -= ADJUSTMENT;
            return true;
        }

        if (offScreenFollowRect_.top > 0.0f)
        {
            const auto OFFSCREEN_TOP_BEFORE { offScreenFollowRect_.top };
            offScreenFollowRect_.top = std::max(0.0f, offScreenFollowRect_.top - ADJUSTMENT);

            const auto ACTUAL_ADJUSTMENT { OFFSCREEN_TOP_BEFORE - offScreenFollowRect_.top };

            UpdateOffscreenTextureCoords();
            playerPosV_.y = std::max(0.0f, (playerPosV_.y - ACTUAL_ADJUSTMENT));

            if (misc::IsRealZero(offScreenFollowRect_.top))
            {
                IncrementTileOffsetsInDirection(gui::Direction::Up);
            }

            return true;
        }

        return false;
    }

    bool MapDisplay::MoveDown(const float ADJUSTMENT)
    {
        // move within the view before moving the view
        if (PlayerPosScreen().y < sfutil::Bottom(onScreenRectInner_))
        {
            playerPosOffsetV_.y += ADJUSTMENT;
            return true;
        }

        const auto VERT_POS { offScreenFollowRect_.top
                              + static_cast<float>(offScreenTileRectI_.top * tileSizeVI_.y) };

        const auto VERT_LIMIT { static_cast<float>(mapRectI_.height) - onScreenRect_.height };

        if (VERT_POS >= VERT_LIMIT)
        {
            return false;
        }

        offScreenFollowRect_.top += ADJUSTMENT;
        UpdateOffscreenTextureCoords();
        playerPosV_.y += ADJUSTMENT;

        if (sfutil::Bottom(offScreenFollowRect_) > sfutil::Bottom(offScreenRectI_))
        {
            IncrementTileOffsetsInDirection(gui::Direction::Down);
        }

        return true;
    }

    bool MapDisplay::MoveLeft(const float ADJUSTMENT)
    {
        // move within the view before moving the view
        if (PlayerPosScreen().x > onScreenRectInner_.left)
        {
            playerPosOffsetV_.x -= ADJUSTMENT;
            return true;
        }

        if (offScreenFollowRect_.left > 0.0f)
        {
            const auto OFFSCREEN_LEFT_BEFORE { offScreenFollowRect_.left };
            offScreenFollowRect_.left = std::max(0.0f, offScreenFollowRect_.left - ADJUSTMENT);

            const auto ACTUAL_ADJUSTMENT { OFFSCREEN_LEFT_BEFORE - offScreenFollowRect_.left };

            UpdateOffscreenTextureCoords();
            playerPosV_.x = std::max(0.0f, (playerPosV_.x - ACTUAL_ADJUSTMENT));

            if (misc::IsRealZero(offScreenFollowRect_.left))
            {
                IncrementTileOffsetsInDirection(gui::Direction::Left);
            }

            return true;
        }

        return false;
    }

    bool MapDisplay::MoveRight(const float ADJUSTMENT)
    {
        // move within the view before moving the view
        if (PlayerPosScreen().x < sfutil::Right(onScreenRectInner_))
        {
            playerPosOffsetV_.x += ADJUSTMENT;
            return true;
        }

        const auto HORIZ_POS { (
            offScreenFollowRect_.left
            + static_cast<float>(offScreenTileRectI_.left * tileSizeVI_.x)) };

        const auto HORIZ_LIMIT { static_cast<float>(
            (mapRectI_.width) - static_cast<int>(onScreenRect_.width)) };

        if (HORIZ_POS >= HORIZ_LIMIT)
        {
            return false;
        }

        offScreenFollowRect_.left += ADJUSTMENT;
        UpdateOffscreenTextureCoords();
        playerPosV_.x += ADJUSTMENT;

        if (sfutil::Right(offScreenFollowRect_) > sfutil::Right(offScreenRectI_))
        {
            IncrementTileOffsetsInDirection(gui::Direction::Right);
        }

        return true;
    }

    void MapDisplay::DrawCharacterImages()
    {
        offScreenTextureAvatar_.clear(sf::Color::Transparent);

        avatarSpriteIndexes_.clear();

        // fill the indexes vector but only with indexes to avatars that are on screen
        const auto AVATAR_SPRITE_COUNT { avatarSprites_.size() };
        for (std::size_t index(0); index < AVATAR_SPRITE_COUNT; ++index)
        {
            const auto AVATAR_ONSCREEN_RECT { ScreenRectFromMapRect(
                avatarSprites_.at(index).getGlobalBounds()) };

            if (onScreenRect_.intersects(AVATAR_ONSCREEN_RECT))
            {
                avatarSpriteIndexes_.emplace_back(index);
            }
        }

        if (avatarSpriteIndexes_.empty())
        {
            return;
        }

        // sort indexes by draw order
        if (avatarSpriteIndexes_.size() > 1)
        {
            std::sort(
                std::begin(avatarSpriteIndexes_),
                std::end(avatarSpriteIndexes_),
                [&](const auto INDEX_A, const auto INDEX_B) {
                    return (
                        sfutil::Bottom(avatarSprites_.at(INDEX_A).getGlobalBounds())
                        < sfutil::Bottom(avatarSprites_.at(INDEX_B).getGlobalBounds()));
                });
        }

        // convert avatar sprite positions from map coordinates to offscreen coordinates
        for (const auto INDEX : avatarSpriteIndexes_)
        {
            auto & sprite { avatarSprites_.at(INDEX) };
            sprite.setPosition(OffScreenPosFromMapPos(sprite.getPosition()));
        }

        // draw all the avatar shadows on the ground
        for (const auto INDEX : avatarSpriteIndexes_)
        {
            const auto & AVATAR_SPRITE { avatarSprites_.at(INDEX) };
            npcShadowSprite_.setPosition(AVATAR_SPRITE.getPosition());
            npcShadowSprite_.setScale(AVATAR_SPRITE.getScale());
            offScreenTextureAvatar_.draw(npcShadowSprite_);
        }

        // draw all the avatar sprites
        for (const auto INDEX : avatarSpriteIndexes_)
        {
            offScreenTextureAvatar_.draw(avatarSprites_.at(INDEX));
        }

        offScreenTextureAvatar_.display();
    }

    void MapDisplay::DrawAnimations()
    {
        offScreenTextureAnim_.clear(sf::Color::Transparent);

        for (auto & animUPtr : animUPtrVec_)
        {
            animUPtr->SetEntityPos(OffScreenPosFromMapPos(animUPtr->GetEntityPos()));

            const auto OFFSCREEN_RECT { animUPtr->GetEntityRegion() };
            if (offScreenFollowRect_.intersects(OFFSCREEN_RECT))
            {
                offScreenTextureAnim_.draw(*animUPtr);
            }
        }

        offScreenTextureAnim_.display();
    }

    void MapDisplay::ReDraw()
    {
        DrawMapBelowAndAbove();
        DrawAnimations();
        DrawCharacterImages();

        if (timeTrials_.ContestCount() >= 4)
        {
            timeTrials_.EndAllContests();
        }
    }

    void MapDisplay::ResetMapSubsections()
    {
        // reset the new way
        tileVertexVecBelow_.clear();
        tileVertexVecAbove_.clear();

        auto appendQuad = [&](sf::VertexArray & vertexArray, const TileDraw & TILE_DRAW) {
            const sf::Vector2i MAP_POS_V(
                (TILE_DRAW.tile_index_v.x * tileSizeVI_.x),
                (TILE_DRAW.tile_index_v.y * tileSizeVI_.y));

            const sf::Vector2f OFFSCREEN_MAP_POS_V { OffScreenPosFromMapPos(
                sf::Vector2f { MAP_POS_V }) };

            const sf::Vector2f MAP_SIZE_V { tileSizeVI_ };
            const sf::FloatRect MAP_RECT(OFFSCREEN_MAP_POS_V, MAP_SIZE_V);

            const sf::FloatRect TEXTURE_RECT { TILE_DRAW.texture_rect };

            vertexArray.append(
                sf::Vertex(sfutil::TopLeft(MAP_RECT), sfutil::TopLeft(TEXTURE_RECT)));

            vertexArray.append(
                sf::Vertex(sfutil::TopRight(MAP_RECT), sfutil::TopRight(TEXTURE_RECT)));

            vertexArray.append(
                sf::Vertex(sfutil::BottomRight(MAP_RECT), sfutil::BottomRight(TEXTURE_RECT)));

            vertexArray.append(
                sf::Vertex(sfutil::BottomLeft(MAP_RECT), sfutil::BottomLeft(TEXTURE_RECT)));
        };

        // this loop assumes tileDraws_ sorted by layer/layer_type/texture
        const sf::Texture * currentTexturePtr { nullptr };
        for (const auto & TILE_DRAW : tileDraws_)
        {
            // TODO CLENAUP
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

    void MapDisplay::DrawMapBelowAndAbove()
    {
        M_HP_SCOPED_TIME_TRIAL(timeTrials_, timeTrialIndexRedDrawSub_);

        offScreenTextureBelow_.clear(sf::Color::Transparent);
        offScreenTextureAbove_.clear(sf::Color::Transparent);

        sf::RenderStates renderStates { sf::RenderStates::Default };

        for (const auto & TEXTURE_VERTS_PAIR : tileVertexVecBelow_)
        {
            if (TEXTURE_VERTS_PAIR.second.getVertexCount() > 0)
            {
                renderStates.texture = TEXTURE_VERTS_PAIR.first;

                offScreenTextureBelow_.draw(
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

                offScreenTextureAbove_.draw(
                    &TEXTURE_VERTS_PAIR.second[0],
                    TEXTURE_VERTS_PAIR.second.getVertexCount(),
                    sf::Quads,
                    renderStates);
            }
        }

        offScreenTextureBelow_.display();
        offScreenTextureAbove_.display();
    }

    void MapDisplay::UpdateOffscreenTextureCoords()
    {
        OffScreenVertexTextureRect(offScreenFollowRect_);
    }

    void MapDisplay::SetupOffScreenTextures()
    {
        const auto WIDTH { static_cast<unsigned>(offScreenRectI_.width) };
        const auto HEIGHT { static_cast<unsigned>(offScreenRectI_.height) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTextureBelow_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'below' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTextureAbove_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'above' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTextureAnim_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'anim' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTextureAvatar_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'avatar' texture.");

        // set the initial position of what is drawn on-screen from the off-screen texture
        offScreenFollowRect_.left = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeVI_.x);

        offScreenFollowRect_.top = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeVI_.y);

        offScreenFollowRect_.width = onScreenRect_.width;
        offScreenFollowRect_.height = onScreenRect_.height;

        OffScreenVertexMapRect(onScreenRect_);
        UpdateOffscreenTextureCoords();
    }

    void MapDisplay::AnalyzeLayers()
    {
        tileDraws_.clear();

        const auto EMPTY_TILES_PANEL_NAME_STR { TilesPanel().name };

        std::size_t layerIndex { 0 };
        for (auto & mapLayer : layout_.layer_vec)
        {
            for (int x(0); x < mapTileRectI_.width; ++x)
            {
                for (int y(0); y < mapTileRectI_.height; ++y)
                {
                    // map tile number
                    const auto TILE_NUMBER_IN_MAP {
                        mapLayer.mapid_vec[static_cast<std::size_t>(x + (y * mapTileRectI_.width))]
                    };

                    // texture index and first tile number (first id)
                    const auto INVALID_FIRST_ID { -999 };
                    auto tilesPanelFromId = [&](const int ID) {
                        for (const auto & TILES_PANEL : layout_.tiles_panel_vec)
                        {
                            if (TILES_PANEL.OwnsId(ID))
                            {
                                if (TILES_PANEL.name == EMPTY_TILES_PANEL_NAME_STR)
                                {
                                    break;
                                }
                                else
                                {
                                    return std::make_tuple(
                                        TILES_PANEL.first_id, TILES_PANEL.texture_index);
                                }
                            }
                        }

                        return std::make_tuple(-INVALID_FIRST_ID, layout_.texture_vec.size());
                    };

                    const auto [FIRST_ID, TEXTURE_INDEX] = tilesPanelFromId(TILE_NUMBER_IN_MAP);
                    if ((FIRST_ID == -INVALID_FIRST_ID)
                        || (TEXTURE_INDEX >= layout_.texture_vec.size()))
                    {
                        continue;
                    }

                    // texture image
                    const sf::Texture * texturePtr { &layout_.texture_vec[TEXTURE_INDEX].Get() };

                    // texture coordinates (texture rect)
                    const auto TILE_NUMBER_IN_TEXTURE { (TILE_NUMBER_IN_MAP - FIRST_ID) };

                    const auto TEXTURE_WIDTH { static_cast<int>(texturePtr->getSize().x) };

                    const auto TEXTURE_WIDTH_IN_TILES { TEXTURE_WIDTH / tileSizeVI_.x };

                    const int TEXTURE_RECT_TILE_LEFT { (
                        TILE_NUMBER_IN_TEXTURE % TEXTURE_WIDTH_IN_TILES) };

                    const int TEXTURE_RECT_TILE_TOP { (
                        TILE_NUMBER_IN_TEXTURE / TEXTURE_WIDTH_IN_TILES) };

                    const int TEXTURE_RECT_LEFT { (TEXTURE_RECT_TILE_LEFT * tileSizeVI_.x) };

                    const int TEXTURE_RECT_TOP { (TEXTURE_RECT_TILE_TOP * tileSizeVI_.y) };

                    const auto TEXTURE_RECT { sf::IntRect(
                        sf::Vector2i(TEXTURE_RECT_LEFT, TEXTURE_RECT_TOP), tileSizeVI_) };

                    //

                    tileDraws_.emplace_back(TileDraw(
                        layerIndex, mapLayer.type, sf::Vector2i(x, y), texturePtr, TEXTURE_RECT));
                }
            }

            ++layerIndex;
        }

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

    const sf::Vector2f MapDisplay::PlayerPosScreen() const
    {
        return ScreenPosFromMapPos(PlayerPosMap());
    }

    const sf::Vector2f MapDisplay::ScreenPosFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        const auto SCREEN_LEFT { (onScreenRect_.left - offScreenFollowRect_.left) };
        const auto SCREEN_TOP { (onScreenRect_.top - offScreenFollowRect_.top) };
        const sf::Vector2f SCREEN_POS_V(SCREEN_LEFT, SCREEN_TOP);

        const auto MAP_OFFSET_LEFT {
            MAP_POS_V.x - static_cast<float>(offScreenTileRectI_.left * tileSizeVI_.x)
        };

        const auto MAP_OFFSET_TOP { MAP_POS_V.y
                                    - static_cast<float>(offScreenTileRectI_.top * tileSizeVI_.y) };

        const sf::Vector2f MAP_OFFSET_V(MAP_OFFSET_LEFT, MAP_OFFSET_TOP);

        return SCREEN_POS_V + MAP_OFFSET_V;
    }

    const sf::FloatRect MapDisplay::ScreenRectFromMapRect(const sf::FloatRect & MAP_RECT) const
    {
        const auto SCREEN_TOP_LEFT_POS_V { ScreenPosFromMapPos(sfutil::TopLeft(MAP_RECT)) };
        const auto SCREEN_BOT_RIGHT_POS_V { ScreenPosFromMapPos(sfutil::BottomRight(MAP_RECT)) };

        const sf::Vector2f SCREEN_RECT_SIZE_V(
            (SCREEN_BOT_RIGHT_POS_V.x - SCREEN_TOP_LEFT_POS_V.x),
            (SCREEN_BOT_RIGHT_POS_V.y - SCREEN_TOP_LEFT_POS_V.y));

        return sf::FloatRect(SCREEN_TOP_LEFT_POS_V, SCREEN_RECT_SIZE_V);
    }

    const sf::Vector2f MapDisplay::OffScreenPosFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        const sf::Vector2f TILE_TOP_LEFT_POS_V { sfutil::VectorMult(
            sfutil::Position(offScreenTileRectI_), tileSizeVI_) };

        const auto TEXTURE_OFFSET_V { sfutil::Position(OffScreenVertexTextureRect()) };
        const auto FOLLOWER_POS_V { sfutil::Position(offScreenFollowRect_) };

        return MAP_POS_V - ((TILE_TOP_LEFT_POS_V + TEXTURE_OFFSET_V) - FOLLOWER_POS_V);
    }

    const sf::FloatRect MapDisplay::OffScreenRectFromMapRect(const sf::FloatRect & MAP_RECT) const
    {
        const auto OFFSCREEN_TOP_LEFT_POS_V { OffScreenPosFromMapPos(sfutil::TopLeft(MAP_RECT)) };

        const auto OFFSCREEN_BOT_RIGHT_POS_V { OffScreenPosFromMapPos(
            sfutil::BottomRight(MAP_RECT)) };

        const sf::Vector2f OFFSCREEN_RECT_SIZE_V(
            (OFFSCREEN_BOT_RIGHT_POS_V.x - OFFSCREEN_TOP_LEFT_POS_V.x),
            (OFFSCREEN_BOT_RIGHT_POS_V.y - OFFSCREEN_TOP_LEFT_POS_V.y));

        return sf::FloatRect(OFFSCREEN_TOP_LEFT_POS_V, OFFSCREEN_RECT_SIZE_V);
    }

    void MapDisplay::IncrementTileOffsetsInDirection(const gui::Direction::Enum DIR)
    {
        auto wereChangesMade { false };

        const sf::Vector2f TILE_SIZE_V { tileSizeVI_ };

        switch (DIR)
        {
            case gui::Direction::Up:
            {
                if (offScreenTileRectI_.top > 0)
                {
                    --offScreenTileRectI_.top;
                    offScreenFollowRect_.top += TILE_SIZE_V.y;
                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Down:
            {
                if (sfutil::Bottom(offScreenTileRectI_) < sfutil::Bottom(mapTileRectI_))
                {
                    ++offScreenTileRectI_.top;

                    offScreenFollowRect_.top
                        = std::max(0.0f, offScreenFollowRect_.top - TILE_SIZE_V.y);

                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Left:
            {
                if (offScreenTileRectI_.left > 0)
                {
                    --offScreenTileRectI_.left;
                    offScreenFollowRect_.left += TILE_SIZE_V.x;
                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Right:
            {
                if (sfutil::Right(offScreenTileRectI_) < sfutil::Right(mapTileRectI_))
                {
                    ++offScreenTileRectI_.left;

                    offScreenFollowRect_.left
                        = std::max(0.0f, offScreenFollowRect_.left - TILE_SIZE_V.x);

                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Count:
            default:
            {
                break;
            }
        }

        if (wereChangesMade)
        {
            UpdateOffscreenTextureCoords();
            ResetMapSubsections();
            ReDraw();
        }
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

    const sf::FloatRect
        MapDisplay::SingleQuadVertexArrayPosition(const sf::VertexArray & VERTEX_ARRAY) const
    {
        const auto WIDTH { (VERTEX_ARRAY[1].position.x - VERTEX_ARRAY[0].position.x) };
        const auto HEIGHT { (VERTEX_ARRAY[3].position.y - VERTEX_ARRAY[0].position.y) };
        return { VERTEX_ARRAY[0].position, sf::Vector2f(WIDTH, HEIGHT) };
    }

    const sf::FloatRect
        MapDisplay::SingleQuadVertexArrayTextureCoords(const sf::VertexArray & VERTEX_ARRAY) const
    {
        const auto WIDTH { (VERTEX_ARRAY[1].texCoords.x - VERTEX_ARRAY[0].texCoords.x) };
        const auto HEIGHT { (VERTEX_ARRAY[3].texCoords.y - VERTEX_ARRAY[0].texCoords.y) };
        return { VERTEX_ARRAY[0].texCoords, sf::Vector2f(WIDTH, HEIGHT) };
    }

    void MapDisplay::SingleQuadVertexArrayPosition(
        sf::VertexArray & vertexArray, const sf::FloatRect & RECT)
    {
        vertexArray[0].position = sfutil::TopLeft(RECT);
        vertexArray[1].position = sfutil::TopRight(RECT);
        vertexArray[2].position = sfutil::BottomRight(RECT);
        vertexArray[3].position = sfutil::BottomLeft(RECT);
    }

    void MapDisplay::SingleQuadVertexArrayTextureCoords(
        sf::VertexArray & vertexArray, const sf::FloatRect & RECT)
    {
        vertexArray[0].texCoords = sfutil::TopLeft(RECT);
        vertexArray[1].texCoords = sfutil::TopRight(RECT);
        vertexArray[2].texCoords = sfutil::BottomRight(RECT);
        vertexArray[3].texCoords = sfutil::BottomLeft(RECT);
    }

} // namespace map
} // namespace heroespath
