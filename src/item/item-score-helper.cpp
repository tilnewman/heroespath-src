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
// item-score-helper.cpp
//
#include "item-score-helper.hpp"

namespace heroespath
{
namespace item
{

    Score_t ScoreHelper::Score(const stats::TraitSet & TRAIT_SET)
    {
        using namespace stats;

        auto score{ 0_score };

        for (int i(1); i < Traits::Count; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_TRAIT_VALUE{ TRAIT_SET.GetCopy(NEXT_TRAIT_ENUM).Current() };

            auto traitScore{ [NEXT_TRAIT_VALUE]() {
                if (NEXT_TRAIT_VALUE >= 0)
                {
                    return 10 * NEXT_TRAIT_VALUE;
                }
                else
                {
                    return 5 * std::abs(NEXT_TRAIT_VALUE);
                }
            }() };

            if ((NEXT_TRAIT_ENUM == Traits::HealthGainAll)
                || (NEXT_TRAIT_ENUM == Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if (
                (NEXT_TRAIT_ENUM == Traits::AnimalResist)
                || (NEXT_TRAIT_ENUM == Traits::ArmorRating) || (NEXT_TRAIT_ENUM == Traits::Backstab)
                || (NEXT_TRAIT_ENUM == Traits::CurseOnDamage)
                || (NEXT_TRAIT_ENUM == Traits::DamageBonusAll)
                || (NEXT_TRAIT_ENUM == Traits::DamageBonusMelee)
                || (NEXT_TRAIT_ENUM == Traits::DamageBonusProj)
                || (NEXT_TRAIT_ENUM == Traits::FindCoinsAmount)
                || (NEXT_TRAIT_ENUM == Traits::PoisonOnAll)
                || (NEXT_TRAIT_ENUM == Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += Score_t(traitScore);
        }

        return score;
    }

    Score_t ScoreHelper::Score(const material::Enum PRI, const material::Enum SEC)
    {
        auto const BONUS_PRI{ material::Bonus(PRI) };
        auto const BONUS_SEC{ material::Bonus(SEC) };
        return Score_t(50 + (BONUS_PRI * BONUS_PRI) + ((BONUS_SEC * BONUS_SEC) / 3));
    }

    Score_t ScoreHelper::Score(const weapon::axe_type::Enum AXE_TYPE)
    {
        using namespace weapon;

        switch (AXE_TYPE)
        {
            case axe_type::Handaxe:
            {
                return 50_score;
            }
            case axe_type::Sickle:
            {
                return 80_score;
            }
            case axe_type::Battleaxe:
            {
                return 100_score;
            }
            case axe_type::Waraxe:
            {
                return 200_score;
            }
            case axe_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::bladedstaff_type::Enum BSTAFF_TYPE)
    {
        using namespace weapon;

        switch (BSTAFF_TYPE)
        {
            case bladedstaff_type::Spear:
            {
                return 70_score;
            }
            case bladedstaff_type::ShortSpear:
            {
                return 50_score;
            }
            case bladedstaff_type::Scythe:
            {
                return 80_score;
            }
            case bladedstaff_type::Pike:
            {
                return 100_score;
            }
            case bladedstaff_type::Partisan:
            {
                return 150_score;
            }
            case bladedstaff_type::Halberd:
            {
                return 200_score;
            }
            case bladedstaff_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::club_type::Enum CLUB_TYPE)
    {
        using namespace weapon;

        switch (CLUB_TYPE)
        {
            case club_type::Spiked:
            {
                return 50_score;
            }
            case club_type::Maul:
            {
                return 60_score;
            }
            case club_type::Mace:
            {
                return 80_score;
            }
            case club_type::Warhammer:
            {
                return 120_score;
            }
            case club_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::projectile_type::Enum PROJ_TYPE)
    {
        using namespace weapon;

        switch (PROJ_TYPE)
        {
            case projectile_type::Blowpipe:
            {
                return 50_score;
            }
            case projectile_type::Sling:
            {
                return 60_score;
            }
            case projectile_type::Shortbow:
            {
                return 100_score;
            }
            case projectile_type::Longbow:
            {
                return 200_score;
            }
            case projectile_type::CompositeBow:
            {
                return 300_score;
            }
            case projectile_type::Crossbow:
            {
                return 300_score;
            }
            case projectile_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::sword_type::Enum SWORD_TYPE)
    {
        using namespace weapon;

        switch (SWORD_TYPE)
        {
            case sword_type::Claymore:
            {
                return 200_score;
            }
            case sword_type::Longsword:
            {
                return 120_score;
            }
            case sword_type::Flamberg:
            {
                return 190_score;
            }
            case sword_type::Knightlysword:
            {
                return 110_score;
            }
            case sword_type::Broadsword:
            {
                return 100_score;
            }
            case sword_type::Falcata:
            {
                return 50_score;
            }
            case sword_type::Saber:
            {
                return 70_score;
            }
            case sword_type::Cutlass:
            {
                return 40_score;
            }
            case sword_type::Rapier:
            {
                return 70_score;
            }
            case sword_type::Gladius:
            {
                return 55_score;
            }
            case sword_type::Shortsword:
            {
                return 50_score;
            }
            case sword_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::whip_type::Enum WHIP_TYPE)
    {
        using namespace weapon;

        switch (WHIP_TYPE)
        {
            case whip_type::Bullwhip:
            {
                return 30_score;
            }
            case whip_type::Flail:
            {
                return 60_score;
            }
            case whip_type::MaceAndChain:
            {
                return 90_score;
            }
            case whip_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::ScoreKnife(const sfml_util::Size::Enum SIZE)
    {
        if (SIZE == sfml_util::Size::Small)
        {
            return 20_score;
        }
        else
        {
            return 25_score;
        }
    }

    Score_t ScoreHelper::ScoreDagger(const sfml_util::Size::Enum SIZE)
    {
        if (SIZE == sfml_util::Size::Small)
        {
            return 30_score;
        }
        else
        {
            return 35_score;
        }
    }

    Score_t ScoreHelper::ScoreStaff() { return 25_score; }

    Score_t ScoreHelper::ScoreQuarterStaff() { return 15_score; }

    Score_t ScoreHelper::Score(const armor::cover_type::Enum COVER_TYPE)
    {
        using namespace armor;

        switch (COVER_TYPE)
        {
            case cover_type::Cape:
            {
                return 25_score;
            }
            case cover_type::Vest:
            {
                return 50_score;
            }
            case cover_type::Robe:
            {
                return 75_score;
            }
            case cover_type::Cloak:
            {
                return 100_score;
            }
            case cover_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const armor::helm_type::Enum HELM_TYPE)
    {
        using namespace armor;

        switch (HELM_TYPE)
        {
            case helm_type::Leather:
            {
                return 25_score;
            }
            case helm_type::MailCoif:
            {
                return 50_score;
            }
            case helm_type::Kettle:
            {
                return 75_score;
            }
            case helm_type::Archers:
            {
                return 100_score;
            }
            case helm_type::Bascinet:
            {
                return 125_score;
            }
            case helm_type::Great:
            {
                return 150_score;
            }
            case helm_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const armor::shield_type::Enum SHIELD_TYPE)
    {
        using namespace armor;

        switch (SHIELD_TYPE)
        {
            case shield_type::Buckler:
            {
                return 50_score;
            }
            case shield_type::Kite:
            {
                return 100_score;
            }
            case shield_type::Heater:
            {
                return 150_score;
            }
            case shield_type::Pavis:
            {
                return 200_score;
            }
            case shield_type::Count:
            default:
            {
                return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(
        const armor_type::Enum ARMOR_TYPE, const armor::base_type::Enum BASE_TYPE)
    {
        if (ARMOR_TYPE == armor_type::Aventail)
        {
            return 34_score + Score(BASE_TYPE);
        }
        else if (ARMOR_TYPE == armor_type::Bracers)
        {
            return 36_score + Score(BASE_TYPE);
        }
        else if (ARMOR_TYPE == armor_type::Shirt)
        {
            return 44_score + Score(BASE_TYPE);
        }
        else if (ARMOR_TYPE == armor_type::Boots)
        {
            return 46_score + Score(BASE_TYPE);
        }
        else if (ARMOR_TYPE == armor_type::Pants)
        {
            return 55_score + Score(BASE_TYPE);
        }
        else if (ARMOR_TYPE == armor_type::Gauntlets)
        {
            return 30_score + Score(BASE_TYPE);
        }
        else
        {
            return Score(BASE_TYPE);
        }
    }

    Score_t ScoreHelper::Score(const armor::base_type::Enum ARMOR_BASE_TYPE)
    {
        using namespace armor;

        switch (ARMOR_BASE_TYPE)
        {
            case base_type::Mail:
            {
                return 75_score;
            }
            case base_type::Scale:
            {
                return 125_score;
            }
            case base_type::Plate:
            {
                return 175_score;
            }
            case base_type::Plain:
            case base_type::Count:
            default:
            {
                return 25_score;
            }
        }
    }

} // namespace item
} // namespace heroespath
