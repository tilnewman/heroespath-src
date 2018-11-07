// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
//
// turn-action-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace combat
{

    struct TurnAction : public misc::EnumBaseCounting<TurnAction, misc::EnumFirstValue::Nothing>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Nothing = 0,
            Attack,
            Block,
            Advance,
            Retreat,
            Fly,
            Land,
            Cast,
            PlaySong,
            SkyPounce,
            LandPounce,
            Roar,
            ChangeWeapon,
            Run,
            TreasureUnlock,
            Count
        };

        static const std::string ToString(const TurnAction::Enum);
        static const std::string Name(const TurnAction::Enum);
        static bool IsMove(const Enum ENUM) { return ((ENUM == Advance) || (ENUM == Retreat)); }
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
