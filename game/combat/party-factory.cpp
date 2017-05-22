//
// PartyFactory.cpp
//
#include "party-factory.hpp"

#include "utilz/random.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"

#include "game/stats/stat-set.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/inventory-factory.hpp"


namespace game
{
namespace combat
{

    PartyFactorySPtr_t PartyFactory::instance_(nullptr);


    PartyFactory::PartyFactory()
    {}


    PartyFactory::~PartyFactory()
    {}


    PartyFactorySPtr_t PartyFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new PartyFactory );

        return instance_;
    }


    non_player::PartySPtr_t PartyFactory::MakeParty_FirstEncounter() const
    {
        non_player::PartySPtr_t partySPtr( new non_player::Party() );
        for(std::size_t i(0); i<10; ++i)
            partySPtr->Add( MakeCreature_GoblinGrunt() );

        return partySPtr;
    }


    non_player::CharacterSPtr_t PartyFactory::MakeCreature_GoblinGrunt() const
    {
        const stats::StatSet GOBLIN_STATS( 7 + utilz::random::Int(5),  //str
                                           5 + utilz::random::Int(7),  //acc
                                           5 + utilz::random::Int(7),  //cha
                                           5 + utilz::random::Int(7),  //lck
                                           5 + utilz::random::Int(7),  //spd
                                           3 + utilz::random::Int(5) );//int

        const stats::Health_t GOBLIN_HEALTH(7 + utilz::random::Int(5));
        const creature::sex::Enum GOBLIN_SEX((utilz::random::Int(100) < 75) ? creature::sex::Male : creature::sex::Female);

        non_player::CharacterSPtr_t goblinGruntSPtr( new non_player::Character(creature::race::Name(creature::race::Goblin),
                                                                               GOBLIN_SEX,
                                                                               creature::BodyType::Make_FromRaceAndRole(creature::race::Goblin, creature::role::Grunt),
                                                                               creature::Race(creature::race::Goblin),
                                                                               creature::Role(creature::role::Grunt),
                                                                               GOBLIN_STATS,
                                                                               GOBLIN_HEALTH) );

        goblinGruntSPtr->ImageFilename( sfml_util::gui::CreatureImageManager::Instance()->GetFilename(creature::race::Goblin,
                                                                                                      creature::role::Grunt,
                                                                                                      GOBLIN_SEX,
                                                                                                      true) );

        non_player::ownership::InventoryFactory::Instance()->PopulateCreatureInventory(goblinGruntSPtr);
        goblinGruntSPtr->SetCurrentWeaponsToBest();
        return goblinGruntSPtr;
    }

}
}
