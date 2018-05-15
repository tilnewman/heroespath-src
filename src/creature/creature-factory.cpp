// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// creature-factory.cpp
//
#include "creature-factory.hpp"

#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "misc/random.hpp"
#include "non-player/inventory-factory.hpp"
#include "player/initial.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "state/game-state-factory.hpp"
#include "stats/stat-set.hpp"

#include <memory>

namespace heroespath
{
namespace creature
{

    const CreaturePtr_t CreatureFactory::MakeDefaultForDeserialization()
    {
        return CreatureWarehouse::Access().Store(std::make_unique<Creature>());
    }

    void CreatureFactory::MakeEquipSaveAndFreePlayer(
        const std::string & NAME,
        const sex::Enum SEX,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const stats::StatSet & STATS,
        const std::string & IMAGE_FILENAME)
    {
        auto const CHARACTER_PTR{ MakePlayerAndEquip(
            NAME, SEX, RACE, ROLE, STATS, IMAGE_FILENAME) };

        state::GameStateFactory::Instance()->SaveCharacter(CHARACTER_PTR);
        creature::CreatureWarehouse::Access().Free(CHARACTER_PTR);
    }

    const CreaturePtr_t CreatureFactory::MakeAndEquipPlayerForTesting(
        const std::string & NAME,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const stats::StatSet & STATS)
    {
        return MakePlayerAndEquip(
            NAME,
            ((misc::random::Bool()) ? creature::sex::Female : creature::sex::Male),
            RACE,
            ROLE,
            STATS,
            "");
    }

    const CreaturePtr_t CreatureFactory::MakeAndEquipEnemy(
        const sex::Enum SEX,
        const race::Enum RACE,
        const role::Enum ROLE,
        const stats::StatSet & STATS,
        const Health_t & HEALTH,
        const Rank_t & RANK,
        const Experience_t & EXPERIENCE,
        const Mana_t & MANA)
    {
        auto const VALID_ROLES{ race::Roles(RACE) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (std::find(std::begin(VALID_ROLES), std::end(VALID_ROLES), ROLE)
             != std::end(VALID_ROLES)),
            "creature::CreatureFactory::MakeAndEquipEnemy(sex="
                << creature::sex::ToString(SEX) << ", race=" << creature::race::ToString(RACE)
                << ", role=" << creature::role::ToString(ROLE) << ", stats=" << STATS.ToString(true)
                << ", health=" << HEALTH << ", rank=" << RANK << ", experience=" << EXPERIENCE
                << ") but that race/role combination is invalid.");

        auto const CREATURE_PTR{ CreatureWarehouse::Access().Store(std::make_unique<Creature>(
            false, race::Name(RACE), SEX, RACE, ROLE, STATS, "", HEALTH, RANK, EXPERIENCE, MANA)) };

        non_player::ownership::InventoryFactory::SetupCreatureInventory(CREATURE_PTR);
        return CREATURE_PTR;
    }

    const CreaturePVec_t CreatureFactory::MakeFirstEncounterEnemies()
    {
        CreaturePVec_t creaturesPVec;

        for (std::size_t i(0); i < 10; ++i)
        {
            creaturesPVec.emplace_back(MakeGoblinGrunt());
        }

        return creaturesPVec;
    }

    const CreaturePtr_t CreatureFactory::MakePlayerAndEquip(
        const std::string & NAME,
        const sex::Enum SEX,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const stats::StatSet & STATS,
        const std::string & IMAGE_FILENAME)
    {
        auto const CHARACTER_PTR{ CreatureWarehouse::Access().Store(
            std::make_unique<Creature>(true, NAME, SEX, RACE, ROLE, STATS, IMAGE_FILENAME)) };

        player::Initial::Setup(CHARACTER_PTR);
        return CHARACTER_PTR;
    }

    const CreaturePtr_t CreatureFactory::MakeGoblinGrunt()
    {
        const stats::StatSet STATS(
            Strength_t(13 + misc::random::Int(5)),
            Accuracy_t(13 + misc::random::Int(5)),
            Charm_t(5 + misc::random::Int(5)),
            Luck_t(5 + misc::random::Int(5)),
            Speed_t(13 + misc::random::Int(5)),
            Intell_t(3 + misc::random::Int(5)));

        return MakeAndEquipEnemy(
            ((misc::random::Int(100) < 75) ? sex::Male : sex::Female),
            race::Goblin,
            role::Grunt,
            STATS,
            Health_t(misc::random::Int(10, 20)));
    }

} // namespace creature
} // namespace heroespath
