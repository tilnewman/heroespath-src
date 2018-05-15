// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_DIRECTION_ENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_DIRECTION_ENUM_HPP_INCLUDED
//
// direction-enum.hpp
//
#include <string>

namespace heroespath
{
namespace sfml_util
{

    struct Direction
    {
        enum Enum
        {
            Left = 0,
            Right,
            Up,
            Down,
            Count
        };

        static const std::string ToString(const Direction::Enum);
        static sfml_util::Direction::Enum Opposite(const Direction::Enum);
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_DIRECTION_ENUM_HPP_INCLUDED
