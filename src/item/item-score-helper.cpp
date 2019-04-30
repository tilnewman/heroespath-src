// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-score-helper.cpp
//
#include "item-score-helper.hpp"

namespace heroespath
{
namespace item
{

    Score_t ScoreHelper::Score(const creature::TraitSet & TRAIT_SET) const
    {
        auto score { 0_score };

        for (EnumUnderlying_t i(1); i < creature::Traits::Count; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<creature::Traits::Enum>(i) };
            const auto NEXT_TRAIT_VALUE { TRAIT_SET.GetCopy(NEXT_TRAIT_ENUM).Current() };

            if (NEXT_TRAIT_VALUE == 0)
            {
                continue;
            }

            auto traitScore { [NEXT_TRAIT_VALUE]() {
                const auto MULTIPLIER { ((NEXT_TRAIT_VALUE >= 0) ? 10 : 5) };
                const auto NEXT_TRAIT_VALUE_ABS { misc::Abs(NEXT_TRAIT_VALUE) };

                const auto MAX_VALUE_BEFORE_REDUCTION { 100 };
                if (NEXT_TRAIT_VALUE_ABS < MAX_VALUE_BEFORE_REDUCTION)
                {
                    return NEXT_TRAIT_VALUE_ABS * MULTIPLIER;
                }
                else
                {
                    double traitReduced { static_cast<double>(MAX_VALUE_BEFORE_REDUCTION) };
                    traitReduced += std::sqrt(
                        static_cast<double>(NEXT_TRAIT_VALUE_ABS - MAX_VALUE_BEFORE_REDUCTION));
                    return static_cast<int>(traitReduced) * MULTIPLIER;
                }
            }() };

            if ((NEXT_TRAIT_ENUM == creature::Traits::HealthGainAll)
                || (NEXT_TRAIT_ENUM == creature::Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if (
                (NEXT_TRAIT_ENUM == creature::Traits::AnimalResist)
                || (NEXT_TRAIT_ENUM == creature::Traits::ArmorRating)
                || (NEXT_TRAIT_ENUM == creature::Traits::Backstab)
                || (NEXT_TRAIT_ENUM == creature::Traits::CurseOnDamage)
                || (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusAll)
                || (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusMelee)
                || (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusProj)
                || (NEXT_TRAIT_ENUM == creature::Traits::FindCoinsAmount)
                || (NEXT_TRAIT_ENUM == creature::Traits::PoisonOnAll)
                || (NEXT_TRAIT_ENUM == creature::Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == creature::Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += Score_t::Make(traitScore);
        }

        return score;
    }

    Score_t ScoreHelper::Score(const material::Enum PRI, const material::Enum SEC) const
    {
        const auto BONUS_PRI { material::EnchantmentBonus(PRI, material::Nothing) };
        const auto BONUS_SEC { material::EnchantmentBonus(SEC, material::Nothing) };
        return Score_t::Make(50 + (BONUS_PRI * BONUS_PRI) + ((BONUS_SEC * BONUS_SEC) / 3));
    }

    Score_t ScoreHelper::Score(const weapon::axe_type::Enum AXE_TYPE) const
    {
        switch (AXE_TYPE)
        {
            case weapon::axe_type::Handaxe: { return 50_score;
            }
            case weapon::axe_type::Sickle: { return 80_score;
            }
            case weapon::axe_type::Battleaxe: { return 100_score;
            }
            case weapon::axe_type::Waraxe: { return 200_score;
            }
            case weapon::axe_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::bladedstaff_type::Enum BSTAFF_TYPE) const
    {
        switch (BSTAFF_TYPE)
        {
            case weapon::bladedstaff_type::Spear: { return 70_score;
            }
            case weapon::bladedstaff_type::ShortSpear: { return 50_score;
            }
            case weapon::bladedstaff_type::Scythe: { return 80_score;
            }
            case weapon::bladedstaff_type::Pike: { return 100_score;
            }
            case weapon::bladedstaff_type::Partisan: { return 150_score;
            }
            case weapon::bladedstaff_type::Halberd: { return 200_score;
            }
            case weapon::bladedstaff_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::club_type::Enum CLUB_TYPE) const
    {
        switch (CLUB_TYPE)
        {
            case weapon::club_type::Spiked: { return 50_score;
            }
            case weapon::club_type::Maul: { return 60_score;
            }
            case weapon::club_type::Mace: { return 80_score;
            }
            case weapon::club_type::Warhammer: { return 120_score;
            }
            case weapon::club_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::projectile_type::Enum PROJ_TYPE) const
    {
        switch (PROJ_TYPE)
        {
            case weapon::projectile_type::Blowpipe: { return 50_score;
            }
            case weapon::projectile_type::Sling: { return 60_score;
            }
            case weapon::projectile_type::Shortbow: { return 100_score;
            }
            case weapon::projectile_type::Longbow: { return 200_score;
            }
            case weapon::projectile_type::CompositeBow: { return 300_score;
            }
            case weapon::projectile_type::Crossbow: { return 300_score;
            }
            case weapon::projectile_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::sword_type::Enum SWORD_TYPE) const
    {
        switch (SWORD_TYPE)
        {
            case weapon::sword_type::Claymore: { return 200_score;
            }
            case weapon::sword_type::Longsword: { return 120_score;
            }
            case weapon::sword_type::Flamberg: { return 190_score;
            }
            case weapon::sword_type::Knightlysword: { return 110_score;
            }
            case weapon::sword_type::Broadsword: { return 100_score;
            }
            case weapon::sword_type::Falcata: { return 50_score;
            }
            case weapon::sword_type::Saber: { return 70_score;
            }
            case weapon::sword_type::Cutlass: { return 40_score;
            }
            case weapon::sword_type::Rapier: { return 70_score;
            }
            case weapon::sword_type::Gladius: { return 55_score;
            }
            case weapon::sword_type::Shortsword: { return 50_score;
            }
            case weapon::sword_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const weapon::whip_type::Enum WHIP_TYPE) const
    {
        switch (WHIP_TYPE)
        {
            case weapon::whip_type::Bullwhip: { return 30_score;
            }
            case weapon::whip_type::Flail: { return 60_score;
            }
            case weapon::whip_type::MaceAndChain: { return 90_score;
            }
            case weapon::whip_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::ScoreKnife() const { return 22_score; }

    Score_t ScoreHelper::ScoreDagger() const { return 33_score; }

    Score_t ScoreHelper::ScoreStaff() const { return 25_score; }

    Score_t ScoreHelper::ScoreQuarterStaff() const { return 15_score; }

    Score_t ScoreHelper::Score(const armor::cover_type::Enum COVER_TYPE) const
    {
        switch (COVER_TYPE)
        {
            case armor::cover_type::Cape: { return 25_score;
            }
            case armor::cover_type::Vest: { return 50_score;
            }
            case armor::cover_type::Robe: { return 75_score;
            }
            case armor::cover_type::Cloak: { return 100_score;
            }
            case armor::cover_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const armor::helm_type::Enum HELM_TYPE) const
    {
        switch (HELM_TYPE)
        {
            case armor::helm_type::Leather: { return 25_score;
            }
            case armor::helm_type::MailCoif: { return 50_score;
            }
            case armor::helm_type::Kettle: { return 75_score;
            }
            case armor::helm_type::Archers: { return 100_score;
            }
            case armor::helm_type::Bascinet: { return 125_score;
            }
            case armor::helm_type::Great: { return 150_score;
            }
            case armor::helm_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(const armor::shield_type::Enum SHIELD_TYPE) const
    {
        switch (SHIELD_TYPE)
        {
            case armor::shield_type::Buckler: { return 50_score;
            }
            case armor::shield_type::Kite: { return 100_score;
            }
            case armor::shield_type::Heater: { return 150_score;
            }
            case armor::shield_type::Pavis: { return 200_score;
            }
            case armor::shield_type::Count:
            default: { return 0_score;
            }
        }
    }

    Score_t ScoreHelper::Score(
        const armor_type::Enum ARMOR_TYPE, const armor::base_type::Enum BASE_TYPE) const
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

    Score_t ScoreHelper::Score(const armor::base_type::Enum ARMOR_BASE_TYPE) const
    {
        switch (ARMOR_BASE_TYPE)
        {
            case armor::base_type::Mail: { return 75_score;
            }
            case armor::base_type::Scale: { return 125_score;
            }
            case armor::base_type::Plate: { return 175_score;
            }
            case armor::base_type::Plain:
            case armor::base_type::Count:
            default: { return 25_score;
            }
        }
    }

} // namespace item
} // namespace heroespath
