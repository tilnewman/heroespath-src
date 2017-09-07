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
// algorithms.cpp
//
#include "algorithms.hpp"

#include "game/game.hpp"
#include "game/combat/encounter.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/party.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/state/game-state.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace creature
{

    const CreaturePVec_t Algorithms::MakeIntoVector(const CreaturePtr_t CREATURE_PTR)
    {
        return CreaturePVec_t{ CREATURE_PTR };
    }


    std::size_t Algorithms::Players(CreaturePVec_t & pVec_OutParam,
                                    const PlayerOpt  OPTIONS)
    {
        auto const PLAYERS_PVEC{ Game::Instance()->State().Party().Characters() };

        std::size_t count{ 0 };
        for (auto const NEXT_PLAYER_PTR : PLAYERS_PVEC)
        {
            if (((OPTIONS & PlayerOpt::Living) == false) || NEXT_PLAYER_PTR->IsAlive())
            {
                pVec_OutParam.push_back(NEXT_PLAYER_PTR);
                ++count;
            }
        }

        if (((OPTIONS & PlayerOpt::Runaway) == false) && (pVec_OutParam.empty() == false))
        {
            auto const & RUNAWAY_PLAYERS_PVEC{
                combat::Encounter::Instance()->RunawayPlayersVec() };

            for (auto const NEXT_RUNAWAY_PLAYER_PTR : RUNAWAY_PLAYERS_PVEC)
            {
                pVec_OutParam.erase( std::remove(pVec_OutParam.begin(),
                                                 pVec_OutParam.end(),
                                                 NEXT_RUNAWAY_PLAYER_PTR), pVec_OutParam.end());
            }
        }

        return count;
    }


    const CreaturePVec_t Algorithms::Players(const PlayerOpt OPTIONS)
    {
        CreaturePVec_t tempPVec;
        Players(tempPVec, OPTIONS);
        return tempPVec;
    }


    std::size_t Algorithms::NonPlayers(CreaturePVec_t & pVec_OutParam,
                                       const PlayerOpt  OPTIONS)
    {
        auto const NONPLAYERS_PVEC{
            combat::Encounter::Instance()->LivingNonPlayerParty().Characters() };

        std::size_t count{ 0 };
        for (auto const NEXT_NONPLAYER_PTR : NONPLAYERS_PVEC)
        {
            if (((OPTIONS & PlayerOpt::Living) == false) || NEXT_NONPLAYER_PTR->IsAlive())
            {
                pVec_OutParam.push_back(NEXT_NONPLAYER_PTR);
                ++count;
            }
        }

        if ((OPTIONS & PlayerOpt::Living) == false)
        {
            auto const DEAD_NONPLAYERS_PVEC(combat::Encounter::Instance()->
                DeadNonPlayerParty().Characters());

            for (auto const NEXT_DEAD_NONPLAYER_PTR : DEAD_NONPLAYERS_PVEC)
            {
                pVec_OutParam.push_back(NEXT_DEAD_NONPLAYER_PTR);
                ++count;
            }
        }

        if (((OPTIONS & PlayerOpt::Runaway) == false) && (pVec_OutParam.empty() == false))
        {
            auto const RUNAWAY_NONPLAYERS_PVEC{
                combat::Encounter::Instance()->RunawayNonPlayerParty().Characters() };

            for (auto const NEXT_RUNAWAY_NONPLAYER_PTR : RUNAWAY_NONPLAYERS_PVEC)
            {
                pVec_OutParam.erase( std::remove(pVec_OutParam.begin(),
                                                 pVec_OutParam.end(),
                                                 NEXT_RUNAWAY_NONPLAYER_PTR), pVec_OutParam.end());
            }
        }

        return count;
    }


    const CreaturePVec_t Algorithms::NonPlayers(const PlayerOpt OPTIONS)
    {
        CreaturePVec_t tempPVec;
        NonPlayers(tempPVec, OPTIONS);
        return tempPVec;
    }


    std::size_t Algorithms::PlayersByType(
        CreaturePVec_t & pVec_OutParam,
        const TypeOpt    TYPE_OPTION,
        const PlayerOpt  PLAYER_OPTIONS)
    {
        if (TYPE_OPTION == TypeOpt::Player)
        {
            return Players(pVec_OutParam, PLAYER_OPTIONS);
        }
        else
        {
            return NonPlayers(pVec_OutParam, PLAYER_OPTIONS);
        }
    }


    const CreaturePVec_t Algorithms::PlayersByType(
        const TypeOpt    TYPE_OPTION,
        const PlayerOpt  PLAYER_OPTIONS)
    {
        if (TYPE_OPTION == TypeOpt::Player)
        {
            return Players(PLAYER_OPTIONS);
        }
        else
        {
            return NonPlayers(PLAYER_OPTIONS);
        }
    }


    const std::string Algorithms::Names(const CreaturePVec_t &      CREATURE_PVEC,
                                        const std::size_t           MAX_COUNT,
                                        const misc::Vector::JoinOpt JOIN_OPTIONS,
                                        const NamesOpt              NAMES_OPTIONS)
    {
        if (NAMES_OPTIONS == NamesOpt::WithRaceAndRole)
        {
            return misc::Vector::Join<CreaturePtr_t>(
                CREATURE_PVEC,
                MAX_COUNT,
                JOIN_OPTIONS,
                [](const CreaturePtr_t CPTR) -> const std::string
                {
                    return CPTR->NameAndRaceAndRole();
                });
        }
        else
        {
            return misc::Vector::Join<CreaturePtr_t>(
                CREATURE_PVEC,
                MAX_COUNT,
                JOIN_OPTIONS,
                [](const CreaturePtr_t CPTR) -> const std::string
                {
                    return CPTR->Name();
                });
        }
    }


    const CreaturePVec_t Algorithms::FindByAlive(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t tempPVec;

        if (CREATURE_PVEC.empty())
        {
            return tempPVec;
        }

        std::copy_if(
            CREATURE_PVEC.begin(),
            CREATURE_PVEC.end(),
            back_inserter(tempPVec),
            [CRITERIA_OPTION]
            (const creature::CreaturePtr_t CPTRC)
            {
                return (CPTRC->IsAlive() == (CRITERIA_OPTION == CriteriaOpt::Meets));
            });

        return tempPVec;
    }


    void Algorithms::SortByLowestHealthNormal(CreaturePVec_t & pVec_OutParam)
    {
        if (pVec_OutParam.empty() == false)
        {
            std::sort(
                pVec_OutParam.begin(),
                pVec_OutParam.end(),
                [](CreatureCPtrC_t A_CPTRC, CreatureCPtrC_t B_CPTRC)
                {
                    return A_CPTRC->HealthNormal() < B_CPTRC->HealthNormal();
                });
        }
    }


    const CreaturePVec_t Algorithms::FindLowestHealthNormal(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
        {
            return CreaturePVec_t();
        }

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealthNormal(tempPVec);
        auto const LOWEST_HEALTH_NORMAL(tempPVec[0]->HealthNormal());

        CreaturePVec_t lowestHealthNormalVec;
        std::copy_if(
            tempPVec.begin(),
            tempPVec.end(),
            back_inserter(lowestHealthNormalVec),
            [LOWEST_HEALTH_NORMAL](CreatureCPtrC_t CPTRC)
            {
                return CPTRC->HealthNormal() == LOWEST_HEALTH_NORMAL;
            });

        return lowestHealthNormalVec;
    }


    void Algorithms::SortByLowestHealth(CreaturePVec_t & pVec_OutParam)
    {
        if (pVec_OutParam.empty() == false)
        {
            std::sort(
                pVec_OutParam.begin(),
                pVec_OutParam.end(),
                [](CreatureCPtrC_t A_CPTRC, CreatureCPtrC_t B_CPTRC)
                {
                    return A_CPTRC->HealthCurrent() < B_CPTRC->HealthCurrent();
                });
        }
    }


    const CreaturePVec_t Algorithms::FindLowestHealth(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
            return CreaturePVec_t();

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealth(tempPVec);
        auto const LOWEST_HEALTH_CURR(tempPVec[0]->HealthCurrent());

        CreaturePVec_t lowestHealthCurrVec;
        std::copy_if(
            tempPVec.begin(),
            tempPVec.end(),
            back_inserter(lowestHealthCurrVec),
            [LOWEST_HEALTH_CURR](CreatureCPtrC_t CPTRC)
            {
                return CPTRC->HealthCurrent() == LOWEST_HEALTH_CURR;
            });

        return lowestHealthCurrVec;
    }


    void Algorithms::SortByLowestHealthRatio(CreaturePVec_t & pVec_OutParam)
    {
        if (pVec_OutParam.empty() == false)
        {
            std::sort(
                pVec_OutParam.begin(),
                pVec_OutParam.end(),
                [](CreatureCPtrC_t A_CPTRC, CreatureCPtrC_t B_CPTRC)
                {
                    return A_CPTRC->HealthRatio() < B_CPTRC->HealthRatio();
                });
        }
    }


    const CreaturePVec_t Algorithms::FindLowestHealthRatio(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
        {
            return CreaturePVec_t();
        }

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealthRatio(tempPVec);
        auto const LOWEST_HEALTH_RATIO(tempPVec[0]->HealthRatio());

        CreaturePVec_t lowestHealthRatioVec;
        std::copy_if(
            tempPVec.begin(),
            tempPVec.end(),
            back_inserter(lowestHealthRatioVec),
            [LOWEST_HEALTH_RATIO](CreatureCPtrC_t CPTRC)
            {
                return misc::IsRealClose(CPTRC->HealthRatio(), LOWEST_HEALTH_RATIO);
            } );

        return lowestHealthRatioVec;
    }


    const CreaturePVec_t Algorithms::FindByConditionMeaningNotAThreatPermenantly(
        const CreaturePVec_t &   CREATURE_PVEC,
        const CondSingleOpt      CONDITION_OPTION,
        const creature::UnconOpt UNCONSCIOUS_OPTION)
    {
        CreaturePVec_t creaturesThatAreStillAThreatVec;

        if (CREATURE_PVEC.empty())
        {
            return creaturesThatAreStillAThreatVec;
        }

        std::copy_if(
            CREATURE_PVEC.begin(),
            CREATURE_PVEC.end(),
            back_inserter(creaturesThatAreStillAThreatVec),
            [&](const CreaturePtr_t CCPTR)
            {
                return CCPTR->HasConditionNotAThreatPerm(UNCONSCIOUS_OPTION) ==
                    (CONDITION_OPTION == CondSingleOpt::Has);
            });

        return creaturesThatAreStillAThreatVec;
    }


    const CreaturePVec_t Algorithms::FindByConditionMeaningNotAThreatTemporarily(
        const CreaturePVec_t & CREATURE_PVEC,
        const CondSingleOpt    CONDITION_OPTION)
    {
        CreaturePVec_t creaturesWithCondsButAreStillAThreatVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(creaturesWithCondsButAreStillAThreatVec),
                [CONDITION_OPTION](const CreatureCPtrC_t CPTRC)
                {
                    return CPTRC->HasConditionNotAThreatTemp() ==
                        (CONDITION_OPTION == CondSingleOpt::Has);
                });
        }

        return creaturesWithCondsButAreStillAThreatVec;
    }


    const CreaturePVec_t Algorithms::FindByRace(
        const CreaturePVec_t & CREATURE_PVEC,
        const race::Enum       RACE_ENUM,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t byRacePVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byRacePVec),
                [RACE_ENUM, CRITERIA_OPTION](CreatureCPtrC_t CPTRC)
                {
                    return (CPTRC->Race() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return byRacePVec;
    }


    const CreaturePVec_t Algorithms::FindByRole(
        const CreaturePVec_t & CREATURE_PVEC,
        const role::Enum       ROLE_ENUM,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t byRolePVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byRolePVec),
                [ROLE_ENUM, CRITERIA_OPTION](CreatureCPtrC_t CPTRC)
                {
                    return (CPTRC->Role() == ROLE_ENUM) == (CRITERIA_OPTION == CriteriaOpt::Meets);
                } );
        }

        return byRolePVec;
    }


    const CreaturePVec_t Algorithms::FindByCanCast(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t canCastPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(canCastPVec),
                [CRITERIA_OPTION](CreatureCPtrC_t CPTRC)
                {
                    return (CPTRC->CanCastSpells() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                } );
        }

        return canCastPVec;
    }


    const CreaturePVec_t Algorithms::FindByIsBeast(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t byBeastPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byBeastPVec),
                [CRITERIA_OPTION](CreatureCPtrC_t CPTRC)
                {
                    return (CPTRC->IsBeast() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                } );
        }

        return byBeastPVec;
    }


    const CreaturePVec_t Algorithms::FindByIsHoldingProjWeapon(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t byProjectileWeaponPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byProjectileWeaponPVec),
                [CRITERIA_OPTION](CreatureCPtrC_t CPTRC)
                {
                    return (CPTRC->IsHoldingProjectileWeapon() ==
                        (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return byProjectileWeaponPVec;
    }


    const CreaturePVec_t Algorithms::FindByCanFly(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t canFlyPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(canFlyPVec),
                [CRITERIA_OPTION](CreatureCPtrC_t CPTRC)
                {
                    return (CPTRC->CanFly() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                } );
        }

        return canFlyPVec;
    }


    const CreaturePVec_t Algorithms::FindByIsFlying(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t isFlyinfPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(isFlyinfPVec),
                [CRITERIA_OPTION](const creature::CreaturePtr_t CPTRC)
                {
                   return (combat::Encounter::Instance()->
                       GetTurnInfoCopy(CPTRC).GetIsFlying() ==
                           (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return isFlyinfPVec;
    }


    const CreaturePVec_t Algorithms::FindByCanTakeAction(
        const CreaturePVec_t & CREATURE_PVEC,
        const CriteriaOpt      CRITERIA_OPTION)
    {
        CreaturePVec_t canTakeActionPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(canTakeActionPVec),
                [CRITERIA_OPTION](const creature::CreaturePtr_t CPTRC)
                {
                    return (CPTRC->CanTakeAction() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return canTakeActionPVec;
    }


    const CreaturePVec_t Algorithms::FindByHasCondition(
        const CreaturePVec_t &  CREATURE_PVEC,
        const Conditions::Enum  COND_ENUM,
        const CondAllOpt        COND_ALL_OPTION,
        const CondSingleOpt     COND_SINGLE_OPTION)
    {
        return FindByHasConditions(
            CREATURE_PVEC,
            CondEnumVec_t(1, COND_ENUM ),
            COND_ALL_OPTION,
            COND_SINGLE_OPTION);
    }


    const CreaturePVec_t Algorithms::FindByHasConditions(
        const CreaturePVec_t & CREATURE_PVEC,
        const CondEnumVec_t &  CONDS_VEC,
        const CondAllOpt       COND_ALL_OPTION,
        const CondSingleOpt    COND_SINGLE_OPTION)
    {
        CreaturePVec_t havingConditionPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(havingConditionPVec),
                [&](const creature::CreaturePtr_t PTR)
                {
                    if (COND_ALL_OPTION == CondAllOpt::HasAll)
                    {
                        auto hasAll{ true };
                        for (auto const NEXT_COND_ENUM : CONDS_VEC)
                        {
                            if (PTR->HasCondition(NEXT_COND_ENUM) !=
                                (COND_SINGLE_OPTION == CondSingleOpt::Has))
                            {
                                hasAll = false;
                                break;
                            }
                        }
                        return hasAll;
                    }
                    else
                    {
                        for (auto const NEXT_COND_ENUM : CONDS_VEC)
                        {
                            if (PTR->HasCondition(NEXT_COND_ENUM) ==
                                (COND_SINGLE_OPTION == CondSingleOpt::Has))
                            {
                                return true;
                            }
                        }
                        return false;
                    }
                });
        }

        return havingConditionPVec;
    }

}
}
