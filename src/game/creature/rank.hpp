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
#ifndef GAME_CREATURE_RANK_INCLUDED
#define GAME_CREATURE_RANK_INCLUDED
//
// rank.hpp
//  Code that gives meaning to all possible rank values.
//
#include "game/stats/types.hpp"

#include <string>
#include <utility>


namespace game
{
namespace creature
{

    struct rank_class
    {
        enum Enum
        {
            Novice = 0,
            Trainee,
            Skilled,
            Expert,
            Master,
            GrandMaster,
            Count
        };

        static rank_class::Enum FromRank(const stats::Rank_t RANK);

        //returns a second/max of zero to represent limitless
        static std::pair<stats::Rank_t, stats::Rank_t> RankRangeByClass(const rank_class::Enum E);

        static const std::string ToString(const rank_class::Enum E);
    };

}
}
#endif //GAME_CREATURE_RANK_INCLUDED
