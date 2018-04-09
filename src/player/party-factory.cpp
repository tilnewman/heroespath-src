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

#include "creature/creature-warehouse.hpp"
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
        {
            const stats::StatSet KNIGHT_STATS(
                STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                STAT_BASE_LOW  + misc::random::Int(STAT_RAND));

            const std::string KNIGHT_NAME( boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "K") );

            auto knightPtr{
        creature::CreatureWarehouse::Access().Store(std::make_unique<creature::Creature>( true,
                KNIGHT_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Humanoid(),
                creature::Race(creature::race::Human),
                creature::Role(creature::role::Knight),
                KNIGHT_STATS)) };

            player::Initial::Setup(knightPtr);
            creaturesPVec.emplace_back(knightPtr);
        }
        */
        {
            const stats::StatSet FIREBRAND_STATS(
                Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

            const std::string FIREBRAND_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "F"));

            auto firebrandPtr{ creature::CreatureWarehouse::Access().Store(
                std::make_unique<creature::Creature>(
                    true,
                    FIREBRAND_NAME,
                    creature::sex::Male,
                    creature::BodyType::Make_Dragon(),
                    creature::race::Dragon,
                    creature::role::Firebrand,
                    FIREBRAND_STATS)) };

            player::Initial::Setup(firebrandPtr);
            creaturesPVec.emplace_back(firebrandPtr);
        }
        /*
        {
            const stats::StatSet ARCHER_STATS(
                15 + misc::random::Int(6),
                15 + misc::random::Int(10),
                5  + misc::random::Int(6),
                10 + misc::random::Int(6),
                10 + misc::random::Int(8),
                5  + misc::random::Int(6));

            const std::string ARCHER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "A"));

            auto archerPtr{
        creature::CreatureWarehouse::Access().Store(std::make_unique<creature::Creature>( true,
                ARCHER_NAME,
                creature::sex::Female,
                creature::BodyType::Make_Humanoid(),
                creature::Race(creature::race::Human),
                creature::Role(creature::role::Archer),
                ARCHER_STATS)) };

            player::Initial::Setup(archerPtr);
            creaturesPVec.emplace_back(archerPtr);
        }

        {
            const stats::StatSet WOLFEN_STATS(
                STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
                STAT_BASE_HIGH + 4 + misc::random::Int(STAT_RAND),
                STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                STAT_BASE_MED  + misc::random::Int(STAT_RAND),
                STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
                STAT_BASE_LOW  + misc::random::Int(STAT_RAND));

            const std::string WOLFEN_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "W"));

            auto wolfenPtr{
        creature::CreatureWarehouse::Access().Store(std::make_unique<creature::Creature>( true,
                WOLFEN_NAME,
                creature::sex::Female,
                creature::BodyType::Make_Wolfen(),
                creature::Race(creature::race::Wolfen),
                creature::Role(creature::role::Wolfen),
                WOLFEN_STATS)) };

            player::Initial::Setup(wolfenPtr);
            creaturesPVec.emplace_back(wolfenPtr);
        }
        */
        {
            const stats::StatSet BARD_STATS(
                Strength_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Charm_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Speed_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

            const std::string BARD_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "B"));

            auto bardPtr{ creature::CreatureWarehouse::Access().Store(
                std::make_unique<creature::Creature>(
                    true,
                    BARD_NAME,
                    creature::sex::Male,
                    creature::BodyType::Make_Humanoid(),
                    creature::race::Human,
                    creature::role::Bard,
                    BARD_STATS)) };

            player::Initial::Setup(bardPtr);
            creaturesPVec.emplace_back(bardPtr);
        }

        {
            const stats::StatSet BEASTMASTER_STATS(
                Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

            const std::string BEASTMASTER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "G"));

            auto bmPtr{ creature::CreatureWarehouse::Access().Store(
                std::make_unique<creature::Creature>(
                    true,
                    BEASTMASTER_NAME,
                    creature::sex::Male,
                    creature::BodyType::Make_Humanoid(),
                    creature::race::Human,
                    creature::role::Beastmaster,
                    BEASTMASTER_STATS)) };

            player::Initial::Setup(bmPtr);
            creaturesPVec.emplace_back(bmPtr);
        }

        {
            const stats::StatSet THEIF_STATS(
                Strength_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Luck_t(STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
                Speed_t(STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
                Intell_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)));

            const std::string THEIF_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "T"));

            auto thiefPtr{ creature::CreatureWarehouse::Access().Store(
                std::make_unique<creature::Creature>(
                    true,
                    THEIF_NAME,
                    creature::sex::Male,
                    creature::BodyType::Make_Humanoid(),
                    creature::race::Gnome,
                    creature::role::Thief,
                    THEIF_STATS)) };

            player::Initial::Setup(thiefPtr);
            creaturesPVec.emplace_back(thiefPtr);
        }

        {
            const stats::StatSet CLERIC_STATS(
                Strength_t(1 + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Charm_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Luck_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Speed_t(STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND)),
                Intell_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)));

            const std::string CLERIC_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "C"));

            auto clericPtr{ creature::CreatureWarehouse::Access().Store(
                std::make_unique<creature::Creature>(
                    true,
                    CLERIC_NAME,
                    creature::sex::Female,
                    creature::BodyType::Make_Pixie(),
                    creature::race::Pixie,
                    creature::role::Cleric,
                    CLERIC_STATS)) };

            player::Initial::Setup(clericPtr);
            creaturesPVec.emplace_back(clericPtr);
        }
        /*
        {
            const stats::StatSet SORCERER_STATS(
                1             +       misc::random::Int(STAT_RAND),
                STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
                STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
                STAT_BASE_MED +       misc::random::Int(STAT_RAND),
                STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND),
                STAT_BASE_HIGH + 4 +  misc::random::Int(STAT_RAND));

            const std::string SORCERER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "S"));

            auto sorcererPtr{
        creature::CreatureWarehouse::Access().Store(std::make_unique<creature::Creature>( true,
                SORCERER_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Pixie(),
                creature::Race(creature::race::Pixie),
                creature::Role(creature::role::Sorcerer),
                SORCERER_STATS)) };

            player::Initial::Setup(sorcererPtr);
            creaturesPVec.emplace_back(sorcererPtr);
        }
        */
        {
            const stats::StatSet SYLAVIN_STATS(
                Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Charm_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Luck_t(STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Speed_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Intell_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)));

            const std::string SYLAVIN_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "S"));

            auto sylavinPtr{ creature::CreatureWarehouse::Access().Store(
                std::make_unique<creature::Creature>(
                    true,
                    SYLAVIN_NAME,
                    creature::sex::Male,
                    creature::BodyType::Make_Dragon(),
                    creature::race::Dragon,
                    creature::role::Sylavin,
                    SYLAVIN_STATS)) };

            player::Initial::Setup(sylavinPtr);
            creaturesPVec.emplace_back(sylavinPtr);
        }

        return std::make_unique<player::Party>(avatar::Avatar::Puck_Male_Light, creaturesPVec);
    }

} // namespace player
} // namespace heroespath
