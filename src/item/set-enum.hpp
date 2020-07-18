// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_SET_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_SET_ENUM_HPP_INCLUDED
//
// set-enum.hpp
//
#include "creature/role-enum.hpp"
#include "misc/enum-common.hpp"

#include <string_view>

namespace heroespath
{
namespace item
{

    // Set items are never Named, Elemental, or Misc::IsUnique(), but are always magical
    struct Set : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Assassins = 0,
            Neverwinter,
            Tickler,
            Magus,
            Necromancers,
            Warlocks,
            LichKings,
            Sages,
            Shamans,
            Oracles,
            Angelic,
            Balladeers,
            Troubadours,
            Muses,
            Cavaliers,
            Champions,
            Paladins,
            Berserkers,
            Rosewood,
            Dragonslayers,
            EventideRider,
            Hunters,
            SureShot,
            Marksmans,
            Deadeye,
            DuskRanger,
            VenomBow,
            CritterChannelers,
            MammalianHead,
            SavageTaskmasters,
            MonsterOverseers,
            BeastRulers,
            Count
        };

        static constexpr std::string_view Name(const Set::Enum SET_TYPE) noexcept
        {
            switch (SET_TYPE)
            {
                case Assassins: return "The Assassin's";
                case Neverwinter: return "The Neverwinter";
                case Tickler: return "The Tickler's";
                case Magus: return "The Magus'";
                case Necromancers: return "The Necromancer's";
                case Warlocks: return "The Warlock's";
                case LichKings: return "The Lich King's";
                case Sages: return "The Sage's";
                case Shamans: return "The Shaman's";
                case Oracles: return "The Oracle's";
                case Angelic: return "The Angelic";
                case Balladeers: return "The Balladeer's";
                case Troubadours: return "The Troubadour's";
                case Muses: return "The Muses'";
                case Cavaliers: return "The Cavalier's";
                case Champions: return "The Champion's";
                case Paladins: return "The Paladin's";
                case Berserkers: return "The Berserker's";
                case Rosewood: return "The Rosewood";
                case Dragonslayers: return "The Dragonslayer's";
                case EventideRider: return "The Eventide Rider's";
                case Hunters: return "The Hunter's";
                case SureShot: return "The Sure Shot's";
                case Marksmans: return "The Marksman's";
                case Deadeye: return "The Deadeye's";
                case DuskRanger: return "The Dusk Ranger's";
                case VenomBow: return "The Venom Bow";
                case CritterChannelers: return "The Critter Channeler's";
                case MammalianHead: return "The Mammalian Head's";
                case SavageTaskmasters: return "The Savage Taskmaster's";
                case MonsterOverseers: return "The Monster Overseer's";
                case BeastRulers: return "The Beast Ruler's";
                case Count: return "Count";
                default: return "item::Set::Name(ENUM=out_of_range)";
            }
        }

        static constexpr creature::role::Enum RoleRestriction(const Set::Enum SET_TYPE) noexcept
        {
            switch (SET_TYPE)
            {
                case Assassins:
                case Tickler:
                case Neverwinter: return creature::role::Thief;

                case Magus:
                case Necromancers:
                case Warlocks:
                case LichKings: return creature::role::Sorcerer;

                case Sages:
                case Shamans:
                case Oracles:
                case Angelic: return creature::role::Cleric;

                case Balladeers:
                case Troubadours:
                case Muses: return creature::role::Bard;

                case Cavaliers:
                case Champions:
                case Paladins:
                case Berserkers:
                case Rosewood:
                case Dragonslayers:
                case EventideRider: return creature::role::Knight;

                case Hunters:
                case SureShot:
                case Marksmans:
                case Deadeye:
                case DuskRanger:
                case VenomBow: return creature::role::Archer;

                case CritterChannelers:
                case MammalianHead:
                case SavageTaskmasters:
                case MonsterOverseers:
                case BeastRulers: return creature::role::Beastmaster;

                case Count:
                default: return creature::role::Count;
            }
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_SET_ENUM_HPP_INCLUDED
