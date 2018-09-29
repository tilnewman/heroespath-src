// ----------------------------------------------------------------------------
// "THE BEER-WARE RICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_BOX_ENTITY_INFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_BOX_ENTITY_INFO_HPP_INCLUDED
//
// box-entity-info.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/color-set.hpp"
#include "sfml-util/corner-enum.hpp"
#include "sfml-util/side-enum.hpp"
#include "sfutil/color.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/Color.hpp>

#include <memory>
#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for wraping all the information needed to consruct a Background entity except
    // for position.  The default constructor will result in a valid object that when used to
    // create a Background entity will also result in a valid obect that will draw nothing.
    struct BoxEntityInfo
    {
        // a background that draws nothing
        BoxEntityInfo();

        // simple solid color if COLOR_TO==sf::Color::Transparent||COLOR_FROM, otherwise a
        // gradient
        explicit BoxEntityInfo(
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO = sf::Color::Transparent,
            const Side::Enum SIDES_WITH_FROM_COLOR = Side::None,
            const Corner::Enum CORNERS_WITH_FROM_COLOR = Corner::None);

        // gui gold bars border if LINE_THICKNESS <= zero, otherwise a border of lines
        explicit BoxEntityInfo(
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION,
            const float LINE_THICKNESS = 0.0f,
            const sf::Color & LINE_COLOR = sf::Color::White,
            const sf::Color & PAD_LINE_COLOR_ADJ = sf::Color::Transparent);

        // if CachedTexture.Options().option_enum&ImageOpt::Repeat then the resulting sprite
        // will be configured to repeat within the full inner region
        explicit BoxEntityInfo(
            const CachedTexture & CACHED_TEXTURE,
            const float SQUARE_SIZE, // ignored if <= zero
            const IntRectOpt_t & TEXTURE_RECT_OPT = boost::none);

        // if CachedTexture.Options().option_enum&ImageOpt::Repeat then the resulting sprite
        // will be configured to repeat within the full inner region
        explicit BoxEntityInfo(
            const CachedTexture & CACHED_TEXTURE,
            const sf::Vector2f & TARGET_SIZE_V = sf::Vector2f(0.0f, 0.0f), // ignored if <= zero
            const IntRectOpt_t & TEXTURE_RECT_OPT = boost::none,
            const bool WILL_SIZE_INSTEAD_OF_FIT = false);

        BoxEntityInfo(const BoxEntityInfo &) = default;
        BoxEntityInfo(BoxEntityInfo &&) = default;
        BoxEntityInfo & operator=(const BoxEntityInfo &) = default;
        BoxEntityInfo & operator=(BoxEntityInfo &&) = default;

        // simple solid color if COLOR_TO==sf::Color::Transparent||COLOR_FROM, otherwise a
        // gradient
        void SetupColor(
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO = sf::Color::Transparent,
            const Side::Enum SIDES_WITH_FROM_COLOR = Side::None,
            const Corner::Enum CORNERS_WITH_FROM_COLOR = Corner::None);

        // gui gold bars border if LINE_THICKNESS <= zero, otherwise a border of lines
        void SetupBorder(
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION,
            const float LINE_THICKNESS = 0.0f,
            const sf::Color & LINE_COLOR = sf::Color::White,
            const sf::Color & PAD_LINE_COLOR_ADJ = sf::Color::Transparent);

        // if CachedTexture.Options().option_enum&ImageOpt::Repeat then the resulting sprite
        // will be configured to repeat within the full inner region, if TARGET_WIDTH is <= zero
        // then the scale of the image is not changed
        void SetupImage(
            const CachedTexture & CACHED_TEXTURE,
            const float SQUARE_SIZE,
            const IntRectOpt_t & TEXTURE_RECT_OPT = boost::none);

        // if CachedTexture.Options().option_enum&ImageOpt::Repeat then the resulting sprite
        // will be configured to repeat within the full inner region, if any value of
        // TARGET_SIZE_V is zero or less then that dimmension is ignored
        void SetupImage(
            const CachedTexture & CACHED_TEXTURE,
            const sf::Vector2f & TARGET_SIZE_V = sf::Vector2f(0.0f, 0.0f),
            const IntRectOpt_t & TEXTURE_RECT_OPT = boost::none,
            const bool WILL_SIZE_INSTEAD_OF_FIT = false);

        bool WillDraw() const { return HasAny(); }

        bool HasAny() const { return (HasBorder() || HasColor() || HasImage()); }

        bool HasBorder() const { return will_draw_border; }

        bool HasColor() const { return (HasColorSolid() || HasColorGradient()); }

        bool HasColorSolid() const
        {
            return ((sf::Color::Transparent != color_from) && (color_from == color_to));
        }

        bool HasColorGradient() const
        {
            return (
                (color_from != color_to)
                && ((Side::None != color_from_sides) || (Corner::None != color_from_corners)));
        }

        bool HasImage() const { return !!cached_texture_opt; }

        bool IsImageRepeated() const
        {
            return (HasImage() && (cached_texture_opt->Options().option_enum & ImageOpt::Repeated));
        }

        sf::Color color_from;
        sf::Color color_to;
        Side::Enum color_from_sides;
        Corner::Enum color_from_corners;
        bool will_draw_border;
        bool will_grow_border;
        float line_thickness;
        sf::Color line_color;
        sf::Color pad_line_color_adj;
        CachedTextureOpt_t cached_texture_opt;
        IntRectOpt_t texture_rect_opt;
        bool will_size_instead_of_fit;
        FocusColors focus_colors;
        bool will_draw_color_over_sprite;
        sf::Vector2f image_size;
    };

    using BoxEntityInfoOpt_t = boost::optional<BoxEntityInfo>;

    inline bool operator<(const BoxEntityInfo & L, const BoxEntityInfo & R)
    {
        return std::tie(
                   L.color_from,
                   L.color_to,
                   L.color_from_sides,
                   L.color_from_corners,
                   L.will_draw_border,
                   L.will_grow_border,
                   L.line_thickness,
                   L.line_color,
                   L.pad_line_color_adj,
                   L.cached_texture_opt,
                   L.texture_rect_opt,
                   L.will_size_instead_of_fit,
                   L.focus_colors,
                   L.will_draw_color_over_sprite,
                   L.image_size)
            < std::tie(
                   R.color_from,
                   R.color_to,
                   R.color_from_sides,
                   R.color_from_corners,
                   R.will_draw_border,
                   R.will_grow_border,
                   R.line_thickness,
                   R.line_color,
                   R.pad_line_color_adj,
                   R.cached_texture_opt,
                   R.texture_rect_opt,
                   R.will_size_instead_of_fit,
                   R.focus_colors,
                   R.will_draw_color_over_sprite,
                   R.image_size);
    }

    inline bool operator==(const BoxEntityInfo & L, const BoxEntityInfo & R)
    {
        return std::tie(
                   L.color_from,
                   L.color_to,
                   L.color_from_sides,
                   L.color_from_corners,
                   L.will_draw_border,
                   L.will_grow_border,
                   L.line_thickness,
                   L.line_color,
                   L.pad_line_color_adj,
                   L.cached_texture_opt,
                   L.texture_rect_opt,
                   L.will_size_instead_of_fit,
                   L.focus_colors,
                   L.will_draw_color_over_sprite,
                   L.image_size)
            == std::tie(
                   R.color_from,
                   R.color_to,
                   R.color_from_sides,
                   R.color_from_corners,
                   R.will_draw_border,
                   R.will_grow_border,
                   R.line_thickness,
                   R.line_color,
                   R.pad_line_color_adj,
                   R.cached_texture_opt,
                   R.texture_rect_opt,
                   R.will_size_instead_of_fit,
                   R.focus_colors,
                   R.will_draw_color_over_sprite,
                   R.image_size);
    }

    inline bool operator!=(const BoxEntityInfo & L, const BoxEntityInfo & R) { return !(L == R); }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_BOX_ENTITY_INFO_HPP_INCLUDED
