// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// complexity-type.cpp
//
#include "complexity-type.hpp"

#include "creature/creature.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"

namespace heroespath
{
namespace creature
{
    namespace nonplayer
    {

        complexity_type::Enum complexity_type::FromCreature(const CreaturePtr_t & CHARACTER_PTR)
        {
            const auto RACE_COMPLEXITY_STR { misc::ConfigFile::Instance()->Value(
                std::string("nonplayer-ownershipprofile-complexitytype-race-")
                    .append(CHARACTER_PTR->RaceName())) };

            if (RACE_COMPLEXITY_STR == "based-on-role")
            {
                return EnumUtil<complexity_type>::FromString(misc::ConfigFile::Instance()->Value(
                    "nonplayer-ownershipprofile-complexitytype-role-"
                    + NAMEOF_ENUM_STR(CHARACTER_PTR->Role())));
            }
            else
            {
                return EnumUtil<complexity_type>::FromString(RACE_COMPLEXITY_STR);
            }
        }

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath
