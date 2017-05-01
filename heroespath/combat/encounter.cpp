//
// encounter.cpp
//
#include "encounter.hpp"

#include "sfml-util/sound-manager.hpp"
#include "sfml-util/music-enum.hpp"

#include "heroespath/game.hpp"
#include "heroespath/log-macros.hpp"
#include "heroespath/player/party.hpp"
#include "heroespath/player/character.hpp"
#include "heroespath/non-player/party.hpp"
#include "heroespath/non-player/character.hpp"
#include "heroespath/combat/strategy-details.hpp"
#include "heroespath/combat/party-factory.hpp"
#include "heroespath/state/game-state.hpp"
#include "heroespath/state/world-state.hpp"
#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/creature/algorithms.hpp"

#include "utilz/vector-operations.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace combat
{

    EncounterSPtr_t Encounter::instance_(nullptr);


    Encounter::Encounter()
    :
        enemyPartySPtr_    (),
        deadEnemyPartySPtr_(std::make_shared<non_player::Party>()),
        roundCounter_      (0),
        hasStarted_        (false),
        turnOverPVec_      (),
        turnIndex_         (0),
        turnInfoMap_       (),
        turnCreaturePtr_   ()
    {}


    Encounter::~Encounter()
    {}


    EncounterSPtr_t Encounter::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset(new Encounter);

        return instance_;
    }


    void Encounter::Setup_First()
    {
        enemyPartySPtr_ = PartyFactory::Instance()->MakeParty_FirstEncounter();
    }


    const TurnInfo Encounter::GetTurnInfoCopy(const creature::CreaturePtr_t P) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((P != nullptr), "heroespath::combat::Encounter::GetTurnInfoCopy() was given a nullptr creature::CreaturePtr_t.");

        auto const FOUND_ITER{ turnInfoMap_.find(P) };

        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER != turnInfoMap_.end()), "heroespath::combat::Encounter::GetTurnInfoCopy(creature::CreaturePtr_t=" << P->Name() << ") was not found in the turnInfoMap_.");

        return FOUND_ITER->second;
    }


    void Encounter::StartTasks()
    {
        roundCounter_ = 0;
        Game::Instance()->State()->World()->EncoundterCountInc();

        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::CombatIntro);

        if (Game::Instance()->State()->IsNewGame())
            GenerateFirstEncounter();

        PopulateTurnInfoMap();
        SortAndSetTurnCreature();
        hasStarted_ = true;
    }


    void Encounter::EndTasks()
    {
        turnOverPVec_.clear();
        turnInfoMap_.clear();
        enemyPartySPtr_.reset();
        roundCounter_ = 0;
        hasStarted_ = false;
        turnIndex_ = 0;
    }


    void Encounter::HandleKilledCreature(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        //make sure no TurnInfo objects refer to a killed creature
        for (auto & nextCreatureTurnInfoPair : turnInfoMap_)
            nextCreatureTurnInfoPair.second.RemoveDeadCreatureTasks(CREATURE_CPTRC);

        //no need to remove from turnOverPVec_

        //no need to null out turnCreaturePtr_

        //move from the enemyParty to the deadEnemyParty for later loot collection
        auto const KILLED_NONPLAYER_CHARACTER_SPTR{ enemyPartySPtr_->FindByCreaturePtr(CREATURE_CPTRC) };
        enemyPartySPtr_->Remove(KILLED_NONPLAYER_CHARACTER_SPTR);
        deadEnemyPartySPtr_->Add(KILLED_NONPLAYER_CHARACTER_SPTR);
    }


    void Encounter::IncrementTurn()
    {
        ++roundCounter_;

        if (turnCreaturePtr_ != nullptr)
            turnOverPVec_.push_back(turnCreaturePtr_);

        //re-sort by speed every turn because spells/conditions/etc could have changed creature speed during any given turn
        SortAndSetTurnCreature();
    }


    void Encounter::GenerateFirstEncounter()
    {
        combat::Encounter::Instance()->Setup_First();
    }


    void Encounter::PopulateTurnInfoMap()
    {
        turnInfoMap_.clear();

        for (auto const & NEXT_CHAR_SPTR : enemyPartySPtr_->Characters())
        {
            //enemy creatures need a real populated strategy info object
            TurnInfo turnInfo;
            turnInfo.SetStrategyInfo( strategy::ChanceFactory::Instance()->Get(NEXT_CHAR_SPTR->Race().Which(), NEXT_CHAR_SPTR->Role().Which()).Make() );
            turnInfoMap_[NEXT_CHAR_SPTR.get()] = turnInfo;
        }

        for (auto const & NEXT_CHAR_SPTR : Game::Instance()->State()->Party()->Characters())
            turnInfoMap_[NEXT_CHAR_SPTR.get()] = TurnInfo();
    }


    void Encounter::SortAndSetTurnCreature()
    {
        creature::CreaturePVec_t allLivingCreaturesPVec;
        utilz::VectorOps::Append(creature::Algorithms::Players(true), allLivingCreaturesPVec);
        utilz::VectorOps::Append(creature::Algorithms::NonPlayers(true), allLivingCreaturesPVec);

        if (turnOverPVec_.size() >= allLivingCreaturesPVec.size())
        {
            turnOverPVec_.clear();
        }

        creature::CreaturePVec_t creaturesThatHaveNotTakenTurnYetPVec(utilz::VectorOps::Exclude(allLivingCreaturesPVec, turnOverPVec_));

        M_ASSERT_OR_LOGANDTHROW_SS((creaturesThatHaveNotTakenTurnYetPVec.empty() == false), "heroespath::combat::Encounter::SortAndSetTurnCreature(" << ((turnCreaturePtr_ == nullptr) ? "nullptr" : turnCreaturePtr_->Name()) << ") resulted in an empty creaturesThatHaveNotTakenTurnYetPVec.");

        if (creaturesThatHaveNotTakenTurnYetPVec.size() > 1)
            std::sort(creaturesThatHaveNotTakenTurnYetPVec.begin(),
                      creaturesThatHaveNotTakenTurnYetPVec.end(),
                      [](const creature::CreaturePtr_t A, const creature::CreaturePtr_t B) { return A->Stats().Spd().Current() > B->Stats().Spd().Current(); });

        turnCreaturePtr_ = creaturesThatHaveNotTakenTurnYetPVec.at(0);

        M_ASSERT_OR_LOGANDTHROW_SS((turnCreaturePtr_ != nullptr), "heroespath::combat::Encounter::SortAndSetTurnCreature(" << ((turnCreaturePtr_ == nullptr) ? "nullptr" : turnCreaturePtr_->Name()) << ") resulted in a nullptr turnCreaturePtr_.");
    }

}
}
