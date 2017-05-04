//
// race.hpp
//
#include "race.hpp"
#include "game/creature/race-stats.hpp"


namespace game
{
namespace creature
{

    Race::Race(const race::Enum RACE_ENUM)
    :
        race_         (RACE_ENUM),
        statModifiers_( StatModifierByRace(RACE_ENUM) )
    {}


    Race::~Race()
    {}

}
}
