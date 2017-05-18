//
// algorithms.cpp
//
#include "algorithms.hpp"

#include "game/game.hpp"
#include "game/creature/creature.hpp"
#include "game/combat/encounter.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/party.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/state/game-state.hpp"

#include "utilz/random.hpp"
#include "utilz/vectors.hpp"

#include <sstream>


namespace game
{
namespace creature
{

    const CreaturePVec_t Algorithms::MakeIntoVector(const CreaturePtr_t CREATURE_PTR)
    {
        return CreaturePVec_t{ CREATURE_PTR };
    }


    std::size_t Algorithms::Players(CreaturePVec_t & pVec_OutParam, const bool LIVING_ONLY)
    {
        const player::CharacterSVec_t PLAYERS_SVEC(Game::Instance()->State()->Party()->Characters());

        CreaturePVec_t playersPVec;
        for (auto const & NEXT_PLAYER_SPTR : PLAYERS_SVEC)
            if ((LIVING_ONLY == false) || NEXT_PLAYER_SPTR->IsAlive())
                pVec_OutParam.push_back(NEXT_PLAYER_SPTR.get());

        return PLAYERS_SVEC.size();
    }


    const CreaturePVec_t Algorithms::Players(const bool LIVING_ONLY)
    {
        CreaturePVec_t tempPVec;
        Players(tempPVec, LIVING_ONLY);
        return tempPVec;
    }


    std::size_t Algorithms::NonPlayers(CreaturePVec_t & pVec_OutParam, const bool LIVING_ONLY)
    {
        game::non_player::CharacterSVec_t NONPLAYERS_SVEC(combat::Encounter::Instance()->NonPlayerParty()->Characters());

        CreaturePVec_t nonPlayersPVec;
        for (auto const & NEXT_NONPLAYER_SPTR : NONPLAYERS_SVEC)
            if ((LIVING_ONLY == false) || NEXT_NONPLAYER_SPTR->IsAlive())
                pVec_OutParam.push_back(NEXT_NONPLAYER_SPTR.get());

        return NONPLAYERS_SVEC.size();
    }


    const CreaturePVec_t Algorithms::NonPlayers(const bool LIVING_ONLY)
    {
        CreaturePVec_t tempPVec;
        NonPlayers(tempPVec, LIVING_ONLY);
        return tempPVec;
    }


    std::size_t Algorithms::PlayersByType(CreaturePVec_t & pVec_OutParam, const bool WILL_FIND_PLAYERS, const bool LIVING_ONLY)
    {
        if (WILL_FIND_PLAYERS)
        {
            return Players(pVec_OutParam, LIVING_ONLY);
        }
        else
        {
            return NonPlayers(pVec_OutParam, LIVING_ONLY);
        }
    }


    const CreaturePVec_t Algorithms::PlayersByType(const bool WILL_FIND_PLAYERS, const bool LIVING_ONLY)
    {
        if (WILL_FIND_PLAYERS)
        {
            return Players(LIVING_ONLY);
        }
        else
        {
            return NonPlayers(LIVING_ONLY);
        }
    }


    const std::string Algorithms::Names(const CreaturePVec_t & CREATURE_PVEC,
                                        const bool             WILL_WRAP,
                                        const bool             WILL_APPEND_AND,
                                        const bool             WILL_ADD_RACE_AND_ROLE,
                                        const std::size_t      MAX_COUNT,
                                        const bool             WILL_ELLIPSIS)
    {
        if (WILL_ADD_RACE_AND_ROLE)
        {
            return utilz::Vector::Join<CreaturePtr_t>(CREATURE_PVEC,
                                                      WILL_WRAP,
                                                      WILL_APPEND_AND,
                                                      MAX_COUNT,
                                                      WILL_ELLIPSIS,
                                                      [](const CreaturePtr_t CPTR) -> const std::string
                                                        {
                                                            return CPTR->NameAndRaceAndRole();
                                                        });
        }
        else
        {
            return utilz::Vector::Join<CreaturePtr_t>(CREATURE_PVEC,
                                                      WILL_WRAP,
                                                      WILL_APPEND_AND,
                                                      MAX_COUNT,
                                                      WILL_ELLIPSIS,
                                                      [](const CreaturePtr_t CPTR) -> const std::string
                                                        {
                                                            return CPTR->Name();
                                                        });
        }
    }


