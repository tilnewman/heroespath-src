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

#include "game/creature/creature.hpp"

#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <algorithm>


namespace game
{
namespace creature
{

    const stats::Stat_t Stats::LUCK_DIVISOR_{ 5 };


    float Stats::Ratio(const CreaturePtr_t     CREATURE_PTR,
                       const stats::stat::Enum STAT_ENUM,
                       const bool              WILL_INCLUDE_STANDARD_OFFSET,
                       const bool              WILL_INCLUDE_LUCK)
    {
        return Ratio(CREATURE_PTR,
                     stats::StatEnumVec_t (1, STAT_ENUM),
                     WILL_INCLUDE_STANDARD_OFFSET,
                     WILL_INCLUDE_LUCK);
    }

   
    float Stats::Ratio(const CreaturePtr_t          CREATURE_PTR,
                       const stats::StatEnumVec_t & STAT_ENUM_VEC,
                       const bool                   WILL_INCLUDE_STANDARD_OFFSET,
                       const bool                   WILL_INCLUDE_LUCK)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((STAT_ENUM_VEC.empty() == false),
            "game::creature::Stats::Ratio() called with STAT_ENUM_VEC empty.");

        stats::Stat_t randSum{ 0 };
        stats::Stat_t normalSum{ 0 };
        for (auto const NEXT_STAT_ENUM : STAT_ENUM_VEC)
        {
            auto const NEXT_STAT{ CREATURE_PTR->Stats().GetCopy(NEXT_STAT_ENUM) };
            auto const MIN{ ((WILL_INCLUDE_STANDARD_OFFSET) ? NEXT_STAT.CurrentReduced() : 0 ) };
            randSum += misc::random::Int(MIN, NEXT_STAT.Current());

            if (WILL_INCLUDE_LUCK)
            {
                randSum += LuckBonus(CREATURE_PTR);
            }

            normalSum += NEXT_STAT.Normal();
        }

        if (randSum > normalSum)
        {
            randSum = normalSum;
        }

        return static_cast<float>(normalSum) / static_cast<float>(randSum);
    }


    bool Stats::Roll(const CreaturePtr_t     CREATURE_PTR,
                     const stats::stat::Enum STAT_ENUM,
                     const bool              WILL_CONSIDER_LUCK,
                     const bool              WILL_CONSIDER_RANK)
    {
        return Roll(CREATURE_PTR,
                    stats::StatEnumVec_t(1, STAT_ENUM),
                    WILL_CONSIDER_LUCK,
                    WILL_CONSIDER_RANK);
    }


