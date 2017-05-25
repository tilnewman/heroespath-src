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
#ifndef GAME_STATS_TYPES_INCLUDED
#define GAME_STATS_TYPES_INCLUDED
//
// types.hpp
//  Various typedefs required by the heroespath project
//
#include <cstddef> //for std::size_t


namespace game
{
namespace stats
{

    //The base numerical type for all Stat objects.  Must be signed.
    using Stat_t = int;

    //for experience points, must be unsigned
    using Exp_t = unsigned int;

    //for Rank, must be unsigned
    using Rank_t = std::size_t;

    //for Creature Health, must be signed
    using Health_t = int;

    //for Armor Rating, must be signed
    using Armor_t = int;

    //for mana (spell points), must be signed
    using Mana_t = int;

}
}
#endif //GAME_STATS_TYPES_INCLUDED
