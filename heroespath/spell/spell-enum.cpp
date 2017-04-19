//
// spell-enum.cpp
//
#include "spell-enum.hpp"

#include "heroespath/game-data-file.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace spell
{

    const std::string Spells::ToString(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks: { return "Sparks"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "heroespath::creature::spell::Spells::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    const std::string Spells::Name(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks: { return "Sparks"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "heroespath::creature::spell::Spells::Name(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    const std::string Spells::Desc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-desc";
        return GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

}
}
