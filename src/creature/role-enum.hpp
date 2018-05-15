// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
//
// role-enum.hpp
//  An enumeration defining each Role of creature, both player and non_player.
//
#include "combat/blocking-pos-type-enum.hpp"
#include "misc/enum-util.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct role : public misc::EnumBaseCounting<role, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Archer = 0,
            Bard,
            Beastmaster,
            Cleric,
            Knight,
            Sorcerer,
            Thief,
            Firebrand,
            Sylavin,
            Wolfen,
            PlayerRoleCount,
            Thug = PlayerRoleCount,
            Mugger,
            Drunk,
            Grunt,
            Brute,
            Berserker,
            Mountain,
            Captain,
            Chieftain,
            Trader,
            Warlord,
            Shaman,
            Smasher,
            Strangler,
            Soldier,
            TwoHeaded,
            Giant,
            Elder,
            FourArmed,
            Tendrilus,
            Wing,
            Whelp,
            Pod,
            Spike,
            Skeleton,
            Ranger,
            Water,
            Blacksmith,
            Spider,
            Beetle,
            Boar,
            Lion,
            Ramonaut,
            Serpent,
            Bat,
            Ghost,
            Cat,
            Wolf,
            Count
        };

        static const std::string ToString(const role::Enum);
        static const std::string Desc(const role::Enum);
        static const std::string Abbr(const role::Enum);
        static bool CanFly(const role::Enum);
        static bool WillInitiallyFly(const role::Enum);
        static combat::BlockingPosType::Enum BlockingPosType(const role::Enum);
        static std::vector<role::Enum> RolesOfBlockingPosType(const combat::BlockingPosType::Enum);
    };

    using RoleVec_t = std::vector<role::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
