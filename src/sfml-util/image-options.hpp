// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_IMAGE_OPTIONS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_IMAGE_OPTIONS_HPP_INCLUDED
//
// image-options.hpp
//
#include "sfml-util/image-option-enum.hpp"
#include "sfml-util/sfml-util-color.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <ostream>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for wrapping all information needed to modify an image.
    struct ImageOptions
    {
        explicit ImageOptions(
            const ImageOpt::Enum OPTION_ENUM = ImageOpt::Default,
            const sf::Color & MASK_COLOR = sfml_util::Colors::None,
            const sf::Uint8 MASK_ALPHA = 0)
            : option_enum(OPTION_ENUM)
            , mask_color(MASK_COLOR)
            , mask_alpha(MASK_ALPHA)
        {}

        explicit ImageOptions(
            const misc::EnumUnderlying_t OPTION_ENUM_VALUE,
            const sf::Color & MASK_COLOR = sfml_util::Colors::None,
            const sf::Uint8 MASK_ALPHA = 0)
            : option_enum(ImageOpt::Enum(OPTION_ENUM_VALUE))
            , mask_color(MASK_COLOR)
            , mask_alpha(MASK_ALPHA)
        {}

        ImageOptions(const ImageOptions &) = default;
        ImageOptions(ImageOptions &&) = default;
        ImageOptions & operator=(const ImageOptions &) = default;
        ImageOptions & operator=(ImageOptions &&) = default;

        bool IsMaskAlphaDefault() const { return (0 == mask_alpha); }

        bool HasMask() const { return (sfml_util::Colors::None != mask_color); }

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
        static const ImageOptions DefaultOptions() { return ImageOptions(); }

        ImageOpt::Enum option_enum;
        sf::Color mask_color;
        sf::Uint8 mask_alpha;
    };

    inline bool operator<(const ImageOptions & L, const ImageOptions & R)
    {
        return std::tie(L.option_enum, L.mask_color, L.mask_alpha)
            < std::tie(R.option_enum, R.mask_color, R.mask_alpha);
    }

    inline bool operator==(const ImageOptions & L, const ImageOptions & R)
    {
        return std::tie(L.option_enum, L.mask_color, L.mask_alpha)
            == std::tie(R.option_enum, R.mask_color, R.mask_alpha);
    }

    inline bool operator!=(const ImageOptions & L, const ImageOptions & R) { return !(L == R); }

} // namespace sfml_util

inline std::ostream & operator<<(std::ostream & os, const sfml_util::ImageOptions & OPTIONS)
{
    os << OPTIONS.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_IMAGE_OPTIONS_HPP_INCLUDED
