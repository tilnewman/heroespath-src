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

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace combat
{
    namespace strategy
    {
        void SelectType::ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR)
        {
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Pixie, "Pixie", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Dragon, "Dragon", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Human, "Human", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Gnome, "Gnome", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Wolfen, "Wolfen", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Archer, "Archer", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Sorcerer, "Sorcerer", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Knight, "Knight", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Beastmaster, "Beastmaster", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Cleric, "Cleric", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Theif, "Theif", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Bard, "Bard", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::FireBrand, "FireBrand", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Sylavin, "Sylavin", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Projectile, "Projectile", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Caster, "Caster", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::CanFly, "CanFly", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::CantFly, "CantFly", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::Beast, "Beast", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, SelectType::NotBeast, "NotBeast", SEPARATOR);
        }

        void RefineType::ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR)
        {
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Murderer, "Murderer", SEPARATOR);

            AppendNameIfBitIsSet(
                ss, ENUM_VALUE, RefineType::Bloodthirsty, "Bloodthirsty", SEPARATOR);

            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Coward, "Coward", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Hit, "Hit", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Attack, "Attack", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::MusicMaker, "MusicMaker", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Caster, "Caster", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Enchanted, "Enchanted", SEPARATOR);

            AppendNameIfBitIsSet(
                ss, ENUM_VALUE, RefineType::NotEnchanted, "NotEnchanted", SEPARATOR);

            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::Steadfast, "Steadfast", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::LastTo, "LastTo", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, RefineType::MostDamage, "MostDamage", SEPARATOR);
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
                default:
                {
                    ThrowInvalidValueForFunction(ADVANCE_TYPE, "ToString");
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
                default:
                {
                    ThrowInvalidValueForFunction(RETREAT_TYPE, "ToString");
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
                default:
                {
                    ThrowInvalidValueForFunction(FREQ_TYPE, "ToString");
                }
            }
        }

    } // namespace strategy
} // namespace combat
} // namespace heroespath
