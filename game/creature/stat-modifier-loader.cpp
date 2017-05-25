// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// stat-modifier-loader.cpp
//
#include "stat-modifier-loader.hpp"

#include "utilz/assertlogandthrow.hpp"

#include "stringutil/stringhelp.hpp"

#include "boost/lexical_cast.hpp"

#include <vector>


namespace game
{
namespace creature
{

    const stats::StatSet StatModifierLoader::ConvertStringToStatSet(const std::string & DATA_FILE_VALUE_STR)
    {
        std::vector<std::string> statValuesStrVec;
        appbase::stringhelp::SplitByChar(DATA_FILE_VALUE_STR, statValuesStrVec, ',', true, true);
        M_ASSERT_OR_LOGANDTHROW_SS((statValuesStrVec.size() == 6), "game::creature::StatModifierLoader::Load() was unable to extract exactly six comma sep values from \"" << DATA_FILE_VALUE_STR << "\".  (There must be a typo in the game data file.)");

        const stats::Stat_t ERROR_VALUE{ -123456789 };

        stats::StatSet statSet;
        for (std::size_t i(0); i < statValuesStrVec.size(); ++i)
        {
            stats::Stat_t nextValue{ ERROR_VALUE };
            try
            {
                nextValue = boost::lexical_cast<stats::Stat_t>(statValuesStrVec.at(i));
            }
            catch (...)
            {
                nextValue = ERROR_VALUE;
            }

            M_ASSERT_OR_LOGANDTHROW_SS((nextValue != ERROR_VALUE), "game::creature::StatModifierLoader::Load() was unable to convert \"" << statValuesStrVec.at(i) << "\" to an int value.");

            auto const NEXT_STAT_ENUM{ static_cast<stats::stat::Enum>(i) };
            statSet.Get(NEXT_STAT_ENUM).ResetAll(nextValue);
        }

        return statSet;
    }

}
}
