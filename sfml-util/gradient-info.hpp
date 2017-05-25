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
#ifndef SFMLUTIL_GRADIENTINFO_INCLUDED
#define SFMLUTIL_GRADIENTINFO_INCLUDED
//
// gradient-info.hpp
//  Gradient drawing helper struct.
//
#include "sfml-util/corner-enum.hpp"
#include "sfml-util/side-enum.hpp"

#include <SFML/Graphics/Color.hpp>


namespace sfml_util
{

    //Encapsulates everything needed to draw a gradient.
    //Note: Designed so that using the constructor (with all default, or no params)
    //      is safe and will cause nothing (no gradient) to be drawn.
    struct GradientInfo
    {
        explicit GradientInfo(const sf::Color &  COLOR_TO   = sf::Color::Transparent,
                              const unsigned int CORNERS    = sfml_util::Corner::None,
                              const unsigned int SIDES      = sfml_util::Side::None,
                              const sf::Color &  COLOR_FROM = sf::Color::Transparent);

        unsigned int sides;
        unsigned int corners;
        sf::Color    color_from;
        sf::Color    color_to;
    };

    bool operator==(const GradientInfo & L, const GradientInfo & R);

    bool operator!=(const GradientInfo & L, const GradientInfo & R);

}
#endif //SFMLUTIL_GRADIENTINFO_INCLUDED
