// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_STATMODIFIERLOADER_HPP_INCLUDED
#define HEROESPATH_CREATURE_STATMODIFIERLOADER_HPP_INCLUDED
//
// stat-modifier-loader.cpp
//
#include "stats/stat-set.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    // Stat Modifier Game Data File Loading helper functions.
    struct StatModifierLoader
    {
        static const stats::StatSet ConvertStringToStatSet(const std::string & DATA_FILE_VALUE_STR);
    };
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_STATMODIFIERLOADER_HPP_INCLUDED
