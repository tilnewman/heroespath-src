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
