// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MOUSE_STATE_ENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MOUSE_STATE_ENUM_HPP_INCLUDED
//
// mouse-state-enum.hpp
//
#include <string>

namespace heroespath
{
namespace sfml_util
{

    struct MouseState
    {
        enum Enum
        {
            Up = 0,
            Down,
            Over,
            Count
        };

        static const std::string ToString(const MouseState::Enum E);
        static bool IsValid(const MouseState::Enum E);
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MOUSE_STATE_ENUM_HPP_INCLUDED
