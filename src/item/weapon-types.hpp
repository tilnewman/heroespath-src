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
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        struct sword_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Shortsword = 0,
                Gladius,
                Cutlass,
                Falcata,
                Rapier,
                Saber,
                Broadsword,
                Longsword,
                KnightlySword,
                Flamberg,
                Claymore,
                Count
            };

            inline static const std::string Name(const sword_type::Enum SWORD_TYPE)
            {
                if (SWORD_TYPE == sword_type::KnightlySword)
                {
                    return "Knightly";
                }
                else
                {
                    return std::string(NAMEOF_ENUM(SWORD_TYPE));
                }
            }
        };

        struct axe_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Handaxe = 0,
                Sickle,
                Battleaxe,
                Waraxe,
                Count
            };

            static constexpr std::string_view Name(const axe_type::Enum AXE_TYPE)
            {
                switch (AXE_TYPE)
                {
                    case Handaxe: return "Hand";
                    case Sickle: return "Sickle";
                    case Battleaxe: return "Battle";
                    case Waraxe: return "War";
                    case Count: return "Count";
                    default: return "item::weapon::Name(ENUM)_out_of_bounds";
                }
            }
        };

        struct club_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Spiked = 0,
                Maul,
                Mace,
                Warhammer,
                Count
            };
        };

        struct whip_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Bullwhip = 0,
                Flail,
                MaceAndChain,
                Count
            };

            inline static const std::string Name(const whip_type::Enum WHIP_TYPE)
            {
                if (WHIP_TYPE == whip_type::MaceAndChain)
                {
                    return "Mace And Chain";
                }
                else
                {
                    return std::string(NAMEOF_ENUM(WHIP_TYPE));
                }
            }
        };

        struct projectile_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Blowpipe = 0,
                Sling,
                Shortbow,
                Longbow,
                Crossbow,
                CompositeBow,
                Count
            };

            inline static const std::string Name(const projectile_type::Enum PROJECTILE_TYPE)
            {
                if (PROJECTILE_TYPE == projectile_type::CompositeBow)
                {
                    return "Composite Bow";
                }
                else
                {
                    return std::string(NAMEOF_ENUM(PROJECTILE_TYPE));
                }
            }
        };

        struct bladedstaff_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                ShortSpear = 0,
                Spear,
                Scythe,
                Pike,
                Partisan,
                Halberd,
                Count
            };

            inline static const std::string Name(const bladedstaff_type::Enum BSTAFF_TYPE)
            {
                if (BSTAFF_TYPE == bladedstaff_type::ShortSpear)
                {
                    return "Short Spear";
                }
                else
                {
                    return std::string(NAMEOF_ENUM(BSTAFF_TYPE));
                }
            }
        };

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
