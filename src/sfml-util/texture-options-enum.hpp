// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXTURE_OPTIONS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTURE_OPTIONS_HPP_INCLUDED
//
// texture-options-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for enumerating all possible alterations for a cached texture.
    struct TextureOpt : public misc::EnumBaseBitField<TextureOpt, misc::NoneEmpty::No>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Smooth = 1 << 0,
            Default = Smooth,
            FlipHoriz = 1 << 1,
            FlipVert = 1 << 2,
            Invert = 1 << 3,
            Last = Invert
        };

        static const std::string ToString(const misc::EnumUnderlying_t ENUM_VALUE)
        {
            return EnumBaseBitField::ToString(ENUM_VALUE, misc::Wrap::Yes, "/");
        }

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR = "/");
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTURE_OPTIONS_HPP_INCLUDED
