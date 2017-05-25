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
// PartyFactory.cpp
//
#include "party-factory.hpp"

#include "game/stats/stat-set.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/inventory-factory.hpp"

#include "sfml-util/gui/creature-image-manager.hpp"

#include "misc/random.hpp"


namespace game
{
namespace combat
{

    std::unique_ptr<PartyFactory> PartyFactory::instanceUPtr_{ nullptr };


    PartyFactory::PartyFactory()
    {}


    PartyFactory * PartyFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void PartyFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new PartyFactory);
        }
    }


    void PartyFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::PartyFactory::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
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
        const stats::StatSet GOBLIN_STATS( 7 + misc::random::Int(5),  //str
                                           5 + misc::random::Int(7),  //acc
                                           5 + misc::random::Int(7),  //cha
                                           5 + misc::random::Int(7),  //lck
                                           5 + misc::random::Int(7),  //spd
                                           3 + misc::random::Int(5) );//int

        const stats::Health_t GOBLIN_HEALTH(7 + misc::random::Int(5));
        const creature::sex::Enum GOBLIN_SEX((misc::random::Int(100) < 75) ? creature::sex::Male : creature::sex::Female);

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
