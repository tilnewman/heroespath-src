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
#include "misc/strings-split-by-char.hpp"
#include "misc/strings.hpp"

#include "boost/lexical_cast.hpp"

#include <vector>

namespace heroespath
{
namespace creature
{

    const StatSet
        StatModifierLoader::ConvertStringToStatSet(const std::string & DATA_FILE_VALUE_STR)
    {
        std::vector<std::string> statValuesStrVec;

        misc::SplitByChar(DATA_FILE_VALUE_STR, statValuesStrVec, ',', true, true);

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (statValuesStrVec.size() == 6),
            "creature::StatModifierLoader::Load() was unable to extract exactly "
                << "six comma sep values from \"" << DATA_FILE_VALUE_STR
                << "\".  (There must be a typo in the game data file.)");

        const Trait_t ERROR_VALUE { -123456789 };

        StatSet statSet;
        for (std::size_t i(0); i < statValuesStrVec.size(); ++i)
        {
            Trait_t nextValue { ERROR_VALUE };
            try
            {
                nextValue = boost::lexical_cast<Trait_t>(statValuesStrVec.at(i));
            }
            catch (...)
            {
                nextValue = ERROR_VALUE;
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (nextValue != ERROR_VALUE),
                "creature::StatModifierLoader::Load() was unable to convert \""
                    << statValuesStrVec.at(i) << "\" to an int value.");

            const auto NEXT_STAT_ENUM { static_cast<Traits::Enum>(i) };
            statSet.Set(NEXT_STAT_ENUM, nextValue);
        }

        return statSet;
    }
} // namespace creature
} // namespace heroespath
