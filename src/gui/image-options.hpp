// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_IMAGE_OPTIONS_HPP_INCLUDED
#define HEROESPATH_GUI_IMAGE_OPTIONS_HPP_INCLUDED
//
// image-options.hpp
//
#include "gui/image-option-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "sfutil/color.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Config.hpp>

#include <string>

namespace sf
{
class Image;
class Color;
class Texture;
} // namespace sf

namespace heroespath
{
namespace gui
{

    // Responsible for wrapping all information needed to modify an image.
    struct ImageOptions
    {
        ImageOptions()
            : option_enum(ImageOpt::Default)
            , mask_color_opt(boost::none)
            , mask_alpha(0)
        {}

        // intentionally can be used for implicit conversions
        ImageOptions(
            const ImageOpt::Enum OPTION_ENUM,
            const ColorOpt_t & MASK_COLOR_OPT = boost::none,
            const sf::Uint8 MASK_ALPHA = 0)
            : option_enum(OPTION_ENUM)
            , mask_color_opt(MASK_COLOR_OPT)
            , mask_alpha(MASK_ALPHA)
        {}

        // intentionally can be used for implicit conversions
        ImageOptions(
            const misc::EnumUnderlying_t OPTION_ENUM_VALUE,
            const ColorOpt_t & MASK_COLOR_OPT = boost::none,
            const sf::Uint8 MASK_ALPHA = 0)
            : option_enum(ImageOpt::Enum(OPTION_ENUM_VALUE))
            , mask_color_opt(MASK_COLOR_OPT)
            , mask_alpha(MASK_ALPHA)
        {}

        ImageOptions(const ImageOptions &) = default;
        ImageOptions(ImageOptions &&) = default;
        ImageOptions & operator=(const ImageOptions &) = default;
        ImageOptions & operator=(ImageOptions &&) = default;

        bool IsMaskAlphaDefault() const { return (0 == mask_alpha); }

        bool HasMask() const { return (!!mask_color_opt); }

        bool IsDefault() const
        {
            return (
                (HasMask() == false) && (ImageOpt::Default == option_enum) && IsMaskAlphaDefault());
        }

        bool IsNoOptions() const
        {
            return (
                (HasMask() == false) && (ImageOpt::None == option_enum) && IsMaskAlphaDefault());
        }

        void Apply(sf::Texture &) const;

        void Apply(sf::Image &) const;

        // lists the options in the order they will be appied
        const std::string ToString(
            const bool WILL_PREFIX = true,
            const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
            const std::string & SEPARATOR = "/") const;

        static const ImageOptions NoOptions() { return ImageOptions(ImageOpt::None); }

        // same as default constructor
        static const ImageOptions InvertedCharacterOptions()
        {
            return ImageOptions((ImageOpt::Default | ImageOpt::Invert), sf::Color::White);
        }

        ImageOpt::Enum option_enum;
        ColorOpt_t mask_color_opt;
        sf::Uint8 mask_alpha;
    };

    bool operator<(const ImageOptions & L, const ImageOptions & R);
    bool operator==(const ImageOptions & L, const ImageOptions & R);

    inline bool operator!=(const ImageOptions & L, const ImageOptions & R) { return !(L == R); }

} // namespace gui

std::ostream & operator<<(std::ostream & os, const gui::ImageOptions & OPTIONS);

} // namespace heroespath

#endif // HEROESPATH_GUI_IMAGE_OPTIONS_HPP_INCLUDED
