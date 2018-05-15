// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MOVING_ENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MOVING_ENUM_HPP_INCLUDED
//
// moving-enum.hpp
//
#include <string>

namespace heroespath
{
namespace sfml_util
{

    struct Moving
    {
        enum Enum
        {
            Still = 0,
            Toward,
            Away,
            Count
        };

        static const std::string ToString(const Moving::Enum);

        static bool IsMoving(const Moving::Enum E) { return ((E == Toward) || (E == Away)); }
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MOVING_ENUM_HPP_INCLUDED
