// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
