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
        const avatar::Avatar::Enum AVATAR, const CreaturePVec_t & CHARACTERS) const
    {
        return std::make_unique<PlayerParty>(AVATAR, CHARACTERS);
    }

    PlayerPartyUPtr_t PlayerPartyFactory::MakeFakeForTesting() const
    {
        CreatureFactory creatureFactory;
        CreaturePVec_t creaturesPVec;

        const Trait_t STAT_BASE_HIGH { 18 };
        const Trait_t STAT_BASE_MED { 9 };
        const Trait_t STAT_BASE_LOW { 5 };
        const Trait_t STAT_RAND { 6 };

        auto makeStatSet = [&](const Trait_t STR,
                               const Trait_t ACC,
                               const Trait_t CHR,
                               const Trait_t LCK,
                               const Trait_t SPD,
                               const Trait_t INT) {
            return StatSet(
                Strength_t::Make(STR + misc::Random(STAT_RAND)),
                Accuracy_t::Make(ACC + misc::Random(STAT_RAND)),
                Charm_t::Make(CHR + misc::Random(STAT_RAND)),
                Luck_t::Make(LCK + misc::Random(STAT_RAND)),
                Speed_t::Make(SPD + misc::Random(STAT_RAND)),
                Intel_t::Make(INT + misc::Random(STAT_RAND)));
        };

        /*
        const StatSet KNIGHT_STATS(
            STAT_BASE_HIGH + misc::Random(STAT_RAND),
            STAT_BASE_HIGH + misc::Random(STAT_RAND),
            STAT_BASE_LOW  + misc::Random(STAT_RAND),
            STAT_BASE_LOW  + misc::Random(STAT_RAND),
            STAT_BASE_HIGH + misc::Random(STAT_RAND),
            STAT_BASE_LOW  + misc::Random(STAT_RAND));

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("K"),
            race::Human,
            role::Knight,
            KNIGHT_STATS));
        */

        const StatSet FIREBRAND_STATS = makeStatSet(
            STAT_BASE_HIGH,
            STAT_BASE_HIGH,
            STAT_BASE_LOW,
            STAT_BASE_LOW,
            STAT_BASE_HIGH,
            STAT_BASE_MED);

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("F"), race::Dragon, role::Firebrand, FIREBRAND_STATS));

        /*
        const StatSet ARCHER_STATS(
            15 + misc::Random(6),
            15 + misc::Random(10),
            5  + misc::Random(6),
            10 + misc::Random(6),
            10 + misc::Random(8),
            5  + misc::Random(6));

                creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("A"),
                        race::Human,
                        role::Archer,
                        ARCHER_STATS));


        const StatSet WOLFEN_STATS(
            STAT_BASE_HIGH + 7 + misc::Random(STAT_RAND),
            STAT_BASE_HIGH + 4 + misc::Random(STAT_RAND),
            STAT_BASE_LOW  + misc::Random(STAT_RAND),
            STAT_BASE_MED  + misc::Random(STAT_RAND),
            STAT_BASE_HIGH + 7 + misc::Random(STAT_RAND),
            STAT_BASE_LOW  + misc::Random(STAT_RAND));

                creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("W"),
                        race::Wolfen,
                        role::Wolfen,
                        WOLFEN_STATS));
        */

        const StatSet BARD_STATS = makeStatSet(
            STAT_BASE_MED,
            STAT_BASE_MED,
            STAT_BASE_MED,
            STAT_BASE_LOW,
            STAT_BASE_MED,
            STAT_BASE_MED);

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("B"), race::Human, role::Bard, BARD_STATS));

        const StatSet BEASTMASTER_STATS = makeStatSet(
            STAT_BASE_HIGH,
            STAT_BASE_MED,
            STAT_BASE_LOW,
            STAT_BASE_LOW,
            STAT_BASE_HIGH,
            STAT_BASE_MED);

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("G"), race::Human, role::Beastmaster, BEASTMASTER_STATS));

        const StatSet THIEF_STATS = makeStatSet(
            STAT_BASE_LOW,
            STAT_BASE_LOW,
            STAT_BASE_LOW,
            (STAT_BASE_HIGH + 7),
            (STAT_BASE_HIGH + 7),
            STAT_BASE_LOW);

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("T"), race::Gnome, role::Thief, THIEF_STATS));

        const StatSet CLERIC_STATS = makeStatSet(
            1, STAT_BASE_LOW, STAT_BASE_HIGH, STAT_BASE_MED, (STAT_BASE_HIGH + 20), STAT_BASE_HIGH);

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("C"), race::Pixie, role::Cleric, CLERIC_STATS));

        /*
        const StatSet SORCERER_STATS(
            1             +       misc::Random(STAT_RAND),
            STAT_BASE_LOW +       misc::Random(STAT_RAND),
            STAT_BASE_LOW +       misc::Random(STAT_RAND),
            STAT_BASE_MED +       misc::Random(STAT_RAND),
            STAT_BASE_HIGH + 20 + misc::Random(STAT_RAND),
            STAT_BASE_HIGH + 4 +  misc::Random(STAT_RAND));

                creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("S"),
                        race::Pixie,
                        role::Sorcerer,
                        SORCERER_STATS));
                */

        const StatSet SYLAVIN_STATS = makeStatSet(
            STAT_BASE_HIGH,
            STAT_BASE_HIGH,
            STAT_BASE_LOW,
            STAT_BASE_LOW,
            STAT_BASE_HIGH,
            STAT_BASE_MED);

        creaturesPVec.emplace_back(creatureFactory.MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("D"), race::Dragon, role::Sylavin, SYLAVIN_STATS));

        return std::make_unique<PlayerParty>(avatar::Avatar::Puck_Male_Light, creaturesPVec);
    }

    const std::string
        PlayerPartyFactory::MakeCharacterNameForTesting(const std::string & POSTFIX) const
    {
        NameInfo creatureNameInfo;

        return boost::algorithm::replace_last_copy(
            creatureNameInfo.LargestName(), creatureNameInfo.LargestLetterString(), POSTFIX);
    }

} // namespace creature
} // namespace heroespath
