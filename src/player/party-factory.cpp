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
// party-factory.cpp
//
#include "party-factory.hpp"

#include "creature/creature-factory.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"
#include "player/initial.hpp"
#include "player/party.hpp"
#include "stats/stat-set.hpp"

#include <algorithm>
#include <string>

namespace heroespath
{
namespace player
{

    PartyUPtr_t PartyFactory::Make(
        const avatar::Avatar::Enum AVATAR, const creature::CreaturePVec_t & CHARACTERS)
    {
        return std::make_unique<Party>(AVATAR, CHARACTERS);
    }

    PartyUPtr_t PartyFactory::MakeFakeForTesting()
    {
        creature::CreaturePVec_t creaturesPVec;

        const int STAT_BASE_HIGH{ 18 };
        const int STAT_BASE_MED{ 9 };
        const int STAT_BASE_LOW{ 5 };
        const int STAT_RAND{ 6 };

        /*
        const stats::StatSet KNIGHT_STATS(
            STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("K"),
            creature::race::Human,
            creature::role::Knight,
            KNIGHT_STATS));
        */

        const stats::StatSet FIREBRAND_STATS(
            Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("F"),
            creature::race::Dragon,
            creature::role::Firebrand,
            FIREBRAND_STATS));

        /*
        const stats::StatSet ARCHER_STATS(
            15 + misc::random::Int(6),
            15 + misc::random::Int(10),
            5  + misc::random::Int(6),
            10 + misc::random::Int(6),
            10 + misc::random::Int(8),
            5  + misc::random::Int(6));

                creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("A"),
                        creature::race::Human,
                        creature::role::Archer,
                        ARCHER_STATS));


        const stats::StatSet WOLFEN_STATS(
            STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 4 + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
            STAT_BASE_MED  + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
            STAT_BASE_LOW  + misc::random::Int(STAT_RAND));

                creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("W"),
                        creature::race::Wolfen,
                        creature::role::Wolfen,
                        WOLFEN_STATS));
        */

        const stats::StatSet BARD_STATS(
            Strength_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("B"),
            creature::race::Human,
            creature::role::Bard,
            BARD_STATS));

        const stats::StatSet BEASTMASTER_STATS(
            Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("G"),
            creature::race::Human,
            creature::role::Beastmaster,
            BEASTMASTER_STATS));

        const stats::StatSet THEIF_STATS(
            Strength_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("T"),
            creature::race::Gnome,
            creature::role::Thief,
            THEIF_STATS));

        const stats::StatSet CLERIC_STATS(
            Strength_t(1 + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("C"),
            creature::race::Pixie,
            creature::role::Cleric,
            CLERIC_STATS));

        /*
        const stats::StatSet SORCERER_STATS(
            1             +       misc::random::Int(STAT_RAND),
            STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
            STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
            STAT_BASE_MED +       misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND),
            STAT_BASE_HIGH + 4 +  misc::random::Int(STAT_RAND));

                creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
                        MakeCharacterNameForTesting("S"),
                        creature::race::Pixie,
                        creature::role::Sorcerer,
                        SORCERER_STATS));
                */

        const stats::StatSet SYLAVIN_STATS(
            Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
            Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
            Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

        creaturesPVec.emplace_back(creature::CreatureFactory::MakeAndEquipPlayerForTesting(
            MakeCharacterNameForTesting("D"),
            creature::race::Dragon,
            creature::role::Sylavin,
            SYLAVIN_STATS));

        return std::make_unique<player::Party>(avatar::Avatar::Puck_Male_Light, creaturesPVec);
    }

    const std::string PartyFactory::MakeCharacterNameForTesting(const std::string & POSTFIX)
    {
        return boost::algorithm::replace_last_copy(
            creature::NameInfo::Instance()->LargestName(),
            creature::NameInfo::Instance()->LargestLetterString(),
            POSTFIX);
    }

} // namespace player
} // namespace heroespath
