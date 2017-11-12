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

#include "stats/stat-set.hpp"
#include "non-player/party.hpp"
#include "non-player/character.hpp"
#include "non-player/inventory-factory.hpp"

#include "sfml-util/gui/creature-image-manager.hpp"

#include "misc/random.hpp"


namespace heroespath
{
namespace combat
{

    std::unique_ptr<PartyFactory> PartyFactory::instanceUPtr_{ nullptr };


    PartyFactory::PartyFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: PartyFactory");
    }


    PartyFactory::~PartyFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: PartyFactory");
    }


    PartyFactory * PartyFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): PartyFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void PartyFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<PartyFactory>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: PartyFactory");
        }
    }


    void PartyFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "PartyFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    non_player::PartyPtr_t PartyFactory::MakeParty_FirstEncounter() const
    {
        auto partyPtr{ new non_player::Party() };

        for (std::size_t i(0); i < 10; ++i)
        {
            partyPtr->Add( MakeCharacter_GoblinGrunt() );
        }

        return partyPtr;
    }


    non_player::CharacterPtr_t PartyFactory::MakeCharacter_GoblinGrunt() const
    {
        const stats::StatSet STATS(
            Strength_t(13 + misc::random::Int(5)),
            Accuracy_t(13 + misc::random::Int(5)),
            Charm_t   ( 5 + misc::random::Int(5)),
            Luck_t    ( 5 + misc::random::Int(5)),
            Speed_t   (13 + misc::random::Int(5)),
            Intell_t  ( 3 + misc::random::Int(5)) );

        auto characterPtr{ MakeCharacter(
            STATS,
            10_health, 20_health,
            ((misc::random::Int(100) < 75) ? creature::sex::Male : creature::sex::Female),
            creature::race::Goblin,
            creature::role::Grunt) };

        non_player::ownership::InventoryFactory::Instance()->
            SetupCreatureInventory(characterPtr);

        return characterPtr;
    }


    non_player::CharacterPtr_t PartyFactory::MakeCharacter_Boar() const
    {
        const stats::StatSet STATS(
            Strength_t(13 + misc::random::Int(3)),
            Accuracy_t(10 + misc::random::Int(2)),
            Charm_t   (4  + misc::random::Int(2)),
            Luck_t    (5  + misc::random::Int(5)),
            Speed_t   (13 + misc::random::Int(5)),
            Intell_t  (1) );

        return MakeCharacter(
            STATS,
            13_health, 18_health,
            ((misc::random::Bool()) ? creature::sex::Male : creature::sex::Female),
            creature::race::Boar,
            creature::role::Boar);
    }


    non_player::CharacterPtr_t PartyFactory::MakeCharacter(
        const stats::StatSet & STATS,
        const Health_t & HEALTH_MIN,
        const Health_t & HEALTH_MAX,
        const creature::sex::Enum SEX,
        const creature::race::Enum RACE,
        const creature::role::Enum ROLE,
        const Rank_t & RANK,
        const Experience_t & EXPERIENCE,
        const Mana_t & MANA) const
    {
        auto characterPtr{ new non_player::Character(
            creature::race::Name(RACE),
            SEX,
            creature::BodyType::Make_FromRaceAndRole(RACE, ROLE),
            RACE,
            ROLE,
            STATS,
            Health_t(misc::random::Int(HEALTH_MIN.AsInt(), HEALTH_MAX.AsInt())),
            RANK,
            EXPERIENCE,
            creature::CondEnumVec_t(),
            creature::TitleEnumVec_t(),
            item::Inventory(),
            sfml_util::DateTime(),
            "",
            spell::SpellVec_t(),
            MANA) };

        characterPtr->ImageFilename(
            sfml_util::gui::CreatureImageManager::Instance()->GetFilename(
                RACE,
                ROLE,
                SEX,
                true,
                characterPtr->WolfenClass(),
                characterPtr->DragonClass()) );

        return characterPtr;
    }

}
}
