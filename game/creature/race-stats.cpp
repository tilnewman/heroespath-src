//
// race-stats.cpp
//
#include "race-stats.hpp"

#include "game/game-data-file.hpp"
#include "game/creature/stat-modifier-loader.hpp"


namespace game
{
namespace creature
{

    RaceStatSetMap_t RaceStatModifier::raceStatSetMap_;


    const stats::StatSet RaceStatModifier::Get(const creature::race::Enum ENUM)
    {
        if (raceStatSetMap_.empty())
        {
            for (int i(0); i < race::Count_PlayerRaces; ++i)
            {
                auto const NEXT_ENUM{ static_cast<race::Enum>(i) };
                auto const NEXT_ENUM_STR{ race::ToString(NEXT_ENUM) };
                auto const NEXT_KEY_STR{ "heroespath-creature-race-stat-modifiers-" + NEXT_ENUM_STR };
                std::string nextStatSetStr{ "" };
                game::GameDataFile::Instance()->GetStr(nextStatSetStr, NEXT_KEY_STR);
                raceStatSetMap_[NEXT_ENUM] = StatModifierLoader::ConvertStringToStatSet(nextStatSetStr);
            }
        }

        return raceStatSetMap_[ENUM];
    }

}
}
