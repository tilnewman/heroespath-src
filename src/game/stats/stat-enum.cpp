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
// stat-enum.hpp
//
#include "stat-enum.hpp"
#include "game/game-data-file.hpp"
#include <sstream>
#include <exception>


namespace game
{
namespace stats
{

    const std::string stat::Name(const stat::Enum E)
    {
        switch (E)
        {
            case Strength:     { return "Strength"; }
            case Accuracy:     { return "Accuracy"; }
            case Charm:        { return "Charm"; }
            case Luck:         { return "Luck"; }
            case Speed:        { return "Speed"; }
            case Intelligence: { return "Intelligence"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stats::stat::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string stat::Desc(const stat::Enum E)
    {
        switch (E)
        {
            case Strength:     { return game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Strength"); }
            case Accuracy:     { return game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Accuracy"); }
            case Charm:        { return game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Charm"); }
            case Luck:         { return game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Luck"); }
            case Speed:        { return game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Speed"); }
            case Intelligence: { return game::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Intelligence"); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stats::stat::Desc(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string stat::Abbr(const stat::Enum E, const bool WILL_CAP)
    {
        switch (E)
        {
            case Strength:      { if (WILL_CAP) return "Str"; else return "str"; }
            case Accuracy:      { if (WILL_CAP) return "Acc"; else return "acc"; }
            case Charm:         { if (WILL_CAP) return "Cha"; else return "cha"; }
            case Luck:          { if (WILL_CAP) return "Lck"; else return "lck"; }
            case Speed:         { if (WILL_CAP) return "Spd"; else return "spd"; }
            case Intelligence:  { if (WILL_CAP) return "Int"; else return "int"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stats::stat::Abbr(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
