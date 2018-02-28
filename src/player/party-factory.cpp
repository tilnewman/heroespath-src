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

#include "stats/stat-set.hpp"
#include "player/party.hpp"
#include "player/character.hpp"
#include "player/initial.hpp"
#include "creature/name-info.hpp"

#include "misc/random.hpp"
#include "misc/boost-string-includes.hpp"

#include <string>
#include <algorithm>


namespace heroespath
{
namespace player
{

    PartyUPtr_t PartyFactory::Make(
        const avatar::Avatar::Enum AVATAR,
        const CharacterPVec_t & CHARACTERS)
    {
        return std::make_unique<Party>(AVATAR, CHARACTERS);
    }


    PartyUPtr_t PartyFactory::MakeFakeForTesting()
    {
        std::string errMsgIgnored{ "" };
        auto partyUPtr{ std::make_unique<player::Party>() };

        const int STAT_BASE_HIGH{18};
        const int STAT_BASE_MED{9};
        const int STAT_BASE_LOW{5};
        const int STAT_RAND{6};
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

            auto knightPtr{ new player::Character(
                KNIGHT_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Humanoid(),
                creature::Race(creature::race::Human),
                creature::Role(creature::role::Knight),
                KNIGHT_STATS) };

            player::Initial::Setup(knightPtr);
            partyUPtr->Add(knightPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet FIREBRAND_STATS(
                Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Charm_t   (STAT_BASE_LOW  + misc::random::Int(STAT_RAND)),
                Luck_t    (STAT_BASE_LOW  + misc::random::Int(STAT_RAND)),
                Speed_t   (STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Intell_t  (STAT_BASE_MED  + misc::random::Int(STAT_RAND)));

            const std::string FIREBRAND_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "F"));

            auto firebrandPtr{  new player::Character(
                FIREBRAND_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Dragon(),
                creature::race::Dragon,
                creature::role::Firebrand,
                FIREBRAND_STATS) };

            player::Initial::Setup(firebrandPtr);
            partyUPtr->Add(firebrandPtr, errMsgIgnored);
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

            auto archerPtr{ new player::Character(
                ARCHER_NAME,
                creature::sex::Female,
                creature::BodyType::Make_Humanoid(),
                creature::Race(creature::race::Human),
                creature::Role(creature::role::Archer),
                ARCHER_STATS) };

            player::Initial::Setup(archerPtr);
            partyUPtr->Add(archerPtr, errMsgIgnored);
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

            auto wolfenPtr{ new player::Character(
                WOLFEN_NAME,
                creature::sex::Female,
                creature::BodyType::Make_Wolfen(),
                creature::Race(creature::race::Wolfen),
                creature::Role(creature::role::Wolfen),
                WOLFEN_STATS) };

            player::Initial::Setup(wolfenPtr);
            partyUPtr->Add(wolfenPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet BARD_STATS(
                Strength_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Charm_t   (STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Luck_t    (STAT_BASE_LOW + misc::random::Int(STAT_RAND)),
                Speed_t   (STAT_BASE_MED + misc::random::Int(STAT_RAND)),
                Intell_t  (STAT_BASE_MED + misc::random::Int(STAT_RAND)));

            const std::string BARD_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "B"));

            auto bardPtr{ new player::Character(
                BARD_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Humanoid(),
                creature::race::Human,
                creature::role::Bard,
                BARD_STATS) };

            player::Initial::Setup(bardPtr);
            partyUPtr->Add(bardPtr, errMsgIgnored);
        }

        {
            const stats::StatSet BEASTMASTER_STATS(
                Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_MED  + misc::random::Int(STAT_RAND)),
                Charm_t   (STAT_BASE_LOW  + misc::random::Int(STAT_RAND)),
                Luck_t    (STAT_BASE_LOW  + misc::random::Int(STAT_RAND)),
                Speed_t   (STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Intell_t  (STAT_BASE_MED  + misc::random::Int(STAT_RAND)));

            const std::string BEASTMASTER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "G"));

            auto bmPtr{ new player::Character(
                BEASTMASTER_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Humanoid(),
                creature::race::Human,
                creature::role::Beastmaster,
                BEASTMASTER_STATS) };

            player::Initial::Setup(bmPtr);
            partyUPtr->Add(bmPtr, errMsgIgnored);
        }

        {
            const stats::StatSet THEIF_STATS(
                Strength_t(STAT_BASE_LOW  +     misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_LOW  +     misc::random::Int(STAT_RAND)),
                Charm_t   (STAT_BASE_LOW  +     misc::random::Int(STAT_RAND)),
                Luck_t    (STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
                Speed_t   (STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND)),
                Intell_t  (STAT_BASE_LOW  +     misc::random::Int(STAT_RAND)));

            const std::string THEIF_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "T"));

            auto thiefPtr{ new player::Character(
                THEIF_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Humanoid(),
                creature::race::Gnome,
                creature::role::Thief,
                THEIF_STATS) };

            player::Initial::Setup(thiefPtr);
            partyUPtr->Add(thiefPtr, errMsgIgnored);
        }

        {
            const stats::StatSet CLERIC_STATS(
                Strength_t(1             +       misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_LOW +       misc::random::Int(STAT_RAND)),
                Charm_t   (STAT_BASE_HIGH +      misc::random::Int(STAT_RAND)),
                Luck_t    (STAT_BASE_MED +       misc::random::Int(STAT_RAND)),
                Speed_t   (STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND)),
                Intell_t  (STAT_BASE_HIGH +      misc::random::Int(STAT_RAND)));

            const std::string CLERIC_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "C"));

            auto clericPtr{ new player::Character(
                CLERIC_NAME,
                creature::sex::Female,
                creature::BodyType::Make_Pixie(),
                creature::race::Pixie,
                creature::role::Cleric,
                CLERIC_STATS) };

            player::Initial::Setup(clericPtr);
            partyUPtr->Add(clericPtr, errMsgIgnored);
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

            auto sorcererPtr{ new player::Character(
                SORCERER_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Pixie(),
                creature::Race(creature::race::Pixie),
                creature::Role(creature::role::Sorcerer),
                SORCERER_STATS) };

            player::Initial::Setup(sorcererPtr);
            partyUPtr->Add(sorcererPtr, errMsgIgnored);
        }
        */
        {
            const stats::StatSet SYLAVIN_STATS(
                Strength_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Accuracy_t(STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Charm_t   (STAT_BASE_LOW  + misc::random::Int(STAT_RAND)),
                Luck_t    (STAT_BASE_LOW  + misc::random::Int(STAT_RAND)),
                Speed_t   (STAT_BASE_HIGH + misc::random::Int(STAT_RAND)),
                Intell_t  (STAT_BASE_MED  + misc::random::Int(STAT_RAND)));

            const std::string SYLAVIN_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "S"));

            auto sylavinPtr{ new player::Character(
                SYLAVIN_NAME,
                creature::sex::Male,
                creature::BodyType::Make_Dragon(),
                creature::race::Dragon,
                creature::role::Sylavin,
                SYLAVIN_STATS) };

            player::Initial::Setup(sylavinPtr);
            partyUPtr->Add(sylavinPtr, errMsgIgnored);
        }

        return partyUPtr;
    }

}
}
