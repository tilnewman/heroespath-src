//
// wolfen-class-enum.hpp
//
#include "wolfen-class-enum.hpp"
#include "heroespath/game-data-file.hpp"
#include <exception>
#include <sstream>


namespace heroespath
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
                ss << "heroespath::creature::wolfen_class::Name(" << E << ")_InvalidValueError.";
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
                ss << "heroespath::creature::race::Name(" << E << ")_InvalidValueError.";
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