    const CreaturePVec_t Algorithms::FindByAlive(const CreaturePVec_t & CREATURE_PVEC, const bool WILL_FIND_ALIVE)
    {
        CreaturePVec_t tempPVec;

        if (CREATURE_PVEC.empty())
            return tempPVec;

        std::copy_if(CREATURE_PVEC.begin(),
                     CREATURE_PVEC.end(),
                     back_inserter(tempPVec),
                     [WILL_FIND_ALIVE] (const creature::CreaturePtr_t CPTRC) { return CPTRC->IsAlive() == WILL_FIND_ALIVE; });

        return tempPVec;
    }


    void Algorithms::SortByLowestHealthNormal(CreaturePVec_t & pVec_OutParam)
    {
        if (pVec_OutParam.empty() == false)
            std::sort(pVec_OutParam.begin(),
                      pVec_OutParam.end(),
                      [] (CreatureCPtrC_t A_CPTRC, CreatureCPtrC_t B_CPTRC) { return A_CPTRC->HealthNormal() < B_CPTRC->HealthNormal(); });
    }


    const CreaturePVec_t Algorithms::FindLowestHealthNormal(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
            return CreaturePVec_t();

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealthNormal(tempPVec);
        auto const LOWEST_HEALTH_NORMAL(tempPVec[0]->HealthNormal());

        CreaturePVec_t lowestHealthNormalVec;
        std::copy_if(tempPVec.begin(),
                     tempPVec.end(),
                     back_inserter(lowestHealthNormalVec),
                     [LOWEST_HEALTH_NORMAL] (CreatureCPtrC_t CPTRC) {return CPTRC->HealthNormal() == LOWEST_HEALTH_NORMAL; });

        return lowestHealthNormalVec;
    }


    void Algorithms::SortByLowestHealth(CreaturePVec_t & pVec_OutParam)
    {
        if (pVec_OutParam.empty() == false)
            std::sort(pVec_OutParam.begin(),
                      pVec_OutParam.end(),
                      [] (CreatureCPtrC_t A_CPTRC, CreatureCPtrC_t B_CPTRC) { return A_CPTRC->HealthCurrent() < B_CPTRC->HealthCurrent(); });
    }


    const CreaturePVec_t Algorithms::FindLowestHealth(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
            return CreaturePVec_t();

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealth(tempPVec);
        auto const LOWEST_HEALTH_CURR(tempPVec[0]->HealthCurrent());

        CreaturePVec_t lowestHealthCurrVec;
        std::copy_if(tempPVec.begin(),
                     tempPVec.end(),
                     back_inserter(lowestHealthCurrVec),
                     [LOWEST_HEALTH_CURR] (CreatureCPtrC_t CPTRC) { return CPTRC->HealthCurrent() == LOWEST_HEALTH_CURR; });

        return lowestHealthCurrVec;
    }


    void Algorithms::SortByLowestHealthRatio(CreaturePVec_t & pVec_OutParam)
    {
        if (pVec_OutParam.empty() == false)
            std::sort(pVec_OutParam.begin(),
                      pVec_OutParam.end(),
                      [] (CreatureCPtrC_t A_CPTRC, CreatureCPtrC_t B_CPTRC) { return A_CPTRC->HealthRatio() < B_CPTRC->HealthRatio(); });
    }


    const CreaturePVec_t Algorithms::FindLowestHealthRatio(const CreaturePVec_t & CREATURE_PVEC)
    {
        if (CREATURE_PVEC.empty())
            return CreaturePVec_t();

        CreaturePVec_t tempPVec(CREATURE_PVEC);
        SortByLowestHealthRatio(tempPVec);
        auto const LOWEST_HEALTH_RATIO(tempPVec[0]->HealthRatio());

        CreaturePVec_t lowestHealthRatioVec;
        std::copy_if(tempPVec.begin(),
                     tempPVec.end(),
                     back_inserter(lowestHealthRatioVec),
                     [LOWEST_HEALTH_RATIO] (CreatureCPtrC_t CPTRC) { return utilz::IsRealClose(CPTRC->HealthRatio(), LOWEST_HEALTH_RATIO); } );

        return lowestHealthRatioVec;
    }


    const CreaturePVec_t Algorithms::FindByConditionMeaningNotAThreatPermenantly(const CreaturePVec_t & CREATURE_PVEC, const bool HAS_CONDITION)
    {
        CreaturePVec_t creaturesThatAreStillAThreatVec;

        if (CREATURE_PVEC.empty())
            return creaturesThatAreStillAThreatVec;

        std::copy_if(CREATURE_PVEC.begin(),
                     CREATURE_PVEC.end(),
                     back_inserter(creaturesThatAreStillAThreatVec),
                     [HAS_CONDITION] (const CreaturePtr_t CCPTR) { return CCPTR->HasConditionNotAThreatPerm() == HAS_CONDITION; });

        return creaturesThatAreStillAThreatVec;
    }


