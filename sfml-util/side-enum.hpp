#ifndef SFMLUTIL_SIDE_ENUM_HPP_INCLUDED
#define SFMLUTIL_SIDE_ENUM_HPP_INCLUDED
//
// side.hpp
//
#include <string>


namespace sfml_util
{

    struct Side
    {
        enum Enum : unsigned int
        {
            Error   = 0,
            None    = Error,
            Top     = 1 << 0,
            Bottom  = 1 << 1,
            Left    = 1 << 2,
            Right   = 1 << 3
        };

        static const std::string ToString(const Side::Enum E);
        static bool IsValid(const Side::Enum E);
    };

}

#endif //SFMLUTIL_SIDE_ENUM_HPP_INCLUDED
