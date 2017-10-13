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
// dragon-class-enum.hpp
//
#include "dragon-class-enum.hpp"
#include "game/game-data-file.hpp"
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    const std::string dragon_class::Name(const dragon_class::Enum E)
    {
        switch (E)
        {
            case Hatchling: { return "Hatchling"; }
            case Whelp:     { return "Whelp"; }
            case Fledgling: { return "Fledgling"; }
            case Juvenile:  { return "Juvenile"; }
            case Adult:     { return "Adult"; }
            case Wyrm:      { return "Wyrm"; }
            case Skycaster: { return "Skycaster"; }
            case Elder:     { return "Elder"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::dragon_class::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string dragon_class::Desc(const dragon_class::Enum E)
    {
        std::ostringstream ss;
        ss << "heroespath-creature-race-desc_Dragon_" << ToString(E);
        return GameDataFile::Instance()->GetCopyStr(ss.str());
    }


    dragon_class::Enum dragon_class::ClassFromRank(const Rank_t RANK)
    {
        if (RANK >= Rank_t(GameDataFile::Instance()->
            GetCopyInt("heroespath-creature-dragon-class-rank-min-Elder")))
        {
            return dragon_class::Elder;
        }
        else if (RANK >= Rank_t(GameDataFile::Instance()->
            GetCopyInt("heroespath-creature-dragon-class-rank-min-Skycaster")))
        {
            return dragon_class::Skycaster;
        }
        else if (RANK >= Rank_t(GameDataFile::Instance()->
            GetCopyInt("heroespath-creature-dragon-class-rank-min-Wyrm")))
        {
            return dragon_class::Wyrm;
        }
        else if (RANK >= Rank_t(GameDataFile::Instance()->
            GetCopyInt("heroespath-creature-dragon-class-rank-min-Adult")))
        {
            return dragon_class::Adult;
        }
        else if (RANK >= Rank_t(GameDataFile::Instance()->
            GetCopyInt("heroespath-creature-dragon-class-rank-min-Juvenile")))
        {
            return dragon_class::Juvenile;
        }
        else if (RANK >= Rank_t(GameDataFile::Instance()->
            GetCopyInt("heroespath-creature-dragon-class-rank-min-Fledgling")))
        {
            return dragon_class::Fledgling;
        }
        else
        {
            return dragon_class::Whelp;
        }
    }

}
}
