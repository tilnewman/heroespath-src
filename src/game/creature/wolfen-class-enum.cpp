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
// wolfen-class-enum.hpp
//
#include "wolfen-class-enum.hpp"
#include "game/game-data-file.hpp"
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    const std::string wolfen_class::Name(const wolfen_class::Enum E)
    {
        switch (E)
        {
            case Pup:       { return "Pup"; }
            case Juvenile:  { return "Juvenile"; }
            case Adult:     { return "Adult"; }
            case Noble:     { return "Noble"; }
            case Highborn:  { return "Highborn"; }
            case Elder:     { return "Elder"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::wolfen_class::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string wolfen_class::Desc(const wolfen_class::Enum E)
    {
        switch (E)
        {
            case Pup:       { return GameDataFile::Instance()->GetCopyStr("heroespath-creature-race-desc_Wolfen_Pup"); }
            case Juvenile:  { return GameDataFile::Instance()->GetCopyStr("heroespath-creature-race-desc_Wolfen_Juvenile"); }
            case Adult:     { return GameDataFile::Instance()->GetCopyStr("heroespath-creature-race-desc_Wolfen_Adult"); }
            case Noble:     { return GameDataFile::Instance()->GetCopyStr("heroespath-creature-race-desc_Wolfen_Noble"); }
            case Highborn:  { return GameDataFile::Instance()->GetCopyStr("heroespath-creature-race-desc_Wolfen_Highborn"); }
            case Elder:     { return GameDataFile::Instance()->GetCopyStr("heroespath-creature-race-desc_Wolfen_Elder"); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::race::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    wolfen_class::Enum wolfen_class::ClassFromRank(const stats::Rank_t RANK)
    {
        if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-wolfen-class-rank-min-Elder"))
            return wolfen_class::Elder;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-wolfen-class-rank-min-Highborn"))
            return wolfen_class::Highborn;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-wolfen-class-rank-min-Noble"))
            return wolfen_class::Noble;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-wolfen-class-rank-min-Adult"))
            return wolfen_class::Adult;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-wolfen-class-rank-min-Juvenile"))
            return wolfen_class::Juvenile;
        else
            return wolfen_class::Pup;
    }

}
}
