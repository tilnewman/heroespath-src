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
            case SingleEnemy:       { return "SingleEnemy"; }
            case AllEnemies:        { return "AllEnemies"; }
            case SingleCharacter:   { return "SingleCharacter"; }
            case AllCharacters:     { return "AllCharacters"; }
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
            case SingleEnemy:       { return "Single Enemy"; }
            case AllEnemies:        { return "All Enemies"; }
            case SingleCharacter:   { return "Single Character"; }
            case AllCharacters:     { return "All Characters"; }
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
            case SingleEnemy:       { ss << "a single enemy"; break; }
            case AllEnemies:        { ss << "all enemies"; break; }
            case SingleCharacter:   { ss << "a single character"; break; }
            case AllCharacters:     { ss << "all characters"; break; }
            case Item:              { ss << "an item"; break; }
            case QuestSpecific:
            case None:
            case Count:
            default:                { return ""; }
        }

        return PREAMBLE + ss.str();
    }

}
