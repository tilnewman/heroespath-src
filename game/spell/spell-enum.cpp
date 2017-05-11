//
// spell-enum.cpp
//
#include "spell-enum.hpp"

#include "game/game-data-file.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace spell
{

    const std::string Spells::ToString(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks:        { return "Sparks"; }
            case Bandage:       { return "Bandage"; }
            case Sleep:         { return "Sleep"; }
            case Awaken:        { return "Awaken"; }
            case Trip:          { return "Trip"; }
            case Lift:          { return "Lift"; }
            case Daze:          { return "Daze"; }
            case Frighten:      { return "Frighten"; }
            case ClearMind:     { return "ClearMind"; }
            case Poison:        { return "Poison"; }
            case Antidote:      { return "Antidote"; }
            case PoisonCloud:   { return "PoisonCloud"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::spell::Spells::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    const std::string Spells::Name(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks:        { return "Sparks"; }
            case Bandage:       { return "Bandage"; }
            case Sleep:         { return "Sleep"; }
            case Awaken:        { return "Awaken"; }
            case Trip:          { return "Trip"; }
            case Lift:          { return "Lift"; }
            case Daze:          { return "Daze"; }
            case Frighten:      { return "Frighten"; }
            case ClearMind:     { return "Clear Mind"; }
            case Poison:        { return "Poison"; }
            case Antidote:      { return "Antidote"; }
            case PoisonCloud:   { return "Poison Cloud"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::spell::Spells::Name(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    const std::string Spells::ShortDesc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-short-desc";
        return GameDataFile::Instance()->GetCopyStr(keySS.str());
    }


    const std::string Spells::ExtraDesc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-extra-desc";
        return GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

}
}
