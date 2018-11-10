// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// strategy-enums.cpp
//
#include "strategy-enums.hpp"

#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace combat
{
    namespace strategy
    {
        const std::string SelectType::ToString(const Enum ENUM, const EnumStringHow & HOW)
        {
            return EnumUtil<SelectType>::ToString(ENUM, HOW);
        }

        const std::string SelectType::ToStringPopulate(
            const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
        {
            std::string str;
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Pixie, "Pixie", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Dragon, "Dragon", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Human, "Human", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Gnome, "Gnome", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Wolfen, "Wolfen", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Archer, "Archer", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Sorcerer, "Sorcerer", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Knight, "Knight", SEPARATOR);

            AppendNameIfBitIsSet(
                str, ENUM_VALUE, SelectType::Beastmaster, "Beastmaster", SEPARATOR);

            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Cleric, "Cleric", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Theif, "Theif", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Bard, "Bard", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::FireBrand, "FireBrand", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Sylavin, "Sylavin", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Projectile, "Projectile", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Caster, "Caster", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::CanFly, "CanFly", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::CantFly, "CantFly", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::Beast, "Beast", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, SelectType::NotBeast, "NotBeast", SEPARATOR);
            return str;
        }

        const std::string RefineType::ToString(const Enum ENUM, const EnumStringHow & HOW)
        {
            return EnumUtil<RefineType>::ToString(ENUM, HOW);
        }

        const std::string RefineType::ToStringPopulate(
            const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
        {
            std::string str;
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Murderer, "Murderer", SEPARATOR);

            AppendNameIfBitIsSet(
                str, ENUM_VALUE, RefineType::Bloodthirsty, "Bloodthirsty", SEPARATOR);

            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Coward, "Coward", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Hit, "Hit", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Attack, "Attack", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::MusicMaker, "MusicMaker", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Caster, "Caster", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Enchanted, "Enchanted", SEPARATOR);

            AppendNameIfBitIsSet(
                str, ENUM_VALUE, RefineType::NotEnchanted, "NotEnchanted", SEPARATOR);

            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::Steadfast, "Steadfast", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::LastTo, "LastTo", SEPARATOR);
            AppendNameIfBitIsSet(str, ENUM_VALUE, RefineType::MostDamage, "MostDamage", SEPARATOR);
            return str;
        }

        const std::string AdvanceType::ToString(const AdvanceType::Enum ADVANCE_TYPE)
        {
            switch (ADVANCE_TYPE)
            {
                case None:
                {
                    return "None";
                }
                case Cowardly:
                {
                    return "Cowardly";
                }
                case Hesitant:
                {
                    return "Hesitant";
                }
                case Charger:
                {
                    return "Charger";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(ADVANCE_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string RetreatType::ToString(const Enum RETREAT_TYPE)
        {
            switch (RETREAT_TYPE)
            {
                case None:
                {
                    return "None";
                }
                case Wary:
                {
                    return "Wary";
                }
                case Coward:
                {
                    return "Coward";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(RETREAT_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

        const std::string FrequencyType::ToString(const FrequencyType::Enum FREQ_TYPE)
        {
            switch (FREQ_TYPE)
            {
                case Never:
                {
                    return "Never";
                }
                case Once:
                {
                    return "Once";
                }
                case Twice:
                {
                    return "Twice";
                }
                case Thrice:
                {
                    return "Thrice";
                }
                case Rarely:
                {
                    return "Rarely";
                }
                case Seldom:
                {
                    return "Seldom";
                }
                case Neutral:
                {
                    return "Neutral";
                }
                case Commonly:
                {
                    return "Commonly";
                }
                case Often:
                {
                    return "Often";
                }
                case Always:
                {
                    return "Always";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(FREQ_TYPE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }

    } // namespace strategy
} // namespace combat
} // namespace heroespath
