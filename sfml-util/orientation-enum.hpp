#ifndef SFMLUTIL_ORIENTATION_ENUM_HPP_INCLUDED
#define SFMLUTIL_ORIENTATION_ENUM_HPP_INCLUDED
//
// orientation.hpp
//
#include <string>


namespace sfml_util
{

    struct Orientation
    {
        enum Enum
        {
            Horiz = 0,
            Vert,
            Count
        };

        static const std::string ToString(const Orientation::Enum E);
        static bool IsValid(const Orientation::Enum E);
    };

}

#endif //SFMLUTIL_ORIENTATION_ENUM_HPP_INCLUDED
