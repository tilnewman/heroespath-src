// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_CORNER_ENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_CORNER_ENUM_HPP_INCLUDED
//
// corner.hpp
//
#include "misc/enum-util.hpp"

namespace heroespath
{
namespace sfml_util
{

    struct Corner : misc::EnumBaseBitField<Corner>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            TopLeft = 1 << 0,
            TopRight = 1 << 1,
            BottomLeft = 1 << 2,
            BottomRight = 1 << 3,
            Last = BottomRight
        };

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR);
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_CORNER_ENUM_HPP_INCLUDED
