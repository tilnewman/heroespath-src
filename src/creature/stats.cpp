// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stats.cpp
//
#include "stats.hpp"

#include "creature/creature.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <numeric>

namespace heroespath
{
namespace creature
{

    float Stats::Ratio(
        const CreaturePtr_t CREATURE_PTR,
        const Traits::Enum TRAIT_ENUM,
        const Trait_t TRAIT_BONUS,
        const With OPTIONS)
    {
        return Ratio(CREATURE_PTR, TraitsVec_t(1, TRAIT_ENUM), TRAIT_BONUS, OPTIONS);
    }

    float Stats::Ratio(
        const CreaturePtr_t CREATURE_PTR,
        const TraitsVec_t & TRAIT_ENUM_VEC,
        const Trait_t TRAIT_BONUS,
        const With OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (TRAIT_ENUM_VEC.empty() == false),
            "creature::Stats::Ratio() called with TRAIT_ENUM_VEC empty.");

        Trait_t randSum { 0 };
        Trait_t normalSum { 0 };
        for (const auto NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            normalSum += CREATURE_PTR->TraitNormal(NEXT_TRAIT_ENUM);

            const auto NEXT_TRAIT_WORKING { CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM) };
            randSum += NEXT_TRAIT_WORKING;

            if (OPTIONS & With::Luck)
            {
                randSum += LuckBonus(CREATURE_PTR);
            }

            if (OPTIONS & With::RaceRoleBonus)
            {
                randSum
                    += RollBonusByRace(NEXT_TRAIT_WORKING, NEXT_TRAIT_ENUM, CREATURE_PTR->Race());

                randSum
                    += RollBonusByRole(NEXT_TRAIT_WORKING, NEXT_TRAIT_ENUM, CREATURE_PTR->Role());
            }
        }

        if (randSum > normalSum)
        {
            randSum = normalSum;
        }

        const auto TRAIT_BONUS_RATIO { static_cast<float>(TRAIT_BONUS) / 100.0f };

