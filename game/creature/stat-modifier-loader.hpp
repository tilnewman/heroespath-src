#ifndef GAME_CREATURE_STATMODIFIERLOADER_HPP_INCLUDED
#define GAME_CREATURE_STATMODIFIERLOADER_HPP_INCLUDED
//
// stat-modifier-loader.cpp
//
#include "game/stats/stat-set.hpp"

#include <string>


namespace game
{
namespace creature
{

    //Stat Modifier Game Data File Loading helper functions.
    struct StatModifierLoader
    {
        static const stats::StatSet ConvertStringToStatSet(const std::string & DATA_FILE_VALUE_STR);
    };

}
}


#endif //GAME_CREATURE_STATMODIFIERLOADER_HPP_INCLUDED
