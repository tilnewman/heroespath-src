#ifndef SFMLUTIL_COMBATIMAGEENUM_HPP_INCLUDED
#define SFMLUTIL_COMBATIMAGEENUM_HPP_INCLUDED
//
// combat-image-enum.hpp
//
#include <string>


namespace sfml_util
{
namespace gui
{

    struct CombatImageType
    {
        enum Enum
        {
            Wing = 0,
            Arrow1,
            Arrow2,
            Arrow3,
            Bolt,
            Dart,
            Stone1,
            Stone2,
            Stone3,
            Stone4,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Filename(const Enum);
    };

}
}

#endif //SFMLUTIL_COMBATIMAGEENUM_HPP_INCLUDED
