// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// race-stats.cpp
//
#include "race-stats.hpp"

#include "creature/stat-modifier-loader.hpp"
#include "game/game-data-file.hpp"

namespace heroespath
{
namespace creature
{

    RaceStatSetMap_t RaceStatModifier::raceStatSetMap_;

    const StatSet RaceStatModifier::Get(const creature::race::Enum ENUM)
    {
        if (raceStatSetMap_.Empty())
        {
            for (misc::EnumUnderlying_t i(0); i < race::Count_PlayerRaces; ++i)
            {
                auto const NEXT_ENUM{ static_cast<race::Enum>(i) };
                auto const NEXT_ENUM_STR{ race::ToString(NEXT_ENUM) };

                auto const NEXT_KEY_STR{ "heroespath-creature-race-stat-modifiers-"
                                         + NEXT_ENUM_STR };

                std::string nextStatSetStr{ "" };
                game::GameDataFile::Instance()->GetStr(nextStatSetStr, NEXT_KEY_STR);

                raceStatSetMap_[NEXT_ENUM]
                    = StatModifierLoader::ConvertStringToStatSet(nextStatSetStr);
            }
        }

        return raceStatSetMap_[ENUM];
    }

} // namespace creature
} // namespace heroespath
