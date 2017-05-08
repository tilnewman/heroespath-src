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

    TitleSVec_t Warehouse::titleSVec_;


    void Warehouse::Setup()
    {
        titleSVec_.resize(Titles::Count);

        titleSVec_[Titles::ProtectorOfThornberry] = std::make_shared<Title>(Titles::ProtectorOfThornberry, AchievementType::None, 0, 0, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(), 0, 500, 2);

        titleSVec_[Titles::Dodger]      = std::make_shared<Title>(Titles::Dodger,      AchievementType::DodgedStanding, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0));
        titleSVec_[Titles::FastFoot]    = std::make_shared<Title>(Titles::FastFoot,    AchievementType::DodgedStanding, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0));
        titleSVec_[Titles::NimbleFoot]  = std::make_shared<Title>(Titles::NimbleFoot,  AchievementType::DodgedStanding, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0));
        titleSVec_[Titles::SwiftFoot]   = std::make_shared<Title>(Titles::SwiftFoot,   AchievementType::DodgedStanding, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0));
        titleSVec_[Titles::MeleeMover]  = std::make_shared<Title>(Titles::MeleeMover,  AchievementType::DodgedStanding, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0));
        titleSVec_[Titles::MeleeGrace]  = std::make_shared<Title>(Titles::MeleeGrace,  AchievementType::DodgedStanding, 6, 2000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0));
        titleSVec_[Titles::MeleeDancer] = std::make_shared<Title>(Titles::MeleeDancer, AchievementType::DodgedStanding, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0));
        titleSVec_[Titles::MeleeShadow] = std::make_shared<Title>(Titles::MeleeShadow, AchievementType::DodgedStanding, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0));

        titleSVec_[Titles::SkyDodger]  = std::make_shared<Title>(Titles::SkyDodger,   AchievementType::DodgedFlying, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0));
        titleSVec_[Titles::FastWing]   = std::make_shared<Title>(Titles::FastWing,    AchievementType::DodgedFlying, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0));
        titleSVec_[Titles::NimbleWing] = std::make_shared<Title>(Titles::NimbleWing,  AchievementType::DodgedFlying, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0));
        titleSVec_[Titles::SwiftWing]  = std::make_shared<Title>(Titles::SwiftWing,   AchievementType::DodgedFlying, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0));
        titleSVec_[Titles::SkyMover]   = std::make_shared<Title>(Titles::SkyMover,    AchievementType::DodgedFlying, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0));
        titleSVec_[Titles::GraceWing]  = std::make_shared<Title>(Titles::GraceWing,   AchievementType::DodgedFlying, 6, 20000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0));
        titleSVec_[Titles::SkyDancer]  = std::make_shared<Title>(Titles::SkyDancer,   AchievementType::DodgedFlying, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0));
        titleSVec_[Titles::SkyShadow]  = std::make_shared<Title>(Titles::SkyShadow,   AchievementType::DodgedFlying, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0));

        titleSVec_[Titles::Sneak]         = std::make_shared<Title>(Titles::Sneak,        AchievementType::BackstabsHits, 1, 10,   RoleVec_t { role::Thief }, stats::StatSet(0, 1,  0, 1,  0, 0), 0, 0, 0);
        titleSVec_[Titles::Stalker]       = std::make_shared<Title>(Titles::Stalker,      AchievementType::BackstabsHits, 2, 50,   RoleVec_t { role::Thief }, stats::StatSet(0, 2,  0, 2,  0, 0), 0, 0, 0);
        titleSVec_[Titles::Creeper]       = std::make_shared<Title>(Titles::Creeper,      AchievementType::BackstabsHits, 3, 100,  RoleVec_t { role::Thief }, stats::StatSet(0, 3,  0, 3,  0, 0), 0, 0, 0);
        titleSVec_[Titles::Savage]        = std::make_shared<Title>(Titles::Savage,       AchievementType::BackstabsHits, 4, 200,  RoleVec_t { role::Thief }, stats::StatSet(0, 4,  0, 4,  0, 0), 0, 0, 1);
        titleSVec_[Titles::Homicidal]     = std::make_shared<Title>(Titles::Homicidal,    AchievementType::BackstabsHits, 5, 300,  RoleVec_t { role::Thief }, stats::StatSet(0, 5,  0, 5,  0, 0), 0, 0, 2);
        titleSVec_[Titles::Bloodthirsty]  = std::make_shared<Title>(Titles::Bloodthirsty, AchievementType::BackstabsHits, 6, 400,  RoleVec_t { role::Thief }, stats::StatSet(0, 6,  0, 6,  0, 0), 0, 0, 5);
        titleSVec_[Titles::ShadowWalker]  = std::make_shared<Title>(Titles::ShadowWalker, AchievementType::BackstabsHits, 7, 500,  RoleVec_t { role::Thief }, stats::StatSet(0, 7,  0, 7,  0, 0), 1, 0, 10);
        titleSVec_[Titles::Assassin]      = std::make_shared<Title>(Titles::Assassin,     AchievementType::BackstabsHits, 8, 1000, RoleVec_t { role::Thief }, stats::StatSet(0, 10, 0, 10, 0, 0), 2, 0, 20);

        titleSVec_[Titles::KnightOfTheClashingBlade]  = std::make_shared<Title>(Titles::KnightOfTheClashingBlade, AchievementType::BattlesSurvived, 1, 10, RoleVec_t{ role::Knight }, stats::StatSet(1, 1, 0, 0, 0, 0), 0, 0, 5);
        titleSVec_[Titles::KnightOfAHundredBattles]   = std::make_shared<Title>(Titles::KnightOfAHundredBattles, AchievementType::BattlesSurvived, 2, 100, RoleVec_t{ role::Knight }, stats::StatSet(2, 2, 0, 0, 0, 0), 0, 0, 10);
        titleSVec_[Titles::KnightOfThePiercingBlade]  = std::make_shared<Title>(Titles::KnightOfThePiercingBlade, AchievementType::BattlesSurvived, 3, 200, RoleVec_t{ role::Knight }, stats::StatSet(3, 3, 0, 0, 0, 0), 0, 0, 15);
        titleSVec_[Titles::KnightOfTheUndauntedBlade] = std::make_shared<Title>(Titles::KnightOfTheUndauntedBlade, AchievementType::BattlesSurvived, 4, 500, RoleVec_t{ role::Knight }, stats::StatSet(4, 4, 0, 0, 0, 0), 0, 0, 20);
        titleSVec_[Titles::KnightOfAThousandBattles]  = std::make_shared<Title>(Titles::KnightOfAThousandBattles, AchievementType::BattlesSurvived, 5, 1000, RoleVec_t{ role::Knight }, stats::StatSet(5, 5, 0, 0, 0, 0), 1, 0, 25);
        titleSVec_[Titles::KnightOfTheFearedBlade]    = std::make_shared<Title>(Titles::KnightOfTheFearedBlade, AchievementType::BattlesSurvived, 6, 2000, RoleVec_t{ role::Knight }, stats::StatSet(6, 6, 0, 0, 0, 0), 1, 0, 30);
        titleSVec_[Titles::KnightOfTheRealm]          = std::make_shared<Title>(Titles::KnightOfTheRealm, AchievementType::BattlesSurvived, 7, 3000, RoleVec_t{ role::Knight }, stats::StatSet(7, 7, 0, 0, 0, 0), 1, 0, 35);
        titleSVec_[Titles::KnightOfEtan]              = std::make_shared<Title>(Titles::KnightOfEtan,                AchievementType::BattlesSurvived, 8, 5000, RoleVec_t{ role::Knight }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50);

        titleSVec_[Titles::Mender]                = std::make_shared<Title>(Titles::Mender,                AchievementType::HealthGiven, 1, 50, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1, 0, 0, 1), 0, 0, 2);
        titleSVec_[Titles::Healer]                = std::make_shared<Title>(Titles::Healer,                AchievementType::HealthGiven, 2, 200, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2, 0, 0, 2), 0, 0, 5);
        titleSVec_[Titles::BlessedHands]          = std::make_shared<Title>(Titles::BlessedHands,          AchievementType::HealthGiven, 3, 500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3, 0, 0, 3), 0, 0, 10);
        titleSVec_[Titles::WitchDoctor]           = std::make_shared<Title>(Titles::WitchDoctor,           AchievementType::HealthGiven, 4, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4, 0, 0, 4), 1, 0, 15);
        titleSVec_[Titles::LightOfTheCure]        = std::make_shared<Title>(Titles::LightOfTheCure,        AchievementType::HealthGiven, 5, 2000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5, 0, 0, 5), 1, 0, 20);
        titleSVec_[Titles::DivineTouch]           = std::make_shared<Title>(Titles::DivineTouch,           AchievementType::HealthGiven, 6, 3000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6, 0, 0, 6), 1, 0, 25);
        titleSVec_[Titles::OrderOfTheWhiteSage]   = std::make_shared<Title>(Titles::OrderOfTheWhiteSage,   AchievementType::HealthGiven, 7, 5000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 8, 0, 0, 8), 1, 0, 35);
        titleSVec_[Titles::ClericOfTheCovenant]   = std::make_shared<Title>(Titles::ClericOfTheCovenant,   AchievementType::HealthGiven, 8, 10000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 50);

        titleSVec_[Titles::HandsOfCharity        ] = std::make_shared<Title>(Titles::HandsOfCharity,          AchievementType::HealthTraded, 1, 50,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1,  0, 0, 1),  0, 0, 2);
        titleSVec_[Titles::HandsOfLove           ] = std::make_shared<Title>(Titles::HandsOfLove,             AchievementType::HealthTraded, 2, 100,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2,  0, 0, 2),  0, 0, 4);
        titleSVec_[Titles::HandsOfDevotion       ] = std::make_shared<Title>(Titles::HandsOfDevotion,         AchievementType::HealthTraded, 3, 200,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3,  0, 0, 3),  0, 0, 8);
        titleSVec_[Titles::HandsOfNobility       ] = std::make_shared<Title>(Titles::HandsOfNobility,         AchievementType::HealthTraded, 4, 400,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4,  0, 0, 4),  0, 0, 12);
        titleSVec_[Titles::HandsOfSacrifice      ] = std::make_shared<Title>(Titles::HandsOfSacrifice,        AchievementType::HealthTraded, 5, 800,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5,  0, 0, 5),  0, 0, 16);
        titleSVec_[Titles::TheSelflessHand       ] = std::make_shared<Title>(Titles::TheSelflessHand,         AchievementType::HealthTraded, 6, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6,  0, 0, 6),  1, 0, 20);
        titleSVec_[Titles::TheIncorruptibleHand  ] = std::make_shared<Title>(Titles::TheIncorruptibleHand,    AchievementType::HealthTraded, 7, 2500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 7,  0, 0, 7),  1, 0, 24);
        titleSVec_[Titles::OrderOfTheBleedingPalm] = std::make_shared<Title>(Titles::OrderOfTheBleedingPalm,  AchievementType::HealthTraded, 8, 4000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 30);

        titleSVec_[Titles::HowlingStray    ] = std::make_shared<Title>(Titles::HowlingStray,     AchievementType::BeastRoars, 1, 10,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(1,  0, 0, 0, 0, 0), 0, 0, 2);
        titleSVec_[Titles::HowlingBully    ] = std::make_shared<Title>(Titles::HowlingBully,     AchievementType::BeastRoars, 2, 30,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(2,  0, 0, 0, 0, 0), 0, 0, 4);
        titleSVec_[Titles::HowlingWildling ] = std::make_shared<Title>(Titles::HowlingWildling,  AchievementType::BeastRoars, 3, 50,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(3,  0, 0, 0, 0, 0), 0, 0, 8);
        titleSVec_[Titles::SnarlingCreature] = std::make_shared<Title>(Titles::SnarlingCreature, AchievementType::BeastRoars, 4, 100,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(4,  0, 0, 0, 0, 0), 0, 0, 12);
        titleSVec_[Titles::SnarlingBeast   ] = std::make_shared<Title>(Titles::SnarlingBeast,    AchievementType::BeastRoars, 5, 200,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(5,  0, 0, 0, 0, 0), 0, 0, 16);
        titleSVec_[Titles::SnarlingFiend   ] = std::make_shared<Title>(Titles::SnarlingFiend,    AchievementType::BeastRoars, 6, 400,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(6,  0, 0, 0, 0, 0), 0, 0, 20);
        titleSVec_[Titles::RagingMonster   ] = std::make_shared<Title>(Titles::RagingMonster,    AchievementType::BeastRoars, 7, 1000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(8,  0, 0, 0, 0, 0), 1, 0, 24);
        titleSVec_[Titles::RagingHorror    ] = std::make_shared<Title>(Titles::RagingHorror,     AchievementType::BeastRoars, 8, 2000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(10, 0, 0, 0, 0, 0), 2, 0, 30);

        titleSVec_[Titles::Prowler    ] = std::make_shared<Title>(Titles::Prowler,     AchievementType::LocksPicked, 1, 10,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 2);
        titleSVec_[Titles::PickPocket ] = std::make_shared<Title>(Titles::PickPocket,  AchievementType::LocksPicked, 2, 50,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 4);
        titleSVec_[Titles::CatBurglar ] = std::make_shared<Title>(Titles::CatBurglar,  AchievementType::LocksPicked, 3, 100,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 8);
        titleSVec_[Titles::Clincher   ] = std::make_shared<Title>(Titles::Clincher,    AchievementType::LocksPicked, 4, 200,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 12);
        titleSVec_[Titles::Picker     ] = std::make_shared<Title>(Titles::Picker,      AchievementType::LocksPicked, 5, 500,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 16);
        titleSVec_[Titles::SafeCracker] = std::make_shared<Title>(Titles::SafeCracker, AchievementType::LocksPicked, 6, 1000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 20);
        titleSVec_[Titles::LockTickler] = std::make_shared<Title>(Titles::LockTickler, AchievementType::LocksPicked, 7, 2000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 30);
        titleSVec_[Titles::LockBane   ] = std::make_shared<Title>(Titles::LockBane,    AchievementType::LocksPicked, 8, 5000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 50);

        titleSVec_[Titles::Versifier      ] = std::make_shared<Title>(Titles::Versifier,       AchievementType::SongsPlayed, 1, 10,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  1,  0, 1),  0, 0, 5);
        titleSVec_[Titles::Player         ] = std::make_shared<Title>(Titles::Player,          AchievementType::SongsPlayed, 2, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  1,  0, 2),  0, 0, 10);
        titleSVec_[Titles::Balladeer      ] = std::make_shared<Title>(Titles::Balladeer,       AchievementType::SongsPlayed, 3, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  2,  0, 3),  0, 0, 15);
        titleSVec_[Titles::Minstrel       ] = std::make_shared<Title>(Titles::Minstrel,        AchievementType::SongsPlayed, 4, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  2,  0, 4),  0, 0, 20);
        titleSVec_[Titles::Instrumentalist] = std::make_shared<Title>(Titles::Instrumentalist, AchievementType::SongsPlayed, 5, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  3,  0, 5),  0, 0, 25);
        titleSVec_[Titles::Soloist        ] = std::make_shared<Title>(Titles::Soloist,         AchievementType::SongsPlayed, 6, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  3,  0, 6),  1, 0, 30);
        titleSVec_[Titles::Artiste        ] = std::make_shared<Title>(Titles::Artiste,         AchievementType::SongsPlayed, 7, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 8,  5,  0, 8),  1, 0, 50);
        titleSVec_[Titles::Virtuoso       ] = std::make_shared<Title>(Titles::Virtuoso,        AchievementType::SongsPlayed, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 10, 0, 10), 2, 0, 60);

        titleSVec_[Titles::BardOfTheTrippingToes  ] = std::make_shared<Title>(Titles::BardOfTheTrippingToes,   AchievementType::SpiritsLifted, 1, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  0, 0, 1 ), 0, 0, 2 );
        titleSVec_[Titles::BardOfTheMerryMelody   ] = std::make_shared<Title>(Titles::BardOfTheMerryMelody,    AchievementType::SpiritsLifted, 2, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  0, 0, 2 ), 0, 0, 4 );
        titleSVec_[Titles::BardOfTheWhimsicalWord ] = std::make_shared<Title>(Titles::BardOfTheWhimsicalWord,  AchievementType::SpiritsLifted, 3, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  1, 0, 3 ), 0, 0, 8 );
        titleSVec_[Titles::BardOfTheCarolingChorus] = std::make_shared<Title>(Titles::BardOfTheCarolingChorus, AchievementType::SpiritsLifted, 4, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  1, 0, 4 ), 0, 0, 12);
        titleSVec_[Titles::BardOfTheBouncingBallad] = std::make_shared<Title>(Titles::BardOfTheBouncingBallad, AchievementType::SpiritsLifted, 5, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  2, 0, 5 ), 0, 0, 16);
        titleSVec_[Titles::BardOfTheDancingDitty  ] = std::make_shared<Title>(Titles::BardOfTheDancingDitty,   AchievementType::SpiritsLifted, 6, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  2, 0, 6 ), 0, 0, 20);
        titleSVec_[Titles::BardOfTheEnchantedEar  ] = std::make_shared<Title>(Titles::BardOfTheEnchantedEar,   AchievementType::SpiritsLifted, 7, 3000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 7,  3, 0, 7 ), 1, 0, 24);
        titleSVec_[Titles::BardOfTheAracneAria    ] = std::make_shared<Title>(Titles::BardOfTheAracneAria,     AchievementType::SpiritsLifted, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 5, 0, 10), 2, 0, 50);

        titleSVec_[Titles::Targeter      ] = std::make_shared<Title>(Titles::Targeter,        AchievementType::ProjectileHits, 1, 20,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  1,  0, 1,  0, 0), 0, 0, 0);
        titleSVec_[Titles::SteadfastFlyer] = std::make_shared<Title>(Titles::SteadfastFlyer,  AchievementType::ProjectileHits, 2, 50,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  2,  0, 2,  0, 0), 0, 0, 0);
        titleSVec_[Titles::DeadEye       ] = std::make_shared<Title>(Titles::DeadEye,         AchievementType::ProjectileHits, 3, 100,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  3,  0, 3,  0, 0), 0, 0, 0);
        titleSVec_[Titles::DeadCenter    ] = std::make_shared<Title>(Titles::DeadCenter,      AchievementType::ProjectileHits, 4, 500,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  4,  0, 4,  0, 0), 0, 0, 0);
        titleSVec_[Titles::SureSail      ] = std::make_shared<Title>(Titles::SureSail,        AchievementType::ProjectileHits, 5, 1000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  5,  0, 5,  0, 0), 0, 0, 0);
        titleSVec_[Titles::SureShot      ] = std::make_shared<Title>(Titles::SureShot,        AchievementType::ProjectileHits, 6, 2000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  6,  0, 6,  0, 0), 0, 0, 0);
        titleSVec_[Titles::PerfectHit    ] = std::make_shared<Title>(Titles::PerfectHit,      AchievementType::ProjectileHits, 7, 3000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(4,  8,  0, 8,  0, 0), 0, 0, 0);
        titleSVec_[Titles::EnchantedAim  ] = std::make_shared<Title>(Titles::EnchantedAim,    AchievementType::ProjectileHits, 8, 50000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(10, 10, 0, 10, 0, 0), 1, 0, 0);

        titleSVec_[Titles::Magus]     = std::make_shared<Title>(Titles::Magus,     AchievementType::SpellsCast, 1, 20,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 1,  0, 1),  0, 0, 2);
        titleSVec_[Titles::Charmer]   = std::make_shared<Title>(Titles::Charmer,   AchievementType::SpellsCast, 2, 50,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 2,  0, 2),  0, 0, 4);
        titleSVec_[Titles::Diviner]   = std::make_shared<Title>(Titles::Diviner,   AchievementType::SpellsCast, 3, 100,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 3,  0, 3),  0, 0, 8);
        titleSVec_[Titles::Enchanter] = std::make_shared<Title>(Titles::Enchanter, AchievementType::SpellsCast, 4, 200,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 4,  0, 4),  0, 0, 12);
        titleSVec_[Titles::Conjurer]  = std::make_shared<Title>(Titles::Conjurer,  AchievementType::SpellsCast, 5, 500,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 5,  0, 5),  0, 0, 16);
        titleSVec_[Titles::Shaman]    = std::make_shared<Title>(Titles::Shaman,    AchievementType::SpellsCast, 6, 1000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 6,  0, 6),  1, 0, 20);
        titleSVec_[Titles::Wizard]    = std::make_shared<Title>(Titles::Wizard,    AchievementType::SpellsCast, 7, 2000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 8,  0, 8),  1, 0, 30);
        titleSVec_[Titles::Warlock]   = std::make_shared<Title>(Titles::Warlock,   AchievementType::SpellsCast, 8, 5000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 10, 0, 10), 2, 0, 50);

        titleSVec_[Titles::Undaunted]     = std::make_shared<Title>(Titles::Undaunted,     AchievementType::EnemiesFaced, 1, 20,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0,  0, 0, 1,  0, 0),  0, 0,  5);
        titleSVec_[Titles::Daring]        = std::make_shared<Title>(Titles::Daring,        AchievementType::EnemiesFaced, 2, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(1,  0, 0, 2,  0, 1),  0, 0, 10);
        titleSVec_[Titles::Bold]          = std::make_shared<Title>(Titles::Bold,          AchievementType::EnemiesFaced, 3, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(2,  0, 0, 3,  0, 2),  0, 0, 15);
        titleSVec_[Titles::Brave]         = std::make_shared<Title>(Titles::Brave,         AchievementType::EnemiesFaced, 4, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(3,  0, 0, 4,  0, 3),  0, 0, 20);
        titleSVec_[Titles::Valorous]      = std::make_shared<Title>(Titles::Valorous,      AchievementType::EnemiesFaced, 5, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(4,  0, 0, 5,  0, 4),  0, 0, 25);
        titleSVec_[Titles::Valiant]       = std::make_shared<Title>(Titles::Valiant,       AchievementType::EnemiesFaced, 6, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(5,  0, 0, 6,  0, 5),  0, 0, 30);
        titleSVec_[Titles::Lionhearted]   = std::make_shared<Title>(Titles::Lionhearted,   AchievementType::EnemiesFaced, 7, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(6,  0, 0, 7,  0, 6),  0, 0, 35);
        titleSVec_[Titles::Heroic]        = std::make_shared<Title>(Titles::Heroic,        AchievementType::EnemiesFaced, 8, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40);
        titleSVec_[Titles::Fearless]      = std::make_shared<Title>(Titles::Fearless,      AchievementType::EnemiesFaced, 9, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50);

        titleSVec_[Titles::PawOfTheLunarPup]          = std::make_shared<Title>(Titles::PawOfTheLunarPup,       AchievementType::MoonHowls, 1, 10,  RoleVec_t{ role::Wolfen }, stats::StatSet(1,  0, 0, 1,  0, 0), 0, 0, 5);
        titleSVec_[Titles::PawOfTheYoungHowler]       = std::make_shared<Title>(Titles::PawOfTheYoungHowler,    AchievementType::MoonHowls, 2, 20,  RoleVec_t{ role::Wolfen }, stats::StatSet(2,  0, 0, 2,  0, 0), 0, 0, 10);
        titleSVec_[Titles::PawOfTheNightSky]          = std::make_shared<Title>(Titles::PawOfTheNightSky,       AchievementType::MoonHowls, 3, 50,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  0, 0, 3,  0, 0), 0, 0, 15);
        titleSVec_[Titles::PawOfTheMidnightWail]      = std::make_shared<Title>(Titles::PawOfTheMidnightWail,   AchievementType::MoonHowls, 4, 100, RoleVec_t{ role::Wolfen }, stats::StatSet(4,  0, 0, 4,  0, 0), 0, 0, 20);
        titleSVec_[Titles::PawOfTheTwilightMoon]      = std::make_shared<Title>(Titles::PawOfTheTwilightMoon,   AchievementType::MoonHowls, 5, 200, RoleVec_t{ role::Wolfen }, stats::StatSet(5,  0, 0, 5,  0, 0), 1, 0, 25);
        titleSVec_[Titles::PawOfTheCelestialCry]      = std::make_shared<Title>(Titles::PawOfTheCelestialCry,   AchievementType::MoonHowls, 6, 300, RoleVec_t{ role::Wolfen }, stats::StatSet(6,  0, 0, 6,  0, 0), 1, 0, 30);
        titleSVec_[Titles::PawOfTheGlowingOrb]        = std::make_shared<Title>(Titles::PawOfTheGlowingOrb,     AchievementType::MoonHowls, 7, 400, RoleVec_t{ role::Wolfen }, stats::StatSet(8,  0, 0, 8,  0, 0), 1, 0, 40);
        titleSVec_[Titles::PawOfTheCrescentLegion]    = std::make_shared<Title>(Titles::PawOfTheCrescentLegion, AchievementType::MoonHowls, 8, 500, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 0, 0, 10, 0, 0), 2, 0, 50);

        titleSVec_[Titles::WindGlider]    = std::make_shared<Title>(Titles::WindGlider,    AchievementType::TurnsInFlight, 1, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 0);
        titleSVec_[Titles::LightFeather]  = std::make_shared<Title>(Titles::LightFeather,  AchievementType::TurnsInFlight, 2, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 0);
        titleSVec_[Titles::FreeFlyer]     = std::make_shared<Title>(Titles::FreeFlyer,     AchievementType::TurnsInFlight, 3, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 0);
        titleSVec_[Titles::SteadyWing]    = std::make_shared<Title>(Titles::SteadyWing,    AchievementType::TurnsInFlight, 4, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 0);
        titleSVec_[Titles::EverSoar]      = std::make_shared<Title>(Titles::EverSoar,      AchievementType::TurnsInFlight, 5, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 0);
        titleSVec_[Titles::SkySail]       = std::make_shared<Title>(Titles::SkySail,       AchievementType::TurnsInFlight, 6, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 0);
        titleSVec_[Titles::Aerialist]     = std::make_shared<Title>(Titles::Aerialist,     AchievementType::TurnsInFlight, 7, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 0);
        titleSVec_[Titles::FeatherDancer] = std::make_shared<Title>(Titles::FeatherDancer, AchievementType::TurnsInFlight, 8, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 0);

        titleSVec_[Titles::PackFollower]  = std::make_shared<Title>(Titles::PackFollower,  AchievementType::PackActions, 1, 20,   RoleVec_t{ role::Wolfen }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5);
        titleSVec_[Titles::PackMember]    = std::make_shared<Title>(Titles::PackMember,    AchievementType::PackActions, 2, 50,   RoleVec_t{ role::Wolfen }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10);
        titleSVec_[Titles::PackTasker]    = std::make_shared<Title>(Titles::PackTasker,    AchievementType::PackActions, 3, 100,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15);
        titleSVec_[Titles::PackCharger]   = std::make_shared<Title>(Titles::PackCharger,   AchievementType::PackActions, 4, 200,  RoleVec_t{ role::Wolfen }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20);
        titleSVec_[Titles::PackNobel]     = std::make_shared<Title>(Titles::PackNobel,     AchievementType::PackActions, 5, 300,  RoleVec_t{ role::Wolfen }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25);
        titleSVec_[Titles::PackLeader]    = std::make_shared<Title>(Titles::PackLeader,    AchievementType::PackActions, 6, 400,  RoleVec_t{ role::Wolfen }, stats::StatSet(6,  6,  0, 0, 0, 0), 0, 0, 30);
        titleSVec_[Titles::PackAlpha]     = std::make_shared<Title>(Titles::PackAlpha,     AchievementType::PackActions, 7, 500,  RoleVec_t{ role::Wolfen }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 40);
        titleSVec_[Titles::PackElder]     = std::make_shared<Title>(Titles::PackElder,     AchievementType::PackActions, 8, 1000, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50);

        titleSVec_[Titles::CritterClairvoyant]    = std::make_shared<Title>(Titles::CritterClairvoyant, AchievementType::BeastMindLinks, 1, 20,   RoleVec_t{ role::Beastmaster }, stats::StatSet(1,  0, 0, 1,  0, 1),  0, 0, 5);
        titleSVec_[Titles::AnimalAnimator]        = std::make_shared<Title>(Titles::AnimalAnimator,     AchievementType::BeastMindLinks, 2, 50,   RoleVec_t{ role::Beastmaster }, stats::StatSet(2,  0, 0, 2,  0, 2),  0, 0, 10);
        titleSVec_[Titles::FriendOfTheFeral]      = std::make_shared<Title>(Titles::FriendOfTheFeral,   AchievementType::BeastMindLinks, 3, 100,  RoleVec_t{ role::Beastmaster }, stats::StatSet(3,  0, 0, 3,  0, 3),  0, 0, 15);
        titleSVec_[Titles::WillOfTheWild]         = std::make_shared<Title>(Titles::WillOfTheWild,      AchievementType::BeastMindLinks, 4, 200,  RoleVec_t{ role::Beastmaster }, stats::StatSet(4,  0, 0, 4,  0, 4),  0, 0, 20);
        titleSVec_[Titles::CreatureChanneler]     = std::make_shared<Title>(Titles::CreatureChanneler,  AchievementType::BeastMindLinks, 5, 300,  RoleVec_t{ role::Beastmaster }, stats::StatSet(5,  0, 0, 5,  0, 5),  0, 0, 25);
        titleSVec_[Titles::BeastTaskmaster]       = std::make_shared<Title>(Titles::BeastTaskmaster,    AchievementType::BeastMindLinks, 6, 400,  RoleVec_t{ role::Beastmaster }, stats::StatSet(6,  0, 0, 6,  0, 6),  1, 0, 30);
        titleSVec_[Titles::MonsterManipulator]    = std::make_shared<Title>(Titles::MonsterManipulator, AchievementType::BeastMindLinks, 7, 500,  RoleVec_t{ role::Beastmaster }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40);
        titleSVec_[Titles::MammalianMaster]       = std::make_shared<Title>(Titles::MammalianMaster,    AchievementType::BeastMindLinks, 8, 1000, RoleVec_t{ role::Beastmaster }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50);

        titleSVec_[Titles::Brawler]               = std::make_shared<Title>(Titles::Brawler,               AchievementType::MeleeHits, 1, 20,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5  );
        titleSVec_[Titles::WildSwing]             = std::make_shared<Title>(Titles::WildSwing,             AchievementType::MeleeHits, 2, 50,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10 );
        titleSVec_[Titles::HavocSmasher]          = std::make_shared<Title>(Titles::HavocSmasher,          AchievementType::MeleeHits, 3, 100,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15 );
        titleSVec_[Titles::SureStroke]            = std::make_shared<Title>(Titles::SureStroke,            AchievementType::MeleeHits, 4, 200,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20 );
        titleSVec_[Titles::Warrior]               = std::make_shared<Title>(Titles::Warrior,               AchievementType::MeleeHits, 5, 500,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25 );
        titleSVec_[Titles::EverMark]              = std::make_shared<Title>(Titles::EverMark,              AchievementType::MeleeHits, 6, 1000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(6,  6,  0, 0, 0, 0), 1, 0, 30 );
        titleSVec_[Titles::MeleeMaster]           = std::make_shared<Title>(Titles::MeleeMaster,           AchievementType::MeleeHits, 7, 2000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(7,  7,  0, 0, 0, 0), 1, 0, 40 );
        titleSVec_[Titles::SteadyStriker]         = std::make_shared<Title>(Titles::SteadyStriker,         AchievementType::MeleeHits, 8, 3000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 50 );
        titleSVec_[Titles::OrderOfTheSteadyBlade] = std::make_shared<Title>(Titles::OrderOfTheSteadyBlade, AchievementType::MeleeHits, 9, 50000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 100);

        titleSVec_[Titles::DragonOfTheUnblinkingEye]      = std::make_shared<Title>(Titles::DragonOfTheUnblinkingEye,      AchievementType::FlyingAttackHits, 1, 20,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(1,  1,  0, 0, 1,  0),  0, 0, 0);
        titleSVec_[Titles::DragonOfTheCurlingClaw]        = std::make_shared<Title>(Titles::DragonOfTheCurlingClaw,        AchievementType::FlyingAttackHits, 2, 50,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(2,  2,  0, 0, 2,  1),  0, 0, 0);
        titleSVec_[Titles::DragonOfTheDaggerTooth]        = std::make_shared<Title>(Titles::DragonOfTheDaggerTooth,        AchievementType::FlyingAttackHits, 3, 100,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(3,  3,  0, 0, 3,  2),  0, 0, 0);
        titleSVec_[Titles::DragonOfTheForebodingGaze]     = std::make_shared<Title>(Titles::DragonOfTheForebodingGaze,     AchievementType::FlyingAttackHits, 4, 200,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(4,  4,  0, 0, 4,  3),  0, 0, 0);
        titleSVec_[Titles::DragonOfTheUnbreakableScale]   = std::make_shared<Title>(Titles::DragonOfTheUnbreakableScale,   AchievementType::FlyingAttackHits, 5, 500,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(5,  5,  0, 0, 5,  4),  0, 0, 0);
        titleSVec_[Titles::DragonOfTheDreadedBreath]      = std::make_shared<Title>(Titles::DragonOfTheDreadedBreath,      AchievementType::FlyingAttackHits, 6, 1000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(6,  6,  0, 0, 6,  5),  0, 0, 1);
        titleSVec_[Titles::DragonOfTheArcaneWing]         = std::make_shared<Title>(Titles::DragonOfTheArcaneWing,         AchievementType::FlyingAttackHits, 7, 2000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(8,  8,  0, 0, 8,  8),  0, 0, 1);
        titleSVec_[Titles::DragonOfTheNightmareSky]       = std::make_shared<Title>(Titles::DragonOfTheNightmareSky,       AchievementType::FlyingAttackHits, 8, 5000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(10, 10, 0, 0, 10, 10), 0, 0, 2);
    }


    TitlePtr_t Warehouse::Get(const Titles::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(((E >= 0) || (E < Titles::Count)),                 "game::creature::Titles::Warehouse::Get(enum=" << E << ")_InvalidValueError.");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < titleSVec_.size()), "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E) << "\") enum given was " << E << " but that was greater than (or equal to) the vec size of " << titleSVec_.size() << ".");
        M_ASSERT_OR_LOGANDTHROW_SS((titleSVec_.empty() == false),                     "game::creature::Titles::Warehouse::Get(\"" << Titles::ToString(E) << "\") called before vec was populated by Setup().");
        const TitlePtr_t TITLE_PTR(titleSVec_[E].get());
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
