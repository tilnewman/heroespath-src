//
// dragon-class-enum.hpp
//
#include "dragon-class-enum.hpp"
#include "heroespath/game-data-file.hpp"
#include <exception>
#include <sstream>


namespace heroespath
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
                ss << "heroespath::creature::dragon_class::Name(" << E << ")_InvalidValueError.";
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


    dragon_class::Enum dragon_class::ClassFromRank(const stats::Rank_t RANK)
    {
        if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-dragon-class-rank-min-Elder"))
            return dragon_class::Elder;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-dragon-class-rank-min-Skycaster"))
            return dragon_class::Skycaster;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-dragon-class-rank-min-Wyrm"))
            return dragon_class::Wyrm;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-dragon-class-rank-min-Adult"))
            return dragon_class::Adult;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-dragon-class-rank-min-Juvenile"))
            return dragon_class::Juvenile;
        else if (RANK >= GameDataFile::Instance()->GetCopySizet("heroespath-creature-dragon-class-rank-min-Fledgling"))
            return dragon_class::Fledgling;
        else
            return dragon_class::Whelp;
    }

}
}
