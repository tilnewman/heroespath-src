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
#ifndef HEROESPATH_MAP_MAPANIM_HPP_INCLUDED
#define HEROESPATH_MAP_MAPANIM_HPP_INCLUDED
//
// map-anim.hpp
//
#include "sfml-util/animation-enum.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <vector>


namespace heroespath
{
namespace map
{

    //Responsible for wrapping all the data about an animation in the map.
    struct MapAnim
    {
        MapAnim(
            const sfml_util::Animations::Enum ENUM = sfml_util::Animations::Count,
            const sf::FloatRect & RECT = sf::FloatRect())
        :
            which_anim(ENUM),
            rect(RECT)
        {}

        sfml_util::Animations::Enum which_anim;
        sf::FloatRect rect;
    };

    using MapAnimVec_t = std::vector<MapAnim>;

}
}

#endif //HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
