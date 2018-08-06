// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_IMAGE_OPTION_ENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_IMAGE_OPTION_ENUM_HPP_INCLUDED
//
// image-option-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for enumerating all possible alterations for a cached texture.
    struct ImageOpt : public misc::EnumBaseBitField<ImageOpt, misc::NoneEmpty::No>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Smooth = 1 << 0,
            Default = Smooth,
            FlipHoriz = 1 << 1,
            FlipVert = 1 << 2,
            Repeated = 1 << 3,
            Invert = 1 << 4,
            InvertAfterMask = 1 << 5,
            InvertIncludesAlpha = 1 << 6,
            Last = InvertIncludesAlpha
        };

        static const std::string ToString(
            const misc::EnumUnderlying_t ENUM_VALUE,
            const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
            const std::string & SEPARATOR = "/")
        {
            return EnumBaseBitField::ToString(ENUM_VALUE, WILL_WRAP, SEPARATOR);
        }

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR = "/");
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_IMAGE_OPTION_ENUM_HPP_INCLUDED
