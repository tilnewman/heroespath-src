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
        : WIN_POS_V_(sf::Vector2f(REGION.left, REGION.top))
        , WIN_SIZE_V_(sf::Vector2f(REGION.width, REGION.height))
        , WIN_RECT_(WIN_POS_V_, WIN_SIZE_V_)
        , WIN_RECT_INNER_(gui::Margins(sfutil::MapByRes(50.0f, 500.0f)).ApplyShrinkCopy(WIN_RECT_))
        , ANIM_SFX_DISTANCE_MIN_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-distance-min"))
        , ANIM_SFX_DISTANCE_MAX_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-distance-max"))
        , ANIM_SFX_VOLUME_MIN_RATIO_(misc::ConfigFile::Instance()->ValueOrDefault<float>(
              "heroespath-sound-map-animsfx-min-volume-ratio"))
        , layout_()
        , tileOffsets_()
        , playerPosV_(0.0f, 0.0f)
        , playerPosOffsetV_(0.0f, 0.0f)
        , offScreenRect_(0.0f, 0.0f, 0.0f, 0.0f)
        , offScreenSpriteAbove_()
        , offScreenSpriteBelow_()
        , offScreenTextureAbove_()
        , offScreenTextureBelow_()
        , offScreenMapSize_(0.0f, 0.0f)
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
        , timeTrials_("MapDisplay", TimeRes::Micro, true, 100, 0.0)
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

        StopAnimMusic();
        animInfoVec_ = ANIM_INFO_VEC;
        tileOffsets_ = map::TileOffsets();
        playerPosOffsetV_ = sf::Vector2f(0.0f, 0.0f);
        playerPosV_ = STARTING_POS_V;
        tileOffsets_ = TileOffsetsFromMapPos(STARTING_POS_V);
        offScreenMapSize_ = CalcOffScreenMapSize();
        SetupOffScreenTexture();
        SetupAnimations();
        ResetMapSubsections();
        ReDraw(0.0f);
        StartAnimMusic();
        UpdateAnimMusicVolume();

        AnalyzeLayers();
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
        DrawNormal(target, states);
    }

    void MapDisplay::Update(const float TIME_ELAPSED) { ReDraw(TIME_ELAPSED); }

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
        if (PlayerPosScreen().y > WIN_RECT_INNER_.top)
        {
            playerPosOffsetV_.y -= ADJUSTMENT;
            return true;
        }

        if (offScreenRect_.top > 0.0f)
        {
            const auto OFFSCREEN_TOP_BEFORE { offScreenRect_.top };
            offScreenRect_.top = std::max(0.0f, offScreenRect_.top - ADJUSTMENT);

            const auto ACTUAL_ADJUSTMENT { OFFSCREEN_TOP_BEFORE - offScreenRect_.top };

            PositionMapSpriteTextureRect();
            playerPosV_.y = std::max(0.0f, playerPosV_.y - ACTUAL_ADJUSTMENT);

            if (misc::IsRealZero(offScreenRect_.top))
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
        if (PlayerPosScreen().y < sfutil::Bottom(WIN_RECT_INNER_))
        {
            playerPosOffsetV_.y += ADJUSTMENT;
            return true;
        }

        const auto VERT_POS {
            offScreenRect_.top + static_cast<float>(tileOffsets_.begin_v.y * layout_.tile_size_v.y)
        };

        const auto VERT_LIMIT { static_cast<float>(layout_.tile_count_v.y * layout_.tile_size_v.y)
                                - static_cast<float>(WIN_SIZE_V_.y) };

        if (VERT_POS >= VERT_LIMIT)
        {
            return false;
        }

        offScreenRect_.top += ADJUSTMENT;
        PositionMapSpriteTextureRect();
        playerPosV_.y += ADJUSTMENT;

        if ((offScreenRect_.top + offScreenRect_.height) > offScreenMapSize_.y)
        {
            IncrementTileOffsetsInDirection(gui::Direction::Down);
        }

        return true;
    }

    bool MapDisplay::MoveLeft(const float ADJUSTMENT)
    {
        // move within the view before moving the view
        if (PlayerPosScreen().x > WIN_RECT_INNER_.left)
        {
            playerPosOffsetV_.x -= ADJUSTMENT;
            return true;
        }

        if (offScreenRect_.left > 0.0f)
        {
            const auto OFFSCREEN_LEFT_BEFORE { offScreenRect_.left };
            offScreenRect_.left = std::max(0.0f, offScreenRect_.left - ADJUSTMENT);

            const auto ACTUAL_ADJUSTMENT { OFFSCREEN_LEFT_BEFORE - offScreenRect_.left };

            PositionMapSpriteTextureRect();
            playerPosV_.x = std::max(0.0f, playerPosV_.x - ACTUAL_ADJUSTMENT);

            if (misc::IsRealZero(offScreenRect_.left))
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
        if (PlayerPosScreen().x < sfutil::Right(WIN_RECT_INNER_))
        {
            playerPosOffsetV_.x += ADJUSTMENT;
            return true;
        }

        const auto HORIZ_POS { (
            offScreenRect_.left
            + static_cast<float>(tileOffsets_.begin_v.x * layout_.tile_size_v.x)) };

        const auto HORIZ_LIMIT { static_cast<float>(
            (layout_.tile_count_v.x * layout_.tile_size_v.x) - static_cast<int>(WIN_SIZE_V_.x)) };

        if (HORIZ_POS >= HORIZ_LIMIT)
        {
            return false;
        }

        offScreenRect_.left += ADJUSTMENT;
        PositionMapSpriteTextureRect();
        playerPosV_.x += ADJUSTMENT;

        if ((offScreenRect_.left + offScreenRect_.width) > offScreenMapSize_.x)
        {
            IncrementTileOffsetsInDirection(gui::Direction::Right);
        }

        return true;
    }

    void MapDisplay::DrawNormal(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(offScreenSpriteBelow_, states);
        target.draw(offScreenSpriteAbove_, states);
    }

    void MapDisplay::DrawDebug(sf::RenderTarget & target, sf::RenderStates) const
    {
        sf::Sprite offScreenSpriteBelow(offScreenTextureBelow_.getTexture());
        offScreenSpriteBelow.setPosition(WIN_POS_V_);
        target.draw(offScreenSpriteBelow);

        sf::Sprite offScreenSpriteAbove(offScreenTextureAbove_.getTexture());
        offScreenSpriteAbove.setPosition(WIN_POS_V_);
        target.draw(offScreenSpriteAbove);

        /*sf::FloatRect offScreenInnerWindowRect(
            WIN_POS_V_.x + offScreenRect_.left,
            WIN_POS_V_.y + offScreenRect_.top,
            offScreenRect_.width,
            offScreenRect_.height);

        target.draw(sfutil::MakeRectangle(offScreenInnerWindowRect), states);*/
    }

    void MapDisplay::DrawCharacterImages()
    {
        avatarSpriteIndexes_.clear();

        // fill the indexes vector but only with indexes to avatars that are on screen
        const auto AVATAR_SPRITE_COUNT { avatarSprites_.size() };
        for (std::size_t index(0); index < AVATAR_SPRITE_COUNT; ++index)
        {
            const auto AVATAR_ONSCREEN_RECT { ScreenRectFromMapRect(
                avatarSprites_.at(index).getGlobalBounds()) };

            if (WIN_RECT_.intersects(AVATAR_ONSCREEN_RECT))
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
            offScreenTextureBelow_.draw(npcShadowSprite_);
        }

        // draw all the avatar sprites
        for (const auto INDEX : avatarSpriteIndexes_)
        {
            offScreenTextureBelow_.draw(avatarSprites_.at(INDEX));
        }
    }

    void MapDisplay::UpdateAndDrawAnimations(const float TIME_ELAPSED)
    {
        for (auto & animUPtr : animUPtrVec_)
        {
            if (TIME_ELAPSED > 0.0f)
            {
                animUPtr->UpdateTime(TIME_ELAPSED);
            }

            animUPtr->SetEntityPos(OffScreenPosFromMapPos(animUPtr->GetEntityPos()));
            offScreenTextureAbove_.draw(*animUPtr);
        }
    }

    void MapDisplay::ReDraw(const float TIME_ELAPSED)
    {
        DrawMapSubsectionOffscreen();
        UpdateAndDrawAnimations(TIME_ELAPSED);
        DrawCharacterImages();

        offScreenTextureBelow_.display();
        offScreenTextureAbove_.display();

        if (timeTrials_.ContestCount() >= 4)
        {
            timeTrials_.EndAllContests();
        }
    }

    void MapDisplay::ResetMapSubsections()
    {
        // reset the old way
        for (auto & nextMapLayer : layout_.layer_vec)
        {
            nextMapLayer.ResetForReDraw();
            SetupMapLayerSubsection(nextMapLayer, tileOffsets_);
        }

        ResetMapSubsectionsNew();
    }

    void MapDisplay::ResetMapSubsectionsNew()
    {
        // reset the new way
        tileVertexVecBelow_.clear();
        tileVertexVecAbove_.clear();

        auto appendQuad = [&](sf::VertexArray & vertexArray, const TileDraw & TILE_DRAW) {
            const sf::Vector2i MAP_POS_V(
                (TILE_DRAW.tile_index_v.x * layout_.tile_size_v.x),
                (TILE_DRAW.tile_index_v.y * layout_.tile_size_v.y));

            const sf::Vector2f OFFSCREEN_MAP_POS_V { OffScreenPosFromMapPos(
                sf::Vector2f { MAP_POS_V }) };

            const sf::Vector2f MAP_SIZE_V { layout_.tile_size_v };
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
            if ((TILE_DRAW.tile_index_v.x < tileOffsets_.begin_v.x)
                || (TILE_DRAW.tile_index_v.x > tileOffsets_.end_v.x)
                || (TILE_DRAW.tile_index_v.y < tileOffsets_.begin_v.y)
                || (TILE_DRAW.tile_index_v.y > tileOffsets_.end_v.y))
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

        std::ostringstream ss;
        ss << "ResetMapSubsections Below:";
        std::size_t count { 0 };
        for (auto & textureVertexsPair : tileVertexVecBelow_)
        {
            ss << "\n\t#" << ++count << " " << textureVertexsPair.first << " "
               << textureVertexsPair.first->getSize() << " with "
               << (textureVertexsPair.second.getVertexCount() / VERTS_PER_QUAD_)
               << " tile draws (%="
               << (textureVertexsPair.second.getVertexCount() % VERTS_PER_QUAD_) << ")";
        }
        M_HP_LOG_WRN(ss.str());

        ss.str("");
        ss << "ResetMapSubsections Above:";
        count = 0;
        for (auto & textureVertexsPair : tileVertexVecAbove_)
        {
            ss << "\n\t#" << ++count << " " << textureVertexsPair.first << " "
               << textureVertexsPair.first->getSize() << " with "
               << (textureVertexsPair.second.getVertexCount() / VERTS_PER_QUAD_)
               << " tile draws (%="
               << (textureVertexsPair.second.getVertexCount() % VERTS_PER_QUAD_) << ")";
        }
        M_HP_LOG_WRN(ss.str());
    }

    void MapDisplay::DrawMapSubsectionOffscreen()
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

    void MapDisplay::PositionMapSpriteTextureRect()
    {
        const sf::IntRect INT_RECT { offScreenRect_ };
        offScreenSpriteBelow_.setTextureRect(INT_RECT);
        offScreenSpriteAbove_.setTextureRect(INT_RECT);
    }

    void MapDisplay::SetupOffScreenTexture()
    {
        const auto EXTRA_WIDTH { layout_.tile_size_v.x * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

        const auto WIDTH { static_cast<unsigned>(WIN_SIZE_V_.x)
                           + static_cast<unsigned>(EXTRA_WIDTH) };

        const auto EXTRA_HEIGHT { layout_.tile_size_v.y * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

        const auto HEIGHT { static_cast<unsigned>(WIN_SIZE_V_.y)
                            + static_cast<unsigned>(EXTRA_HEIGHT) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTextureBelow_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'below' texture.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            offScreenTextureAbove_.create(WIDTH, HEIGHT),
            "gui::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'above' texture.");

        // set the initial position of what is drawn on-screen from the off-screen texture
        offScreenRect_.width = WIN_SIZE_V_.x;
        offScreenRect_.height = WIN_SIZE_V_.y;

        offScreenRect_.left
            = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * layout_.tile_size_v.x);

        offScreenRect_.top
            = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * layout_.tile_size_v.y);

        offScreenSpriteBelow_.setTexture(offScreenTextureBelow_.getTexture());
        offScreenSpriteAbove_.setTexture(offScreenTextureAbove_.getTexture());

        offScreenSpriteBelow_.setPosition(WIN_POS_V_);
        offScreenSpriteAbove_.setPosition(WIN_POS_V_);

        PositionMapSpriteTextureRect();
    }

    void MapDisplay::SetupMapLayerSubsection(
        map::Layer & mapLayer, const map::TileOffsets & TILE_OFFSETS)
    {
        // resize the vertex array to fit the level size
        const auto TILE_WIDTH { TILE_OFFSETS.end_v.x - TILE_OFFSETS.begin_v.x };
        const auto TILE_HEIGHT { TILE_OFFSETS.end_v.y - TILE_OFFSETS.begin_v.y };
        mapLayer.vert_array.setPrimitiveType(sf::Quads);

        mapLayer.vert_array.resize(
            static_cast<std::size_t>(TILE_WIDTH * TILE_HEIGHT) * VERTS_PER_QUAD_);

        mapLayer.tiles_panel_for_layers_vec.reserve(
            static_cast<std::size_t>(TILE_WIDTH * TILE_HEIGHT));

        const auto EMPTY_TILES_PANEL_NAME_STR { TilesPanel().name };

        // populate the vertex array with one quad per tile
        std::size_t vertIndex { 0 };
        for (int x(TILE_OFFSETS.begin_v.x); x < TILE_OFFSETS.end_v.x; ++x)
        {
            for (int y(TILE_OFFSETS.begin_v.y); y < TILE_OFFSETS.end_v.y; ++y)
            {
                // get the current tile number
                const auto TILE_INDEX { x + (y * layout_.tile_count_v.x) };

                const auto TILE_NUM_ORIG {
                    mapLayer.mapid_vec[static_cast<std::size_t>(TILE_INDEX)]
                };

                // get the texture/image this tile can be found in
                const map::TilesPanel & TILES_PANEL { TilesPanelFromId(TILE_NUM_ORIG) };

                mapLayer.tiles_panel_for_layers_vec.emplace_back(map::TilesPanelForLayers(
                    (TILES_PANEL.name == EMPTY_TILES_PANEL_NAME_STR), TILES_PANEL.texture_index));

                // get the actual tile number (id)
                const auto TILE_NUM_FINAL { (TILE_NUM_ORIG - TILES_PANEL.first_id) };

                // get a pointer to the current tile's quad
                const auto VERT_ARRAY_INDEX { vertIndex++ * VERTS_PER_QUAD_ };
                sf::Vertex * quadPtr { &mapLayer.vert_array[VERT_ARRAY_INDEX] };

                // define its four corners on screen
                const auto POS_WIDTH { static_cast<float>(
                    (x - TILE_OFFSETS.begin_v.x) * layout_.tile_size_v.x) };

                const auto POS_WIDTH_PLUS_ONE_TILE { POS_WIDTH
                                                     + static_cast<float>(layout_.tile_size_v.x) };

                const auto POS_HEIGHT { static_cast<float>(
                    (y - TILE_OFFSETS.begin_v.y) * layout_.tile_size_v.y) };

                const auto POS_HEIGHT_PLUS_ONE_TILE { POS_HEIGHT
                                                      + static_cast<float>(layout_.tile_size_v.y) };

                quadPtr[0].position = sf::Vector2f(POS_WIDTH, POS_HEIGHT);

                quadPtr[1].position = sf::Vector2f(POS_WIDTH_PLUS_ONE_TILE, POS_HEIGHT);

                quadPtr[2].position
                    = sf::Vector2f(POS_WIDTH_PLUS_ONE_TILE, POS_HEIGHT_PLUS_ONE_TILE);

                quadPtr[3].position = sf::Vector2f(POS_WIDTH, POS_HEIGHT_PLUS_ONE_TILE);

                // find its position in the tileset texture
                const auto TEXTURE_TILE_COUNT_HORIZ {
                    static_cast<int>(
                        layout_.texture_vec[TILES_PANEL.texture_index].Get().getSize().x)
                    / layout_.tile_size_v.x
                };

                const auto TILE_COUNT_U { static_cast<float>(
                    TILE_NUM_FINAL % TEXTURE_TILE_COUNT_HORIZ) };

                const auto TILE_COUNT_V { static_cast<float>(
                    TILE_NUM_FINAL / TEXTURE_TILE_COUNT_HORIZ) };

                const auto TEXTCOORD_WIDTH { TILE_COUNT_U
                                             * static_cast<float>(layout_.tile_size_v.x) };

                const auto TEXTCOORD_WIDTH_PLUS_ONE_TILE {
                    TEXTCOORD_WIDTH + static_cast<float>(layout_.tile_size_v.x)
                };

                const auto TEXTCOORD_HEIGHT { TILE_COUNT_V
                                              * static_cast<float>(layout_.tile_size_v.y) };

                const auto TEXTCOORD_HEIGHT_PLUS_ONE_TILE {
                    TEXTCOORD_HEIGHT + static_cast<float>(layout_.tile_size_v.y)
                };

                // define its four texture coordinates
                quadPtr[0].texCoords = sf::Vector2f(TEXTCOORD_WIDTH, TEXTCOORD_HEIGHT);

                quadPtr[1].texCoords
                    = sf::Vector2f(TEXTCOORD_WIDTH_PLUS_ONE_TILE, TEXTCOORD_HEIGHT);

                quadPtr[2].texCoords
                    = sf::Vector2f(TEXTCOORD_WIDTH_PLUS_ONE_TILE, TEXTCOORD_HEIGHT_PLUS_ONE_TILE);

                quadPtr[3].texCoords
                    = sf::Vector2f(TEXTCOORD_WIDTH, TEXTCOORD_HEIGHT_PLUS_ONE_TILE);
            }
        }
    }

    void MapDisplay::AnalyzeLayers()
    {
        tileDraws_.clear();

        const auto EMPTY_TILES_PANEL_NAME_STR { TilesPanel().name };

        std::size_t layerIndex { 0 };
        for (auto & mapLayer : layout_.layer_vec)
        {
            for (int x(0); x < layout_.tile_count_v.x; ++x)
            {
                for (int y(0); y < layout_.tile_count_v.y; ++y)
                {
                    // map tile number
                    const auto TILE_NUMBER_IN_MAP { mapLayer.mapid_vec[static_cast<std::size_t>(
                        x + (y * layout_.tile_count_v.x))] };

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

                    const auto TEXTURE_WIDTH_IN_TILES { TEXTURE_WIDTH / layout_.tile_size_v.x };

                    const int TEXTURE_RECT_TILE_LEFT { (
                        TILE_NUMBER_IN_TEXTURE % TEXTURE_WIDTH_IN_TILES) };

                    const int TEXTURE_RECT_TILE_TOP { (
                        TILE_NUMBER_IN_TEXTURE / TEXTURE_WIDTH_IN_TILES) };

                    const int TEXTURE_RECT_LEFT { (
                        TEXTURE_RECT_TILE_LEFT * layout_.tile_size_v.x) };

                    const int TEXTURE_RECT_TOP { (TEXTURE_RECT_TILE_TOP * layout_.tile_size_v.y) };

                    const auto TEXTURE_RECT { sf::IntRect(
                        sf::Vector2i(TEXTURE_RECT_LEFT, TEXTURE_RECT_TOP), layout_.tile_size_v) };

                    //

                    tileDraws_.emplace_back(TileDraw(
                        layerIndex, mapLayer.type, sf::Vector2i(x, y), texturePtr, TEXTURE_RECT));
                }
            }

            ++layerIndex;
        }

        std::sort(std::begin(tileDraws_), std::end(tileDraws_));

        const auto COUNT_ORIG { tileDraws_.size() };

        tileDraws_.erase(
            std::unique(std::begin(tileDraws_), std::end(tileDraws_)), std::end(tileDraws_));

        const auto COUNT { tileDraws_.size() };

        M_HP_LOG_WRN(
            "\t" << COUNT << "  TileDraws (" << (COUNT_ORIG - COUNT)
                 << " duplicates removed) (tiles_panel_count=" << layout_.tiles_panel_vec.size()
                 << ")(map_tile_count=" << (layout_.tile_count_v.x * layout_.tile_count_v.y)
                 << ", layer_count=" << layout_.layer_vec.size() << ", total_tiles_count="
                 << (static_cast<std::size_t>(layout_.tile_count_v.x * layout_.tile_count_v.y)
                     * layout_.layer_vec.size())
                 << ")");

        misc::VectorMap<
            const sf::Texture *,
            std::pair<std::vector<sf::IntRect>, std::vector<sf::Vector2i>>>
            textureMap;

        for (const auto & TILE_DRAW : tileDraws_)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((TILE_DRAW.texture_rect.width == layout_.tile_size_v.x)
                 && (TILE_DRAW.texture_rect.height == layout_.tile_size_v.y)),
                "TILE_DRAW.texture_rect=" << TILE_DRAW.texture_rect
                                          << " had an invalid size that should have been "
                                          << layout_.tile_size_v << ".");

            if (((TILE_DRAW.texture_rect.left % layout_.tile_size_v.x) != 0)
                || ((TILE_DRAW.texture_rect.top % layout_.tile_size_v.y) != 0))
            {
                M_HP_LOG_WRN("Invalid texture rect position=" << TILE_DRAW.texture_rect);
            }

            textureMap[TILE_DRAW.texture_ptr].first.emplace_back(TILE_DRAW.texture_rect);
            textureMap[TILE_DRAW.texture_ptr].second.emplace_back(TILE_DRAW.tile_index_v);
        }

        std::size_t unique { 0 };
        std::size_t sum { 0 };
        std::ostringstream ss;
        std::size_t count { 0 };
        for (auto & infoPair : textureMap)
        {
            ss << "\n\t#" << ++count << " Texture size=" << infoPair.first->getSize()
               << ", map_tile_count=" << infoPair.second.second.size();

            std::sort(std::begin(infoPair.second.first), std::end(infoPair.second.first));
            std::sort(std::begin(infoPair.second.second), std::end(infoPair.second.second));

            infoPair.second.second.erase(
                std::unique(std::begin(infoPair.second.second), std::end(infoPair.second.second)),
                std::end(infoPair.second.second));

            ss << " (" << infoPair.second.second.size() << " unique) ("
               << (layout_.tile_count_v.x * layout_.tile_count_v.y)
               << " total), coords_count=" << infoPair.second.first.size();

            infoPair.second.first.erase(
                std::unique(std::begin(infoPair.second.first), std::end(infoPair.second.first)),
                std::end(infoPair.second.first));

            const auto UNIQUE { infoPair.second.first.size() };
            unique += UNIQUE;

            ss << " (" << UNIQUE << " unique)";

            const auto TOTAL { (
                (infoPair.first->getSize().x / static_cast<unsigned>(layout_.tile_size_v.x))
                * (infoPair.first->getSize().y / static_cast<unsigned>(layout_.tile_size_v.y))) };
            sum += TOTAL;

            const auto RATIO_OF_COVERAGE { (
                static_cast<double>(UNIQUE) / static_cast<double>(TOTAL)) };

            ss << " (" << TOTAL << " total)";

            ss << "(" << std::setprecision(3) << (RATIO_OF_COVERAGE * 100.0) << "% coverage)";
        }

        ss << "\n\tTotals: unique=" << unique << ", sum=" << sum << ", " << std::setprecision(3)
           << ((static_cast<double>(unique) / static_cast<double>(sum)) * 100.0) << "%)";

        M_HP_LOG_WRN(
            "Texture Summary (tile_size=" << layout_.tile_size_v << ")(tile_counts="
                                          << layout_.tile_count_v << "):" << ss.str());

        misc::VectorMap<sf::Vector2i, std::size_t> posCountMap;
        for (const auto & TILE_DRAW : tileDraws_)
        {
            posCountMap[TILE_DRAW.tile_index_v]++;
        }

        misc::VectorMap<std::size_t, std::vector<sf::Vector2i>> countPosMap;
        for (const auto & POS_COUNT_PAIR : posCountMap)
        {
            countPosMap[POS_COUNT_PAIR.second].emplace_back(POS_COUNT_PAIR.first);
        }

        ss.str("");
        ss << "Position Draw Counts (" << countPosMap.Size() << "):";
        for (const auto & COUNT_POS_PAIR : countPosMap)
        {
            ss << "\n\t" << COUNT_POS_PAIR.first << "\t" << COUNT_POS_PAIR.second.size();
        }

        M_HP_LOG_WRN(ss.str());

        ResetMapSubsectionsNew();
    }

    const map::TileOffsets MapDisplay::TileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        map::TileOffsets offsets;

        {
            const auto PLAYER_POS_IN_TILES_X { static_cast<int>(MAP_POS_V.x)
                                               / layout_.tile_size_v.x };

            const auto WINDOW_TILE_SIZE_X { (
                static_cast<int>(WIN_SIZE_V_.x) / layout_.tile_size_v.x) };

            const auto OFFSCREEN_TILE_SIZE_X { WINDOW_TILE_SIZE_X
                                               + (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

            offsets.begin_v.x = std::max(0, (PLAYER_POS_IN_TILES_X - (OFFSCREEN_TILE_SIZE_X / 2)));
            offsets.end_v.x = offsets.begin_v.x + OFFSCREEN_TILE_SIZE_X;

            if (offsets.end_v.x > layout_.tile_count_v.x)
            {
                offsets.end_v.x = layout_.tile_count_v.x;
                offsets.begin_v.x = offsets.end_v.x - OFFSCREEN_TILE_SIZE_X;
            }
        }

        {
            const auto PLAYER_POS_IN_TILES_Y { static_cast<int>(MAP_POS_V.y)
                                               / layout_.tile_size_v.y };

            const auto WINDOW_TILE_SIZE_Y { (
                static_cast<int>(WIN_SIZE_V_.y) / layout_.tile_size_v.y) };

            const auto OFFSCREEN_TILE_SIZE_Y { WINDOW_TILE_SIZE_Y
                                               + (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

            offsets.begin_v.y = std::max(0, (PLAYER_POS_IN_TILES_Y - (OFFSCREEN_TILE_SIZE_Y / 2)));
            offsets.end_v.y = offsets.begin_v.y + OFFSCREEN_TILE_SIZE_Y;

            if (offsets.end_v.y > layout_.tile_count_v.y)
            {
                offsets.end_v.y = layout_.tile_count_v.y;
                offsets.begin_v.y = offsets.end_v.y - OFFSCREEN_TILE_SIZE_Y;
            }
        }

        return offsets;
    }

    const sf::Vector2f MapDisplay::PlayerPosScreen() const
    {
        return ScreenPosFromMapPos(PlayerPosMap());
    }

    const sf::Vector2f MapDisplay::ScreenPosFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        const auto SCREEN_LEFT { (WIN_POS_V_.x - offScreenRect_.left) };
        const auto SCREEN_TOP { (WIN_POS_V_.y - offScreenRect_.top) };
        const sf::Vector2f SCREEN_POS_V(SCREEN_LEFT, SCREEN_TOP);

        const auto MAP_OFFSET_LEFT {
            MAP_POS_V.x - static_cast<float>(layout_.tile_size_v.x * tileOffsets_.begin_v.x)
        };

        const auto MAP_OFFSET_TOP {
            MAP_POS_V.y - static_cast<float>(layout_.tile_size_v.y * tileOffsets_.begin_v.y)
        };

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
        const sf::Vector2f OS_TEXTURE_POS_V(
            static_cast<float>(tileOffsets_.begin_v.x * layout_.tile_size_v.x),
            static_cast<float>(tileOffsets_.begin_v.y * layout_.tile_size_v.y));

        const sf::Vector2f OS_TEXTURE_OFFSET_V(
            static_cast<float>(offScreenSpriteAbove_.getTextureRect().left),
            static_cast<float>(offScreenSpriteAbove_.getTextureRect().top));

        const sf::Vector2f OS_RECT_POS_V(offScreenRect_.left, offScreenRect_.top);

        return MAP_POS_V - ((OS_TEXTURE_POS_V + OS_TEXTURE_OFFSET_V) - OS_RECT_POS_V);
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

    const map::TilesPanel & MapDisplay::TilesPanelFromId(const int ID) const
    {
        for (const auto & TILES_PANEL : layout_.tiles_panel_vec)
        {
            if (TILES_PANEL.OwnsId(ID))
            {
                return TILES_PANEL;
            }
        }

        std::ostringstream ss;
        ss << "gui::MapDisplay::TilesPanelFromId(id=" << ID
           << ") failed to find the owning TilesPanel.";

        throw std::runtime_error(ss.str());
    }

    void MapDisplay::IncrementTileOffsetsInDirection(const gui::Direction::Enum DIR)
    {
        auto wereChangesMade { false };

        const auto TILE_SIZE_X { static_cast<float>(layout_.tile_size_v.x) };
        const auto TILE_SIZE_Y { static_cast<float>(layout_.tile_size_v.y) };

        switch (DIR)
        {
            case gui::Direction::Up:
            {
                if (tileOffsets_.begin_v.y > 0)
                {
                    --tileOffsets_.begin_v.y;
                    --tileOffsets_.end_v.y;
                    offScreenRect_.top += TILE_SIZE_Y;
                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Down:
            {
                if (tileOffsets_.end_v.y < layout_.tile_count_v.y)
                {
                    ++tileOffsets_.begin_v.y;
                    ++tileOffsets_.end_v.y;
                    offScreenRect_.top = std::max(0.0f, offScreenRect_.top - TILE_SIZE_Y);
                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Left:
            {
                if (tileOffsets_.begin_v.x > 0)
                {
                    --tileOffsets_.begin_v.x;
                    --tileOffsets_.end_v.x;
                    offScreenRect_.left += TILE_SIZE_Y;
                    wereChangesMade = true;
                }
                break;
            }

            case gui::Direction::Right:
            {
                if (tileOffsets_.end_v.x < layout_.tile_count_v.x)
                {
                    ++tileOffsets_.begin_v.x;
                    ++tileOffsets_.end_v.x;
                    offScreenRect_.left = std::max(0.0f, offScreenRect_.left - TILE_SIZE_X);
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
            PositionMapSpriteTextureRect();
            ResetMapSubsections();
            ReDraw(0.0f);
        }
    }

    const sf::Vector2f MapDisplay::CalcOffScreenMapSize() const
    {
        const auto WIDTH { (tileOffsets_.end_v.x - tileOffsets_.begin_v.x)
                           * layout_.tile_size_v.x };

        const auto HEIGHT { (tileOffsets_.end_v.y - tileOffsets_.begin_v.y)
                            * layout_.tile_size_v.y };

        return sf::Vector2f(static_cast<float>(WIDTH), static_cast<float>(HEIGHT));
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
} // namespace map
} // namespace heroespath
