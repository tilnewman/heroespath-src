// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_IMAGE_ENTITY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_IMAGE_ENTITY_HPP_INCLUDED
//
// image-entity.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gui/entity.hpp"
#include "sfml-util/gui/mouse-image-info.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for being an Entity that has different images for different MouseStates.
    // Use this class when you need different images for different MouseStates.  The
    // global bounds of each sprite could be different, but so far I have not found a situation
    // where that is a good idea.  Sync() keeeps entityRegion_ equal to the current sprite's
    // global bounds, which might be different if the sprites do not line up.  If they don't,
    // you can use MinimallyEnclosingRegion() to find the rect that contains them all.
    class ImageEntity : public Entity
    {
    public:
        explicit ImageEntity(
            const std::string & NAME, const MouseImageInfo & MOUSE_IMAGE_INFO = MouseImageInfo());

        ImageEntity(
            const std::string & NAME,
            const FloatRectOpt_t & REGION_OPT,
            const sfml_util::CachedTextureOpt_t & TEXTURE_UP_OPT = boost::none,
            const sfml_util::CachedTextureOpt_t & TEXTURE_DOWN_OPT = boost::none,
            const sfml_util::CachedTextureOpt_t & TEXTURE_OVER_OPT = boost::none,
            const sfml_util::CachedTextureOpt_t & TEXTURE_DISABLED_OPT = boost::none,
            const bool WILL_DRAW_UP_IF_MISSING = false,
            const ColorOpt_t & COLOR_OPT = boost::none,
            const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION = false);

        virtual ~ImageEntity();

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        virtual void Color(const sf::Color & COLOR = sf::Color::White);

        virtual void Color(
            const ColorOpt_t & COLOR_UP_OPT = boost::none,
            const ColorOpt_t & COLOR_DOWN_OPT = boost::none,
            const ColorOpt_t & COLOR_OVER_OPT = boost::none,
            const ColorOpt_t & COLOR_DISABLED_OPT = boost::none);

        EntityImageInfo & Up() { return mouseImageInfo_.up; }
        const EntityImageInfo & Up() const { return mouseImageInfo_.up; }

        EntityImageInfo & Down() { return mouseImageInfo_.down; }
        const EntityImageInfo & Down() const { return mouseImageInfo_.down; }

        EntityImageInfo & Over() { return mouseImageInfo_.over; }
        const EntityImageInfo & Over() const { return mouseImageInfo_.over; }

        EntityImageInfo & Disabled() { return mouseImageInfo_.disabled; }
        const EntityImageInfo & Disabled() const { return mouseImageInfo_.disabled; }

        EntityImageInfo & FromMouseState(const MouseState::Enum MOUSE_STATE)
        {
            return mouseImageInfo_.FromMouseState(MOUSE_STATE);
        }

        const EntityImageInfo & FromMouseState(const MouseState::Enum MOUSE_STATE) const
        {
            return mouseImageInfo_.FromMouseState(MOUSE_STATE);
        }

        EntityImageInfo & FromMouseState() { return FromMouseState(GetMouseState()); }

        const EntityImageInfo & FromMouseState() const { return FromMouseState(GetMouseState()); }

        // does not actually set the position, instead it moves relative to GetEntityPos()
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

        void MoveEntityPos(const float HORIZ, const float VERT) override;

        void SetEntityRegion(const sf::FloatRect & R) override;

        // returns GetEntityRegion() unless the global bounds of all sprites are not equal
        const sf::FloatRect MinimallyEnclosingRegion() const
        {
            return mouseImageInfo_.MinimallyEnclosingRegion();
        }

        // if returns true then the mouse state changed and Sync() was called
        bool SetMouseState(const MouseState::Enum) override;

        // call after changing mouseImageInfo_ or mouse state
        virtual void Sync();

        bool WillDraw() const { return willDraw_; }
        void WillDraw(const bool WILL_DRAW) { willDraw_ = WILL_DRAW; }

    private:
        MouseImageInfo mouseImageInfo_;
        sf::Sprite sprite_;
        bool willDraw_;
    };

    using ImageEntityPtr_t = misc::NotNull<ImageEntity *>;
    using ImageEntityUPtr_t = std::unique_ptr<ImageEntity>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_IMAGE_ENTITY_HPP_INCLUDED
