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

#include "misc/config-file.hpp"

#include <algorithm>

namespace heroespath
{
namespace creature
{

    rank_class::Enum rank_class::FromRank(const Rank_t & RANK_PARAM)
    {
        const auto HIGHEST_RANK_CLASS_INT { static_cast<int>(rank_class::Count - 1) };

        // loop over ranks starting at the first but only going up to the second highest
        Rank_t rankCumulative { 0_rank };
        for (int i(0); i < HIGHEST_RANK_CLASS_INT; ++i)
        {
            const rank_class::Enum RANK_ENUM { static_cast<rank_class::Enum>(i) };

            std::ostringstream ss;
            ss << "heroespath-rankclass-" << ToString(RANK_ENUM) << "-rankmax";

            rankCumulative += Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(ss.str()));

            if (RANK_PARAM <= rankCumulative)
            {
                return RANK_ENUM;
            }
        }

        return static_cast<rank_class::Enum>(HIGHEST_RANK_CLASS_INT);
    }

    const RankRange_t rank_class::RankRangeByClass(const rank_class::Enum ENUM)
    {
        Rank_t min { 0_rank };
        Rank_t max { 0_rank };

        if (ENUM == GrandMaster)
        {
            min = Rank_t(
                misc::ConfigFile::Instance()->ValueOrDefault<int>(
                    "heroespath-rankclass-" + ToString(Master) + "-rankmax")
                + 1);
        }
        else if (ENUM == Novice)
        {
            min = 1_rank;

            max = Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-rankclass-" + ToString(Novice) + "-rankmax"));
        }
        else
        {
            min = Rank_t(
                misc::ConfigFile::Instance()->ValueOrDefault<int>(
                    "heroespath-rankclass-" + ToString(static_cast<rank_class::Enum>(ENUM - 1))
                    + "-rankmax")
                + 1);

            max = Rank_t(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                "heroespath-rankclass-" + ToString(ENUM) + "-rankmax"));
        }

        return RankRange_t(min, max);
    }

    const std::string rank_class::ToString(const rank_class::Enum ENUM)
    {
        switch (ENUM)
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
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(ENUM));
                return "";
            }
        }
    }

} // namespace creature
} // namespace heroespath
