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
// creature-factory.cpp
//
#include "creature-factory.hpp"

#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "misc/random.hpp"
#include "non-player/inventory-factory.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "stats/stat-set.hpp"

#include <memory>

namespace heroespath
{
namespace creature
{

    const CreaturePVec_t CreatureFactory::Make_FirstEncounter() const
    {
        CreaturePVec_t creaturesPVec;

        for (std::size_t i(0); i < 10; ++i)
        {
            creaturesPVec.emplace_back(Make_GoblinGrunt());
        }

        return creaturesPVec;
    }

    const CreaturePtr_t CreatureFactory::Make_GoblinGrunt() const
    {
        const stats::StatSet STATS(
            Strength_t(13 + misc::random::Int(5)),
            Accuracy_t(13 + misc::random::Int(5)),
            Charm_t(5 + misc::random::Int(5)),
            Luck_t(5 + misc::random::Int(5)),
            Speed_t(13 + misc::random::Int(5)),
            Intell_t(3 + misc::random::Int(5)));

        auto const CHARACTER_PTR{ Make(
            STATS,
            10_health,
            20_health,
            ((misc::random::Int(100) < 75) ? sex::Male : sex::Female),
            race::Goblin,
            role::Grunt) };

        non_player::ownership::InventoryFactory::Instance()->SetupCreatureInventory(CHARACTER_PTR);

        return CHARACTER_PTR;
    }

    const CreaturePtr_t CreatureFactory::Make_Boar() const
    {
        const stats::StatSet STATS(
            Strength_t(13 + misc::random::Int(3)),
            Accuracy_t(10 + misc::random::Int(2)),
            Charm_t(4 + misc::random::Int(2)),
            Luck_t(5 + misc::random::Int(5)),
            Speed_t(13 + misc::random::Int(5)),
            Intell_t(1));

        return Make(
            STATS,
            13_health,
            18_health,
            ((misc::random::Bool()) ? sex::Male : sex::Female),
            race::Boar,
            role::Boar);
    }

    const CreaturePtr_t CreatureFactory::Make(
        const stats::StatSet & STATS,
        const Health_t & HEALTH_MIN,
        const Health_t & HEALTH_MAX,
        const sex::Enum SEX,
        const race::Enum RACE,
        const role::Enum ROLE,
        const Rank_t & RANK,
        const Experience_t & EXPERIENCE,
        const Mana_t & MANA) const
    {
        auto const CHARACTER_PTR{ CreatureWarehouse::Access().Store(std::make_unique<Creature>(
            false,
            race::Name(RACE),
            SEX,
            BodyType::Make_FromRaceAndRole(RACE, ROLE),
            RACE,
            ROLE,
            STATS,
            Health_t(misc::random::Int(HEALTH_MIN.As<int>(), HEALTH_MAX.As<int>())),
            RANK,
            EXPERIENCE,
            CondEnumVec_t(),
            TitleEnumVec_t(),
            item::Inventory(),
            sfml_util::DateTime(),
            "",
            spell::SpellEnumVec_t(),
            MANA)) };

        CHARACTER_PTR->ImageFilename(sfml_util::gui::CreatureImageManager::Instance()->GetFilename(
            RACE, ROLE, SEX, true, CHARACTER_PTR->WolfenClass(), CHARACTER_PTR->DragonClass()));

        return CHARACTER_PTR;
    }

} // namespace creature
} // namespace heroespath
