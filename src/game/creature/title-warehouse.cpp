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

#include "game/creature/role.hpp"
#include "game/creature/title.hpp"
#include "game/loop-manager.hpp"

#include "utilz/assertlogandthrow.hpp"

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

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::ProtectorOfThornberry, AchievementType::None, 0, 0, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(), 0, 500, 2)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Dodger,      AchievementType::DodgedStanding, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::FastFoot,    AchievementType::DodgedStanding, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::NimbleFoot,  AchievementType::DodgedStanding, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SwiftFoot,   AchievementType::DodgedStanding, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MeleeMover,  AchievementType::DodgedStanding, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MeleeGrace,  AchievementType::DodgedStanding, 6, 2000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MeleeDancer, AchievementType::DodgedStanding, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MeleeShadow, AchievementType::DodgedStanding, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0))) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SkyDodger,   AchievementType::DodgedFlying, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::FastWing,    AchievementType::DodgedFlying, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::NimbleWing,  AchievementType::DodgedFlying, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SwiftWing,   AchievementType::DodgedFlying, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SkyMover,    AchievementType::DodgedFlying, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::GraceWing,   AchievementType::DodgedFlying, 6, 20000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SkyDancer,   AchievementType::DodgedFlying, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0))) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SkyShadow,   AchievementType::DodgedFlying, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0))) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Sneak,        AchievementType::BackstabsHits, 1, 10,   RoleVec_t { role::Thief }, stats::StatSet(0, 1,  0, 1,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Stalker,      AchievementType::BackstabsHits, 2, 50,   RoleVec_t { role::Thief }, stats::StatSet(0, 2,  0, 2,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Creeper,      AchievementType::BackstabsHits, 3, 100,  RoleVec_t { role::Thief }, stats::StatSet(0, 3,  0, 3,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Savage,       AchievementType::BackstabsHits, 4, 200,  RoleVec_t { role::Thief }, stats::StatSet(0, 4,  0, 4,  0, 0), 0, 0, 1)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Homicidal,    AchievementType::BackstabsHits, 5, 300,  RoleVec_t { role::Thief }, stats::StatSet(0, 5,  0, 5,  0, 0), 0, 0, 2)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Bloodthirsty, AchievementType::BackstabsHits, 6, 400,  RoleVec_t { role::Thief }, stats::StatSet(0, 6,  0, 6,  0, 0), 0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::ShadowWalker, AchievementType::BackstabsHits, 7, 500,  RoleVec_t { role::Thief }, stats::StatSet(0, 7,  0, 7,  0, 0), 1, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Assassin,     AchievementType::BackstabsHits, 8, 1000, RoleVec_t { role::Thief }, stats::StatSet(0, 10, 0, 10, 0, 0), 2, 0, 20)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfTheClashingBlade, AchievementType::BattlesSurvived, 1, 10, RoleVec_t{ role::Knight }, stats::StatSet(1, 1, 0, 0, 0, 0), 0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfAHundredBattles, AchievementType::BattlesSurvived, 2, 100, RoleVec_t{ role::Knight }, stats::StatSet(2, 2, 0, 0, 0, 0), 0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfThePiercingBlade, AchievementType::BattlesSurvived, 3, 200, RoleVec_t{ role::Knight }, stats::StatSet(3, 3, 0, 0, 0, 0), 0, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfTheUndauntedBlade, AchievementType::BattlesSurvived, 4, 500, RoleVec_t{ role::Knight }, stats::StatSet(4, 4, 0, 0, 0, 0), 0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfAThousandBattles, AchievementType::BattlesSurvived, 5, 1000, RoleVec_t{ role::Knight }, stats::StatSet(5, 5, 0, 0, 0, 0), 1, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfTheFearedBlade, AchievementType::BattlesSurvived, 6, 2000, RoleVec_t{ role::Knight }, stats::StatSet(6, 6, 0, 0, 0, 0), 1, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfTheRealm, AchievementType::BattlesSurvived, 7, 3000, RoleVec_t{ role::Knight }, stats::StatSet(7, 7, 0, 0, 0, 0), 1, 0, 35)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::KnightOfEtan,                AchievementType::BattlesSurvived, 8, 5000, RoleVec_t{ role::Knight }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Mender,                AchievementType::HealthGiven, 1, 50, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1, 0, 0, 1), 0, 0, 2)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Healer,                AchievementType::HealthGiven, 2, 200, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2, 0, 0, 2), 0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BlessedHands,          AchievementType::HealthGiven, 3, 500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3, 0, 0, 3), 0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::WitchDoctor,           AchievementType::HealthGiven, 4, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4, 0, 0, 4), 1, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::LightOfTheCure,        AchievementType::HealthGiven, 5, 2000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5, 0, 0, 5), 1, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DivineTouch,           AchievementType::HealthGiven, 6, 3000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6, 0, 0, 6), 1, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::OrderOfTheWhiteSage,   AchievementType::HealthGiven, 7, 5000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 8, 0, 0, 8), 1, 0, 35)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::ClericOfTheCovenant,   AchievementType::HealthGiven, 8, 10000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HandsOfCharity,          AchievementType::HealthTraded, 1, 50,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1,  0, 0, 1),  0, 0, 2)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HandsOfLove,             AchievementType::HealthTraded, 2, 100,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2,  0, 0, 2),  0, 0, 4)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HandsOfDevotion,         AchievementType::HealthTraded, 3, 200,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3,  0, 0, 3),  0, 0, 8)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HandsOfNobility,         AchievementType::HealthTraded, 4, 400,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4,  0, 0, 4),  0, 0, 12)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HandsOfSacrifice,        AchievementType::HealthTraded, 5, 800,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5,  0, 0, 5),  0, 0, 16)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::TheSelflessHand,         AchievementType::HealthTraded, 6, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6,  0, 0, 6),  1, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::TheIncorruptibleHand,    AchievementType::HealthTraded, 7, 2500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 7,  0, 0, 7),  1, 0, 24)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::OrderOfTheBleedingPalm,  AchievementType::HealthTraded, 8, 4000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 30)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HowlingStray,     AchievementType::BeastRoars, 1, 10,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(1,  0, 0, 0, 0, 0), 0, 0, 2)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HowlingBully,     AchievementType::BeastRoars, 2, 30,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(2,  0, 0, 0, 0, 0), 0, 0, 4)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HowlingWildling,  AchievementType::BeastRoars, 3, 50,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(3,  0, 0, 0, 0, 0), 0, 0, 8)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SnarlingCreature, AchievementType::BeastRoars, 4, 100,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(4,  0, 0, 0, 0, 0), 0, 0, 12)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SnarlingBeast,    AchievementType::BeastRoars, 5, 200,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(5,  0, 0, 0, 0, 0), 0, 0, 16)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SnarlingFiend,    AchievementType::BeastRoars, 6, 400,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(6,  0, 0, 0, 0, 0), 0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::RagingMonster,    AchievementType::BeastRoars, 7, 1000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(8,  0, 0, 0, 0, 0), 1, 0, 24)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::RagingHorror,     AchievementType::BeastRoars, 8, 2000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(10, 0, 0, 0, 0, 0), 2, 0, 30)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Prowler,     AchievementType::LocksPicked, 1, 10,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 2)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PickPocket,  AchievementType::LocksPicked, 2, 50,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 4)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::CatBurglar,  AchievementType::LocksPicked, 3, 100,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 8)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Clincher,    AchievementType::LocksPicked, 4, 200,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 12)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Picker,      AchievementType::LocksPicked, 5, 500,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 16)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SafeCracker, AchievementType::LocksPicked, 6, 1000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::LockTickler, AchievementType::LocksPicked, 7, 2000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::LockBane,    AchievementType::LocksPicked, 8, 5000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Versifier,       AchievementType::SongsPlayed, 1, 10,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  1,  0, 1),  0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Player,          AchievementType::SongsPlayed, 2, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  1,  0, 2),  0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Balladeer,       AchievementType::SongsPlayed, 3, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  2,  0, 3),  0, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Minstrel,        AchievementType::SongsPlayed, 4, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  2,  0, 4),  0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Instrumentalist, AchievementType::SongsPlayed, 5, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  3,  0, 5),  0, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Soloist,         AchievementType::SongsPlayed, 6, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  3,  0, 6),  1, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Artiste,         AchievementType::SongsPlayed, 7, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 8,  5,  0, 8),  1, 0, 50)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Virtuoso,        AchievementType::SongsPlayed, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 10, 0, 10), 2, 0, 60)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheTrippingToes,   AchievementType::SpiritsLifted, 1, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  0, 0, 1 ), 0, 0, 2 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheMerryMelody,    AchievementType::SpiritsLifted, 2, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  0, 0, 2 ), 0, 0, 4 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheWhimsicalWord,  AchievementType::SpiritsLifted, 3, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  1, 0, 3 ), 0, 0, 8 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheCarolingChorus, AchievementType::SpiritsLifted, 4, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  1, 0, 4 ), 0, 0, 12)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheBouncingBallad, AchievementType::SpiritsLifted, 5, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  2, 0, 5 ), 0, 0, 16)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheDancingDitty,   AchievementType::SpiritsLifted, 6, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  2, 0, 6 ), 0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheEnchantedEar,   AchievementType::SpiritsLifted, 7, 3000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 7,  3, 0, 7 ), 1, 0, 24)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BardOfTheAracneAria,     AchievementType::SpiritsLifted, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 5, 0, 10), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Targeter,        AchievementType::ProjectileHits, 1, 20,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  1,  0, 1,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SteadfastFlyer,  AchievementType::ProjectileHits, 2, 50,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  2,  0, 2,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DeadEye,         AchievementType::ProjectileHits, 3, 100,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  3,  0, 3,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DeadCenter,      AchievementType::ProjectileHits, 4, 500,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  4,  0, 4,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SureSail,        AchievementType::ProjectileHits, 5, 1000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  5,  0, 5,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SureShot,        AchievementType::ProjectileHits, 6, 2000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  6,  0, 6,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PerfectHit,      AchievementType::ProjectileHits, 7, 3000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(4,  8,  0, 8,  0, 0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::EnchantedAim,    AchievementType::ProjectileHits, 8, 50000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(10, 10, 0, 10, 0, 0), 1, 0, 0)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Magus,     AchievementType::SpellsCast, 1, 20,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 1,  0, 1),  0, 0, 2)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Charmer,   AchievementType::SpellsCast, 2, 50,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 2,  0, 2),  0, 0, 4)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Diviner,   AchievementType::SpellsCast, 3, 100,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 3,  0, 3),  0, 0, 8)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Enchanter, AchievementType::SpellsCast, 4, 200,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 4,  0, 4),  0, 0, 12)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Conjurer,  AchievementType::SpellsCast, 5, 500,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 5,  0, 5),  0, 0, 16)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Shaman,    AchievementType::SpellsCast, 6, 1000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 6,  0, 6),  1, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Wizard,    AchievementType::SpellsCast, 7, 2000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 8,  0, 8),  1, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Warlock,   AchievementType::SpellsCast, 8, 5000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 10, 0, 10), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Undaunted,     AchievementType::EnemiesFaced, 1, 20,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0,  0, 0, 1,  0, 0),  0, 0,  5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Daring,        AchievementType::EnemiesFaced, 2, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(1,  0, 0, 2,  0, 1),  0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Bold,          AchievementType::EnemiesFaced, 3, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(2,  0, 0, 3,  0, 2),  0, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Brave,         AchievementType::EnemiesFaced, 4, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(3,  0, 0, 4,  0, 3),  0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Valorous,      AchievementType::EnemiesFaced, 5, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(4,  0, 0, 5,  0, 4),  0, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Valiant,       AchievementType::EnemiesFaced, 6, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(5,  0, 0, 6,  0, 5),  0, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Lionhearted,   AchievementType::EnemiesFaced, 7, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(6,  0, 0, 7,  0, 6),  0, 0, 35)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Heroic,        AchievementType::EnemiesFaced, 8, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Fearless,      AchievementType::EnemiesFaced, 9, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheLunarPup,       AchievementType::MoonHowls, 1, 10,  RoleVec_t{ role::Wolfen }, stats::StatSet(1,  0, 0, 1,  0, 0), 0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheYoungHowler,    AchievementType::MoonHowls, 2, 20,  RoleVec_t{ role::Wolfen }, stats::StatSet(2,  0, 0, 2,  0, 0), 0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheNightSky,       AchievementType::MoonHowls, 3, 50,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  0, 0, 3,  0, 0), 0, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheMidnightWail,   AchievementType::MoonHowls, 4, 100, RoleVec_t{ role::Wolfen }, stats::StatSet(4,  0, 0, 4,  0, 0), 0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheTwilightMoon,   AchievementType::MoonHowls, 5, 200, RoleVec_t{ role::Wolfen }, stats::StatSet(5,  0, 0, 5,  0, 0), 1, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheCelestialCry,   AchievementType::MoonHowls, 6, 300, RoleVec_t{ role::Wolfen }, stats::StatSet(6,  0, 0, 6,  0, 0), 1, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheGlowingOrb,     AchievementType::MoonHowls, 7, 400, RoleVec_t{ role::Wolfen }, stats::StatSet(8,  0, 0, 8,  0, 0), 1, 0, 40)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PawOfTheCrescentLegion, AchievementType::MoonHowls, 8, 500, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 0, 0, 10, 0, 0), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::WindGlider,    AchievementType::TurnsInFlight, 1, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::LightFeather,  AchievementType::TurnsInFlight, 2, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::FreeFlyer,     AchievementType::TurnsInFlight, 3, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SteadyWing,    AchievementType::TurnsInFlight, 4, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::EverSoar,      AchievementType::TurnsInFlight, 5, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SkySail,       AchievementType::TurnsInFlight, 6, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Aerialist,     AchievementType::TurnsInFlight, 7, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::FeatherDancer, AchievementType::TurnsInFlight, 8, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 0)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackFollower,  AchievementType::PackActions, 1, 20,   RoleVec_t{ role::Wolfen }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackMember,    AchievementType::PackActions, 2, 50,   RoleVec_t{ role::Wolfen }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackTasker,    AchievementType::PackActions, 3, 100,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackCharger,   AchievementType::PackActions, 4, 200,  RoleVec_t{ role::Wolfen }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackNobel,     AchievementType::PackActions, 5, 300,  RoleVec_t{ role::Wolfen }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackLeader,    AchievementType::PackActions, 6, 400,  RoleVec_t{ role::Wolfen }, stats::StatSet(6,  6,  0, 0, 0, 0), 0, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackAlpha,     AchievementType::PackActions, 7, 500,  RoleVec_t{ role::Wolfen }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 40)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::PackElder,     AchievementType::PackActions, 8, 1000, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::CritterClairvoyant, AchievementType::BeastMindLinks, 1, 20,   RoleVec_t{ role::Beastmaster }, stats::StatSet(1,  0, 0, 1,  0, 1),  0, 0, 5)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::AnimalAnimator,     AchievementType::BeastMindLinks, 2, 50,   RoleVec_t{ role::Beastmaster }, stats::StatSet(2,  0, 0, 2,  0, 2),  0, 0, 10)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::FriendOfTheFeral,   AchievementType::BeastMindLinks, 3, 100,  RoleVec_t{ role::Beastmaster }, stats::StatSet(3,  0, 0, 3,  0, 3),  0, 0, 15)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::WillOfTheWild,      AchievementType::BeastMindLinks, 4, 200,  RoleVec_t{ role::Beastmaster }, stats::StatSet(4,  0, 0, 4,  0, 4),  0, 0, 20)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::CreatureChanneler,  AchievementType::BeastMindLinks, 5, 300,  RoleVec_t{ role::Beastmaster }, stats::StatSet(5,  0, 0, 5,  0, 5),  0, 0, 25)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::BeastTaskmaster,    AchievementType::BeastMindLinks, 6, 400,  RoleVec_t{ role::Beastmaster }, stats::StatSet(6,  0, 0, 6,  0, 6),  1, 0, 30)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MonsterManipulator, AchievementType::BeastMindLinks, 7, 500,  RoleVec_t{ role::Beastmaster }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MammalianMaster,    AchievementType::BeastMindLinks, 8, 1000, RoleVec_t{ role::Beastmaster }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Brawler,               AchievementType::MeleeHits, 1, 20,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5  )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::WildSwing,             AchievementType::MeleeHits, 2, 50,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::HavocSmasher,          AchievementType::MeleeHits, 3, 100,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SureStroke,            AchievementType::MeleeHits, 4, 200,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::Warrior,               AchievementType::MeleeHits, 5, 500,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::EverMark,              AchievementType::MeleeHits, 6, 1000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(6,  6,  0, 0, 0, 0), 1, 0, 30 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::MeleeMaster,           AchievementType::MeleeHits, 7, 2000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(7,  7,  0, 0, 0, 0), 1, 0, 40 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::SteadyStriker,         AchievementType::MeleeHits, 8, 3000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 50 )) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::OrderOfTheSteadyBlade, AchievementType::MeleeHits, 9, 50000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 100)) );

        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheUnblinkingEye,      AchievementType::FlyingAttackHits, 1, 20,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(1,  1,  0, 0, 1,  0),  0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheCurlingClaw,        AchievementType::FlyingAttackHits, 2, 50,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(2,  2,  0, 0, 2,  1),  0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheDaggerTooth,        AchievementType::FlyingAttackHits, 3, 100,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(3,  3,  0, 0, 3,  2),  0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheForebodingGaze,     AchievementType::FlyingAttackHits, 4, 200,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(4,  4,  0, 0, 4,  3),  0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheUnbreakableScale,   AchievementType::FlyingAttackHits, 5, 500,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(5,  5,  0, 0, 5,  4),  0, 0, 0)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheDreadedBreath,      AchievementType::FlyingAttackHits, 6, 1000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(6,  6,  0, 0, 6,  5),  0, 0, 1)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheArcaneWing,         AchievementType::FlyingAttackHits, 7, 2000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(8,  8,  0, 0, 8,  8),  0, 0, 1)) );
        titleUVec_.push_back( std::unique_ptr<Title>( new Title(Titles::DragonOfTheNightmareSky,       AchievementType::FlyingAttackHits, 8, 5000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(10, 10, 0, 0, 10, 10), 0, 0, 2)) );
    }


    void Warehouse::Empty()
    {
        titleUVec_.clear();
    }


    TitlePtr_t Warehouse::Get(const Titles::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(((E >= 0) && (E < Titles::Count)),                 "game::creature::Titles::Warehouse::Get(enum=" << E << ")_InvalidValueError.");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < titleUVec_.size()), "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E) << "\") enum given was " << E << " but that was greater than (or equal to) the vec size of " << titleUVec_.size() << ".");
        M_ASSERT_OR_LOGANDTHROW_SS((titleUVec_.empty() == false),                     "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E) << "\") called before vec was populated by Setup().");
        const TitlePtr_t TITLE_PTR(titleUVec_[static_cast<std::size_t>(E)].get());
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),                            "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E) << "\") found a nullptr in the vector.");
        return TITLE_PTR;
    }


    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend("game::creature::Titles::Warehouse::Test() Starting Tests...");
        }

        static auto titleIndex{ 0 };
        if (titleIndex < Titles::Count)
        {
            auto const NEXT_ENUM(static_cast<Titles::Enum>(titleIndex));
            auto TITLE_PTR{ Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),                          "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Desc().empty() == false),            "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->LongDesc().empty() == false),        "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->ImageFilename().empty() == false),   "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->RolesCopy().empty() == false),       "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in an empty RolesVec().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Which() == NEXT_ENUM),               "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in a Title with a different tile::Enum (\"" << Titles::ToString(TITLE_PTR->Which()) << "\")");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Name() == Titles::Name(NEXT_ENUM)),  "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") Title is out of order.");
            auto const TEXTURE_SPTR{ sfml_util::gui::TitleImageManager::Instance()->Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr),                 "game::creature::Titles::Warehouse::Test(\"" << Titles::ToString(NEXT_ENUM) << "\") resulted in an empty image/texture.");
            ++titleIndex;
            LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            LoopManager::Instance()->TestingStrIncrement("Title Test #");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::creature::Titles::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }

}
}
}
