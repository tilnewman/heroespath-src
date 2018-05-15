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
#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <numeric>

namespace heroespath
{
namespace creature
{

    float Stats::Ratio(
        const CreaturePtr_t CREATURE_PTR,
        const stats::Traits::Enum TRAIT_ENUM,
        const stats::Trait_t TRAIT_BONUS,
        const With OPTIONS)
    {
        return Ratio(CREATURE_PTR, stats::TraitsVec_t(1, TRAIT_ENUM), TRAIT_BONUS, OPTIONS);
    }

    float Stats::Ratio(
        const CreaturePtr_t CREATURE_PTR,
        const stats::TraitsVec_t & TRAIT_ENUM_VEC,
        const stats::Trait_t TRAIT_BONUS,
        const With OPTIONS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (TRAIT_ENUM_VEC.empty() == false),
            "creature::Stats::Ratio() called with TRAIT_ENUM_VEC empty.");

        stats::Trait_t randSum{ 0 };
        stats::Trait_t normalSum{ 0 };
        for (auto const NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            normalSum += CREATURE_PTR->TraitNormal(NEXT_TRAIT_ENUM);

            auto const NEXT_TRAIT_WORKING{ CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM) };
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

        auto const TRAIT_BONUS_RATIO{ static_cast<float>(TRAIT_BONUS) / 100.0f };

        return (static_cast<float>(randSum) / static_cast<float>(normalSum)) + TRAIT_BONUS_RATIO;
    }

    stats::Trait_t Stats::Roll(
        const CreaturePtr_t CREATURE_PTR, const stats::Traits::Enum TRAIT_ENUM, const With OPTIONS)
    {
        return Roll(CREATURE_PTR, stats::TraitsVec_t(1, TRAIT_ENUM), OPTIONS);
    }

    stats::Trait_t Stats::Roll(
        const CreaturePtr_t CREATURE_PTR,
        const stats::TraitsVec_t & TRAIT_ENUM_VEC,
        const With OPTIONS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (TRAIT_ENUM_VEC.empty() == false),
            "creature::Stats::Roll() called with TRAIT_ENUM_VEC empty.");

        auto const RAND_SUM{ std::accumulate(
            std::begin(TRAIT_ENUM_VEC),
            std::end(TRAIT_ENUM_VEC),
            0.0f,
            [&](auto const SUBTOTAL, auto const TRAIT) {
                auto const CURRENT_VAL{ static_cast<float>(CREATURE_PTR->TraitWorking(TRAIT)) };
                auto const RATIO{ Ratio(CREATURE_PTR, TRAIT_ENUM_VEC, OPTIONS) };
                return SUBTOTAL + (CURRENT_VAL * RATIO);
            }) };

