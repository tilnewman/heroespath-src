// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// rank.hpp
//
#include "rank.hpp"

#include "game/game-data-file.hpp"

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace creature
{

    rank_class::Enum rank_class::FromRank(const Rank_t & RANK_PARAM)
    {
        Rank_t rankCumulative{ 0_rank };
        for (int i(0); i < (rank_class::Count - 1); ++i)
        {
            const rank_class::Enum RANK_ENUM{ static_cast<rank_class::Enum>(i) };

            std::ostringstream ss;
            ss << "heroespath-rankclass-" << ToString(RANK_ENUM) << "-rankmax";

            rankCumulative += Rank_t(game::GameDataFile::Instance()->GetCopyInt(ss.str()));

            if (RANK_PARAM <= rankCumulative)
            {
                return RANK_ENUM;
            }
        }

        return static_cast<rank_class::Enum>(rank_class::Count - 1);
    }

    const RankRange_t rank_class::RankRangeByClass(const rank_class::Enum E)
    {
        Rank_t min{ 0_rank };
        Rank_t max{ 0_rank };

        if (E == GrandMaster)
        {
            min = Rank_t(
                game::GameDataFile::Instance()->GetCopyInt(
                    "heroespath-rankclass-" + ToString(Master) + "-rankmax")
                + 1);
        }
        else if (E == Novice)
        {
            min = 1_rank;

            max = Rank_t(game::GameDataFile::Instance()->GetCopyInt(
                "heroespath-rankclass-" + ToString(Novice) + "-rankmax"));
        }
        else
        {
            min = Rank_t(
                game::GameDataFile::Instance()->GetCopyInt(
                    "heroespath-rankclass-" + ToString(static_cast<rank_class::Enum>(E - 1))
                    + "-rankmax")
                + 1);

            max = Rank_t(game::GameDataFile::Instance()->GetCopyInt(
                "heroespath-rankclass-" + ToString(E) + "-rankmax"));
        }

        return RankRange_t(min, max);
    }

    const std::string rank_class::ToString(const rank_class::Enum E)
    {
        switch (E)
        {
            case Novice:
            {
                return "Novice";
            }
            case Trainee:
            {
                return "Trainee";
            }
            case Skilled:
            {
                return "Skilled";
            }
            case Expert:
            {
                return "Expert";
            }
            case Master:
            {
                return "Master";
            }
            case GrandMaster:
            {
                return "GrandMaster";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::rank_class::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace creature
} // namespace heroespath
