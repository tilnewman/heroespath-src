#ifndef SFMLUTIL_MOUSE_STATE_ENUM_HPP_INCLUDED
#define SFMLUTIL_MOUSE_STATE_ENUM_HPP_INCLUDED
//
// mouse-state-enum.hpp
//
#include <string>


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

}

#endif //SFMLUTIL_MOUSE_STATE_ENUM_HPP_INCLUDED
