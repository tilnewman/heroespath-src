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
// title-warehouse.cpp
//
#include "title-warehouse.hpp"

#include "sfml-util/gui/title-image-manager.hpp"

#include "game/creature/role-enum.hpp"
#include "game/creature/title.hpp"
#include "game/loop-manager.hpp"

#include "misc/assertlogandthrow.hpp"

#include <memory>
#include <sstream>
#include <exception>


namespace game
{
namespace creature
{
namespace title
{

    TitleUVec_t Warehouse::titleUVec_;


    void Warehouse::Fill()
    {
        //Note:  Keep order in sync with game::creature::Titles::Enum

        titleUVec_.push_back( std::make_unique<Title>(Titles::ProtectorOfThornberry, AchievementType::None, 0, 0, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(), 0, 500_exp, 2) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Dodger,      AchievementType::DodgedStanding, 1, 50,    Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::FastFoot,    AchievementType::DodgedStanding, 2, 100,   Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::NimbleFoot,  AchievementType::DodgedStanding, 3, 200,   Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SwiftFoot,   AchievementType::DodgedStanding, 4, 500,   Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MeleeMover,  AchievementType::DodgedStanding, 5, 1000,  Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MeleeGrace,  AchievementType::DodgedStanding, 6, 2000,  Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MeleeDancer, AchievementType::DodgedStanding, 7, 5000,  Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MeleeShadow, AchievementType::DodgedStanding, 8, 10000, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0)) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::SkyDodger,   AchievementType::DodgedFlying, 1, 20,  Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::FastWing,    AchievementType::DodgedFlying, 2, 40,  Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::NimbleWing,  AchievementType::DodgedFlying, 3, 80,  Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SwiftWing,   AchievementType::DodgedFlying, 4, 120, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SkyMover,    AchievementType::DodgedFlying, 5, 200, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::GraceWing,   AchievementType::DodgedFlying, 6, 300, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SkyDancer,   AchievementType::DodgedFlying, 7, 450, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0)) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SkyShadow,   AchievementType::DodgedFlying, 8, 650, Title::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0)) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Sneak,        AchievementType::BackstabsHits, 1, 10,   RoleVec_t { role::Thief }, stats::StatSet(0, 1,  0, 1,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Stalker,      AchievementType::BackstabsHits, 2, 50,   RoleVec_t { role::Thief }, stats::StatSet(0, 2,  0, 2,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Creeper,      AchievementType::BackstabsHits, 3, 100,  RoleVec_t { role::Thief }, stats::StatSet(0, 3,  0, 3,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Savage,       AchievementType::BackstabsHits, 4, 200,  RoleVec_t { role::Thief }, stats::StatSet(0, 4,  0, 4,  0, 0), 0, 0_exp, 1) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Homicidal,    AchievementType::BackstabsHits, 5, 300,  RoleVec_t { role::Thief }, stats::StatSet(0, 5,  0, 5,  0, 0), 0, 0_exp, 2) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Bloodthirsty, AchievementType::BackstabsHits, 6, 400,  RoleVec_t { role::Thief }, stats::StatSet(0, 6,  0, 6,  0, 0), 0, 0_exp, 5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::ShadowWalker, AchievementType::BackstabsHits, 7, 500,  RoleVec_t { role::Thief }, stats::StatSet(0, 7,  0, 7,  0, 0), 1, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Assassin,     AchievementType::BackstabsHits, 8, 1000, RoleVec_t { role::Thief }, stats::StatSet(0, 10, 0, 10, 0, 0), 2, 0_exp, 20) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfTheClashingBlade,  AchievementType::BattlesSurvived, 1, 10,   RoleVec_t{ role::Knight }, stats::StatSet(1,   1, 0, 0, 0, 0), 0, 0_exp,  5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfAHundredBattles,   AchievementType::BattlesSurvived, 2, 100,  RoleVec_t{ role::Knight }, stats::StatSet(2,   2, 0, 0, 0, 0), 0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfThePiercingBlade,  AchievementType::BattlesSurvived, 3, 200,  RoleVec_t{ role::Knight }, stats::StatSet(3,   3, 0, 0, 0, 0), 0, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfTheUndauntedBlade, AchievementType::BattlesSurvived, 4, 500,  RoleVec_t{ role::Knight }, stats::StatSet(4,   4, 0, 0, 0, 0), 0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfAThousandBattles,  AchievementType::BattlesSurvived, 5, 1000, RoleVec_t{ role::Knight }, stats::StatSet(5,   5, 0, 0, 0, 0), 1, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfTheFearedBlade,    AchievementType::BattlesSurvived, 6, 2000, RoleVec_t{ role::Knight }, stats::StatSet(6,   6, 0, 0, 0, 0), 1, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfTheRealm,          AchievementType::BattlesSurvived, 7, 3000, RoleVec_t{ role::Knight }, stats::StatSet(7,   7, 0, 0, 0, 0), 1, 0_exp, 35) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::KnightOfEtan,              AchievementType::BattlesSurvived, 8, 5000, RoleVec_t{ role::Knight }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Mender,                AchievementType::HealthGiven, 1, 50,    RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  1, 0, 0,  1), 0, 0_exp,  2) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Healer,                AchievementType::HealthGiven, 2, 200,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  2, 0, 0,  2), 0, 0_exp,  5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BlessedHands,          AchievementType::HealthGiven, 3, 500,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  3, 0, 0,  3), 0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::WitchDoctor,           AchievementType::HealthGiven, 4, 1000,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  4, 0, 0,  4), 1, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::LightOfTheCure,        AchievementType::HealthGiven, 5, 2000,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  5, 0, 0,  5), 1, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DivineTouch,           AchievementType::HealthGiven, 6, 3000,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  6, 0, 0,  6), 1, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::OrderOfTheWhiteSage,   AchievementType::HealthGiven, 7, 5000,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0,  8, 0, 0,  8), 1, 0_exp, 35) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::ClericOfTheCovenant,   AchievementType::HealthGiven, 8, 10000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::HandsOfCharity,          AchievementType::HealthTraded, 1, 50,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1,  0, 0, 1),  0, 0_exp, 2) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HandsOfLove,             AchievementType::HealthTraded, 2, 100,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2,  0, 0, 2),  0, 0_exp, 4) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HandsOfDevotion,         AchievementType::HealthTraded, 3, 200,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3,  0, 0, 3),  0, 0_exp, 8) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HandsOfNobility,         AchievementType::HealthTraded, 4, 400,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4,  0, 0, 4),  0, 0_exp, 12) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HandsOfSacrifice,        AchievementType::HealthTraded, 5, 800,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5,  0, 0, 5),  0, 0_exp, 16) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::TheSelflessHand,         AchievementType::HealthTraded, 6, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6,  0, 0, 6),  1, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::TheIncorruptibleHand,    AchievementType::HealthTraded, 7, 2500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 7,  0, 0, 7),  1, 0_exp, 24) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::OrderOfTheBleedingPalm,  AchievementType::HealthTraded, 8, 4000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0_exp, 30) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::HowlingStray,     AchievementType::BeastRoars, 1, 10,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(1,  0, 0, 0, 0, 0), 0, 0_exp, 2) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HowlingBully,     AchievementType::BeastRoars, 2, 30,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(2,  0, 0, 0, 0, 0), 0, 0_exp, 4) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HowlingWildling,  AchievementType::BeastRoars, 3, 50,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(3,  0, 0, 0, 0, 0), 0, 0_exp, 8) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SnarlingCreature, AchievementType::BeastRoars, 4, 100,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(4,  0, 0, 0, 0, 0), 0, 0_exp, 12) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SnarlingBeast,    AchievementType::BeastRoars, 5, 200,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(5,  0, 0, 0, 0, 0), 0, 0_exp, 16) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SnarlingFiend,    AchievementType::BeastRoars, 6, 400,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(6,  0, 0, 0, 0, 0), 0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::RagingMonster,    AchievementType::BeastRoars, 7, 1000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(8,  0, 0, 0, 0, 0), 1, 0_exp, 24) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::RagingHorror,     AchievementType::BeastRoars, 8, 2000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(10, 0, 0, 0, 0, 0), 2, 0_exp, 30) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Prowler,     AchievementType::LocksPicked, 1, 10,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0_exp, 2) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PickPocket,  AchievementType::LocksPicked, 2, 50,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0_exp, 4) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::CatBurglar,  AchievementType::LocksPicked, 3, 100,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0_exp, 8) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Clincher,    AchievementType::LocksPicked, 4, 200,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0_exp, 12) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Picker,      AchievementType::LocksPicked, 5, 500,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0_exp, 16) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SafeCracker, AchievementType::LocksPicked, 6, 1000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::LockTickler, AchievementType::LocksPicked, 7, 2000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::LockBane,    AchievementType::LocksPicked, 8, 5000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Versifier,       AchievementType::SongsPlayed, 1, 10,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  1,  0, 1),  0, 0_exp, 5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Player,          AchievementType::SongsPlayed, 2, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  1,  0, 2),  0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Balladeer,       AchievementType::SongsPlayed, 3, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  2,  0, 3),  0, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Minstrel,        AchievementType::SongsPlayed, 4, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  2,  0, 4),  0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Instrumentalist, AchievementType::SongsPlayed, 5, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  3,  0, 5),  0, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Soloist,         AchievementType::SongsPlayed, 6, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  3,  0, 6),  1, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Artiste,         AchievementType::SongsPlayed, 7, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 8,  5,  0, 8),  1, 0_exp, 50) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Virtuoso,        AchievementType::SongsPlayed, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 10, 0, 10), 2, 0_exp, 60) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheTrippingToes,   AchievementType::SpiritsLifted, 1, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  0, 0, 1 ), 0, 0_exp, 2 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheMerryMelody,    AchievementType::SpiritsLifted, 2, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  0, 0, 2 ), 0, 0_exp, 4 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheWhimsicalWord,  AchievementType::SpiritsLifted, 3, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  1, 0, 3 ), 0, 0_exp, 8 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheCarolingChorus, AchievementType::SpiritsLifted, 4, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  1, 0, 4 ), 0, 0_exp, 12) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheBouncingBallad, AchievementType::SpiritsLifted, 5, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  2, 0, 5 ), 0, 0_exp, 16) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheDancingDitty,   AchievementType::SpiritsLifted, 6, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  2, 0, 6 ), 0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheEnchantedEar,   AchievementType::SpiritsLifted, 7, 3000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 7,  3, 0, 7 ), 1, 0_exp, 24) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BardOfTheAracneAria,     AchievementType::SpiritsLifted, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 5, 0, 10), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Targeter,        AchievementType::ProjectileHits, 1, 20,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  1,  0, 1,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SteadfastFlyer,  AchievementType::ProjectileHits, 2, 50,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  2,  0, 2,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DeadEye,         AchievementType::ProjectileHits, 3, 100,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  3,  0, 3,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DeadCenter,      AchievementType::ProjectileHits, 4, 500,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  4,  0, 4,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SureSail,        AchievementType::ProjectileHits, 5, 1000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  5,  0, 5,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SureShot,        AchievementType::ProjectileHits, 6, 2000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  6,  0, 6,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PerfectHit,      AchievementType::ProjectileHits, 7, 3000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(4,  8,  0, 8,  0, 0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::EnchantedAim,    AchievementType::ProjectileHits, 8, 5000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(10, 10, 0, 10, 0, 0), 1, 0_exp, 0) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Magus,     AchievementType::SpellsCast, 1, 20,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 1,  0, 1),  0, 0_exp, 2) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Charmer,   AchievementType::SpellsCast, 2, 50,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 2,  0, 2),  0, 0_exp, 4) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Diviner,   AchievementType::SpellsCast, 3, 100,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 3,  0, 3),  0, 0_exp, 8) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Enchanter, AchievementType::SpellsCast, 4, 200,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 4,  0, 4),  0, 0_exp, 12) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Conjurer,  AchievementType::SpellsCast, 5, 500,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 5,  0, 5),  0, 0_exp, 16) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Shaman,    AchievementType::SpellsCast, 6, 1000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 6,  0, 6),  1, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Wizard,    AchievementType::SpellsCast, 7, 2000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 8,  0, 8),  1, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Warlock,   AchievementType::SpellsCast, 8, 5000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 10, 0, 10), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Undaunted,     AchievementType::EnemiesFaced, 1, 20,   RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0,  0, 0, 1,  0, 0),  0, 0_exp,  5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Daring,        AchievementType::EnemiesFaced, 2, 50,   RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(1,  0, 0, 2,  0, 1),  0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Bold,          AchievementType::EnemiesFaced, 3, 100,  RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(2,  0, 0, 3,  0, 2),  0, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Brave,         AchievementType::EnemiesFaced, 4, 200,  RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(3,  0, 0, 4,  0, 3),  0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Valorous,      AchievementType::EnemiesFaced, 5, 500,  RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(4,  0, 0, 5,  0, 4),  0, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Valiant,       AchievementType::EnemiesFaced, 6, 1000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(5,  0, 0, 6,  0, 5),  0, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Lionhearted,   AchievementType::EnemiesFaced, 7, 2000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(6,  0, 0, 7,  0, 6),  0, 0_exp, 35) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Heroic,        AchievementType::EnemiesFaced, 8, 3000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0_exp, 40) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Fearless,      AchievementType::EnemiesFaced, 9, 5000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheLunarPup,       AchievementType::MoonHowls, 1, 10,  RoleVec_t{ role::Wolfen }, stats::StatSet(1,  0, 0, 1,  0, 0), 0, 0_exp, 5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheYoungHowler,    AchievementType::MoonHowls, 2, 20,  RoleVec_t{ role::Wolfen }, stats::StatSet(2,  0, 0, 2,  0, 0), 0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheNightSky,       AchievementType::MoonHowls, 3, 50,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  0, 0, 3,  0, 0), 0, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheMidnightWail,   AchievementType::MoonHowls, 4, 100, RoleVec_t{ role::Wolfen }, stats::StatSet(4,  0, 0, 4,  0, 0), 0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheTwilightMoon,   AchievementType::MoonHowls, 5, 200, RoleVec_t{ role::Wolfen }, stats::StatSet(5,  0, 0, 5,  0, 0), 1, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheCelestialCry,   AchievementType::MoonHowls, 6, 300, RoleVec_t{ role::Wolfen }, stats::StatSet(6,  0, 0, 6,  0, 0), 1, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheGlowingOrb,     AchievementType::MoonHowls, 7, 500, RoleVec_t{ role::Wolfen }, stats::StatSet(8,  0, 0, 8,  0, 0), 1, 0_exp, 40) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PawOfTheCrescentLegion, AchievementType::MoonHowls, 8, 800, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 0, 0, 10, 0, 0), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::WindGlider,    AchievementType::TurnsInFlight, 1, 50,   RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::LightFeather,  AchievementType::TurnsInFlight, 2, 100,  RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::FreeFlyer,     AchievementType::TurnsInFlight, 3, 200,  RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SteadyWing,    AchievementType::TurnsInFlight, 4, 500,  RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::EverSoar,      AchievementType::TurnsInFlight, 5, 1000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SkySail,       AchievementType::TurnsInFlight, 6, 2000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Aerialist,     AchievementType::TurnsInFlight, 7, 3000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::FeatherDancer, AchievementType::TurnsInFlight, 8, 5000, RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0_exp, 0) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::PackFollower,  AchievementType::PackActions, 1, 20,   RoleVec_t{ role::Wolfen }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0_exp, 5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackMember,    AchievementType::PackActions, 2, 50,   RoleVec_t{ role::Wolfen }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackTasker,    AchievementType::PackActions, 3, 100,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackCharger,   AchievementType::PackActions, 4, 200,  RoleVec_t{ role::Wolfen }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackNobel,     AchievementType::PackActions, 5, 300,  RoleVec_t{ role::Wolfen }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackLeader,    AchievementType::PackActions, 6, 400,  RoleVec_t{ role::Wolfen }, stats::StatSet(6,  6,  0, 0, 0, 0), 0, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackAlpha,     AchievementType::PackActions, 7, 500,  RoleVec_t{ role::Wolfen }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0_exp, 40) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::PackElder,     AchievementType::PackActions, 8, 1000, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::CritterClairvoyant, AchievementType::BeastMindLinks, 1, 20,   RoleVec_t{ role::Beastmaster }, stats::StatSet(1,  0, 0, 1,  0, 1),  0, 0_exp, 5) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::AnimalAnimator,     AchievementType::BeastMindLinks, 2, 50,   RoleVec_t{ role::Beastmaster }, stats::StatSet(2,  0, 0, 2,  0, 2),  0, 0_exp, 10) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::FriendOfTheFeral,   AchievementType::BeastMindLinks, 3, 100,  RoleVec_t{ role::Beastmaster }, stats::StatSet(3,  0, 0, 3,  0, 3),  0, 0_exp, 15) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::WillOfTheWild,      AchievementType::BeastMindLinks, 4, 200,  RoleVec_t{ role::Beastmaster }, stats::StatSet(4,  0, 0, 4,  0, 4),  0, 0_exp, 20) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::CreatureChanneler,  AchievementType::BeastMindLinks, 5, 300,  RoleVec_t{ role::Beastmaster }, stats::StatSet(5,  0, 0, 5,  0, 5),  0, 0_exp, 25) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::BeastTaskmaster,    AchievementType::BeastMindLinks, 6, 400,  RoleVec_t{ role::Beastmaster }, stats::StatSet(6,  0, 0, 6,  0, 6),  1, 0_exp, 30) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MonsterManipulator, AchievementType::BeastMindLinks, 7, 500,  RoleVec_t{ role::Beastmaster }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0_exp, 40) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MammalianMaster,    AchievementType::BeastMindLinks, 8, 1000, RoleVec_t{ role::Beastmaster }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0_exp, 50) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::Brawler,               AchievementType::MeleeHits, 1, 20,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0_exp, 5  ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::WildSwing,             AchievementType::MeleeHits, 2, 50,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0_exp, 10 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::HavocSmasher,          AchievementType::MeleeHits, 3, 100,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0_exp, 15 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SureStroke,            AchievementType::MeleeHits, 4, 200,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0_exp, 20 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::Warrior,               AchievementType::MeleeHits, 5, 500,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0_exp, 25 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::EverMark,              AchievementType::MeleeHits, 6, 1000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(6,  6,  0, 0, 0, 0), 1, 0_exp, 30 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::MeleeMaster,           AchievementType::MeleeHits, 7, 2000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(7,  7,  0, 0, 0, 0), 1, 0_exp, 40 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::SteadyStriker,         AchievementType::MeleeHits, 8, 3000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0_exp, 50 ) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::OrderOfTheEverWarrior, AchievementType::MeleeHits, 9, 5000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Firebrand, role::Sylavin }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0_exp, 100) );

        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheUnblinkingEye,      AchievementType::FlyingAttackHits, 1, 20,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(1,  1,  0, 0, 1,  0),  0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheCurlingClaw,        AchievementType::FlyingAttackHits, 2, 50,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(2,  2,  0, 0, 2,  1),  0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheDaggerTooth,        AchievementType::FlyingAttackHits, 3, 100,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(3,  3,  0, 0, 3,  2),  0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheForebodingGaze,     AchievementType::FlyingAttackHits, 4, 200,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(4,  4,  0, 0, 4,  3),  0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheUnbreakableScale,   AchievementType::FlyingAttackHits, 5, 500,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(5,  5,  0, 0, 5,  4),  0, 0_exp, 0) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheDreadedBreath,      AchievementType::FlyingAttackHits, 6, 1000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(6,  6,  0, 0, 6,  5),  0, 0_exp, 1) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheArcaneWing,         AchievementType::FlyingAttackHits, 7, 2000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(8,  8,  0, 0, 8,  8),  0, 0_exp, 1) );
        titleUVec_.push_back( std::make_unique<Title>(Titles::DragonOfTheNightmareSky,       AchievementType::FlyingAttackHits, 8, 5000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(10, 10, 0, 0, 10, 10), 0, 0_exp, 2) );
    }


    void Warehouse::Empty()
    {
        titleUVec_.clear();
    }


    TitlePtr_t Warehouse::Get(const Titles::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(((E >= 0) && (E < Titles::Count)),
            "game::creature::Titles::Warehouse::Get(enum=" << E << ")_InvalidValueError.");

        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < titleUVec_.size()),
            "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E)
            << "\") enum given was " << E << " but that was greater than (or equal to) "
            << "the vec size of " << titleUVec_.size() << ".");

        M_ASSERT_OR_LOGANDTHROW_SS((titleUVec_.empty() == false),
            "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E)
            << "\") called before vec was populated by Setup().");

        const TitlePtr_t TITLE_PTR(titleUVec_[static_cast<std::size_t>(E)].get());

        M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),
            "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E)
            << "\") found a nullptr in the vector.");

        return TITLE_PTR;
    }


    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend(
                "game::creature::Titles::Warehouse::Test() Starting Tests...");
        }

        static auto titleIndex{ 0 };
        if (titleIndex < Titles::Count)
        {
            auto const NEXT_ENUM(static_cast<Titles::Enum>(titleIndex));
            auto TITLE_PTR{ Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") resulted in a nullptr being returned.");

            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Desc().empty() == false),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") resulted in an empty Desc().");

            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->LongDesc().empty() == false),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") resulted in an empty LongDesc().");

            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->ImageFilename().empty() == false),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") resulted in an empty ImageFilename().");

            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->RolesCopy().empty() == false),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") resulted in an empty RolesVec().");

            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Which() == NEXT_ENUM),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") resulted in a Title with a different tile::Enum (\""
                << Titles::ToString(TITLE_PTR->Which()) << "\")");

            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Name() == Titles::Name(NEXT_ENUM)),
                "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM)
                << "\") Title is out of order.");

            sf::Texture texture;
            sfml_util::gui::TitleImageManager::Instance()->Get(texture, NEXT_ENUM);
            LoopManager::Instance()->TestingImageSet(texture);
            LoopManager::Instance()->TestingStrIncrement("Title Test #");
            ++titleIndex;
            return false;
        }

        LoopManager::Instance()->TestingStrAppend(
            "game::creature::Titles::Warehouse::Test()  ALL TESTS PASSED.");

        return true;
    }

}
}
}
