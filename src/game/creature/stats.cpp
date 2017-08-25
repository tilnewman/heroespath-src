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
// stats.cpp
//
#include "stats.hpp"

#include "game/game-data-file.hpp"
#include "game/creature/creature.hpp"

#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <algorithm>


namespace game
{
namespace creature
{

    float Stats::Ratio(const CreaturePtr_t       CREATURE_PTR,
                       const stats::Traits::Enum TRAIT_ENUM,
                       const bool                WILL_INCLUDE_LUCK,
                       const bool                WILL_INCLUDE_RACEROLE_BONUS)
    {
        return Ratio(CREATURE_PTR,
                     stats::TraitsVec_t(1, TRAIT_ENUM),
                     WILL_INCLUDE_LUCK,
                     WILL_INCLUDE_RACEROLE_BONUS);
    }


    float Stats::Ratio(const CreaturePtr_t        CREATURE_PTR,
                       const stats::TraitsVec_t & TRAIT_ENUM_VEC,
                       const bool                 WILL_INCLUDE_LUCK,
                       const bool                 WILL_INCLUDE_RACEROLE_BONUS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr),
            "game::creature::Stats::Ratio() called with a null CREATURE_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((TRAIT_ENUM_VEC.empty() == false),
            "game::creature::Stats::Ratio() called with TRAIT_ENUM_VEC empty.");

        stats::Trait_t randSum{ 0 };
        stats::Trait_t normalSum{ 0 };
        for (auto const NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            normalSum += CREATURE_PTR->TraitNormal(NEXT_TRAIT_ENUM);

            auto const NEXT_TRAIT_WORKING{ CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM) };
            randSum += NEXT_TRAIT_WORKING;

            if (WILL_INCLUDE_LUCK)
            {
                randSum += LuckBonus(CREATURE_PTR);
            }

