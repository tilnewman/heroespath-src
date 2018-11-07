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
//  An enumeration of all Dragon classes as determined by Rank.
//
#include "misc/enum-util.hpp"
#include "misc/types.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct dragon_class : misc::EnumBaseCounting<dragon_class, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
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

        static const std::string ToString(const dragon_class::Enum);
        static const std::string Desc(const dragon_class::Enum);
        static dragon_class::Enum ClassFromRank(const Rank_t &);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_DRAGONCLASSENUM_HPP_INCLUDED
