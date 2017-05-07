//
// condition-enum.cpp
//
#include "condition-enum.hpp"

#include "game/game-data-file.hpp"

#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    namespace condition
    {

        const std::string ToString(const condition::Enum E)
        {
            switch (E)
            {
                case Good:          { return "Good"; }
                case Frightened:    { return "Frightened"; }
                case Dazed:         { return "Dazed"; }
                case Tripped:       { return "Tripped"; }
                case AsleepNatural: { return "AsleepNatural"; }
                case Poisoned:      { return "Poisoned"; }
                case AsleepMagical: { return "AsleepMagical"; }
                case Unconscious:   { return "Unconscious"; }
                case Stone:         { return "Stone"; }
                case Dead:          { return "Dead"; }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "game::creature::condition::Enum::ToString(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }


        const std::string Name(const condition::Enum E)
        {
            if (E == AsleepNatural)
            {
                return "Asleep";
            }
            else if (E == AsleepMagical)
            {
                return "Magical Sleep";
            }
            else
            {
                return ToString(E);
            }
        }


        const std::string Desc(const condition::Enum E)
        {
            std::ostringstream keySS;
            keySS << "heroespath-creature-condition-" << ToString(E) << "-desc";
            return GameDataFile::Instance()->GetCopyStr(keySS.str());
        }


        const std::size_t Severity::ALL          (0);
        const std::size_t Severity::BENEFITIAL   (1);
        const std::size_t Severity::LEAST_HARMFUL(2);
        const std::size_t Severity::MOST_HARMFUL (100);


        std::size_t Severity::ByCondition(const condition::Enum E)
        {
            switch (E)
            {
                //Note:  Keep the 'Good' condition at a different number from 'Benefitial'
                case Good:          { return ALL; }

                case Frightened:    { return LEAST_HARMFUL; }
                case Dazed:         { return 10; }
                case Tripped:       { return 20; }
                case AsleepNatural: { return 30; }
                case Poisoned:      { return 40; }
                case AsleepMagical: { return 50; }
                case Unconscious:   { return 80; }
                case Stone:         { return 90; }
                case Dead:          { return MOST_HARMFUL; }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "game::creature::condition::Enum::Severity(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

    }

}
}
