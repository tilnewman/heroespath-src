// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
//
// weapons-types.hpp
//  A set of enumerations that together define a weapon.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        struct sword_type : public misc::EnumBaseCounting<sword_type, misc::EnumFirstValue::Valid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Shortsword = 0,
                Gladius,
                Cutlass,
                Falcata,
                Rapier,
                Saber,
                Broadsword,
                Longsword,
                Knightlysword,
                Flamberg,
                Claymore,
                Count
            };

            static const std::string ToString(const sword_type::Enum);
            static const std::string Name(const sword_type::Enum);
        };

        struct axe_type : public misc::EnumBaseCounting<axe_type, misc::EnumFirstValue::Valid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Handaxe = 0,
                Sickle,
                Battleaxe,
                Waraxe,
                Count
            };

            static const std::string ToString(const axe_type::Enum);
            static const std::string Name(const axe_type::Enum);
        };

        struct club_type : public misc::EnumBaseCounting<club_type, misc::EnumFirstValue::Valid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Spiked = 0,
                Maul,
                Mace,
                Warhammer,
                Count
            };

            static const std::string ToString(const club_type::Enum);
        };

        struct whip_type : public misc::EnumBaseCounting<whip_type, misc::EnumFirstValue::Valid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Bullwhip = 0,
                Flail,
                MaceAndChain,
                Count
            };

            static const std::string ToString(const whip_type::Enum);
            static const std::string Name(const whip_type::Enum);
        };

        struct projectile_type
            : public misc::EnumBaseCounting<projectile_type, misc::EnumFirstValue::Valid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Blowpipe = 0,
                Sling,
                Shortbow,
                Longbow,
                Crossbow,
                CompositeBow,
                Count
            };

            static const std::string ToString(const projectile_type::Enum);
            static const std::string Name(const projectile_type::Enum);
        };

        struct bladedstaff_type
            : public misc::EnumBaseCounting<bladedstaff_type, misc::EnumFirstValue::Valid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                ShortSpear = 0,
                Spear,
                Scythe,
                Pike,
                Partisan,
                Halberd,
                Count
            };

            static const std::string ToString(const bladedstaff_type::Enum);
            static const std::string Name(const bladedstaff_type::Enum);
        };

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
