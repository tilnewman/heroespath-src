//
// target-enum.cpp
//
#include "target-enum.hpp"

#include "utilz/random.hpp"

#include <sstream>


namespace game
{

    const std::string TargetType::ToString(const Enum E)
    {
        switch (E)
        {
            case None:              { return "None"; }
            case SingleOpponent:    { return "SingleOpponent"; }
            case AllOpponents:      { return "AllOpponents"; }
            case SingleCompanion:   { return "SingleCompanion"; }
            case AllCompanions:     { return "AllCompanions"; }
            case Item:              { return "Item"; }
            case QuestSpecific:     { return "QuestSpecific"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::TargetType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string TargetType::Name(const Enum E)
    {
        switch (E)
        {
            case None:              { return "(None)"; }
            case SingleOpponent:    { return "Single Opponent"; }
            case AllOpponents:      { return "All Opponents"; }
            case SingleCompanion:   { return "Single Companion"; }
            case AllCompanions:     { return "All Companions"; }
            case Item:              { return "Item"; }
            case QuestSpecific:     { return "(Quest Specific)"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::TargetType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string TargetType::ActionPhrase(const Enum E)
    {
        auto const PREAMBLE{ ((utilz::random::Bool()) ? "at " : "upon ") };

        std::ostringstream ss;
        switch (E)
        {
            case SingleOpponent:    { ss << "a single opponent"; break; }
            case AllOpponents:      { ss << "all opponent"; break; }
            case SingleCompanion:   { ss << "a single companion"; break; }
            case AllCompanions:     { ss << "all companion"; break; }
            case Item:              { ss << "an item"; break; }
            case QuestSpecific:
            case None:
            case Count:
            default:                { return ""; }
        }

        return PREAMBLE + ss.str();
    }

}
