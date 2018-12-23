// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_BOX_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_BOX_ENTITY_HPP_INCLUDED
//
// box-entity.hpp
//
#include "gui/border.hpp"
#include "gui/box-entity-info.hpp"
#include "gui/colored-rect.hpp"
#include "gui/image-entity.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "sfutil/sprite-texture.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <tuple>

namespace heroespath
{
namespace gui
{

    using SpriteOpt_t = boost::optional<sf::Sprite>;

    // Responsible for implementing an Entity that is a simple box on screen with a customizable
    // border and background.  Wraps any combination of an image, a border, and a solid color or
    // gradient background. The default constructor results in a valid object that will draw
    // nothing. The REGION provided might grow if the Border is configured to. If there is a
    // Border then the ColoredRect's region will be set to Border.InnerRegion().  If there is an
    // image then the sprite will be scaled or fit to the ColoredRect's region, but if there is
    // none then Border.InnerRegion() will be used.  If the CachedTexture's ImageOptions include
    // Repeat, then the sprite will be configured to tile the entire inner region.  This class
    // is designed to act as a background to other Entity's, and responds to focus color changes
    // accordingly.  When using BoxEntity this way, simply have the owning/creating entity add
    // it to the Stage on construction and remove it upon destruction.
    class BoxEntity : public Entity
    {
    public:
        explicit BoxEntity(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const BoxEntityInfo & BACKGROUND_INFO);

        BoxEntity(const BoxEntity &) = default;
        BoxEntity(BoxEntity &&) = default;
        BoxEntity & operator=(const BoxEntity &) = default;
        BoxEntity & operator=(BoxEntity &&) = default;

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        const BoxEntityInfo & Info() const { return backgroundInfo_; }

        // if you don't want to change the region then set BACKGROUND_INFO.will_grow_border to
        // false and REGION=GetEntityRegion()
        void Setup(const sf::FloatRect & REGION, const BoxEntityInfo & BACKGROUND_INFO);

        // does not actually set the position, instead it moves relative to GetEntityPos()
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

        void MoveEntityPos(const float HORIZ, const float VERT) override;

        void SetEntityRegion(const sf::FloatRect & REGION) override;

        void OnColorChange() override;

        // same as GetEntityRegion()
        const sf::FloatRect OuterRegion() const { return GetEntityRegion(); }

        // same as OuterRegion() if there is no border
        const sf::FloatRect InnerRegion() const { return innerRegion_; }

        // friend bool operator<(const BoxEntity & L, const BoxEntity & R);
        // friend bool operator==(const BoxEntity & L, const BoxEntity & R);

    private:
        BoxEntityInfo backgroundInfo_;
        Border border_;
        ColoredRect coloredRect_;
        sf::Sprite sprite_;
        bool willDrawImage_;
        sf::FloatRect innerRegion_;
    };

    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

    // inline bool operator<(const BoxEntity & L, const BoxEntity & R)
    //{
    //    return std::tie(
    //               L.backgroundInfo_,
    //               L.border_,
    //               L.coloredRect_,
    //               L.sprite_,
    //               L.willDrawImage_,
    //               L.innerRegion_)
    //        < std::tie(
    //               R.backgroundInfo_,
    //               R.border_,
    //               R.coloredRect_,
    //               R.sprite_,
    //               R.willDrawImage_,
    //               R.innerRegion_);
    //}
    //
    // inline bool operator==(const BoxEntity & L, const BoxEntity & R)
    //{
    //    return std::tie(
    //               L.backgroundInfo_,
    //               L.border_,
    //               L.coloredRect_,
    //               L.sprite_,
    //               L.willDrawImage_,
    //               L.innerRegion_)
    //        == std::tie(
    //               R.backgroundInfo_,
    //               R.border_,
    //               R.coloredRect_,
    //               R.sprite_,
    //               R.willDrawImage_,
    //               R.innerRegion_);
    //}
    //
    // inline bool operator!=(const BoxEntity & L, const BoxEntity & R) { return !(L == R); }

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_BOX_ENTITY_HPP_INCLUDED