        return (static_cast<float>(randSum) / static_cast<float>(normalSum)) + TRAIT_BONUS_RATIO;
    }

    Trait_t Stats::Roll(
        const CreaturePtr_t CREATURE_PTR, const Traits::Enum TRAIT_ENUM, const With OPTIONS)
    {
        return Roll(CREATURE_PTR, TraitsVec_t(1, TRAIT_ENUM), OPTIONS);
    }

    Trait_t Stats::Roll(
        const CreaturePtr_t CREATURE_PTR, const TraitsVec_t & TRAIT_ENUM_VEC, const With OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (TRAIT_ENUM_VEC.empty() == false),
            "creature::Stats::Roll() called with TRAIT_ENUM_VEC empty.");

        const auto RAND_SUM { std::accumulate(
            std::begin(TRAIT_ENUM_VEC),
            std::end(TRAIT_ENUM_VEC),
            0.0f,
            [&](const auto SUBTOTAL, const auto TRAIT) {
                const auto CURRENT_VAL { static_cast<float>(CREATURE_PTR->TraitWorking(TRAIT)) };
                const auto RATIO { Ratio(CREATURE_PTR, TRAIT_ENUM_VEC, OPTIONS) };
                return SUBTOTAL + (CURRENT_VAL * RATIO);
            }) };

        return static_cast<Trait_t>(RAND_SUM / static_cast<float>(TRAIT_ENUM_VEC.size()));
    }

    bool Stats::Test(
        const CreaturePtr_t CREATURE_PTR, const Traits::Enum TRAIT_ENUM, const With OPTIONS)
    {
        return Test(CREATURE_PTR, TraitsVec_t(1, TRAIT_ENUM), OPTIONS);
    }

    bool Stats::Test(
        const CreaturePtr_t CREATURE_PTR, const TraitsVec_t & TRAIT_ENUM_VEC, const With OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (TRAIT_ENUM_VEC.empty() == false),
            "creature::Stats::Test() called with TRAIT_ENUM_VEC empty.");

        Trait_t rollChance { 0 };

        for (const auto NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            const auto TRAIT_WORKING { CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM) };
            rollChance += TRAIT_WORKING;

            if (OPTIONS & With::RaceRoleBonus)
            {
                rollChance += RollBonusByRace(TRAIT_WORKING, NEXT_TRAIT_ENUM, CREATURE_PTR->Race());

                rollChance += RollBonusByRole(TRAIT_WORKING, NEXT_TRAIT_ENUM, CREATURE_PTR->Role());
            }

            if (OPTIONS & With::StandardBonus)
            {
                const auto STD_BONUS { static_cast<Trait_t>(
                    static_cast<float>(Trait::STAT_MAX_ESTIMATED_)
                    * (static_cast<float>(CREATURE_PTR->TraitBonusCurrent(NEXT_TRAIT_ENUM))
                       / 100.0f)) };

                rollChance += STD_BONUS;
            }
        }

        if (rollChance < 0)
        {
            rollChance = 0;
        }

        rollChance /= static_cast<int>(TRAIT_ENUM_VEC.size());

        if (OPTIONS & With::Luck)
        {
            rollChance += LuckBonus(CREATURE_PTR);
        }

        if (OPTIONS & With::RankBonus)
        {
            const auto RANK_BONUS { static_cast<Trait_t>(
                CREATURE_PTR->Rank().GetAs<float>()
                * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "fight-stats-rank-bonus-ratio")) };

            rollChance += RANK_BONUS;
        }

        return (misc::Random(Trait::STAT_MAX_ESTIMATED_) < rollChance);
    }

    bool Stats::Versus(
        const CreaturePtr_t CHALLENGER_PTR,
        const Traits::Enum CHALLENGER_TRAIT,
        const CreaturePtr_t DEFENDER_PTR,
        const Traits::Enum DEFENDER_TRAIT_PARAM,
        const Trait_t CHALLENGER_BONUS_PER,
        const Trait_t DEFENDER_BONUS_PER,
        const With OPTIONS)
    {
        const auto DEFENDER_TRAIT { (
            (DEFENDER_TRAIT_PARAM == Traits::Count) ? CHALLENGER_TRAIT : DEFENDER_TRAIT_PARAM) };

        return Versus(
            CHALLENGER_PTR,
            TraitsVec_t(1, CHALLENGER_TRAIT),
            DEFENDER_PTR,
            TraitsVec_t(1, DEFENDER_TRAIT),
            CHALLENGER_BONUS_PER,
            DEFENDER_BONUS_PER,
            OPTIONS);
    }

    bool Stats::Versus(
        const CreaturePtr_t CHALLENGER_PTR,
        const TraitsVec_t & CHALLENGER_TRAIT_VEC,
        const CreaturePtr_t DEFENDER_PTR,
        const TraitsVec_t & DEFENDER_TRAIT_VEC_PARAM,
        const Trait_t CHALLENGER_BONUS_PER,
        const Trait_t DEFENDER_BONUS_PER,
        const With OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CHALLENGER_TRAIT_VEC.empty() == false),
            "creature::Stats::Versus() called with CHALLENGER_TRAIT_VEC empty.");

        Trait_t chaRandSum { 0 };
        Trait_t chaNormalSum { 0 };

        const auto CHALLENGER_ROLL_OPTIONS { [CHALLENGER_PTR, OPTIONS]() {
            With rollOptions { With::None };

            if (OPTIONS & With::RaceRoleBonus)
            {
                rollOptions = static_cast<With>(rollOptions | With::RaceRoleBonus);
            }

            if (CHALLENGER_PTR->IsPlayerCharacter() && (OPTIONS & With::Luck))
            {
                rollOptions = static_cast<With>(rollOptions | With::Luck);
            }

            return rollOptions;
        }() };

        for (const auto NEXT_TRAIT_ENUM : CHALLENGER_TRAIT_VEC)
        {
            chaRandSum += Roll(CHALLENGER_PTR, NEXT_TRAIT_ENUM, CHALLENGER_ROLL_OPTIONS);

            chaRandSum += CHALLENGER_BONUS_PER;
            chaNormalSum += CHALLENGER_PTR->TraitNormal(NEXT_TRAIT_ENUM);
        }

        // check for player natural success (if the challenger is a player)
        if ((OPTIONS & With::PlayerNaturalWins) && CHALLENGER_PTR->IsPlayerCharacter()
            && (chaRandSum >= chaNormalSum))
        {
            return true;
        }

        const auto CHALLENGER_ROLL {
            chaRandSum + ((OPTIONS & With::RankBonus) ? CHALLENGER_PTR->Rank().GetAsTrait() : 0)
        };

        const auto DEFENDER_TRAIT_VEC { (
            (DEFENDER_TRAIT_VEC_PARAM.empty()) ? CHALLENGER_TRAIT_VEC : DEFENDER_TRAIT_VEC_PARAM) };

        Trait_t defRandSum { 0 };
        Trait_t defNormalSum { 0 };

        const auto DEFENDER_ROLL_OPTIONS { [DEFENDER_PTR, OPTIONS]() {
            With rollOptions { With::None };

            if (OPTIONS & With::RaceRoleBonus)
            {
                rollOptions = static_cast<With>(rollOptions | With::RaceRoleBonus);
            }

            if (DEFENDER_PTR->IsPlayerCharacter() && (OPTIONS & With::Luck))
            {
                rollOptions = static_cast<With>(rollOptions | With::Luck);
            }

            return rollOptions;
        }() };

        for (const auto NEXT_TRAIT_ENUM : DEFENDER_TRAIT_VEC)
        {
            defRandSum += Roll(DEFENDER_PTR, NEXT_TRAIT_ENUM, DEFENDER_ROLL_OPTIONS);

            defRandSum += DEFENDER_BONUS_PER;
            defNormalSum += DEFENDER_PTR->TraitNormal(NEXT_TRAIT_ENUM);
        }

        // check for player natural success (if the defender is a player)
        if ((OPTIONS & With::PlayerNaturalWins) && DEFENDER_PTR->IsPlayerCharacter()
            && (defRandSum >= defNormalSum))
        {
            return true;
        }

        const auto DEFENDER_ROLL {
            defRandSum + ((OPTIONS & With::RankBonus) ? DEFENDER_PTR->Rank().GetAsTrait() : 0)
        };

        // handle roll tie
        if (CHALLENGER_ROLL == DEFENDER_ROLL)
        {
            const auto CHA_NORMAL_PLUS_RANK { chaNormalSum + CHALLENGER_PTR->Rank().GetAsTrait() };
            const auto DEF_NORMAL_PLUS_RANK { defNormalSum + DEFENDER_PTR->Rank().GetAsTrait() };

            // handle normal+rank tie
            if (CHA_NORMAL_PLUS_RANK == DEF_NORMAL_PLUS_RANK)
            {
                // handle normal tie
                if (chaNormalSum == defNormalSum)
                {
                    // handle everything tied case
                    if (CHALLENGER_PTR->IsPlayerCharacter() && DEFENDER_PTR->IsPlayerCharacter())
                    {
                        // handle everything tied and both are players by fair coin toss
                        return misc::RandomBool();
                    }
                    else
                    {
                        // normal tie goes to the player
                        return CHALLENGER_PTR->IsPlayerCharacter();
                    }
                }
                else
                {
                    // normal+rank tie goes to whoever has the greatest normal stats
                    return (chaNormalSum > defNormalSum);
                }
            }
            else
            {
                // roll tie goes to whoever has the greatest normal+rank
                return (CHA_NORMAL_PLUS_RANK > DEF_NORMAL_PLUS_RANK);
            }
        }
        else
        {
            // handle normal win condition
            return (CHALLENGER_ROLL > DEFENDER_ROLL);
        }
    }

    Trait_t Stats::LuckBonus(const CreaturePtr_t CREATURE_PTR)
    {
        return misc::Random(static_cast<int>(
            static_cast<float>(CREATURE_PTR->TraitWorking(Traits::Luck))
            / misc::ConfigFile::Instance()->ValueOrDefault<float>("fight-stats-luck-adj-ratio")));
    }

    Trait_t Stats::RollBonusByRace(
        const Trait_t TRAIT_VALUE, const Traits::Enum TRAIT_ENUM, const race::Enum RACE_ENUM)
    {
        const auto BASE { static_cast<Trait_t>(
            static_cast<float>(TRAIT_VALUE)
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "stats-race-bonus-base-adj-ratio")) };

        const auto MINOR { static_cast<Trait_t>(
            static_cast<float>(BASE)
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "stats-race-bonus-minor-adj-ratio")) };

        if (TRAIT_ENUM == Traits::Strength)
        {
            if (RACE_ENUM == creature::race::Boar)
                return MINOR;
            else if (RACE_ENUM == creature::race::Demon)
                return BASE + (BASE / 2);
            else if (RACE_ENUM == creature::race::Dragon)
                return BASE;
            else if (RACE_ENUM == creature::race::Giant)
                return BASE;
            else if (RACE_ENUM == creature::race::Hydra)
                return BASE;
            else if (RACE_ENUM == creature::race::Lion)
                return BASE - MINOR;
            else if (RACE_ENUM == creature::race::LionBoar)
                return BASE + MINOR;
            else if (RACE_ENUM == creature::race::Minotaur)
                return BASE / 2;
            else if (RACE_ENUM == creature::race::Ogre)
                return BASE - MINOR;
            else if (RACE_ENUM == creature::race::Ramonaut)
                return BASE;
            else if (RACE_ENUM == creature::race::Troll)
                return MINOR;
            else if (RACE_ENUM == creature::race::Werebear)
                return MINOR * 2;
            else if (RACE_ENUM == creature::race::Wereboar)
                return MINOR;
            else if (RACE_ENUM == creature::race::Wyvern)
                return MINOR;
            else
                return 0;
        }
        else if (TRAIT_ENUM == Traits::Accuracy)
        {
            return 0;
        }
        else if (TRAIT_ENUM == Traits::Charm)
        {
            if (RACE_ENUM == creature::race::Pixie)
                return BASE;
            else
                return 0;
        }
        else if (TRAIT_ENUM == Traits::Luck)
        {
            if (RACE_ENUM == creature::race::Gnome)
                return BASE;
            else if (RACE_ENUM == creature::race::Pixie)
                return MINOR;
            else if (RACE_ENUM == creature::race::Demon)
                return MINOR;
            else
                return 0;
        }
        else if (TRAIT_ENUM == Traits::Speed)
        {
            const Trait_t FLY_BONUS { ((creature::race::CanFly(RACE_ENUM)) ? BASE : 0) };

            if (RACE_ENUM == creature::race::Cobra)
                return BASE + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Demon)
                return MINOR + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Dragon)
                return MINOR + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Gnome)
                return BASE + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Pixie)
                return BASE + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Werecat)
                return MINOR + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Wolfen)
                return MINOR + FLY_BONUS;
            else
                return FLY_BONUS;
        }
        else if (TRAIT_ENUM == Traits::Intelligence)
        {
            if (RACE_ENUM == creature::race::Demon)
                return BASE;
            else if (RACE_ENUM == creature::race::Dragon)
                return MINOR;
            else if (RACE_ENUM == creature::race::Pixie)
                return BASE;
            else if (RACE_ENUM == creature::race::Witch)
                return BASE;
            else
                return 0;
        }
        else
        {
            M_HP_LOG_ERR(
                "creature::Stats::RollBonusByRace(stat_enum="
                << NAMEOF_ENUM(TRAIT_ENUM) << ", race_enum=" << RACE_ENUM
                << ") but that trait value is invalid.");

            return 0;
        }
    }

    Trait_t Stats::RollBonusByRole(
        const Trait_t TRAIT_VALUE, const Traits::Enum TRAIT_ENUM, const role::Enum ROLE_ENUM)
    {
        const auto BASE { static_cast<Trait_t>(
            static_cast<float>(TRAIT_VALUE)
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "stats-role-bonus-base-adj-ratio")) };

        const auto MINOR { static_cast<Trait_t>(
            static_cast<float>(BASE)
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                  "stats-role-bonus-minor-adj-ratio")) };

        if (TRAIT_ENUM == Traits::Strength)
        {
            if (ROLE_ENUM == creature::role::Archer)
                return MINOR;
            else if (ROLE_ENUM == creature::role::Beastmaster)
                return MINOR;
            else if (ROLE_ENUM == creature::role::Knight)
                return BASE;
            else if (ROLE_ENUM == creature::role::Sylavin)
                return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == Traits::Accuracy)
        {
            if (ROLE_ENUM == creature::role::Archer)
                return BASE;
            else if (ROLE_ENUM == creature::role::Knight)
                return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == Traits::Charm)
        {
            if (ROLE_ENUM == creature::role::Cleric)
                return BASE;
            return 0;
        }
        else if (TRAIT_ENUM == Traits::Luck)
        {
            if (ROLE_ENUM == creature::role::Thief)
                return BASE;
            return 0;
        }
        else if (TRAIT_ENUM == Traits::Speed)
        {
            if (ROLE_ENUM == creature::role::Thief)
                return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == Traits::Intelligence)
        {
            if (ROLE_ENUM == creature::role::Sorcerer)
                return BASE;
            else if (ROLE_ENUM == creature::role::Cleric)
                return BASE;
            else if (ROLE_ENUM == creature::role::Bard)
                return MINOR;
            else if (ROLE_ENUM == creature::role::Firebrand)
                return MINOR;
            else if (ROLE_ENUM == creature::role::Sylavin)
                return MINOR;

            return 0;
        }
        else
        {
            M_HP_LOG_ERR(
                "creature::Stats::RollBonusByRole(stat_enum="
                << NAMEOF_ENUM(TRAIT_ENUM) << ", role_enum=" << ROLE_ENUM
                << ") but that trait value is invalid.");

            return 0;
        }
    }

    int Stats::RandomRatio(
        const CreaturePtr_t CREATURE_PTR,
        const Traits::Enum TRAIT_ENUM,
        const int RAND_SPREAD,
        const float RANK_BONUS_MULT,
        const Trait_t TRAIT_BONUS,
        const With OPTIONS)
    {
        const auto RAND_RATIO { Ratio(CREATURE_PTR, TRAIT_ENUM, TRAIT_BONUS, OPTIONS) };

        const auto TRAIT_BONUS_RATIO { static_cast<float>(TRAIT_BONUS) / 100.0f };

        auto x { static_cast<int>(
            static_cast<float>(RAND_SPREAD) * (RAND_RATIO + TRAIT_BONUS_RATIO)) };

        x += static_cast<int>(CREATURE_PTR->Rank().GetAs<float>() * RANK_BONUS_MULT);

        return x;
    }

} // namespace creature
} // namespace heroespath
