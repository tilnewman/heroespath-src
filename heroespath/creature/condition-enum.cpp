//
// condition-enum.cpp
//
#include "condition-enum.hpp"

#include "heroespath/game-data-file.hpp"

#include <exception>
#include <sstream>


namespace heroespath
{
namespace creature
{

    namespace condition
    {

        const std::string ToString(const condition::Enum E)
        {
            return Name(E);
        }


        const std::string Name(const condition::Enum E)
        {
            switch (E)
            {
                case Good:          { return "Good"; }
                case Frightened:    { return "Frightened"; }
                case Dazed:         { return "Dazed"; }
                case Tripped:       { return "Tripped"; }
                //case Diseased:      { return "Diseased"; }
                //case Poisoned:      { return "Poisoned"; }
                case Unconscious:   { return "Unconscious"; }
                case Stone:         { return "Stone"; }
                case Dead:          { return "Dead"; }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "heroespath::creature::condition::Enum::Name(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
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
                case Good:          { return ALL; } //keep the 'Good' condition at a different number from 'Benefitial'
                case Frightened:    { return LEAST_HARMFUL; }
                case Dazed:         { return 10; }
                case Tripped:       { return 20; }
                //case Diseased:      { return 40; }
                //case Poisoned:      { return 30; }
                case Unconscious:   { return 80; }
                case Stone:         { return 90; }
                case Dead:          { return MOST_HARMFUL; }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "heroespath::creature::condition::Enum::Severity(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

    }

}
}
