// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// player-party-factory.cpp
//
#include "player-party-factory.hpp"

#include "creature/creature-factory.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "creature/player-initial-setup.hpp"
#include "creature/player-party.hpp"
#include "creature/stat-set.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <string>

namespace heroespath
{
namespace creature
{

    PlayerPartyUPtr_t PlayerPartyFactory::Make(
        const avatar::Avatar::Enum AVATAR, const creature::CreaturePVec_t & CHARACTERS) const
    {
        return std::make_unique<PlayerParty>(AVATAR, CHARACTERS);
    }

    PlayerPartyUPtr_t PlayerPartyFactory::MakeFakeForTesting() const
    {
        creature::CreatureFactory creatureFactory;

        creature::CreaturePVec_t creaturesPVec;

        const int STAT_BASE_HIGH{ 18 };
        const int STAT_BASE_MED{ 9 };
        const int STAT_BASE_LOW{ 5 };
        const int STAT_RAND{ 6 };

        /*
        const StatSet KNIGHT_STATS(
            STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("K"),
            creature::race::Human,
            creature::role::Knight,
            KNIGHT_STATS));
        */

        const StatSet FIREBRAND_STATS(
            Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("F"),
            creature::race::Dragon,
            creature::role::Firebrand,
            FIREBRAND_STATS));

        /*
        const StatSet ARCHER_STATS(
            15 + misc::random::Int(6),
            15 + misc::random::Int(10),
            5  + misc::random::Int(6),
            10 + misc::random::Int(6),
            10 + misc::random::Int(8),
            5  + misc::random::Int(6));

                creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("A"),
                        creature::race::Human,
                        creature::role::Archer,
                        ARCHER_STATS));


        const StatSet WOLFEN_STATS(
            STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 4 + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
            STAT_BASE_MED  + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND));

                creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("W"),
                        creature::race::Wolfen,
                        creature::role::Wolfen,
                        WOLFEN_STATS));
        */

        const StatSet BARD_STATS(
            Strength_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("B"),
            creature::race::Human,
            creature::role::Bard,
            BARD_STATS));

        const StatSet BEASTMASTER_STATS(
            Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("G"),
            creature::race::Human,
            creature::role::Beastmaster,
            BEASTMASTER_STATS));

        const StatSet THEIF_STATS(
            Strength_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("T"),
            creature::race::Gnome,
            creature::role::Thief,
            THEIF_STATS));

        const StatSet CLERIC_STATS(
            Strength_t(1 + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("C"),
            creature::race::Pixie,
            creature::role::Cleric,
            CLERIC_STATS));

        /*
        const StatSet SORCERER_STATS(
            1             +       misc::random::Int(STAT_RAND),
            STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
            STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
            STAT_BASE_MED +       misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 4 +  misc::random::Int(STAT_RAND));

                creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("S"),
                        creature::race::Pixie,
                        creature::role::Sorcerer,
                        SORCERER_STATS));
                */

        const StatSet SYLAVIN_STATS(
            Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("D"),
            creature::race::Dragon,
            creature::role::Sylavin,
            SYLAVIN_STATS));

        return std::make_unique<creature::PlayerParty>(
            avatar::Avatar::Puck_Male_Light, creaturesPVec);
    }

    const std::string
        PlayerPartyFactory::MakeCharacterNameForTesting(const std::string & POSTFIX) const
    {
        creature::NameInfo creatureNameInfo;

        return boost::algorithm::replace_last_copy(
            creatureNameInfo.LargestName(), creatureNameInfo.LargestLetterString(), POSTFIX);
    }

} // namespace creature
} // namespace heroespath
