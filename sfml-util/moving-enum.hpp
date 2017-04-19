#ifndef SFMLUTIL_MOVING_ENUM_HPP_INCLUDED
#define SFMLUTIL_MOVING_ENUM_HPP_INCLUDED
//
// moving-enum.hpp
//
#include <string>


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
        inline static bool IsMoving(const Moving::Enum E) { return ((E == Toward) || (E == Away)); }
    };

}

#endif //SFMLUTIL_MOVING_ENUM_HPP_INCLUDED
