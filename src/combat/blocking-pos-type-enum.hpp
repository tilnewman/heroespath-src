// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_BLOCKINGPOS_TYPE_ENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_BLOCKINGPOS_TYPE_ENUM_HPP_INCLUDED
//
// target-enum.hpp
//
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace combat
{

    // Responsible for enumerating blocking position types.
    struct BlockingPosType : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Front = 0,
            Support,
            Commanders,
            Reluctant,
            Casters,
            Last,
            Count
        };
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_BLOCKINGPOS_TYPE_ENUM_HPP_INCLUDED
