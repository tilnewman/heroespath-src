//
// titles.cpp
//
#include "titles.hpp"

#include "sfml-util/gui/title-image-manager.hpp"

#include "game/creature/role.hpp"
#include "game/creature/title.hpp"
#include "utilz/assertlogandthrow.hpp"
#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"

#include <memory>
#include <sstream>
#include <exception>


namespace game
{
namespace creature
{
namespace title
{

    TitleUVec_t TitleWarehouse::titleUPtrVec_;


    void TitleWarehouse::Setup()
    {
        titleUPtrVec_.resize(title::Count);

        titleUPtrVec_[title::ProtectorOfThornberry] = std::make_unique<Title>(title::ProtectorOfThornberry, AchievementType::None, 0, 0, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(), 0, 500, 2);

        titleUPtrVec_[title::Dodger]      = std::make_unique<Title>(title::Dodger,      AchievementType::DodgedStanding, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0));
        titleUPtrVec_[title::FastFoot]    = std::make_unique<Title>(title::FastFoot,    AchievementType::DodgedStanding, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0));
        titleUPtrVec_[title::NimbleFoot]  = std::make_unique<Title>(title::NimbleFoot,  AchievementType::DodgedStanding, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0));
        titleUPtrVec_[title::SwiftFoot]   = std::make_unique<Title>(title::SwiftFoot,   AchievementType::DodgedStanding, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0));
        titleUPtrVec_[title::MeleeMover]  = std::make_unique<Title>(title::MeleeMover,  AchievementType::DodgedStanding, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0));
        titleUPtrVec_[title::MeleeGrace]  = std::make_unique<Title>(title::MeleeGrace,  AchievementType::DodgedStanding, 6, 2000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0));
        titleUPtrVec_[title::MeleeDancer] = std::make_unique<Title>(title::MeleeDancer, AchievementType::DodgedStanding, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0));
        titleUPtrVec_[title::MeleeShadow] = std::make_unique<Title>(title::MeleeShadow, AchievementType::DodgedStanding, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0));

        titleUPtrVec_[title::SkyDodger]  = std::make_unique<Title>(title::SkyDodger,   AchievementType::DodgedFlying, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0));
        titleUPtrVec_[title::FastWing]   = std::make_unique<Title>(title::FastWing,    AchievementType::DodgedFlying, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0));
        titleUPtrVec_[title::NimbleWing] = std::make_unique<Title>(title::NimbleWing,  AchievementType::DodgedFlying, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0));
        titleUPtrVec_[title::SwiftWing]  = std::make_unique<Title>(title::SwiftWing,   AchievementType::DodgedFlying, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0));
        titleUPtrVec_[title::SkyMover]   = std::make_unique<Title>(title::SkyMover,    AchievementType::DodgedFlying, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0));
        titleUPtrVec_[title::GraceWing]  = std::make_unique<Title>(title::GraceWing,   AchievementType::DodgedFlying, 6, 20000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0));
        titleUPtrVec_[title::SkyDancer]  = std::make_unique<Title>(title::SkyDancer,   AchievementType::DodgedFlying, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0));
        titleUPtrVec_[title::SkyShadow]  = std::make_unique<Title>(title::SkyShadow,   AchievementType::DodgedFlying, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0));

        titleUPtrVec_[title::Sneak]         = std::make_unique<Title>(title::Sneak,        AchievementType::BackstabsHits, 1, 10,   RoleVec_t { role::Thief }, stats::StatSet(0, 1,  0, 1,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::Stalker]       = std::make_unique<Title>(title::Stalker,      AchievementType::BackstabsHits, 2, 50,   RoleVec_t { role::Thief }, stats::StatSet(0, 2,  0, 2,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::Creeper]       = std::make_unique<Title>(title::Creeper,      AchievementType::BackstabsHits, 3, 100,  RoleVec_t { role::Thief }, stats::StatSet(0, 3,  0, 3,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::Savage]        = std::make_unique<Title>(title::Savage,       AchievementType::BackstabsHits, 4, 200,  RoleVec_t { role::Thief }, stats::StatSet(0, 4,  0, 4,  0, 0), 0, 0, 1);
        titleUPtrVec_[title::Homicidal]     = std::make_unique<Title>(title::Homicidal,    AchievementType::BackstabsHits, 5, 300,  RoleVec_t { role::Thief }, stats::StatSet(0, 5,  0, 5,  0, 0), 0, 0, 2);
        titleUPtrVec_[title::Bloodthirsty]  = std::make_unique<Title>(title::Bloodthirsty, AchievementType::BackstabsHits, 6, 400,  RoleVec_t { role::Thief }, stats::StatSet(0, 6,  0, 6,  0, 0), 0, 0, 5);
        titleUPtrVec_[title::ShadowWalker]  = std::make_unique<Title>(title::ShadowWalker, AchievementType::BackstabsHits, 7, 500,  RoleVec_t { role::Thief }, stats::StatSet(0, 7,  0, 7,  0, 0), 1, 0, 10);
        titleUPtrVec_[title::Assassin]      = std::make_unique<Title>(title::Assassin,     AchievementType::BackstabsHits, 8, 1000, RoleVec_t { role::Thief }, stats::StatSet(0, 10, 0, 10, 0, 0), 2, 0, 20);

        titleUPtrVec_[title::KnightOfTheClashingBlade]  = std::make_unique<Title>(title::KnightOfTheClashingBlade, AchievementType::BattlesSurvived, 1, 10, RoleVec_t{ role::Knight }, stats::StatSet(1, 1, 0, 0, 0, 0), 0, 0, 5);
        titleUPtrVec_[title::KnightOfAHundredBattles]   = std::make_unique<Title>(title::KnightOfAHundredBattles, AchievementType::BattlesSurvived, 2, 100, RoleVec_t{ role::Knight }, stats::StatSet(2, 2, 0, 0, 0, 0), 0, 0, 10);
        titleUPtrVec_[title::KnightOfThePiercingBlade]  = std::make_unique<Title>(title::KnightOfThePiercingBlade, AchievementType::BattlesSurvived, 3, 200, RoleVec_t{ role::Knight }, stats::StatSet(3, 3, 0, 0, 0, 0), 0, 0, 15);
        titleUPtrVec_[title::KnightOfTheUndauntedBlade] = std::make_unique<Title>(title::KnightOfTheUndauntedBlade, AchievementType::BattlesSurvived, 4, 500, RoleVec_t{ role::Knight }, stats::StatSet(4, 4, 0, 0, 0, 0), 0, 0, 20);
        titleUPtrVec_[title::KnightOfAThousandBattles]  = std::make_unique<Title>(title::KnightOfAThousandBattles, AchievementType::BattlesSurvived, 5, 1000, RoleVec_t{ role::Knight }, stats::StatSet(5, 5, 0, 0, 0, 0), 1, 0, 25);
        titleUPtrVec_[title::KnightOfTheFearedBlade]    = std::make_unique<Title>(title::KnightOfTheFearedBlade, AchievementType::BattlesSurvived, 6, 2000, RoleVec_t{ role::Knight }, stats::StatSet(6, 6, 0, 0, 0, 0), 1, 0, 30);
        titleUPtrVec_[title::KnightOfTheRealm]          = std::make_unique<Title>(title::KnightOfTheRealm, AchievementType::BattlesSurvived, 7, 3000, RoleVec_t{ role::Knight }, stats::StatSet(7, 7, 0, 0, 0, 0), 1, 0, 35);
        titleUPtrVec_[title::KnightOfEtan]              = std::make_unique<Title>(title::KnightOfEtan,                AchievementType::BattlesSurvived, 8, 5000, RoleVec_t{ role::Knight }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50);

        titleUPtrVec_[title::Mender]                = std::make_unique<Title>(title::Mender,                AchievementType::HealthGiven, 1, 50, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1, 0, 0, 1), 0, 0, 2);
        titleUPtrVec_[title::Healer]                = std::make_unique<Title>(title::Healer,                AchievementType::HealthGiven, 2, 200, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2, 0, 0, 2), 0, 0, 5);
        titleUPtrVec_[title::BlessedHands]          = std::make_unique<Title>(title::BlessedHands,          AchievementType::HealthGiven, 3, 500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3, 0, 0, 3), 0, 0, 10);
        titleUPtrVec_[title::WitchDoctor]           = std::make_unique<Title>(title::WitchDoctor,           AchievementType::HealthGiven, 4, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4, 0, 0, 4), 1, 0, 15);
        titleUPtrVec_[title::LightOfTheCure]        = std::make_unique<Title>(title::LightOfTheCure,        AchievementType::HealthGiven, 5, 2000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5, 0, 0, 5), 1, 0, 20);
        titleUPtrVec_[title::DivineTouch]           = std::make_unique<Title>(title::DivineTouch,           AchievementType::HealthGiven, 6, 3000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6, 0, 0, 6), 1, 0, 25);
        titleUPtrVec_[title::OrderOfTheWhiteSage]   = std::make_unique<Title>(title::OrderOfTheWhiteSage,   AchievementType::HealthGiven, 7, 5000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 8, 0, 0, 8), 1, 0, 35);
        titleUPtrVec_[title::ClericOfTheCovenant]   = std::make_unique<Title>(title::ClericOfTheCovenant,   AchievementType::HealthGiven, 8, 10000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 50);

        titleUPtrVec_[title::HandsOfCharity        ] = std::make_unique<Title>(title::HandsOfCharity,          AchievementType::HealthTraded, 1, 50,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1,  0, 0, 1),  0, 0, 2);
        titleUPtrVec_[title::HandsOfLove           ] = std::make_unique<Title>(title::HandsOfLove,             AchievementType::HealthTraded, 2, 100,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2,  0, 0, 2),  0, 0, 4);
        titleUPtrVec_[title::HandsOfDevotion       ] = std::make_unique<Title>(title::HandsOfDevotion,         AchievementType::HealthTraded, 3, 200,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3,  0, 0, 3),  0, 0, 8);
        titleUPtrVec_[title::HandsOfNobility       ] = std::make_unique<Title>(title::HandsOfNobility,         AchievementType::HealthTraded, 4, 400,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4,  0, 0, 4),  0, 0, 12);
        titleUPtrVec_[title::HandsOfSacrifice      ] = std::make_unique<Title>(title::HandsOfSacrifice,        AchievementType::HealthTraded, 5, 800,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5,  0, 0, 5),  0, 0, 16);
        titleUPtrVec_[title::TheSelflessHand       ] = std::make_unique<Title>(title::TheSelflessHand,         AchievementType::HealthTraded, 6, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6,  0, 0, 6),  1, 0, 20);
        titleUPtrVec_[title::TheIncorruptibleHand  ] = std::make_unique<Title>(title::TheIncorruptibleHand,    AchievementType::HealthTraded, 7, 2500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 7,  0, 0, 7),  1, 0, 24);
        titleUPtrVec_[title::OrderOfTheBleedingPalm] = std::make_unique<Title>(title::OrderOfTheBleedingPalm,  AchievementType::HealthTraded, 8, 4000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 30);

        titleUPtrVec_[title::HowlingStray    ] = std::make_unique<Title>(title::HowlingStray,     AchievementType::BeastRoars, 1, 10,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(1,  0, 0, 0, 0, 0), 0, 0, 2);
        titleUPtrVec_[title::HowlingBully    ] = std::make_unique<Title>(title::HowlingBully,     AchievementType::BeastRoars, 2, 30,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(2,  0, 0, 0, 0, 0), 0, 0, 4);
        titleUPtrVec_[title::HowlingWildling ] = std::make_unique<Title>(title::HowlingWildling,  AchievementType::BeastRoars, 3, 50,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(3,  0, 0, 0, 0, 0), 0, 0, 8);
        titleUPtrVec_[title::SnarlingCreature] = std::make_unique<Title>(title::SnarlingCreature, AchievementType::BeastRoars, 4, 100,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(4,  0, 0, 0, 0, 0), 0, 0, 12);
        titleUPtrVec_[title::SnarlingBeast   ] = std::make_unique<Title>(title::SnarlingBeast,    AchievementType::BeastRoars, 5, 200,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(5,  0, 0, 0, 0, 0), 0, 0, 16);
        titleUPtrVec_[title::SnarlingFiend   ] = std::make_unique<Title>(title::SnarlingFiend,    AchievementType::BeastRoars, 6, 400,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(6,  0, 0, 0, 0, 0), 0, 0, 20);
        titleUPtrVec_[title::RagingMonster   ] = std::make_unique<Title>(title::RagingMonster,    AchievementType::BeastRoars, 7, 1000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(8,  0, 0, 0, 0, 0), 1, 0, 24);
        titleUPtrVec_[title::RagingHorror    ] = std::make_unique<Title>(title::RagingHorror,     AchievementType::BeastRoars, 8, 2000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(10, 0, 0, 0, 0, 0), 2, 0, 30);

        titleUPtrVec_[title::Prowler    ] = std::make_unique<Title>(title::Prowler,     AchievementType::LocksPicked, 1, 10,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 2);
        titleUPtrVec_[title::PickPocket ] = std::make_unique<Title>(title::PickPocket,  AchievementType::LocksPicked, 2, 50,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 4);
        titleUPtrVec_[title::CatBurglar ] = std::make_unique<Title>(title::CatBurglar,  AchievementType::LocksPicked, 3, 100,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 8);
        titleUPtrVec_[title::Clincher   ] = std::make_unique<Title>(title::Clincher,    AchievementType::LocksPicked, 4, 200,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 12);
        titleUPtrVec_[title::Picker     ] = std::make_unique<Title>(title::Picker,      AchievementType::LocksPicked, 5, 500,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 16);
        titleUPtrVec_[title::SafeCracker] = std::make_unique<Title>(title::SafeCracker, AchievementType::LocksPicked, 6, 1000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 20);
        titleUPtrVec_[title::LockTickler] = std::make_unique<Title>(title::LockTickler, AchievementType::LocksPicked, 7, 2000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 30);
        titleUPtrVec_[title::LockBane   ] = std::make_unique<Title>(title::LockBane,    AchievementType::LocksPicked, 8, 5000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 50);

        titleUPtrVec_[title::Versifier      ] = std::make_unique<Title>(title::Versifier,       AchievementType::SongsPlayed, 1, 10,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  1,  0, 1),  0, 0, 5);
        titleUPtrVec_[title::Player         ] = std::make_unique<Title>(title::Player,          AchievementType::SongsPlayed, 2, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  1,  0, 2),  0, 0, 10);
        titleUPtrVec_[title::Balladeer      ] = std::make_unique<Title>(title::Balladeer,       AchievementType::SongsPlayed, 3, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  2,  0, 3),  0, 0, 15);
        titleUPtrVec_[title::Minstrel       ] = std::make_unique<Title>(title::Minstrel,        AchievementType::SongsPlayed, 4, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  2,  0, 4),  0, 0, 20);
        titleUPtrVec_[title::Instrumentalist] = std::make_unique<Title>(title::Instrumentalist, AchievementType::SongsPlayed, 5, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  3,  0, 5),  0, 0, 25);
        titleUPtrVec_[title::Soloist        ] = std::make_unique<Title>(title::Soloist,         AchievementType::SongsPlayed, 6, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  3,  0, 6),  1, 0, 30);
        titleUPtrVec_[title::Artiste        ] = std::make_unique<Title>(title::Artiste,         AchievementType::SongsPlayed, 7, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 8,  5,  0, 8),  1, 0, 50);
        titleUPtrVec_[title::Virtuoso       ] = std::make_unique<Title>(title::Virtuoso,        AchievementType::SongsPlayed, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 10, 0, 10), 2, 0, 60);

        titleUPtrVec_[title::BardOfTheTrippingToes  ] = std::make_unique<Title>(title::BardOfTheTrippingToes,   AchievementType::SpiritsLifted, 1, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  0, 0, 1 ), 0, 0, 2 );
        titleUPtrVec_[title::BardOfTheMerryMelody   ] = std::make_unique<Title>(title::BardOfTheMerryMelody,    AchievementType::SpiritsLifted, 2, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  0, 0, 2 ), 0, 0, 4 );
        titleUPtrVec_[title::BardOfTheWhimsicalWord ] = std::make_unique<Title>(title::BardOfTheWhimsicalWord,  AchievementType::SpiritsLifted, 3, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  1, 0, 3 ), 0, 0, 8 );
        titleUPtrVec_[title::BardOfTheCarolingChorus] = std::make_unique<Title>(title::BardOfTheCarolingChorus, AchievementType::SpiritsLifted, 4, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  1, 0, 4 ), 0, 0, 12);
        titleUPtrVec_[title::BardOfTheBouncingBallad] = std::make_unique<Title>(title::BardOfTheBouncingBallad, AchievementType::SpiritsLifted, 5, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  2, 0, 5 ), 0, 0, 16);
        titleUPtrVec_[title::BardOfTheDancingDitty  ] = std::make_unique<Title>(title::BardOfTheDancingDitty,   AchievementType::SpiritsLifted, 6, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  2, 0, 6 ), 0, 0, 20);
        titleUPtrVec_[title::BardOfTheEnchantedEar  ] = std::make_unique<Title>(title::BardOfTheEnchantedEar,   AchievementType::SpiritsLifted, 7, 3000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 7,  3, 0, 7 ), 1, 0, 24);
        titleUPtrVec_[title::BardOfTheAracneAria    ] = std::make_unique<Title>(title::BardOfTheAracneAria,     AchievementType::SpiritsLifted, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 5, 0, 10), 2, 0, 50);

        titleUPtrVec_[title::Targeter      ] = std::make_unique<Title>(title::Targeter,        AchievementType::ProjectileHits, 1, 20,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  1,  0, 1,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::SteadfastFlyer] = std::make_unique<Title>(title::SteadfastFlyer,  AchievementType::ProjectileHits, 2, 50,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  2,  0, 2,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::DeadEye       ] = std::make_unique<Title>(title::DeadEye,         AchievementType::ProjectileHits, 3, 100,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  3,  0, 3,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::DeadCenter    ] = std::make_unique<Title>(title::DeadCenter,      AchievementType::ProjectileHits, 4, 500,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  4,  0, 4,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::SureSail      ] = std::make_unique<Title>(title::SureSail,        AchievementType::ProjectileHits, 5, 1000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  5,  0, 5,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::SureShot      ] = std::make_unique<Title>(title::SureShot,        AchievementType::ProjectileHits, 6, 2000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  6,  0, 6,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::PerfectHit    ] = std::make_unique<Title>(title::PerfectHit,      AchievementType::ProjectileHits, 7, 3000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(4,  8,  0, 8,  0, 0), 0, 0, 0);
        titleUPtrVec_[title::EnchantedAim  ] = std::make_unique<Title>(title::EnchantedAim,    AchievementType::ProjectileHits, 8, 50000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(10, 10, 0, 10, 0, 0), 1, 0, 0);

        titleUPtrVec_[title::Magus]     = std::make_unique<Title>(title::Magus,     AchievementType::SpellsCast, 1, 20,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 1,  0, 1),  0, 0, 2);
        titleUPtrVec_[title::Charmer]   = std::make_unique<Title>(title::Charmer,   AchievementType::SpellsCast, 2, 50,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 2,  0, 2),  0, 0, 4);
        titleUPtrVec_[title::Diviner]   = std::make_unique<Title>(title::Diviner,   AchievementType::SpellsCast, 3, 100,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 3,  0, 3),  0, 0, 8);
        titleUPtrVec_[title::Enchanter] = std::make_unique<Title>(title::Enchanter, AchievementType::SpellsCast, 4, 200,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 4,  0, 4),  0, 0, 12);
        titleUPtrVec_[title::Conjurer]  = std::make_unique<Title>(title::Conjurer,  AchievementType::SpellsCast, 5, 500,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 5,  0, 5),  0, 0, 16);
        titleUPtrVec_[title::Shaman]    = std::make_unique<Title>(title::Shaman,    AchievementType::SpellsCast, 6, 1000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 6,  0, 6),  1, 0, 20);
        titleUPtrVec_[title::Wizard]    = std::make_unique<Title>(title::Wizard,    AchievementType::SpellsCast, 7, 2000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 8,  0, 8),  1, 0, 30);
        titleUPtrVec_[title::Warlock]   = std::make_unique<Title>(title::Warlock,   AchievementType::SpellsCast, 8, 5000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 10, 0, 10), 2, 0, 50);

        titleUPtrVec_[title::Undaunted]     = std::make_unique<Title>(title::Undaunted,     AchievementType::EnemiesFaced, 1, 20,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0,  0, 0, 1,  0, 0),  0, 0,  5);
        titleUPtrVec_[title::Daring]        = std::make_unique<Title>(title::Daring,        AchievementType::EnemiesFaced, 2, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(1,  0, 0, 2,  0, 1),  0, 0, 10);
        titleUPtrVec_[title::Bold]          = std::make_unique<Title>(title::Bold,          AchievementType::EnemiesFaced, 3, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(2,  0, 0, 3,  0, 2),  0, 0, 15);
        titleUPtrVec_[title::Brave]         = std::make_unique<Title>(title::Brave,         AchievementType::EnemiesFaced, 4, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(3,  0, 0, 4,  0, 3),  0, 0, 20);
        titleUPtrVec_[title::Valorous]      = std::make_unique<Title>(title::Valorous,      AchievementType::EnemiesFaced, 5, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(4,  0, 0, 5,  0, 4),  0, 0, 25);
        titleUPtrVec_[title::Valiant]       = std::make_unique<Title>(title::Valiant,       AchievementType::EnemiesFaced, 6, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(5,  0, 0, 6,  0, 5),  0, 0, 30);
        titleUPtrVec_[title::Lionhearted]   = std::make_unique<Title>(title::Lionhearted,   AchievementType::EnemiesFaced, 7, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(6,  0, 0, 7,  0, 6),  0, 0, 35);
        titleUPtrVec_[title::Heroic]        = std::make_unique<Title>(title::Heroic,        AchievementType::EnemiesFaced, 8, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40);
        titleUPtrVec_[title::Fearless]      = std::make_unique<Title>(title::Fearless,      AchievementType::EnemiesFaced, 9, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50);

        titleUPtrVec_[title::PawOfTheLunarPup]          = std::make_unique<Title>(title::PawOfTheLunarPup,       AchievementType::MoonHowls, 1, 10,  RoleVec_t{ role::Wolfen }, stats::StatSet(1,  0, 0, 1,  0, 0), 0, 0, 5);
        titleUPtrVec_[title::PawOfTheYoungHowler]       = std::make_unique<Title>(title::PawOfTheYoungHowler,    AchievementType::MoonHowls, 2, 20,  RoleVec_t{ role::Wolfen }, stats::StatSet(2,  0, 0, 2,  0, 0), 0, 0, 10);
        titleUPtrVec_[title::PawOfTheNightSky]          = std::make_unique<Title>(title::PawOfTheNightSky,       AchievementType::MoonHowls, 3, 50,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  0, 0, 3,  0, 0), 0, 0, 15);
        titleUPtrVec_[title::PawOfTheMidnightWail]      = std::make_unique<Title>(title::PawOfTheMidnightWail,   AchievementType::MoonHowls, 4, 100, RoleVec_t{ role::Wolfen }, stats::StatSet(4,  0, 0, 4,  0, 0), 0, 0, 20);
        titleUPtrVec_[title::PawOfTheTwilightMoon]      = std::make_unique<Title>(title::PawOfTheTwilightMoon,   AchievementType::MoonHowls, 5, 200, RoleVec_t{ role::Wolfen }, stats::StatSet(5,  0, 0, 5,  0, 0), 1, 0, 25);
        titleUPtrVec_[title::PawOfTheCelestialCry]      = std::make_unique<Title>(title::PawOfTheCelestialCry,   AchievementType::MoonHowls, 6, 300, RoleVec_t{ role::Wolfen }, stats::StatSet(6,  0, 0, 6,  0, 0), 1, 0, 30);
        titleUPtrVec_[title::PawOfTheGlowingOrb]        = std::make_unique<Title>(title::PawOfTheGlowingOrb,     AchievementType::MoonHowls, 7, 400, RoleVec_t{ role::Wolfen }, stats::StatSet(8,  0, 0, 8,  0, 0), 1, 0, 40);
        titleUPtrVec_[title::PawOfTheCrescentLegion]    = std::make_unique<Title>(title::PawOfTheCrescentLegion, AchievementType::MoonHowls, 8, 500, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 0, 0, 10, 0, 0), 2, 0, 50);

        titleUPtrVec_[title::WindGlider]    = std::make_unique<Title>(title::WindGlider,    AchievementType::TurnsInFlight, 1, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 0);
        titleUPtrVec_[title::LightFeather]  = std::make_unique<Title>(title::LightFeather,  AchievementType::TurnsInFlight, 2, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 0);
        titleUPtrVec_[title::FreeFlyer]     = std::make_unique<Title>(title::FreeFlyer,     AchievementType::TurnsInFlight, 3, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 0);
        titleUPtrVec_[title::SteadyWing]    = std::make_unique<Title>(title::SteadyWing,    AchievementType::TurnsInFlight, 4, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 0);
        titleUPtrVec_[title::EverSoar]      = std::make_unique<Title>(title::EverSoar,      AchievementType::TurnsInFlight, 5, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 0);
        titleUPtrVec_[title::SkySail]       = std::make_unique<Title>(title::SkySail,       AchievementType::TurnsInFlight, 6, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 0);
        titleUPtrVec_[title::Aerialist]     = std::make_unique<Title>(title::Aerialist,     AchievementType::TurnsInFlight, 7, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 0);
        titleUPtrVec_[title::FeatherDancer] = std::make_unique<Title>(title::FeatherDancer, AchievementType::TurnsInFlight, 8, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 0);

        titleUPtrVec_[title::PackFollower]  = std::make_unique<Title>(title::PackFollower,  AchievementType::PackActions, 1, 20,   RoleVec_t{ role::Wolfen }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5);
        titleUPtrVec_[title::PackMember]    = std::make_unique<Title>(title::PackMember,    AchievementType::PackActions, 2, 50,   RoleVec_t{ role::Wolfen }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10);
        titleUPtrVec_[title::PackTasker]    = std::make_unique<Title>(title::PackTasker,    AchievementType::PackActions, 3, 100,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15);
        titleUPtrVec_[title::PackCharger]   = std::make_unique<Title>(title::PackCharger,   AchievementType::PackActions, 4, 200,  RoleVec_t{ role::Wolfen }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20);
        titleUPtrVec_[title::PackNobel]     = std::make_unique<Title>(title::PackNobel,     AchievementType::PackActions, 5, 300,  RoleVec_t{ role::Wolfen }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25);
        titleUPtrVec_[title::PackLeader]    = std::make_unique<Title>(title::PackLeader,    AchievementType::PackActions, 6, 400,  RoleVec_t{ role::Wolfen }, stats::StatSet(6,  6,  0, 0, 0, 0), 0, 0, 30);
        titleUPtrVec_[title::PackAlpha]     = std::make_unique<Title>(title::PackAlpha,     AchievementType::PackActions, 7, 500,  RoleVec_t{ role::Wolfen }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 40);
        titleUPtrVec_[title::PackElder]     = std::make_unique<Title>(title::PackElder,     AchievementType::PackActions, 8, 1000, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50);

        titleUPtrVec_[title::CritterClairvoyant]    = std::make_unique<Title>(title::CritterClairvoyant, AchievementType::BeastMindLinks, 1, 20,   RoleVec_t{ role::Beastmaster }, stats::StatSet(1,  0, 0, 1,  0, 1),  0, 0, 5);
        titleUPtrVec_[title::AnimalAnimator]        = std::make_unique<Title>(title::AnimalAnimator,     AchievementType::BeastMindLinks, 2, 50,   RoleVec_t{ role::Beastmaster }, stats::StatSet(2,  0, 0, 2,  0, 2),  0, 0, 10);
        titleUPtrVec_[title::FriendOfTheFeral]      = std::make_unique<Title>(title::FriendOfTheFeral,   AchievementType::BeastMindLinks, 3, 100,  RoleVec_t{ role::Beastmaster }, stats::StatSet(3,  0, 0, 3,  0, 3),  0, 0, 15);
        titleUPtrVec_[title::WillOfTheWild]         = std::make_unique<Title>(title::WillOfTheWild,      AchievementType::BeastMindLinks, 4, 200,  RoleVec_t{ role::Beastmaster }, stats::StatSet(4,  0, 0, 4,  0, 4),  0, 0, 20);
        titleUPtrVec_[title::CreatureChanneler]     = std::make_unique<Title>(title::CreatureChanneler,  AchievementType::BeastMindLinks, 5, 300,  RoleVec_t{ role::Beastmaster }, stats::StatSet(5,  0, 0, 5,  0, 5),  0, 0, 25);
        titleUPtrVec_[title::BeastTaskmaster]       = std::make_unique<Title>(title::BeastTaskmaster,    AchievementType::BeastMindLinks, 6, 400,  RoleVec_t{ role::Beastmaster }, stats::StatSet(6,  0, 0, 6,  0, 6),  1, 0, 30);
        titleUPtrVec_[title::MonsterManipulator]    = std::make_unique<Title>(title::MonsterManipulator, AchievementType::BeastMindLinks, 7, 500,  RoleVec_t{ role::Beastmaster }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40);
        titleUPtrVec_[title::MammalianMaster]       = std::make_unique<Title>(title::MammalianMaster,    AchievementType::BeastMindLinks, 8, 1000, RoleVec_t{ role::Beastmaster }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50);

        titleUPtrVec_[title::Brawler]               = std::make_unique<Title>(title::Brawler,               AchievementType::MeleeHits, 1, 20,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5  );
        titleUPtrVec_[title::WildSwing]             = std::make_unique<Title>(title::WildSwing,             AchievementType::MeleeHits, 2, 50,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10 );
        titleUPtrVec_[title::HavocSmasher]          = std::make_unique<Title>(title::HavocSmasher,          AchievementType::MeleeHits, 3, 100,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15 );
        titleUPtrVec_[title::SureStroke]            = std::make_unique<Title>(title::SureStroke,            AchievementType::MeleeHits, 4, 200,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20 );
        titleUPtrVec_[title::Warrior]               = std::make_unique<Title>(title::Warrior,               AchievementType::MeleeHits, 5, 500,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25 );
        titleUPtrVec_[title::EverMark]              = std::make_unique<Title>(title::EverMark,              AchievementType::MeleeHits, 6, 1000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(6,  6,  0, 0, 0, 0), 1, 0, 30 );
        titleUPtrVec_[title::MeleeMaster]           = std::make_unique<Title>(title::MeleeMaster,           AchievementType::MeleeHits, 7, 2000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(7,  7,  0, 0, 0, 0), 1, 0, 40 );
        titleUPtrVec_[title::SteadyStriker]         = std::make_unique<Title>(title::SteadyStriker,         AchievementType::MeleeHits, 8, 3000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 50 );
        titleUPtrVec_[title::OrderOfTheSteadyBlade] = std::make_unique<Title>(title::OrderOfTheSteadyBlade, AchievementType::MeleeHits, 9, 50000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 100);

        titleUPtrVec_[title::DragonOfTheUnblinkingEye]      = std::make_unique<Title>(title::DragonOfTheUnblinkingEye,      AchievementType::FlyingAttackHits, 1, 20,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(1,  1,  0, 0, 1,  0),  0, 0, 0);
        titleUPtrVec_[title::DragonOfTheCurlingClaw]        = std::make_unique<Title>(title::DragonOfTheCurlingClaw,        AchievementType::FlyingAttackHits, 2, 50,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(2,  2,  0, 0, 2,  1),  0, 0, 0);
        titleUPtrVec_[title::DragonOfTheDaggerTooth]        = std::make_unique<Title>(title::DragonOfTheDaggerTooth,        AchievementType::FlyingAttackHits, 3, 100,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(3,  3,  0, 0, 3,  2),  0, 0, 0);
        titleUPtrVec_[title::DragonOfTheForebodingGaze]     = std::make_unique<Title>(title::DragonOfTheForebodingGaze,     AchievementType::FlyingAttackHits, 4, 200,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(4,  4,  0, 0, 4,  3),  0, 0, 0);
        titleUPtrVec_[title::DragonOfTheUnbreakableScale]   = std::make_unique<Title>(title::DragonOfTheUnbreakableScale,   AchievementType::FlyingAttackHits, 5, 500,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(5,  5,  0, 0, 5,  4),  0, 0, 0);
        titleUPtrVec_[title::DragonOfTheDreadedBreath]      = std::make_unique<Title>(title::DragonOfTheDreadedBreath,      AchievementType::FlyingAttackHits, 6, 1000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(6,  6,  0, 0, 6,  5),  0, 0, 1);
        titleUPtrVec_[title::DragonOfTheArcaneWing]         = std::make_unique<Title>(title::DragonOfTheArcaneWing,         AchievementType::FlyingAttackHits, 7, 2000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(8,  8,  0, 0, 8,  8),  0, 0, 1);
        titleUPtrVec_[title::DragonOfTheNightmareSky]       = std::make_unique<Title>(title::DragonOfTheNightmareSky,       AchievementType::FlyingAttackHits, 8, 5000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(10, 10, 0, 0, 10, 10), 0, 0, 2);
    }


    TitlePtr_t TitleWarehouse::Get(const title::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(((E >= 0) || (E < title::Count)),                     "game::creature::title::TitleWarehouse::Get(enum=" << E << ")_InvalidValueError.");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < titleUPtrVec_.size()), "game::creature::title::TitleWarehouse::Get(\"" << title::ToString(E) << "\") enum given was " << E << " but that was greater than (or equal to) the vec size of " << titleUPtrVec_.size() << ".");
        M_ASSERT_OR_LOGANDTHROW_SS((titleUPtrVec_.empty() == false),                     "game::creature::title::TitleWarehouse::Get(\"" << title::ToString(E) << "\") called before vec was populated by Setup().");
        const TitlePtr_t TITLE_PTR(titleUPtrVec_[E].get());
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),                               "game::creature::title::TitleWarehouse::Get(\"" << title::ToString(E) << "\") found a nullptr in the vector.");
        return TITLE_PTR;
    }


    bool TitleWarehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend("game::creature::title::TitleWarehouse::Test() Starting Tests...");
        }

        static auto titleIndex{ 0 };
        if (titleIndex < title::Count)
        {
            auto const NEXT_ENUM(static_cast<title::Enum>(titleIndex));
            TitleCPtrC_t TITLE_PTR(Get(NEXT_ENUM));
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),                          "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Desc().empty() == false),            "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->LongDesc().empty() == false),        "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->ImageFilename().empty() == false),   "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->RolesCopy().empty() == false),       "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty RolesVec().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Which() == NEXT_ENUM),               "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in a Title with a different tile::Enum (\"" << title::ToString(TITLE_PTR->Which()) << "\")");
            auto const TEXTURE_SPTR{ sfml_util::gui::TitleImageManager::Instance()->Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr),                 "game::creature::title::TitleWarehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty image/texture.");
            ++titleIndex;
            LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            LoopManager::Instance()->TestingStrIncrement("Title Test #");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::creature::title::TitleWarehouse::Test()  ALL TESTS PASSED.");
        return true;
    }

}
}
}
