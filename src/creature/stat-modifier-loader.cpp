// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stat-modifier-loader.cpp
//
#include "stat-modifier-loader.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/strings.hpp"

#include <vector>

namespace heroespath
{
namespace creature
{

    const StatSet
        StatModifierLoader::ConvertStringToStatSet(const std::string & DATA_FILE_VALUE_STR)
    {
        const std::vector<std::string> STAT_VALUES_STR_VEC { misc::SplitByChars(
            DATA_FILE_VALUE_STR, misc::SplitHow(",")) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (STAT_VALUES_STR_VEC.size() == 6),
            "creature::StatModifierLoader::Load() was unable to extract exactly "
                << "six comma sep values from \"" << DATA_FILE_VALUE_STR
                << "\".  (There must be a typo in the game data file.)");

        const Trait_t ERROR_VALUE { -123456789 };

        StatSet statSet;
        for (std::size_t i(0); i < STAT_VALUES_STR_VEC.size(); ++i)
        {
            Trait_t nextValue = misc::ToNumberOr(STAT_VALUES_STR_VEC.at(i), ERROR_VALUE);

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (nextValue != ERROR_VALUE),
                "creature::StatModifierLoader::Load() was unable to convert \""
                    << STAT_VALUES_STR_VEC.at(i) << "\" to an int value.");

            const auto NEXT_STAT_ENUM { static_cast<Traits::Enum>(i) };
            statSet.Set(NEXT_STAT_ENUM, nextValue);
        }

        return statSet;
    }
} // namespace creature
} // namespace heroespath
