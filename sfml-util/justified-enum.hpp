#ifndef SFMLUTIL_JUSTIFIED_ENUM_HPP_INCLUDED
#define SFMLUTIL_JUSTIFIED_ENUM_HPP_INCLUDED
//
// justified-enum.hpp
//
#include <string>


namespace sfml_util
{

    struct Justified
    {
        enum Enum
        {
            Left = 0,
            Right,
            Center,
            Count
        };

        static const std::string ToString(const Justified::Enum E);
        static bool IsValid(const Justified::Enum E);
    };

}

#endif //SFMLUTIL_JUSTIFIED_ENUM_HPP_INCLUDED
