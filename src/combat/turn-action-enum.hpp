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
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace combat
{

    struct TurnAction : public EnumBaseCounting<EnumNameOfZeroIsNothing>
    {
        enum Enum : EnumUnderlying_t
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

        static constexpr std::string_view Name(const TurnAction::Enum ENUM)
        {
            switch (ENUM)
            {
                case Nothing: return "Does Nothing";
                case Attack: return "Attacks";
                case Block: return "Blocks";
                case Advance: return "Advances";
                case Retreat: return "Retreats";
                case Fly: return "Flies";
                case Land: return "Lands";
                case Cast: return "Casts a Spell";
                case PlaySong: return "Plays a Magical Song";
                case SkyPounce: return "Pounces from the Sky";
                case LandPounce: return "Pounces";
                case Roar: return "Roars";
                case ChangeWeapon: return "Changes Weapons";
                case Run: return "Runs Away";
                case TreasureUnlock: return "Springs the trap";
                case Count: return "Count";
                default: return "combat::TurnAction_out_of_range";
            }
        }
        static bool IsMove(const Enum ENUM) { return ((ENUM == Advance) || (ENUM == Retreat)); }
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
