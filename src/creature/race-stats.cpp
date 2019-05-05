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
#include "misc/config-file.hpp"

namespace heroespath
{
namespace creature
{

    RaceStatSetMap_t RaceStatModifier::raceStatSetMap_;

    const StatSet RaceStatModifier::Get(const creature::race::Enum ENUM)
    {
        if (raceStatSetMap_.Empty())
        {
            raceStatSetMap_.Reserve(64);

            for (EnumUnderlying_t i(0); i < race::Count_PlayerRaces; ++i)
            {
                const auto NEXT_ENUM { static_cast<race::Enum>(i) };
                const auto NEXT_ENUM_STR { race::ToString(NEXT_ENUM) };

                const auto NEXT_KEY_STR { "creature-race-stat-modifiers-" + NEXT_ENUM_STR };

                const auto STAT_STR { misc::ConfigFile::Instance()->Value(NEXT_KEY_STR) };

                raceStatSetMap_[NEXT_ENUM] = StatModifierLoader::ConvertStringToStatSet(STAT_STR);
            }
        }

        return raceStatSetMap_[ENUM];
    }

} // namespace creature
} // namespace heroespath