            if (WILL_INCLUDE_RACEROLE_BONUS)
            {
                randSum += RollBonusByRace(NEXT_TRAIT_WORKING,
                                           NEXT_TRAIT_ENUM,
                                           CREATURE_PTR->Race());

                randSum += RollBonusByRole(NEXT_TRAIT_WORKING,
                                           NEXT_TRAIT_ENUM,
                                           CREATURE_PTR->Role());
            }
        }

        if (randSum > normalSum)
        {
            randSum = normalSum;
        }

        return static_cast<float>(randSum) / static_cast<float>(normalSum);
    }


    stats::Trait_t Stats::Roll(const CreaturePtr_t      CREATURE_PTR,
                              const stats::Traits::Enum TRAIT_ENUM,
                              const bool               WILL_INCLUDE_LUCK,
                              const bool               WILL_INCLUDE_RACEROLE_BONUS)
    {
        return Roll(CREATURE_PTR,
                    stats::TraitsVec_t (1, TRAIT_ENUM),
                    WILL_INCLUDE_LUCK,
                    WILL_INCLUDE_RACEROLE_BONUS);
    }


    stats::Trait_t Stats::Roll(const CreaturePtr_t       CREATURE_PTR,
                              const stats::TraitsVec_t & TRAIT_ENUM_VEC,
                              const bool                 WILL_INCLUDE_LUCK,
                              const bool                 WILL_INCLUDE_RACEROLE_BONUS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr),
            "game::creature::Stats::Roll() called with a null CREATURE_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((TRAIT_ENUM_VEC.empty() == false),
            "game::creature::Stats::Roll() called with TRAIT_ENUM_VEC empty.");

        auto randSum{ 0.0f };
        for (auto const NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            auto const CURRENT{ static_cast<float>(CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM)) };

            auto const RATIO{ Ratio(CREATURE_PTR,
                                    TRAIT_ENUM_VEC,
                                    WILL_INCLUDE_LUCK,
                                    WILL_INCLUDE_RACEROLE_BONUS) };

            randSum += (CURRENT * RATIO);
        }

        return static_cast<stats::Trait_t>(randSum / static_cast<float>(TRAIT_ENUM_VEC.size()));
    }


    bool Stats::Test(const CreaturePtr_t       CREATURE_PTR,
                     const stats::Traits::Enum TRAIT_ENUM,
                     const float               RANK_BONUS_RATIO,
                     const bool                WILL_INCLUDE_LUCK,
                     const bool                WILL_INCLUDE_RACEROLE_BONUS)
    {
        return Test(CREATURE_PTR,
                    stats::TraitsVec_t(1, TRAIT_ENUM),
                    RANK_BONUS_RATIO,
                    WILL_INCLUDE_LUCK,
                    WILL_INCLUDE_RACEROLE_BONUS);
    }


    bool Stats::Test(const CreaturePtr_t        CREATURE_PTR,
                     const stats::TraitsVec_t & TRAIT_ENUM_VEC,
                     const float                RANK_BONUS_RATIO,
                     const bool                 WILL_INCLUDE_LUCK,
                     const bool                 WILL_INCLUDE_RACEROLE_BONUS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr),
            "game::creature::Stats::Test() called with a null CREATURE_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((TRAIT_ENUM_VEC.empty() == false),
            "game::creature::Stats::Test() called with TRAIT_ENUM_VEC empty.");

        stats::Trait_t rollBaseSum{ 0 };
        for (auto const NEXT_TRAIT_ENUM : TRAIT_ENUM_VEC)
        {
            auto const TRAIT_WORKING{ CREATURE_PTR->TraitWorking(NEXT_TRAIT_ENUM) };
            rollBaseSum += TRAIT_WORKING;

            if (WILL_INCLUDE_LUCK)
            {
                rollBaseSum += LuckBonus(CREATURE_PTR);
            }

            if (WILL_INCLUDE_RACEROLE_BONUS)
            {
                rollBaseSum += RollBonusByRace(TRAIT_WORKING,
                                               NEXT_TRAIT_ENUM,
                                               CREATURE_PTR->Race());

                rollBaseSum += RollBonusByRole(TRAIT_WORKING,
                                               NEXT_TRAIT_ENUM,
                                               CREATURE_PTR->Role());
            }
        }

        rollBaseSum /= static_cast<int>(TRAIT_ENUM_VEC.size());

        auto const RANK_BONUS{ static_cast<stats::Trait_t>(
            static_cast<float>(CREATURE_PTR->Rank()) * RANK_BONUS_RATIO) };

        return (misc::random::Int(stats::Trait::STAT_MAX_ESTIMATED_) < (rollBaseSum + RANK_BONUS));
    }


    bool Stats::Versus(const CreaturePtr_t       CHALLENGER_PTR,
                       const stats::Traits::Enum CHALLENGER_TRAIT,
                       const CreaturePtr_t       DEFENDER_PTR,
                       const stats::Traits::Enum DEFENDER_TRAIT_PARAM,
                       const stats::Trait_t      CHALLENGER_BONUS_PER,
                       const stats::Trait_t      DEFENDER_BONUS_PER,
                       const bool                WILL_INCLUDE_RACEROLE_BONUS,
                       const bool                WILL_INCLUDE_RANK,
                       const bool                WILL_INCLUDE_PLAYER_LUCK,
                       const bool                ALLOW_PLAYER_NATURAL_WINS)
    {
        auto const DEFENDER_TRAIT{ ((DEFENDER_TRAIT_PARAM == stats::Traits::Count) ?
            CHALLENGER_TRAIT : DEFENDER_TRAIT_PARAM) };

        return Versus(CHALLENGER_PTR,
                      stats::TraitsVec_t(1, CHALLENGER_TRAIT),
                      DEFENDER_PTR,
                      stats::TraitsVec_t(1, DEFENDER_TRAIT),
                      CHALLENGER_BONUS_PER,
                      DEFENDER_BONUS_PER,
                      WILL_INCLUDE_RACEROLE_BONUS,
                      WILL_INCLUDE_RANK,
                      WILL_INCLUDE_PLAYER_LUCK,
                      ALLOW_PLAYER_NATURAL_WINS);
    }


    bool Stats::Versus(const CreaturePtr_t        CHALLENGER_PTR,
                       const stats::TraitsVec_t & CHALLENGER_TRAIT_VEC,
                       const CreaturePtr_t        DEFENDER_PTR,
                       const stats::TraitsVec_t & DEFENDER_TRAIT_VEC_PARAM,
                       const stats::Trait_t       CHALLENGER_BONUS_PER,
                       const stats::Trait_t       DEFENDER_BONUS_PER,
                       const bool                 WILL_INCLUDE_RACEROLE_BONUS,
                       const bool                 WILL_INCLUDE_RANK,
                       const bool                 WILL_INCLUDE_PLAYER_LUCK,
                       const bool                 ALLOW_PLAYER_NATURAL_WINS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CHALLENGER_PTR != nullptr),
            "game::creature::Stats::Versus() called with a null CHALLENGER_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((DEFENDER_PTR != nullptr),
            "game::creature::Stats::Versus() called with a null DEFENDER_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((CHALLENGER_TRAIT_VEC.empty() == false),
            "game::creature::Stats::Versus() called with CHALLENGER_TRAIT_VEC empty.");

        stats::Trait_t chaRandSum{ 0 };
        stats::Trait_t chaNormalSum{ 0 };

        for (auto const NEXT_TRAIT_ENUM : CHALLENGER_TRAIT_VEC)
        {
            chaRandSum += Roll(
                CHALLENGER_PTR,
                NEXT_TRAIT_ENUM,
                (CHALLENGER_PTR->IsPlayerCharacter() && WILL_INCLUDE_PLAYER_LUCK),
                WILL_INCLUDE_RACEROLE_BONUS);

            chaRandSum += CHALLENGER_BONUS_PER;
            chaNormalSum += CHALLENGER_PTR->TraitNormal(NEXT_TRAIT_ENUM);
        }

        //check for player natural success (challenger)
        if (ALLOW_PLAYER_NATURAL_WINS &&
            CHALLENGER_PTR->IsPlayerCharacter() &&
            (chaRandSum >= chaNormalSum))
        {
            return true;
        }

        auto const CHALLENGER_ROLL{ chaRandSum + ((WILL_INCLUDE_RANK) ?
            static_cast<stats::Trait_t>(CHALLENGER_PTR->Rank()) : 0 ) };

        auto const DEFENDER_TRAIT_VEC{ ((DEFENDER_TRAIT_VEC_PARAM.empty()) ?
            CHALLENGER_TRAIT_VEC : DEFENDER_TRAIT_VEC_PARAM) };

        stats::Trait_t defRandSum{ 0 };
        stats::Trait_t defNormalSum{ 0 };

        for (auto const NEXT_TRAIT_ENUM : DEFENDER_TRAIT_VEC)
        {
            defRandSum += Roll(
                DEFENDER_PTR,
                NEXT_TRAIT_ENUM,
                (DEFENDER_PTR->IsPlayerCharacter() && WILL_INCLUDE_PLAYER_LUCK),
                WILL_INCLUDE_RACEROLE_BONUS);

            defRandSum += DEFENDER_BONUS_PER;
            defNormalSum += DEFENDER_PTR->TraitNormal(NEXT_TRAIT_ENUM);
        }

        //check for player natural success (defender)
        if (ALLOW_PLAYER_NATURAL_WINS &&
            DEFENDER_PTR->IsPlayerCharacter() &&
            (defRandSum >= defNormalSum))
        {
            return true;
        }

        auto const DEFENDER_ROLL{ defRandSum + ((WILL_INCLUDE_RANK) ?
            static_cast<stats::Trait_t>(DEFENDER_PTR->Rank()) : 0 ) };

        //handle roll tie
        if (CHALLENGER_ROLL == DEFENDER_ROLL)
        {
            auto const CHA_NORMAL_PLUS_RANK{ chaNormalSum +
                static_cast<stats::Trait_t>(CHALLENGER_PTR->Rank()) };

            auto const DEF_NORMAL_PLUS_RANK{ defNormalSum +
                static_cast<stats::Trait_t>(DEFENDER_PTR->Rank()) };

            //handle normal+rank tie
            if (CHA_NORMAL_PLUS_RANK == DEF_NORMAL_PLUS_RANK)
            {
                //handle normal tie
                if (chaNormalSum == defNormalSum)
                {
                    //handle everything tied case
                    if (CHALLENGER_PTR->IsPlayerCharacter() &&
                        DEFENDER_PTR->IsPlayerCharacter())
                    {
                        //handle everything tied and both are players by fair coin toss
                        return misc::random::Bool();
                    }
                    else
                    {
                        //normal tie goes to the player
                        return CHALLENGER_PTR->IsPlayerCharacter();
                    }
                }
                else
                {
                    //normal+rank tie goes to whoever has the greatest normal stats
                    return (chaNormalSum > defNormalSum);
                }
            }
            else
            {
                //roll tie goes to whoever has the greatest normal+rank
                return (CHA_NORMAL_PLUS_RANK > DEF_NORMAL_PLUS_RANK);
            }
        }
        else
        {
            //handle normal win condition
            return (CHALLENGER_ROLL > DEFENDER_ROLL);
        }
    }


    stats::Trait_t Stats::LuckBonus(const CreaturePtr_t CREATURE_PTR)
    {
        return misc::random::Int( static_cast<int>(static_cast<float>(
            CREATURE_PTR->TraitWorking(stats::Traits::Luck)) /
                GameDataFile::Instance()->GetCopyFloat("heroespath-fight-stats-luck-adj-ratio")) );
    }


    stats::Trait_t Stats::RollBonusByRace(const stats::Trait_t      TRAIT_VALUE,
                                          const stats::Traits::Enum TRAIT_ENUM,
                                          const race::Enum          RACE_ENUM)
    {
        auto const BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(TRAIT_VALUE) * GameDataFile::Instance()->
                GetCopyFloat("heroespath-stats-race-bonus-base-adj-ratio")) };

        auto const MINOR{ static_cast<stats::Trait_t>(
            static_cast<float>(BASE) * GameDataFile::Instance()->
                GetCopyFloat("heroespath-stats-race-bonus-minor-adj-ratio")) };

        if (TRAIT_ENUM == stats::Traits::Strength)
        {
            if      (RACE_ENUM == creature::race::Boar)     return MINOR;
            else if (RACE_ENUM == creature::race::Demon)    return BASE + (BASE / 2);
            else if (RACE_ENUM == creature::race::Dragon)   return BASE;
            else if (RACE_ENUM == creature::race::Giant)    return BASE;
            else if (RACE_ENUM == creature::race::Hydra)    return BASE;
            else if (RACE_ENUM == creature::race::Lion)     return BASE - MINOR;
            else if (RACE_ENUM == creature::race::LionBoar) return BASE + MINOR;
            else if (RACE_ENUM == creature::race::Minotaur) return BASE / 2;
            else if (RACE_ENUM == creature::race::Ogre)     return BASE - MINOR;
            else if (RACE_ENUM == creature::race::Ramonaut) return BASE;
            else if (RACE_ENUM == creature::race::Troll)    return MINOR;
            else if (RACE_ENUM == creature::race::Werebear) return MINOR * 2;
            else if (RACE_ENUM == creature::race::Wereboar) return MINOR;
            else if (RACE_ENUM == creature::race::Wyvern)   return MINOR;
            else return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Accuracy)
        {
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Charm)
        {
            if (RACE_ENUM == creature::race::Pixie) return BASE;
            else return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Luck)
        {
            if      (RACE_ENUM == creature::race::Gnome) return BASE;
            else if (RACE_ENUM == creature::race::Pixie) return MINOR;
            else if (RACE_ENUM == creature::race::Demon) return MINOR;
            else return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Speed)
        {
            const stats::Trait_t FLY_BONUS{ ((creature::race::CanFly(RACE_ENUM)) ? BASE : 0 ) };

            if      (RACE_ENUM == creature::race::Cobra)   return BASE + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Demon)   return MINOR + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Dragon)  return MINOR + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Gnome)   return BASE + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Pixie)   return BASE + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Werecat) return MINOR + FLY_BONUS;
            else if (RACE_ENUM == creature::race::Wolfen)  return MINOR + FLY_BONUS;
            else return FLY_BONUS;
        }
        else if (TRAIT_ENUM == stats::Traits::Intelligence)
        {
            if      (RACE_ENUM == creature::race::Demon)  return BASE;
            else if (RACE_ENUM == creature::race::Dragon) return MINOR;
            else if (RACE_ENUM == creature::race::Pixie)  return BASE;
            else if (RACE_ENUM == creature::race::Witch)  return BASE;
            else return 0;
        }
        else
        {
            std::ostringstream ss;
            ss << "game::creature::Stats::RollBonusByRace(stat_enum="
                << stats::Traits::ToString(TRAIT_ENUM) << ", race_enum="
                << RACE_ENUM << ")_InvalidValueError.";

            throw std::range_error(ss.str());
        }
    }


    stats::Trait_t Stats::RollBonusByRole(const stats::Trait_t      TRAIT_VALUE,
                                          const stats::Traits::Enum TRAIT_ENUM,
                                          const role::Enum          ROLE_ENUM)
    {
        auto const BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(TRAIT_VALUE) * GameDataFile::Instance()->
            GetCopyFloat("heroespath-stats-role-bonus-base-adj-ratio")) };

        auto const MINOR{ static_cast<stats::Trait_t>(
            static_cast<float>(BASE) * GameDataFile::Instance()->
            GetCopyFloat("heroespath-stats-role-bonus-minor-adj-ratio")) };

        if (TRAIT_ENUM == stats::Traits::Strength)
        {
            if      (ROLE_ENUM == creature::role::Archer)      return MINOR;
            else if (ROLE_ENUM == creature::role::Beastmaster) return MINOR;
            else if (ROLE_ENUM == creature::role::Knight)      return BASE;
            else if (ROLE_ENUM == creature::role::Sylavin)     return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Accuracy)
        {
            if      (ROLE_ENUM == creature::role::Archer) return BASE;
            else if (ROLE_ENUM == creature::role::Knight) return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Charm)
        {
            if (ROLE_ENUM == creature::role::Cleric) return BASE;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Luck)
        {
            if (ROLE_ENUM == creature::role::Thief) return BASE;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Speed)
        {
            if (ROLE_ENUM == creature::role::Thief) return MINOR;
            return 0;
        }
        else if (TRAIT_ENUM == stats::Traits::Intelligence)
        {
            if      (ROLE_ENUM == creature::role::Sorcerer)  return BASE;
            else if (ROLE_ENUM == creature::role::Cleric)    return BASE;
            else if (ROLE_ENUM == creature::role::Bard)      return MINOR;
            else if (ROLE_ENUM == creature::role::Firebrand) return MINOR;
            else if (ROLE_ENUM == creature::role::Sylavin)   return MINOR;
            return 0;
        }
        else
        {
            std::ostringstream ss;
            ss << "game::creature::Stats::RollBonusByRole(stat_enum="
                << stats::Traits::ToString(TRAIT_ENUM) << ", role_enum="
                << ROLE_ENUM << ")_InvalidValueError.";

            throw std::range_error(ss.str());
        }
    }


    int Stats::RandomRatioWithFloorAndRankBonus(
        const CreaturePtr_t       CREATURE_PTR,
        const stats::Traits::Enum TRAIT_ENUM,
        const int                 RAND_SPREAD,
        const int                 FLOOR_DIVISOR,
        const float               RANK_BONUS_MULT,
        const bool                WILL_INCLUDE_LUCK,
        const bool                WILL_INCLUDE_RACEROLE_BONUS)
    {
        auto const RATIO{ Ratio(CREATURE_PTR,
                                TRAIT_ENUM,
                                WILL_INCLUDE_LUCK,
                                WILL_INCLUDE_RACEROLE_BONUS) };

        int x{ static_cast<int>((static_cast<float>(RAND_SPREAD) * RATIO)) };

        if (FLOOR_DIVISOR > 0)
        {
            x += (CREATURE_PTR->TraitWorking(TRAIT_ENUM) / FLOOR_DIVISOR);
        }

        x += static_cast<int>(static_cast<float>(CREATURE_PTR->Rank()) * RANK_BONUS_MULT);

        return x;
    }

}
}
