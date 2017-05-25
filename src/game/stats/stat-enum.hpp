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
#ifndef GAME_STAT_ENUM_INCLUDED
#define GAME_STAT_ENUM_INCLUDED
//
// stat-enum.hpp
//  An enumeration defining all stats.
//
#include <string>
#include <vector>


namespace game
{
namespace stats
{

    struct stat
    {
        enum Enum
        {
            Strength = 0,
            Accuracy,
            Charm,
            Luck,
            Speed,
            Intelligence,
            Count
        };

        static const std::string Name(const stat::Enum E);
        static const std::string Desc(const stat::Enum E);
        static const std::string Abbr(const stat::Enum E, const bool WILL_CAP = false);
        inline static const std::string ToString(const stat::Enum E) { return Name(E); }
    };

    using StatEnumVec_t = std::vector<stat::Enum>;

}
}
#endif //GAME_STAT_ENUM_INCLUDED
