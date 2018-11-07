// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_WOLFENCLASSENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_WOLFENCLASSENUM_HPP_INCLUDED
//
// wolfen-class-enum.hpp
//  An enumeration of all Wolfen classes as determined by Rank.
//
#include "misc/enum-util.hpp"
#include "misc/types.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct wolfen_class : misc::EnumBaseCounting<wolfen_class, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Pup = 0,
            Juvenile,
            Adult,
            Noble,
            Highborn,
            Elder,
            Count
        };

        static const std::string ToString(const wolfen_class::Enum);
        static const std::string Desc(const wolfen_class::Enum);
        static wolfen_class::Enum ClassFromRank(const Rank_t &);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_WOLFENCLASSENUM_HPP_INCLUDED
