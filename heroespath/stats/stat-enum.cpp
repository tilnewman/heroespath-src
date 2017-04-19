//
// stat-enum.hpp
//
#include "stat-enum.hpp"
#include "heroespath/game-data-file.hpp"
#include <sstream>
#include <exception>


namespace heroespath
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
                ss << "heroespath::stats::stat::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string stat::Desc(const stat::Enum E)
    {
        switch (E)
        {
            case Strength:     { return heroespath::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Strength"); }
            case Accuracy:     { return heroespath::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Accuracy"); }
            case Charm:        { return heroespath::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Charm"); }
            case Luck:         { return heroespath::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Luck"); }
            case Speed:        { return heroespath::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Speed"); }
            case Intelligence: { return heroespath::GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Intelligence"); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "heroespath::stats::stat::Desc(" << E << ")_InvalidValueError.";
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
                ss << "heroespath::stats::stat::Abbr(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