    const CreaturePVec_t Algorithms::FindByConditionMeaningNotAThreatTemporarily(const CreaturePVec_t & CREATURE_PVEC, const bool HAS_CONDITION)
    {
        CreaturePVec_t creaturesWithCondsButAreStillAThreatVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(creaturesWithCondsButAreStillAThreatVec),
                         [HAS_CONDITION] (const CreatureCPtrC_t CPTRC) { return CPTRC->HasConditionNotAThreatTemp() == HAS_CONDITION; });

        return creaturesWithCondsButAreStillAThreatVec;
    }


    const CreaturePVec_t Algorithms::FindByRace(const CreaturePVec_t & CREATURE_PVEC, const race::Enum RACE_ENUM, const bool IS_GIVEN_RACE)
    {
        CreaturePVec_t byRacePVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(byRacePVec),
                         [RACE_ENUM, IS_GIVEN_RACE] (CreatureCPtrC_t CPTRC) { return IS_GIVEN_RACE == (CPTRC->Race().Which() == RACE_ENUM); });

        return byRacePVec;
    }


    const CreaturePVec_t Algorithms::FindByRole(const CreaturePVec_t & CREATURE_PVEC, const role::Enum ROLE_ENUM, const bool IS_GIVEN_ROLE)
    {
        CreaturePVec_t byRolePVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(byRolePVec),
                         [ROLE_ENUM, IS_GIVEN_ROLE] (CreatureCPtrC_t CPTRC) { return IS_GIVEN_ROLE == (CPTRC->Role().Which() == ROLE_ENUM); } );

        return byRolePVec;
    }


    const CreaturePVec_t Algorithms::FindBySpellCaster(const CreaturePVec_t & CREATURE_PVEC, const bool CAN_CAST)
    {
        CreaturePVec_t canCastPVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(canCastPVec),
                         [CAN_CAST] (CreatureCPtrC_t CPTRC) { return CAN_CAST == CPTRC->CanCastSpells(); } );

        return canCastPVec;
    }


    const CreaturePVec_t Algorithms::FindByBeast(const CreaturePVec_t & CREATURE_PVEC, const bool IS_BEAST)
    {
        CreaturePVec_t byBeastPVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(byBeastPVec),
                         [IS_BEAST] (CreatureCPtrC_t CPTRC) { return IS_BEAST == CPTRC->IsBeast(); } );

        return byBeastPVec;
    }


    const CreaturePVec_t Algorithms::FindByProjectileWeapons(const CreaturePVec_t & CREATURE_PVEC, const bool IS_HOLDING_PROJ_WEAPON)
    {
        CreaturePVec_t byProjectileWeaponPVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(byProjectileWeaponPVec),
                         [IS_HOLDING_PROJ_WEAPON] (CreatureCPtrC_t CPTRC) { return IS_HOLDING_PROJ_WEAPON == CPTRC->IsHoldingProjectileWeapon(); } );

        return byProjectileWeaponPVec;
    }


    const CreaturePVec_t Algorithms::FindByCanFly(const CreaturePVec_t & CREATURE_PVEC, const bool CAN_FLY)
    {
        CreaturePVec_t canFlyPVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(canFlyPVec),
                         [CAN_FLY] (CreatureCPtrC_t CPTRC) { return CAN_FLY == CPTRC->CanFly(); } );

        return canFlyPVec;
    }


    const CreaturePVec_t Algorithms::FindByFlying(const CreaturePVec_t & CREATURE_PVEC, const bool IS_FLYING)
    {
        CreaturePVec_t isFlyinfPVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(isFlyinfPVec),
                         [IS_FLYING] (const creature::CreaturePtr_t CPTRC) { return (combat::Encounter::Instance()->GetTurnInfoCopy(CPTRC).GetIsFlying() == IS_FLYING); });

        return isFlyinfPVec;
    }


    const CreaturePVec_t Algorithms::FindByCanTakeAction(const CreaturePVec_t & CREATURE_PVEC, const bool CAN_TAKE_ACTION)
    {
        CreaturePVec_t canTakeActionPVec;

        if (CREATURE_PVEC.empty() == false)
            std::copy_if(CREATURE_PVEC.begin(),
                         CREATURE_PVEC.end(),
                         back_inserter(canTakeActionPVec),
                         [CAN_TAKE_ACTION](const creature::CreaturePtr_t CPTRC) { return (CPTRC->CanTakeAction() == CAN_TAKE_ACTION); });

        return canTakeActionPVec;
    }

}
}
