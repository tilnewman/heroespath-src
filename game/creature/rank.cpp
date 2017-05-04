//
// rank.hpp
//
#include "rank.hpp"
#include "game/game-data-file.hpp"
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    rank_class::Enum rank_class::FromRank(const stats::Rank_t RANK_PARAM)
    {
        stats::Rank_t rankCumulative(0);
        for (std::size_t i(0); i < (rank_class::Count-1); ++i)
        {
            const rank_class::Enum NEXT_RANK_TYPE(static_cast<rank_class::Enum>(i));
            std::ostringstream ss;
            ss << "heroespath-rankclass-" << ToString(NEXT_RANK_TYPE) << "-rankmax";
            rankCumulative += GameDataFile::Instance()->GetCopyInt(ss.str());
            if (RANK_PARAM <= rankCumulative)
                return NEXT_RANK_TYPE;
        }

        return static_cast<rank_class::Enum>(rank_class::Count - 1);
    }


    std::pair<stats::Rank_t, stats::Rank_t> rank_class::RankRangeByClass(const rank_class::Enum E)
    {
        stats::Rank_t min(0);
        stats::Rank_t max(0);

        if (E == GrandMaster)
        {
            min = GameDataFile::Instance()->GetCopyInt("heroespath-rankclass-" + ToString(Master) + "-rankmax") + 1;
            max = 0;
            return std::make_pair(min, max);
        }
        else if (E == Novice)
        {
            min = 1;
            max = GameDataFile::Instance()->GetCopyInt("heroespath-rankclass-" + ToString(Novice) + "-rankmax");
            return std::make_pair(min, max);
        }
        else
        {
            min = GameDataFile::Instance()->GetCopyInt("heroespath-rankclass-" + ToString(static_cast<rank_class::Enum>(E - 1)) + "-rankmax") + 1;
            max = GameDataFile::Instance()->GetCopyInt("heroespath-rankclass-" + ToString(E) + "-rankmax");
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
