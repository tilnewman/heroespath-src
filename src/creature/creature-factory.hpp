// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_CREATURE_FACTORY_HPP_INCLUDED
#define HEROESPATH_CREATURE_CREATURE_FACTORY_HPP_INCLUDED
//
// creature-factory.hpp
//
#include "creature/dragon-class-enum.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "stats/stat-set.hpp"
#include "stats/trait.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;

    // Creates sets of creatures that are enemies, not player characters.
    class CreatureFactory
    {
    public:
        CreatureFactory(const CreatureFactory &) = delete;
        CreatureFactory(CreatureFactory &&) = delete;
        CreatureFactory & operator=(const CreatureFactory &) = delete;
        CreatureFactory & operator=(CreatureFactory &&) = delete;
        CreatureFactory() = delete;

        static const CreaturePtr_t MakeDefaultForDeserialization();

        static void MakeEquipSaveAndFreePlayer(
            const std::string & NAME,
            const sex::Enum SEX,
            const race::Enum & RACE,
            const role::Enum & ROLE,
            const stats::StatSet & STATS,
            const std::string & IMAGE_FILENAME);

        static const CreaturePtr_t MakeAndEquipPlayerForTesting(
            const std::string & NAME,
            const race::Enum & RACE,
            const role::Enum & ROLE,
            const stats::StatSet & STATS);

        static const CreaturePtr_t MakeAndEquipEnemy(
            const sex::Enum SEX,
            const race::Enum RACE,
            const role::Enum ROLE,
            const stats::StatSet & STATS,
            const Health_t & HEALTH,
            const Rank_t & RANK = 1_rank,
            const Experience_t & EXPERIENCE = 0_exp,
            const Mana_t & MANA = 0_mana);

        static const CreaturePVec_t MakeFirstEncounterEnemies();

    private:
        static const CreaturePtr_t MakePlayerAndEquip(
            const std::string & NAME,
            const sex::Enum SEX,
            const race::Enum & RACE,
            const role::Enum & ROLE,
            const stats::StatSet & STATS,
            const std::string & IMAGE_FILENAME);

        static const CreaturePtr_t MakeGoblinGrunt();
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CREATURE_FACTORY_HPP_INCLUDED
