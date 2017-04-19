#ifndef SFMLUTIL_BRIGHTNESS_ENUM_HPP_INCLUDED
#define SFMLUTIL_BRIGHTNESS_ENUM_HPP_INCLUDED
//
// brightness.hpp
//
#include <string>


namespace sfml_util
{

    struct Brightness
    {
        enum Enum
        {
            Bright = 0, //Default
            Medium,
            Dark,
            Count
        };

        static const std::string ToString(const Brightness::Enum E);
        static bool IsValid(const Brightness::Enum E);
    };

}

#endif //SFMLUTIL_BRIGHTNESS_ENUM_HPP_INCLUDED
