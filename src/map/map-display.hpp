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
#include "map/layer.hpp"
#include "map/layout.hpp"
#include "map/map-anim.hpp"
#include "map/shadow-masker.hpp"
#include "map/tile-offsets.hpp"
#include "map/tiles-panel.hpp"
#include "misc/types.hpp"
#include "sfml-util/animation-base.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/collision-quad-tree.hpp"
#include "sfml-util/direction-enum.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    class Map;

    // Encapsulates a tiled map, along with the player's position.
    class MapDisplay : public sf::Drawable
    {
    public:
        MapDisplay(const MapDisplay &) = delete;
        MapDisplay(MapDisplay &&) = delete;
        MapDisplay & operator=(const MapDisplay &) = delete;
        MapDisplay & operator=(MapDisplay &&) = delete;

    public:
        MapDisplay(const Map & MAP, const sf::FloatRect & REGION);
        virtual ~MapDisplay();

        void Load(const sf::Vector2f & STARTING_POS_V, const MapAnimVec_t &);

        bool Move(const sfml_util::Direction::Enum, const float ADJUSTMENT);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        const sf::Vector2f PlayerPosMap() const { return playerPosV_ + playerPosOffsetV_; }

        Layout & GetLayoutRef() { return layout_; }

        void Update(const float TIME_ELAPSED);
        void UpdateAnimMusicVolume();

    private:
        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        void DrawNormal(sf::RenderTarget &, sf::RenderStates) const;
        void DrawDebug(sf::RenderTarget &, sf::RenderStates) const;
        void DrawCharacterImages();
        void UpdateAndDrawAnimations(const float TIME_ELAPSED);
        void ReDraw(const float TIME_ELAPSED);
        void ResetMapSubsections();
        void DrawMapSubsectionOffscreen();

        void PositionMapSpriteTextureRect();

        void SetupOffScreenTexture();

        void SetupMapLayerSubsection(Layer &, const TileOffsets &);

        const TileOffsets TileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const;

        const sf::Vector2f PlayerPosScreen() const;

        const sf::Vector2f ScreenPosFromMapPos(const sf::Vector2f &) const;
        const sf::Vector2f OffScreenPosFromMapPos(const sf::Vector2f &) const;

        const TilesPanel & TilesPanelFromId(const int) const;

        void IncrementTileOffsetsInDirection(const sfml_util::Direction::Enum);

        const sf::Vector2f CalcOffScreenMapSize() const;

        void SetupAnimations();

        void StartAnimMusic();
        void StopAnimMusic();

        float CalcAnimationVolume(const float DISTANCE_TO_PLAYER) const;

        void SetupNPCShadowImage();

    public:
        // how many extra tiles to draw offscreen that are outside the visible map area
        static const int EXTRA_OFFSCREEN_TILE_COUNT_;
        static const std::size_t VERTS_PER_QUAD_;

    private:
        const Map & MAP_;

        // This is how close the player position can get to
        // the edge of the map before being forced to stop.
        const float BORDER_PAD_;

        const sf::Vector2f WIN_POS_V_;
        const sf::Vector2f WIN_SIZE_V_;
        const sf::Vector2f WIN_CENTER_V_;

        // These distances are in offscreen coordinates.
        const float ANIM_SFX_DISTANCE_MIN_;
        const float ANIM_SFX_DISTANCE_MAX_;
        const float ANIM_SFX_VOLUME_MIN_RATIO_;

        Layout layout_;
        TileOffsets tileOffsets_;

        // these two variables are in centered map coordinates
        sf::Vector2f playerPosV_;
        sf::Vector2f playerPosOffsetV_;

        sf::FloatRect offScreenRect_;
        sf::Sprite offScreenSpriteAbove_;
        sf::Sprite offScreenSpriteBelow_;
        sf::RenderTexture offScreenTextureAbove_;
        sf::RenderTexture offScreenTextureBelow_;
        sf::Vector2f offScreenMapSize_;
        sf::Texture npcShadowTexture_;
        sf::Sprite npcShadowSprite_;

        MapAnimVec_t animInfoVec_;
        std::vector<sfml_util::AnimationUPtr_t> animUPtrVec_;

        ShadowMasker shadowMasker_;
    };

    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
