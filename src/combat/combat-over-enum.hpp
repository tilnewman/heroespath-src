// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_OVER_ENUM_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_OVER_ENUM_HPP_INCLUDED
//
// combat-over-enum.hpp
//
#include <string>

namespace heroespath
{
namespace combat
{

    struct CombatEnd
    {
        enum Enum
        {
            Win = 0,
            Lose,
            Ran,
            Count
        };

        static const std::string ToString(const CombatEnd::Enum);
    };
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_OVER_ENUM_HPP_INCLUDED
