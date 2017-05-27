// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// gradient-info.cpp
//
#include "gradient-info.hpp"


namespace sfml_util
{

    GradientInfo::GradientInfo(const sf::Color &  COLOR_TO,
                               const unsigned int CORNERS,
                               const unsigned int SIDES,
                               const sf::Color &  COLOR_FROM)
    :
        sides     (SIDES),
        corners   (CORNERS),
        color_from(COLOR_FROM),
        color_to  (COLOR_TO)
    {}


    bool operator==(const GradientInfo & L, const GradientInfo & R)
    {
        return ((L.sides == R.sides) &&
                (L.corners == R.corners) &&
                (L.color_from == R.color_from) &&
                (L.color_to == R.color_to));
    }

}
