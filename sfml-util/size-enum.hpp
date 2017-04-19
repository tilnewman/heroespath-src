#ifndef SFMLUTIL_SIZE_ENUM_HPP_INCLUDED
#define SFMLUTIL_SIZE_ENUM_HPP_INCLUDED
//
// size-enum.hpp
//
#include <string>


namespace sfml_util
{

    namespace Size
    {
        enum Enum
        {
            Small = 0,
            Medium,
            Large,
            Count
        };

        const std::string ToString(const Size::Enum);
    }

}

#endif //SFMLUTIL_SIZE_ENUM_HPP_INCLUDED
