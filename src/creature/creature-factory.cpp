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
//#include "creature/nonplayer-inventory-factory.hpp"
#include "creature/player-initial-setup.hpp"
#include "creature/stat-set.hpp"
#include "game/game-state-factory.hpp"
#include "misc/random.hpp"

#include <memory>

namespace heroespath
{
namespace creature
{

    const CreaturePtr_t CreatureFactory::MakeDefaultForDeserialization() const
    {
        return CreatureWarehouse::Access().Store(std::make_unique<Creature>());
    }

    void CreatureFactory::MakeEquipSaveAndFreePlayer(
        const std::string & NAME,
        const sex::Enum SEX,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const StatSet & STATS,
        const std::string & IMAGE_FILENAME) const
    {
        const auto CHARACTER_PTR { MakeAndEquipPlayer(
            NAME, SEX, RACE, ROLE, STATS, IMAGE_FILENAME) };

        game::GameStateFactory::Instance()->SaveCharacter(CHARACTER_PTR);
        creature::CreatureWarehouse::Access().Free(CHARACTER_PTR);
    }

    const CreaturePtr_t CreatureFactory::MakeAndEquipPlayerForTesting(
        const std::string & NAME,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const StatSet & STATS) const
    {
        return MakeAndEquipPlayer(
            NAME,
            ((misc::RandomBool()) ? creature::sex::Female : creature::sex::Male),
            RACE,
            ROLE,
            STATS,
            "");
    }

    const CreaturePtr_t CreatureFactory::MakeAndEquipEnemy(
        const sex::Enum SEX,
        const race::Enum RACE,
        const role::Enum ROLE,
        const StatSet & STATS,
        const Health_t & HEALTH,
        const Rank_t & RANK,
        const Experience_t & EXPERIENCE,
        const Mana_t & MANA) const
    {
        const auto VALID_ROLES { race::Roles(RACE) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (std::find(std::begin(VALID_ROLES), std::end(VALID_ROLES), ROLE)
             != std::end(VALID_ROLES)),
            "creature::CreatureFactory::MakeAndEquipEnemy(sex="
                << NAMEOF_ENUM(SEX) << ", race=" << NAMEOF_ENUM(RACE)
                << ", role=" << NAMEOF_ENUM(ROLE) << ", stats=" << STATS.ToString(true)
                << ", health=" << HEALTH << ", rank=" << RANK << ", experience=" << EXPERIENCE
                << ") but that race/role combination is invalid.");

        const auto CREATURE_PTR { CreatureWarehouse::Access().Store(std::make_unique<Creature>(
            false, race::Name(RACE), SEX, RACE, ROLE, STATS, "", HEALTH, RANK, EXPERIENCE, MANA)) };

        // TODO PUT BACK AFTER re-tooling
        // creature::nonplayer::InventoryFactory inventoryFactory;
        // inventoryFactory.SetupCreatureInventory(CREATURE_PTR);
        return CREATURE_PTR;
    }

    const CreaturePVec_t CreatureFactory::MakeFirstEncounterEnemies() const
    {
        CreaturePVec_t creaturesPVec;

        for (std::size_t i(0); i < 10; ++i)
        {
            creaturesPVec.emplace_back(MakeGoblinGrunt());
        }

        return creaturesPVec;
    }

    const CreaturePtr_t CreatureFactory::MakeAndEquipPlayer(
        const std::string & NAME,
        const sex::Enum SEX,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const StatSet & STATS,
        const std::string & IMAGE_FILENAME) const
    {
        const auto CHARACTER_PTR { CreatureWarehouse::Access().Store(
            std::make_unique<Creature>(true, NAME, SEX, RACE, ROLE, STATS, IMAGE_FILENAME)) };

        PlayerInitialSetup::Setup(CHARACTER_PTR);
        return CHARACTER_PTR;
    }

    const CreaturePtr_t CreatureFactory::MakeGoblinGrunt() const
    {
        const StatSet STATS(
            Strength_t::Make(13 + misc::Random(5)),
            Accuracy_t::Make(13 + misc::Random(5)),
            Charm_t::Make(5 + misc::Random(5)),
            Luck_t::Make(5 + misc::Random(5)),
            Speed_t::Make(13 + misc::Random(5)),
            Intel_t::Make(3 + misc::Random(5)));

        return MakeAndEquipEnemy(
            ((misc::Random(100) < 75) ? sex::Male : sex::Female),
            race::Goblin,
            role::Grunt,
            STATS,
            Health_t::Make(misc::Random(10, 20)));
    }

} // namespace creature
} // namespace heroespath