        return static_cast<stats::Trait_t>(RAND_SUM / static_cast<float>(TRAIT_ENUM_VEC.size()));
    }

    bool Stats::Test(
        const CreaturePtr_t CREATURE_PTR, const stats::Traits::Enum TRAIT_ENUM, const With OPTIONS)
    {
        return Test(CREATURE_PTR, stats::TraitsVec_t(1, TRAIT_ENUM), OPTIONS);
    }

    bool Stats::Test(
        const CreaturePtr_t CREATURE_PTR,
        const stats::TraitsVec_t & TRAIT_ENUM_VEC,
        const With OPTIONS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (TRAIT_ENUM_VEC.empty() == false),
            "creature::Stats::Test() called with TRAIT_ENUM_VEC empty.");

        stats::Trait_t rollChance{ 0 };

        for (auto const NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            auto const TRAIT_WORKING{ CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM) };
            rollChance += TRAIT_WORKING;

            if (OPTIONS & With::RaceRoleBonus)
            {
                rollChance += RollBonusByRace(TRAIT_WORKING, NEXT_TRAIT_ENUM, CREATURE_PTR->Race());

                rollChance += RollBonusByRole(TRAIT_WORKING, NEXT_TRAIT_ENUM, CREATURE_PTR->Role());
            }

            if (OPTIONS & With::StandardBonus)
            {
                auto const STD_BONUS{ static_cast<stats::Trait_t>(
                    static_cast<float>(stats::Trait::STAT_MAX_ESTIMATED_)
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
            auto const RANK_BONUS{ static_cast<int>(
                CREATURE_PTR->Rank().As<float>()
                * game::GameDataFile::Instance()->GetCopyFloat(
                      "heroespath-fight-stats-rank-bonus-ratio")) };

            rollChance += RANK_BONUS;
        }

        return (misc::random::Int(stats::Trait::STAT_MAX_ESTIMATED_) < rollChance);
    }

    bool Stats::Versus(
        const CreaturePtr_t CHALLENGER_PTR,
        const stats::Traits::Enum CHALLENGER_TRAIT,
        const CreaturePtr_t DEFENDER_PTR,
        const stats::Traits::Enum DEFENDER_TRAIT_PARAM,
        const stats::Trait_t CHALLENGER_BONUS_PER,
        const stats::Trait_t DEFENDER_BONUS_PER,
        const With OPTIONS)
    {
        auto const DEFENDER_TRAIT{ (
            (DEFENDER_TRAIT_PARAM == stats::Traits::Count) ? CHALLENGER_TRAIT
                                                           : DEFENDER_TRAIT_PARAM) };

        return Versus(
            CHALLENGER_PTR,
            stats::TraitsVec_t(1, CHALLENGER_TRAIT),
            DEFENDER_PTR,
            stats::TraitsVec_t(1, DEFENDER_TRAIT),
            CHALLENGER_BONUS_PER,
            DEFENDER_BONUS_PER,
            OPTIONS);
    }

    bool Stats::Versus(
        const CreaturePtr_t CHALLENGER_PTR,
        const stats::TraitsVec_t & CHALLENGER_TRAIT_VEC,
        const CreaturePtr_t DEFENDER_PTR,
        const stats::TraitsVec_t & DEFENDER_TRAIT_VEC_PARAM,
        const stats::Trait_t CHALLENGER_BONUS_PER,
        const stats::Trait_t DEFENDER_BONUS_PER,
        const With OPTIONS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (CHALLENGER_TRAIT_VEC.empty() == false),
            "creature::Stats::Versus() called with CHALLENGER_TRAIT_VEC empty.");

        stats::Trait_t chaRandSum{ 0 };
        stats::Trait_t chaNormalSum{ 0 };

        auto const CHALLENGER_ROLL_OPTIONS{ [CHALLENGER_PTR, OPTIONS]() {
            With rollOptions{ With::None };

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

        for (auto const NEXT_TRAIT_ENUM : CHALLENGER_TRAIT_VEC)
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

        auto const CHALLENGER_ROLL{
            chaRandSum + ((OPTIONS & With::RankBonus) ? CHALLENGER_PTR->Rank().As<int>() : 0)
        };

        auto const DEFENDER_TRAIT_VEC{ (
            (DEFENDER_TRAIT_VEC_PARAM.empty()) ? CHALLENGER_TRAIT_VEC : DEFENDER_TRAIT_VEC_PARAM) };

        stats::Trait_t defRandSum{ 0 };
        stats::Trait_t defNormalSum{ 0 };

        auto const DEFENDER_ROLL_OPTIONS{ [DEFENDER_PTR, OPTIONS]() {
            With rollOptions{ With::None };

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

        for (auto const NEXT_TRAIT_ENUM : DEFENDER_TRAIT_VEC)
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

        auto const DEFENDER_ROLL{
            defRandSum + ((OPTIONS & With::RankBonus) ? DEFENDER_PTR->Rank().As<int>() : 0)
        };

        // handle roll tie
        if (CHALLENGER_ROLL == DEFENDER_ROLL)
        {
            auto const CHA_NORMAL_PLUS_RANK{ chaNormalSum + CHALLENGER_PTR->Rank().As<int>() };
            auto const DEF_NORMAL_PLUS_RANK{ defNormalSum + DEFENDER_PTR->Rank().As<int>() };

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
                        return misc::random::Bool();
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

    stats::Trait_t Stats::LuckBonus(const CreaturePtr_t CREATURE_PTR)
    {
        return misc::random::Int(static_cast<int>(
            static_cast<float>(CREATURE_PTR->TraitWorking(stats::Traits::Luck))
            / game::GameDataFile::Instance()->GetCopyFloat(
                  "heroespath-fight-stats-luck-adj-ratio")));
    }

    stats::Trait_t Stats::RollBonusByRace(
        const stats::Trait_t TRAIT_VALUE,
        const stats::Traits::Enum TRAIT_ENUM,
        const race::Enum RACE_ENUM)
    {
        auto const BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(TRAIT_VALUE)
            * game::GameDataFile::Instance()->GetCopyFloat(
                  "heroespath-stats-race-bonus-base-adj-ratio")) };

        auto const MINOR{ static_cast<stats::Trait_t>(
            static_cast<float>(BASE)
            * game::GameDataFile::Instance()->GetCopyFloat(
                  "heroespath-stats-race-bonus-minor-adj-ratio")) };

        if (TRAIT_ENUM == stats::Traits::Strength)
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
        else if (TRAIT_ENUM == stats::Traits::Accuracy)
        {
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Charm)
        {
            if (RACE_ENUM == creature::race::Pixie)
                return BASE;
            else
                return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Luck)
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
        else if (TRAIT_ENUM == stats::Traits::Speed)
        {
            const stats::Trait_t FLY_BONUS{ ((creature::race::CanFly(RACE_ENUM)) ? BASE : 0) };

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
        else if (TRAIT_ENUM == stats::Traits::Intelligence)
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
            std::ostringstream ss;
            ss << "creature::Stats::RollBonusByRace(stat_enum="
               << stats::Traits::ToString(TRAIT_ENUM) << ", race_enum=" << RACE_ENUM
               << ")_InvalidValueError.";

            throw std::range_error(ss.str());
        }
    }

    stats::Trait_t Stats::RollBonusByRole(
        const stats::Trait_t TRAIT_VALUE,
        const stats::Traits::Enum TRAIT_ENUM,
        const role::Enum ROLE_ENUM)
    {
        auto const BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(TRAIT_VALUE)
            * game::GameDataFile::Instance()->GetCopyFloat(
                  "heroespath-stats-role-bonus-base-adj-ratio")) };

        auto const MINOR{ static_cast<stats::Trait_t>(
            static_cast<float>(BASE)
            * game::GameDataFile::Instance()->GetCopyFloat(
                  "heroespath-stats-role-bonus-minor-adj-ratio")) };

        if (TRAIT_ENUM == stats::Traits::Strength)
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
        else if (TRAIT_ENUM == stats::Traits::Accuracy)
        {
            if (ROLE_ENUM == creature::role::Archer)
                return BASE;
            else if (ROLE_ENUM == creature::role::Knight)
                return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Charm)
        {
            if (ROLE_ENUM == creature::role::Cleric)
                return BASE;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Luck)
        {
            if (ROLE_ENUM == creature::role::Thief)
                return BASE;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Speed)
        {
            if (ROLE_ENUM == creature::role::Thief)
                return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Intelligence)
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
            std::ostringstream ss;
            ss << "creature::Stats::RollBonusByRole(stat_enum="
               << stats::Traits::ToString(TRAIT_ENUM) << ", role_enum=" << ROLE_ENUM
               << ")_InvalidValueError.";

            throw std::range_error(ss.str());
        }
    }

    int Stats::RandomRatio(
        const CreaturePtr_t CREATURE_PTR,
        const stats::Traits::Enum TRAIT_ENUM,
        const int RAND_SPREAD,
        const float RANK_BONUS_MULT,
        const stats::Trait_t TRAIT_BONUS,
        const With OPTIONS)
    {
        auto const RAND_RATIO{ Ratio(CREATURE_PTR, TRAIT_ENUM, TRAIT_BONUS, OPTIONS) };

        auto const TRAIT_BONUS_RATIO{ static_cast<float>(TRAIT_BONUS) / 100.0f };

        auto x{ static_cast<int>(
            static_cast<float>(RAND_SPREAD) * (RAND_RATIO + TRAIT_BONUS_RATIO)) };

        x += static_cast<int>(CREATURE_PTR->Rank().As<float>() * RANK_BONUS_MULT);

        return x;
    }

} // namespace creature
} // namespace heroespath
