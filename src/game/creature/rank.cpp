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
// rank.hpp
//
#include "rank.hpp"

#include "game/game-data-file.hpp"

#include <exception>
#include <sstream>
#include <algorithm>


namespace game
{
namespace creature
{

    rank_class::Enum rank_class::FromRank(const stats::Trait_t RANK_PARAM)
    {
        stats::Trait_t rankCumulative(0);
        for (std::size_t i(0); i < (rank_class::Count-1); ++i)
        {
            const rank_class::Enum NEXT_RANK_TYPE(static_cast<rank_class::Enum>(i));

            std::ostringstream ss;
            ss << "heroespath-rankclass-" << ToString(NEXT_RANK_TYPE) << "-rankmax";

            rankCumulative += static_cast<stats::Trait_t>(
                GameDataFile::Instance()->GetCopyInt(ss.str()));

            if (RANK_PARAM <= rankCumulative)
            {
                return NEXT_RANK_TYPE;
            }
        }

        return static_cast<rank_class::Enum>(rank_class::Count - 1);
    }


    std::pair<stats::Trait_t, stats::Trait_t> rank_class::RankRangeByClass(const rank_class::Enum E)
    {
        stats::Trait_t min(0);
        stats::Trait_t max(0);

        if (E == GrandMaster)
        {
            min = static_cast<stats::Trait_t>(GameDataFile::Instance()->GetCopyInt(
                "heroespath-rankclass-" + ToString(Master) + "-rankmax") + 1);

            max = 0;
            return std::make_pair(min, max);
        }
        else if (E == Novice)
        {
            min = 1;
            max = static_cast<stats::Trait_t>(GameDataFile::Instance()->GetCopyInt(
                "heroespath-rankclass-" + ToString(Novice) + "-rankmax"));

            return std::make_pair(min, max);
        }
        else
        {
            min = static_cast<stats::Trait_t>(GameDataFile::Instance()->GetCopyInt(
                "heroespath-rankclass-" + ToString(
                    static_cast<rank_class::Enum>(E - 1)) + "-rankmax") + 1);

            max = static_cast<stats::Trait_t>(GameDataFile::Instance()->GetCopyInt(
                "heroespath-rankclass-" + ToString(E) + "-rankmax"));

            return std::make_pair(min, max);
        }
    }


    const std::string rank_class::ToString(const rank_class::Enum E)
    {
        switch (E)
        {
            case Novice:      { return "Novice"; }
            case Trainee:     { return "Trainee"; }
            case Skilled:     { return "Skilled"; }
            case Expert:      { return "Expert"; }
            case Master:      { return "Master"; }
            case GrandMaster: { return "GrandMaster"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::rank_class::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
