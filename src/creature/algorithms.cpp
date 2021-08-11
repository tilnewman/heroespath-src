// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// algorithms.cpp
//
#include "algorithms.hpp"

#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"

#include "misc/random.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace creature
{

    const CreaturePVec_t Algorithms::MakeIntoVector(const CreaturePtr_t CREATURE_PTR)
    {
        return CreaturePVec_t { CREATURE_PTR };
    }

    std::size_t Algorithms::Players(CreaturePVec_t & pVec_OutParam, const PlayerOpt OPTIONS)
    {
        const auto PLAYERS_PVEC { game::Game::Instance()->State().Party().Characters() };

        std::size_t count { 0 };
        for (const auto & NEXT_PLAYER_PTR : PLAYERS_PVEC)
        {
            if (((OPTIONS & PlayerOpt::Living) == false) || NEXT_PLAYER_PTR->IsAlive())
            {
                pVec_OutParam.emplace_back(NEXT_PLAYER_PTR);
                ++count;
            }
        }

        if (((OPTIONS & PlayerOpt::Runaway) == false) && (pVec_OutParam.empty() == false))
        {
            for (const auto & NEXT_RUNAWAY_PLAYER_PTR :
                 combat::Encounter::Instance()->RunawayPlayers())
            {
                pVec_OutParam.erase(
                    std::remove(
                        pVec_OutParam.begin(), pVec_OutParam.end(), NEXT_RUNAWAY_PLAYER_PTR),
                    pVec_OutParam.end());
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

    std::size_t Algorithms::NonPlayers(CreaturePVec_t & pVec_OutParam, const PlayerOpt OPTIONS)
    {
        std::size_t count { 0 };
        for (const auto & NEXT_NONPLAYER_PTR : combat::Encounter::Instance()->LivingNonPlayers())
        {
            if (((OPTIONS & PlayerOpt::Living) == false) || NEXT_NONPLAYER_PTR->IsAlive())
            {
                pVec_OutParam.emplace_back(NEXT_NONPLAYER_PTR);
                ++count;
            }
        }

        if ((OPTIONS & PlayerOpt::Living) == false)
        {
            for (const auto & NEXT_DEAD_NONPLAYER_PTR :
                 combat::Encounter::Instance()->DeadNonPlayers())
            {
                pVec_OutParam.emplace_back(NEXT_DEAD_NONPLAYER_PTR);
                ++count;
            }
        }

        if (((OPTIONS & PlayerOpt::Runaway) == false) && (pVec_OutParam.empty() == false))
        {
            for (const auto & NEXT_RUNAWAY_NONPLAYER_PTR :
                 combat::Encounter::Instance()->RunawayNonPlayers())
            {
                pVec_OutParam.erase(
                    std::remove(
                        pVec_OutParam.begin(), pVec_OutParam.end(), NEXT_RUNAWAY_NONPLAYER_PTR),
                    pVec_OutParam.end());
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
        CreaturePVec_t & pVec_OutParam, const TypeOpt TYPE_OPTION, const PlayerOpt PLAYER_OPTIONS)
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

    const CreaturePVec_t
        Algorithms::PlayersByType(const TypeOpt TYPE_OPTION, const PlayerOpt PLAYER_OPTIONS)
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

    const std::string Algorithms::Names(
        const CreaturePVec_t & CREATURE_PVEC,
        const std::size_t MAX_COUNT,
        const misc::Vector::JoinOpt JOIN_OPTIONS,
        const NamesOpt NAMES_OPTIONS)
    {
        if (NAMES_OPTIONS == NamesOpt::WithRaceAndRole)
        {
            return misc::Vector::Join<CreaturePtr_t>(
                CREATURE_PVEC,
                MAX_COUNT,
                JOIN_OPTIONS,
                [](const CreaturePtr_t & CPTR) -> const std::string {
                    return CPTR->NameAndRaceAndRole();
                });
        }
        else
        {
            return misc::Vector::Join<CreaturePtr_t>(
                CREATURE_PVEC,
                MAX_COUNT,
                JOIN_OPTIONS,
                [](const CreaturePtr_t & CPTR) -> const std::string { return CPTR->Name(); });
        }
    }

    const CreaturePVec_t Algorithms::FindByAlive(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
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
            [CRITERIA_OPTION](const auto & PTR) {
                return (PTR->IsAlive() == (CRITERIA_OPTION == CriteriaOpt::Meets));
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
                [](const auto & A_PTR, const auto & B_PTR) {
                    return A_PTR->HealthNormal() < B_PTR->HealthNormal();
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
        const auto LOWEST_HEALTH_NORMAL(tempPVec[0]->HealthNormal());

        CreaturePVec_t lowestHealthNormalVec;
        std::copy_if(
            tempPVec.begin(),
            tempPVec.end(),
            back_inserter(lowestHealthNormalVec),
            [LOWEST_HEALTH_NORMAL](const auto & PTR) {
                return PTR->HealthNormal() == LOWEST_HEALTH_NORMAL;
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
                [](const auto & A_PTR, const auto & B_PTR) {
                    return A_PTR->HealthCurrent() < B_PTR->HealthCurrent();
                });
        }
    }

    const CreaturePVec_t Algorithms::FindLowestHealth(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
            return CreaturePVec_t();

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealth(tempPVec);
        const auto LOWEST_HEALTH_CURR(tempPVec[0]->HealthCurrent());

        CreaturePVec_t lowestHealthCurrVec;
        std::copy_if(
            tempPVec.begin(),
            tempPVec.end(),
            back_inserter(lowestHealthCurrVec),
            [LOWEST_HEALTH_CURR](const auto & PTR) {
                return PTR->HealthCurrent() == LOWEST_HEALTH_CURR;
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
                [](const auto & A_PTR, const auto & B_PTR) {
                    return A_PTR->HealthRatio() < B_PTR->HealthRatio();
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
        const auto LOWEST_HEALTH_RATIO(tempPVec[0]->HealthRatio());

        CreaturePVec_t lowestHealthRatioVec;
        std::copy_if(
            tempPVec.begin(),
            tempPVec.end(),
            back_inserter(lowestHealthRatioVec),
            [LOWEST_HEALTH_RATIO](const auto & PTR) {
                return misc::IsRealClose(PTR->HealthRatio(), LOWEST_HEALTH_RATIO);
            });

        return lowestHealthRatioVec;
    }

    const CreaturePVec_t Algorithms::FindByConditionMeaningNotAThreatPermenantly(
        const CreaturePVec_t & CREATURE_PVEC,
        const CondSingleOpt CONDITION_OPTION,
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
            [&](const auto & PTR) {
                return PTR->HasConditionNotAThreatPerm(UNCONSCIOUS_OPTION)
                    == (CONDITION_OPTION == CondSingleOpt::Has);
            });

        return creaturesThatAreStillAThreatVec;
    }

    const CreaturePVec_t Algorithms::FindByConditionMeaningNotAThreatTemporarily(
        const CreaturePVec_t & CREATURE_PVEC, const CondSingleOpt CONDITION_OPTION)
    {
        CreaturePVec_t creaturesWithCondsButAreStillAThreatVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(creaturesWithCondsButAreStillAThreatVec),
                [CONDITION_OPTION](const auto & PTR) {
                    return PTR->HasConditionNotAThreatTemp()
                        == (CONDITION_OPTION == CondSingleOpt::Has);
                });
        }

        return creaturesWithCondsButAreStillAThreatVec;
    }

    const CreaturePVec_t Algorithms::FindByRace(
        const CreaturePVec_t & CREATURE_PVEC,
        const race::Enum RACE_ENUM,
        const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t byRacePVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byRacePVec),
                [RACE_ENUM, CRITERIA_OPTION](const auto & PTR) {
                    return (PTR->Race() == RACE_ENUM) == (CRITERIA_OPTION == CriteriaOpt::Meets);
                });
        }

        return byRacePVec;
    }

    const CreaturePVec_t Algorithms::FindByRole(
        const CreaturePVec_t & CREATURE_PVEC,
        const role::Enum ROLE_ENUM,
        const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t byRolePVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byRolePVec),
                [ROLE_ENUM, CRITERIA_OPTION](const auto & PTR) {
                    return (PTR->Role() == ROLE_ENUM) == (CRITERIA_OPTION == CriteriaOpt::Meets);
                });
        }

        return byRolePVec;
    }

    const CreaturePVec_t Algorithms::FindByCanCast(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t canCastPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(canCastPVec),
                [CRITERIA_OPTION](const auto & PTR) {
                    return (PTR->CanCastSpells() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return canCastPVec;
    }

    const CreaturePVec_t Algorithms::FindByIsBeast(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t byBeastPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byBeastPVec),
                [CRITERIA_OPTION](const auto & PTR) {
                    return (PTR->IsBeast() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return byBeastPVec;
    }

    const CreaturePVec_t Algorithms::FindByIsHoldingProjWeapon(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t byProjectileWeaponPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(byProjectileWeaponPVec),
                [CRITERIA_OPTION](const auto & PTR) {
                    return (
                        PTR->IsHoldingProjectileWeapon()
                        == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return byProjectileWeaponPVec;
    }

    const CreaturePVec_t Algorithms::FindByCanFly(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t canFlyPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(canFlyPVec),
                [CRITERIA_OPTION](const auto & PTR) {
                    return (PTR->CanFly() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return canFlyPVec;
    }

    const CreaturePVec_t Algorithms::FindByIsFlying(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t isFlyinfPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(isFlyinfPVec),
                [CRITERIA_OPTION](const auto & PTR) {
                    return (
                        combat::Encounter::Instance()->GetTurnInfoCopy(PTR).GetIsFlying()
                        == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return isFlyinfPVec;
    }

    const CreaturePVec_t Algorithms::FindByCanTakeAction(
        const CreaturePVec_t & CREATURE_PVEC, const CriteriaOpt CRITERIA_OPTION)
    {
        CreaturePVec_t canTakeActionPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(canTakeActionPVec),
                [CRITERIA_OPTION](const auto & PTR) {
                    return (PTR->CanTakeAction() == (CRITERIA_OPTION == CriteriaOpt::Meets));
                });
        }

        return canTakeActionPVec;
    }

    const CreaturePVec_t Algorithms::FindByHasCondition(
        const CreaturePVec_t & CREATURE_PVEC,
        const Conditions::Enum COND_ENUM,
        const CondAllOpt COND_ALL_OPTION,
        const CondSingleOpt COND_SINGLE_OPTION)
    {
        return FindByHasConditions(
            CREATURE_PVEC, CondEnumVec_t(1, COND_ENUM), COND_ALL_OPTION, COND_SINGLE_OPTION);
    }

    const CreaturePVec_t Algorithms::FindByHasConditions(
        const CreaturePVec_t & CREATURE_PVEC,
        const CondEnumVec_t & CONDS_VEC,
        const CondAllOpt COND_ALL_OPTION,
        const CondSingleOpt COND_SINGLE_OPTION)
    {
        CreaturePVec_t havingConditionPVec;

        if (CREATURE_PVEC.empty() == false)
        {
            std::copy_if(
                CREATURE_PVEC.begin(),
                CREATURE_PVEC.end(),
                back_inserter(havingConditionPVec),
                [&](const auto & PTR) {
                    if (COND_ALL_OPTION == CondAllOpt::HasAll)
                    {
                        auto hasAll { true };
                        for (const auto NEXT_COND_ENUM : CONDS_VEC)
                        {
                            if (PTR->HasCondition(NEXT_COND_ENUM)
                                != (COND_SINGLE_OPTION == CondSingleOpt::Has))
                            {
                                hasAll = false;
                                break;
                            }
                        }
                        return hasAll;
                    }
                    else
                    {
                        for (const auto NEXT_COND_ENUM : CONDS_VEC)
                        {
                            if (PTR->HasCondition(NEXT_COND_ENUM)
                                == (COND_SINGLE_OPTION == CondSingleOpt::Has))
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

} // namespace creature
} // namespace heroespath
