//
// role-enum.cpp
//
#include "role-enum.hpp"
#include "game/game-data-file.hpp"
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    const std::string role::ToString(const role::Enum E)
    {
        switch (E)
        {
            case Beastmaster:   { return "Beastmaster"; }
            case Sorcerer:      { return "Sorcerer"; }
            case Cleric:        { return "Cleric"; }
            case Knight:        { return "Knight"; }
            case Archer:        { return "Archer"; }
            case Bard:          { return "Bard"; }
            case Thief:         { return "Thief"; }
            case Wolfen:        { return "Wolfen"; }
            case Firebrand:     { return "Firebrand"; }
            case Sylavin:       { return "Sylavin"; }
            //
            case Thug:          { return "Thug"; }
            case Mugger:        { return "Mugger"; }
            case Drunk:         { return "Drunk"; }
            case Brute:         { return "Brute"; }
            case Berserker:     { return "Berserker"; }
            case Mountain:      { return "Mountain"; }
            case Grunt:         { return "Grunt"; }
            case Captain:       { return "Captain"; }
            case Chieftain:     { return "Chieftain"; }
            case Trader:        { return "Trader"; }
            case Warlord:       { return "Warlord"; }
            case Shaman:        { return "Shaman"; }
            case Smasher:       { return "Smasher"; }
            case Strangler:     { return "Strangler"; }
            case Soldier:       { return "Soldier"; }
            case TwoHeaded:     { return "TwoHeaded"; }
            case Giant:         { return "Giant"; }
            case Elder:         { return "Elder"; }
            case FourArmed:     { return "FourArmed"; }
            case Tendrilus:     { return "Tendrilus"; }
            case Wing:          { return "Wing"; }
            case Whelp:         { return "Whelp"; }
            case Pod:           { return "Pod"; }
            case Spike:         { return "Spike"; }
            case Skeleton:      { return "Skeleton"; }
            case Ranger:        { return "Ranger"; }
            case Desert:        { return "Desert"; }
            case Water:         { return "Water"; }
            case Blacksmith:    { return "Blacksmith"; }
            case Witch:         { return "Witch"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::player::role::Enum::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string role::Desc(const role::Enum E)
    {
        std::ostringstream ss;
        ss << "heroespath-creature-role-" << ToString(E) << "-desc";
        return GameDataFile::Instance()->GetCopyStr(ss.str());
    }


    const std::string role::Abbr(const role::Enum E)
    {
        if (E == Beastmaster) { return "Bsm"; }
        else if (E == Thief)  { return "Thf"; }
        else if (E == Wolfen) { return "Wlf"; }
        else
            return Name(E).substr(0, 3);
    }


    bool role::CanFly(const role::Enum E)
    {
        return ((E == role::Firebrand) ||
                (E == role::Sylavin) ||
                (E == role::Wing) ||
                (E == role::Whelp));
    }


    bool role::WillInitiallyFly(const role::Enum E)
    {
        return ((E == role::Wing) || (E == role::Whelp));
    }

}
}
