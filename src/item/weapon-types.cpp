// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// weapon-types.cpp
//
#include "weapon-types.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        const std::string sword_type::ToString(const sword_type::Enum SWORD_TYPE)
        {
            switch (SWORD_TYPE)
            {
                case Claymore:
                {
                    return "Claymore";
                }
                case Longsword:
                {
                    return "Longsword";
                }
                case Flamberg:
                {
                    return "Flamberg";
                }
                case Knightlysword:
                {
                    return "KnightlySword";
                }
                case Broadsword:
                {
                    return "Broadsword";
                }
                case Falcata:
                {
                    return "Falcata";
                }
                case Saber:
                {
                    return "Saber";
                }
                case Cutlass:
                {
                    return "Cutlass";
                }
                case Rapier:
                {
                    return "Rapier";
                }
                case Gladius:
                {
                    return "Gladius";
                }
                case Shortsword:
                {
                    return "Shortsword";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(SWORD_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string sword_type::Name(const sword_type::Enum SWORD_TYPE)
        {
            if (SWORD_TYPE == sword_type::Knightlysword)
            {
                return "Knightly";
            }
            else
            {
                return sword_type::ToString(SWORD_TYPE);
            }
        }

        const std::string axe_type::ToString(const axe_type::Enum AXE_TYPE)
        {
            switch (AXE_TYPE)
            {
                case Handaxe:
                {
                    return "Handaxe";
                }
                case Sickle:
                {
                    return "Sickle";
                }
                case Battleaxe:
                {
                    return "Battleaxe";
                }
                case Waraxe:
                {
                    return "Waraxe";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(AXE_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string axe_type::Name(const axe_type::Enum AXE_TYPE)
        {
            switch (AXE_TYPE)
            {
                case Handaxe:
                {
                    return "Hand";
                }
                case Sickle:
                {
                    return "Sickle";
                }
                case Battleaxe:
                {
                    return "Battle";
                }
                case Waraxe:
                {
                    return "War";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(AXE_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string club_type::ToString(const club_type::Enum CLUB_TYPE)
        {
            switch (CLUB_TYPE)
            {
                case Spiked:
                {
                    return "Spiked";
                }
                case Maul:
                {
                    return "Maul";
                }
                case Mace:
                {
                    return "Mace";
                }
                case Warhammer:
                {
                    return "Warhammer";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(CLUB_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string whip_type::ToString(const whip_type::Enum WHIP_TYPE)
        {
            switch (WHIP_TYPE)
            {
                case Bullwhip:
                {
                    return "Bullwhip";
                }
                case Flail:
                {
                    return "Flail";
                }
                case MaceAndChain:
                {
                    return "MaceAndChain";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(WHIP_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string whip_type::Name(const whip_type::Enum WHIP_TYPE)
        {
            if (WHIP_TYPE == whip_type::MaceAndChain)
            {
                return "Mace And Chain";
            }
            else
            {
                return whip_type::ToString(WHIP_TYPE);
            }
        }

        const std::string projectile_type::ToString(const projectile_type::Enum PROJECTILE_TYPE)
        {
            switch (PROJECTILE_TYPE)
            {
                case Blowpipe:
                {
                    return "Blowpipe";
                }
                case Sling:
                {
                    return "Sling";
                }
                case Shortbow:
                {
                    return "Shortbow";
                }
                case Longbow:
                {
                    return "Longbow";
                }
                case CompositeBow:
                {
                    return "CompositeBow";
                }
                case Crossbow:
                {
                    return "Crossbow";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(PROJECTILE_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string projectile_type::Name(const projectile_type::Enum PROJECTILE_TYPE)
        {
            if (PROJECTILE_TYPE == projectile_type::CompositeBow)
            {
                return "Composite Bow";
            }
            else
            {
                return projectile_type::ToString(PROJECTILE_TYPE);
            }
        }

        const std::string bladedstaff_type::ToString(const bladedstaff_type::Enum BSTAFF_TYPE)
        {
            switch (BSTAFF_TYPE)
            {
                case Spear:
                {
                    return "Spear";
                }
                case ShortSpear:
                {
                    return "ShortSpear";
                }
                case Scythe:
                {
                    return "Scythe";
                }
                case Pike:
                {
                    return "Pike";
                }
                case Partisan:
                {
                    return "Partisan";
                }
                case Halberd:
                {
                    return "Halberd";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(BSTAFF_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string bladedstaff_type::Name(const bladedstaff_type::Enum BSTAFF_TYPE)
        {
            if (BSTAFF_TYPE == bladedstaff_type::ShortSpear)
            {
                return "Short Spear";
            }
            else
            {
                return bladedstaff_type::ToString(BSTAFF_TYPE);
            }
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
