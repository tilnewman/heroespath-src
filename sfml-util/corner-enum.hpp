#ifndef SFMLUTIL_CORNER_ENUM_HPP_INCLUDED
#define SFMLUTIL_CORNER_ENUM_HPP_INCLUDED
//
// corner.hpp
//
#include <string>


namespace sfml_util
{

    struct Corner
    {
        enum Enum : unsigned int
        {
            Error        = 0,
            None         = Error,
            TopLeft      = 1 << 0,
            TopRight     = 1 << 1,
            BottomLeft   = 1 << 2,
            BottomRight  = 1 << 3
        };

        static const std::string ToString(const Corner::Enum E);
        static bool IsValid(const Corner::Enum E);
    };

}

#endif //SFMLUTIL_CORNER_ENUM_HPP_INCLUDED
