///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_SFMLUTIL_RESOLUTION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_RESOLUTION_HPP_INCLUDED
//
// resolution.hpp
//  A class that contains all the info about a screen resolution.
//
#include "sfml-util/sfml-graphics.hpp"

#include <string>
#include <vector>


namespace heroespath
{
namespace sfml_util
{

    //Enapsulates everything about a screen resolution
    struct Resolution
    {
        explicit Resolution(const unsigned int  WIDTH          = 0,
                            const unsigned int  HEIGHT         = 0,
                            const unsigned int  BITS_PER_PIXEL = 0,
                            const std::string & NAME           = "",
                            const std::string & ASPECT_RATIO   = "");

        explicit Resolution(const sf::VideoMode & VM);

        const std::string ToString(const bool WILL_WRAP = true) const;

        std::string  name;
        unsigned int width;
        unsigned int height;
        std::string  aspect_ratio;
        unsigned int bits_per_pixel;
    };

    using ResolutionVec_t = std::vector<Resolution>;

    bool operator==(const Resolution & L, const Resolution & R);

    inline bool operator!=(const Resolution & L, const Resolution & R)
    {
        return ! (L == R);
    }

    bool operator==(const Resolution & R, const sf::VideoMode & V);

    inline bool operator!=(const Resolution & R, const sf::VideoMode & V)
    {
        return ! (R == V);
    }


}
}

#endif //HEROESPATH_SFMLUTIL_RESOLUTION_HPP_INCLUDED