    bool Stats::Roll(const CreaturePtr_t          CREATURE_PTR,
                     const stats::StatEnumVec_t & STAT_ENUM_VEC,
                     const bool                   WILL_CONSIDER_LUCK,
                     const bool                   WILL_CONSIDER_RANK)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr),
            "game::creature::Stats::Versus() called with a null CREATURE_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((STAT_ENUM_VEC.empty() == false),
            "game::creature::Stats::Versus() called with STAT_ENUM_VEC empty.");

        stats::Stat_t rollBase{ 0 };
        for (auto const NEXT_STAT_ENUM : STAT_ENUM_VEC)
        {
            rollBase += CREATURE_PTR->Stats().GetCopy(NEXT_STAT_ENUM).Current();

            if (WILL_CONSIDER_LUCK)
            {
                rollBase += LuckBonus(CREATURE_PTR);
            }
        }
        rollBase /= STAT_ENUM_VEC.size();

        stats::Stat_t bonus{ 0 };
        for (auto const NEXT_STAT_ENUM : STAT_ENUM_VEC)
        {
            bonus += RollBonusByRace(NEXT_STAT_ENUM, CREATURE_PTR->Race().Which());
            bonus += RollBonusByRole(NEXT_STAT_ENUM, CREATURE_PTR->Role().Which());
        }

        auto const ROLL{ rollBase + bonus + static_cast<stats::Stat_t>(
            (WILL_CONSIDER_RANK) ? CREATURE_PTR->Rank() : 0) };
       
        return (misc::random::Int(stats::Stat::VAL_ESTIMATED_MAX_) < ROLL);
    }


    bool Stats::Versus(const CreaturePtr_t     CHALLENGER_PTR,
                       const stats::stat::Enum CHALLENGER_STAT,
                       const CreaturePtr_t     DEFENDER_PTR,
                       const stats::stat::Enum DEFENDER_STAT_PARAM,
                       const stats::Stat_t     CHALLENGER_BONUS_PER,
                       const stats::Stat_t     DEFENDER_BONUS_PER,
                       const bool              WILL_CONDSIDER_RANK,
                       const bool              WILL_CONSIDER_PLAYER_LUCK,
                       const bool              ALLOW_PLAYER_NATURAL_WINS)
    {
        auto const DEFENDER_STAT{ ((DEFENDER_STAT_PARAM == stats::stat::Count) ?
            CHALLENGER_STAT : DEFENDER_STAT_PARAM) };

        return Versus(CHALLENGER_PTR,
                      stats::StatEnumVec_t(1, CHALLENGER_STAT),
                      DEFENDER_PTR,
                      stats::StatEnumVec_t(1, DEFENDER_STAT),
                      CHALLENGER_BONUS_PER,
                      DEFENDER_BONUS_PER,
                      WILL_CONDSIDER_RANK,
                      WILL_CONSIDER_PLAYER_LUCK,
                      ALLOW_PLAYER_NATURAL_WINS);
    }


    bool Stats::Versus(const CreaturePtr_t          CHALLENGER_PTR,
                       const stats::StatEnumVec_t & CHALLENGER_STAT_VEC,
                       const CreaturePtr_t          DEFENDER_PTR,
                       const stats::StatEnumVec_t & DEFENDER_STAT_VEC_PARAM,
                       const stats::Stat_t          CHALLENGER_BONUS_PER,
                       const stats::Stat_t          DEFENDER_BONUS_PER,
                       const bool                   WILL_CONDSIDER_RANK,
                       const bool                   WILL_CONSIDER_PLAYER_LUCK,
                       const bool                   ALLOW_PLAYER_NATURAL_WINS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CHALLENGER_PTR != nullptr),
            "game::creature::Stats::Versus() called with a null CHALLENGER_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((DEFENDER_PTR != nullptr),
            "game::creature::Stats::Versus() called with a null DEFENDER_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((CHALLENGER_STAT_VEC.empty() == false),
            "game::creature::Stats::Versus() called with CHALLENGER_STAT_VEC empty.");

        stats::Stat_t chaRandSum{ 0 };
        stats::Stat_t chaNormalSum{ 0 };

        for (auto const NEXT_STAT_ENUM : CHALLENGER_STAT_VEC)
        {
            auto const NEXT_STAT{ CHALLENGER_PTR->Stats().GetCopy(NEXT_STAT_ENUM) };
            chaRandSum += misc::random::Int(NEXT_STAT.CurrentReduced(), NEXT_STAT.Current());

            if (WILL_CONSIDER_PLAYER_LUCK && CHALLENGER_PTR->IsPlayerCharacter())
            {
                chaRandSum += LuckBonus(CHALLENGER_PTR);
            }

            chaRandSum += CHALLENGER_BONUS_PER;

            chaNormalSum += NEXT_STAT.Normal();
        }

        //check for player natural success (challenger)
        if (ALLOW_PLAYER_NATURAL_WINS &&
            CHALLENGER_PTR->IsPlayerCharacter() &&
            (chaRandSum >= chaNormalSum))
        {
            return true;
        }

        auto const CHALLENGER_ROLL{ chaRandSum + ((WILL_CONDSIDER_RANK) ?
            static_cast<stats::Stat_t>(CHALLENGER_PTR->Rank()) : 0 ) };

        auto const DEFENDER_STAT_VEC{ ((DEFENDER_STAT_VEC_PARAM.empty()) ?
            CHALLENGER_STAT_VEC : DEFENDER_STAT_VEC_PARAM) };

        stats::Stat_t defRandSum{ 0 };
        stats::Stat_t defNormalSum{ 0 };

        for (auto const NEXT_STAT_ENUM : DEFENDER_STAT_VEC)
        {
            auto const NEXT_STAT{ DEFENDER_PTR->Stats().GetCopy(NEXT_STAT_ENUM) };
            defRandSum += misc::random::Int(NEXT_STAT.CurrentReduced(), NEXT_STAT.Current());

            if (WILL_CONSIDER_PLAYER_LUCK && DEFENDER_PTR->IsPlayerCharacter())
            {
                defRandSum += LuckBonus(DEFENDER_PTR);
            }

            defRandSum += DEFENDER_BONUS_PER;

            defNormalSum += NEXT_STAT.Normal();
        }

        //check for player natural success (defender)
        if (ALLOW_PLAYER_NATURAL_WINS &&
            DEFENDER_PTR->IsPlayerCharacter() &&
            (defRandSum >= defNormalSum))
        {
            return true;
        }

        auto const DEFENDER_ROLL{ defRandSum + ((WILL_CONDSIDER_RANK) ?
            static_cast<stats::Stat_t>(DEFENDER_PTR->Rank()) : 0 ) };

        //handle roll tie
        if (CHALLENGER_ROLL == DEFENDER_ROLL)
        {
            auto const CHA_NORMAL_PLUS_RANK{ chaNormalSum +
                static_cast<stats::Stat_t>(CHALLENGER_PTR->Rank()) };

            auto const DEF_NORMAL_PLUS_RANK{ defNormalSum +
                static_cast<stats::Stat_t>(DEFENDER_PTR->Rank()) };

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


    stats::Stat_t Stats::LuckBonus(const CreaturePtr_t CREATURE_PTR)
    {
        return misc::random::Int(CREATURE_PTR->Stats().Lck().Current() /
            LUCK_DIVISOR_);
    }


    stats::Stat_t Stats::RollBonusByRace(const stats::stat::Enum STAT_ENUM,
                                         const race::Enum        RACE_ENUM)
    {
        const stats::Stat_t BASE{ stats::Stat::VAL_ESTIMATED_MAX_ / 8 };
        const stats::Stat_t MINOR{ BASE / 3 };

        switch (STAT_ENUM)
        {
            case stats::stat::Strength:
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
            case stats::stat::Accuracy:
            {
                return 0;
            }
            case stats::stat::Charm:
            {
                if (RACE_ENUM == creature::race::Pixie) return BASE;
                else return 0;
            }
            case stats::stat::Luck:
            {
                if      (RACE_ENUM == creature::race::Gnome) return BASE;
                else if (RACE_ENUM == creature::race::Pixie) return MINOR;
                else if (RACE_ENUM == creature::race::Demon) return MINOR;
                else return 0;
            }
            case stats::stat::Speed:
            {
                const stats::Stat_t FLY_BONUS{ ((creature::race::CanFly(RACE_ENUM)) ? BASE : 0 ) };

                if      (RACE_ENUM == creature::race::Cobra)   return BASE + FLY_BONUS;
                else if (RACE_ENUM == creature::race::Demon)   return MINOR + FLY_BONUS;
                else if (RACE_ENUM == creature::race::Dragon)  return MINOR + FLY_BONUS;
                else if (RACE_ENUM == creature::race::Gnome)   return BASE + FLY_BONUS;
                else if (RACE_ENUM == creature::race::Pixie)   return BASE + FLY_BONUS;
                else if (RACE_ENUM == creature::race::Werecat) return MINOR + FLY_BONUS;
                else if (RACE_ENUM == creature::race::Wolfen)  return MINOR + FLY_BONUS;
                else return FLY_BONUS;
            }
            case stats::stat::Intelligence:
            {
                if      (RACE_ENUM == creature::race::Demon)  return BASE;
                else if (RACE_ENUM == creature::race::Dragon) return MINOR;
                else if (RACE_ENUM == creature::race::Pixie)  return BASE;
                else return 0;
            }
            case stats::stat::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::Stats::RollBonusByRace(stat_enum="
                    << stats::stat::ToString(STAT_ENUM) << ", race_enum="
                    << RACE_ENUM << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    stats::Stat_t Stats::RollBonusByRole(const stats::stat::Enum STAT_ENUM,
                                         const role::Enum        ROLE_ENUM)
    {
        const stats::Stat_t BASE{ stats::Stat::VAL_ESTIMATED_MAX_ / 8 };
        const stats::Stat_t MINOR{ BASE / 3 };

        switch (STAT_ENUM)
        {
            case stats::stat::Strength:
            {
                if      (ROLE_ENUM == creature::role::Archer)      return MINOR;
                else if (ROLE_ENUM == creature::role::Beastmaster) return MINOR;
                else if (ROLE_ENUM == creature::role::Knight)      return BASE;
                else if (ROLE_ENUM == creature::role::Sylavin)     return MINOR;
                return 0;
            }
            case stats::stat::Accuracy:
            {
                if      (ROLE_ENUM == creature::role::Archer) return BASE;
                else if (ROLE_ENUM == creature::role::Knight) return MINOR;
                return 0;
            }
            case stats::stat::Charm:
            {
                if (ROLE_ENUM == creature::role::Cleric) return BASE;
                return 0;
            }
            case stats::stat::Luck:
            {
                if (ROLE_ENUM == creature::role::Thief) return BASE;
                return 0;
            }
            case stats::stat::Speed:
            {
                if (ROLE_ENUM == creature::role::Thief) return MINOR;
                return 0;
            }
            case stats::stat::Intelligence:
            {
                if      (ROLE_ENUM == creature::role::Sorcerer)  return BASE;
                else if (ROLE_ENUM == creature::role::Cleric)    return BASE;
                else if (ROLE_ENUM == creature::role::Bard)      return MINOR;
                else if (ROLE_ENUM == creature::role::Firebrand) return MINOR;
                else if (ROLE_ENUM == creature::role::Sylavin)   return MINOR;
                return 0;
            }
            case stats::stat::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::Stats::RollBonusByRole(stat_enum="
                    << stats::stat::ToString(STAT_ENUM) << ", role_enum="
                    << ROLE_ENUM << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    int Stats::RandomRatioWithFloorAndRankBonus(
        const CreaturePtr_t     CREATURE_PTR,
        const stats::stat::Enum STAT_ENUM,
        const int               RAND_SPREAD,
        const int               FLOOR_DIVISOR,
        const float             RANK_BONUS_MULT)
    {   
        int x{ static_cast<int>(1.0f + (static_cast<float>(RAND_SPREAD) *
            Ratio(CREATURE_PTR, STAT_ENUM, false, true))) };
        
        if (FLOOR_DIVISOR > 0)
        {
            x = (CREATURE_PTR->Stats().GetCopy(STAT_ENUM).Current() / FLOOR_DIVISOR);
        }

        x += static_cast<int>(static_cast<float>(CREATURE_PTR->Rank()) * RANK_BONUS_MULT);
        
        return x;
    }

}
}
