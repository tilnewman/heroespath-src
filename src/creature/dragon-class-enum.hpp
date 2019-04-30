// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_DRAGONCLASSENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_DRAGONCLASSENUM_HPP_INCLUDED
//
// dragon-class-enum.hpp
//
#include "game/strong-types.hpp"
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct dragon_class : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Hatchling = 0,
            Whelp,
            Fledgling,
            Juvenile,
            Adult,
            Wyrm,
            Skycaster,
            Elder,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum ENUM) { return ToString(ENUM); }
        static const std::string Desc(const Enum);
        static dragon_class::Enum ClassFromRank(const Rank_t &);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_DRAGONCLASSENUM_HPP_INCLUDED
