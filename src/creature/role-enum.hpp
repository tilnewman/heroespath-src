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
            Thug,
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

        static constexpr Enum PlayerRoleCount = Thug;

        static constexpr std::string_view Name(const Enum ENUM) { return NAMEOF_ENUM(ENUM); }

        static const std::string Desc(const Enum);

        static constexpr std::string_view Abbr(const role::Enum ENUM) noexcept
        {
            if (ENUM == Beastmaster)
            {
                return "Bsm";
            }
            else if (ENUM == Thief)
            {
                return "Thf";
            }
            else if (ENUM == Wolfen)
            {
                return "Wlf";
            }
            else
            {
                return NAMEOF_ENUM(ENUM).substr(0, 3);
            }
        }

        static constexpr bool CanFly(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Firebrand) || (ENUM == Sylavin) || (ENUM == Wing) || (ENUM == Whelp)
                || (ENUM == Bat));
        }

        static constexpr bool WillInitiallyFly(const Enum ENUM) noexcept
        {
            return ((ENUM == Wing) || (ENUM == Whelp) || (ENUM == Bat));
        }

        static constexpr combat::BlockingPosType::Enum BlockingPosType(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Mountain:
                case Spider:
                case Beetle:
                case Boar:
                case Lion:
                case Ramonaut:
                case Serpent:
                case Whelp:
                case Pod:
                case Spike:
                case Skeleton:
                case Thug:
                case Mugger:
                case Drunk:
                case Grunt:
                case FourArmed:
                case Tendrilus:
                case TwoHeaded:
                case Giant:
                case Smasher:
                case Strangler:
                case Soldier:
                case Brute:
                case Berserker:
                case Knight:
                case Wolfen:
                case Firebrand:
                case Ranger:
                case Water:
                case Blacksmith:
                case Cat:
                case Wolf:
                case Sylavin: return combat::BlockingPosType::Front;

                case Ghost:
                case Bat:
                case Beastmaster:
                case Bard:
                case Archer:
                case Wing: return combat::BlockingPosType::Support;

                case Captain:
                case Warlord:
                case Chieftain: return combat::BlockingPosType::Commanders;

                case Thief: return combat::BlockingPosType::Reluctant;

                case Elder:
                case Shaman:
                case Sorcerer:
                case Cleric: return combat::BlockingPosType::Casters;

                case Trader: return combat::BlockingPosType::Last;

                case Count:
                default: return combat::BlockingPosType::Count;
            }
        }

        static std::vector<Enum> RolesOfBlockingPosType(const combat::BlockingPosType::Enum);
    };

    using RoleVec_t = std::vector<role::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
