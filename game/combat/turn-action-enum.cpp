//
// turn-state-enum.cpp
//
#include "turn-action-enum.hpp"
#include <sstream>
#include <exception>


namespace game
{
namespace combat
{

    const std::string TurnAction::ToString(const TurnAction::Enum E)
    {
        switch (E)
        {
            case Nothing:       { return "Nothing"; }
            case Attack:        { return "Attack"; }
            case Block:         { return "Block"; }
            case Advance:       { return "Advance"; }
            case Retreat:       { return "Retreat"; }
            case Fly:           { return "Fly"; }
            case Land:          { return "Land"; }
            case Cast:          { return "Cast"; }
            case SkyPounce:     { return "SkyPounce"; }
            case LandPounce:    { return "LandPounce"; }
            case Roar:          { return "Roar"; }
            case ChangeWeapon:  { return "ChangeWeapon"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::combat::TurnAction::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string TurnAction::Name(const TurnAction::Enum E)
    {
        switch (E)
        {
            case Nothing:       { return "Does Nothing"; }
            case Attack:        { return "Attacks"; }
            case Block:         { return "Blocks"; }
            case Advance:       { return "Advances"; }
            case Retreat:       { return "Retreats"; }
            case Fly:           { return "Flies"; }
            case Land:          { return "Lands"; }
            case Cast:          { return "Casts a Spell"; }
            case SkyPounce:     { return "Pounces from the Sky"; }
            case LandPounce:    { return "Pounces"; }
            case Roar:          { return "Roars"; }
            case ChangeWeapon:  { return "Changes Weapons"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::combat::TurnAction::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
