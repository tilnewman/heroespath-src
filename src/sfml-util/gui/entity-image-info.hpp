// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_ENTITY_IMAGE_INFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_ENTITY_IMAGE_INFO_HPP_INCLUDED
//
// entity-image-info.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/sfml-util-color.hpp"
#include "sfml-util/sfml-util-sprite-texture.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for wrapping all the information required by a Entity to store and draw an
        // image.
        struct EntityImageInfo : public sf::Drawable
        {
            EntityImageInfo(
                const CachedTextureOpt_t & TEXTURE_OPT = boost::none,
                const FloatRectOpt_t & REGION_OPT = boost::none,
                const IntRectOpt_t & TEXTURE_REGION_OPT = boost::none,
                const ColorOpt_t & COLOR_OPT = boost::none,
                const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION = false);

            EntityImageInfo(
                const CachedTexture & TEXTURE,
                const Vector2fOpt_t & POS_V_OPT,
                const Vector2fOpt_t & SCALE_V_OPT = sf::Vector2f(1.0f, 1.0f),
                const IntRectOpt_t & TEXTURE_REGION_OPT = boost::none,
                const ColorOpt_t & COLOR_OPT = boost::none,
                const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION = false);

            EntityImageInfo(const EntityImageInfo &) = default;
            EntityImageInfo(EntityImageInfo &&) = default;
            EntityImageInfo & operator=(const EntityImageInfo &) = default;
            EntityImageInfo & operator=(EntityImageInfo &&) = default;

            void Setup(
                const CachedTexture & TEXTURE,
                const Vector2fOpt_t & POS_V_OPT,
                const Vector2fOpt_t & SCALE_V_OPT = sf::Vector2f(1.0f, 1.0f),
                const IntRectOpt_t & TEXTURE_REGION_OPT = boost::none,
                const ColorOpt_t & COLOR_OPT = boost::none,
                const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION = false);

            void Setup(
                const CachedTextureOpt_t & TEXTURE_OPT = boost::none,
                const FloatRectOpt_t & REGION_OPT = boost::none,
                const IntRectOpt_t & TEXTURE_REGION_OPT = boost::none,
                const ColorOpt_t & COLOR_OPT = boost::none,
                const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION = false);

            bool HasTexture() const { return (sprite.getTexture() != nullptr); }

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override
            {
                if (HasTexture())
                {
                    target.draw(sprite, states);
                }
            }

            const std::string ToString(
                const bool WILL_PREFIX = true, const misc::Wrap WILL_WRAP = misc::Wrap::Yes) const;

            void SetRegion(const sf::FloatRect & NEW_GLOBAL_BOUNDS);

            CachedTextureOpt_t cached_texture_opt;
            sf::Sprite sprite;
            bool will_resize_instead_of_fit;
        };

        using EntityImageInfoOpt_t = boost::optional<EntityImageInfo>;

        inline bool operator<(const EntityImageInfo & L, const EntityImageInfo & R)
        {
            return std::tie(L.cached_texture_opt, L.sprite, L.will_resize_instead_of_fit)
                < std::tie(R.cached_texture_opt, R.sprite, R.will_resize_instead_of_fit);
        }

        inline bool operator==(const EntityImageInfo & L, const EntityImageInfo & R)
        {
            return std::tie(L.cached_texture_opt, L.sprite, L.will_resize_instead_of_fit)
                == std::tie(R.cached_texture_opt, R.sprite, R.will_resize_instead_of_fit);
        }

        inline bool operator!=(const EntityImageInfo & L, const EntityImageInfo & R)
        {
            return !(R == L);
        }

    } // namespace gui
} // namespace sfml_util

inline std::ostream & operator<<(std::ostream & os, const sfml_util::gui::EntityImageInfo & EII)
{
    os << EII.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_ENTITY_IMAGE_INFO_HPP_INCLUDED
