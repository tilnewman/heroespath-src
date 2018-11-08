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
//  An enumeration defining each Role of creature, both player and non-player.
//
#include "combat/blocking-pos-type-enum.hpp"
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct role : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
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

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum ENUM) { return ToString(ENUM); }
        static const std::string Desc(const Enum);
        static const std::string Abbr(const Enum);
        static bool CanFly(const Enum);
        static bool WillInitiallyFly(const Enum);
        static combat::BlockingPosType::Enum BlockingPosType(const Enum);
        static std::vector<Enum> RolesOfBlockingPosType(const combat::BlockingPosType::Enum);
    };

    using RoleVec_t = std::vector<role::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
