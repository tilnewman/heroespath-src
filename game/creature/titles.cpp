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

    TitleSVec_t Warehouse::titleSVec_;


    void Warehouse::Setup()
    {
        titleSVec_.resize(title::Count);

        titleSVec_[title::ProtectorOfThornberry] = std::make_shared<Title>(title::ProtectorOfThornberry, AchievementType::None, 0, 0, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(), 0, 500, 2);

        titleSVec_[title::Dodger]      = std::make_shared<Title>(title::Dodger,      AchievementType::DodgedStanding, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0));
        titleSVec_[title::FastFoot]    = std::make_shared<Title>(title::FastFoot,    AchievementType::DodgedStanding, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0));
        titleSVec_[title::NimbleFoot]  = std::make_shared<Title>(title::NimbleFoot,  AchievementType::DodgedStanding, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0));
        titleSVec_[title::SwiftFoot]   = std::make_shared<Title>(title::SwiftFoot,   AchievementType::DodgedStanding, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0));
        titleSVec_[title::MeleeMover]  = std::make_shared<Title>(title::MeleeMover,  AchievementType::DodgedStanding, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0));
        titleSVec_[title::MeleeGrace]  = std::make_shared<Title>(title::MeleeGrace,  AchievementType::DodgedStanding, 6, 2000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0));
        titleSVec_[title::MeleeDancer] = std::make_shared<Title>(title::MeleeDancer, AchievementType::DodgedStanding, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0));
        titleSVec_[title::MeleeShadow] = std::make_shared<Title>(title::MeleeShadow, AchievementType::DodgedStanding, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0));

        titleSVec_[title::SkyDodger]  = std::make_shared<Title>(title::SkyDodger,   AchievementType::DodgedFlying, 1, 50,    Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 1,  0));
        titleSVec_[title::FastWing]   = std::make_shared<Title>(title::FastWing,    AchievementType::DodgedFlying, 2, 100,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 2,  0));
        titleSVec_[title::NimbleWing] = std::make_shared<Title>(title::NimbleWing,  AchievementType::DodgedFlying, 3, 200,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 0, 3,  0));
        titleSVec_[title::SwiftWing]  = std::make_shared<Title>(title::SwiftWing,   AchievementType::DodgedFlying, 4, 500,   Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 1, 4,  0));
        titleSVec_[title::SkyMover]   = std::make_shared<Title>(title::SkyMover,    AchievementType::DodgedFlying, 5, 1000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 2, 5,  0));
        titleSVec_[title::GraceWing]  = std::make_shared<Title>(title::GraceWing,   AchievementType::DodgedFlying, 6, 20000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 3, 6,  0));
        titleSVec_[title::SkyDancer]  = std::make_shared<Title>(title::SkyDancer,   AchievementType::DodgedFlying, 7, 5000,  Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 4, 7,  0));
        titleSVec_[title::SkyShadow]  = std::make_shared<Title>(title::SkyShadow,   AchievementType::DodgedFlying, 8, 10000, Role::ROLEVEC_ALL_PLAYER_ROLES_, stats::StatSet(0, 0, 0, 7, 10, 0));

        titleSVec_[title::Sneak]         = std::make_shared<Title>(title::Sneak,        AchievementType::BackstabsHits, 1, 10,   RoleVec_t { role::Thief }, stats::StatSet(0, 1,  0, 1,  0, 0), 0, 0, 0);
        titleSVec_[title::Stalker]       = std::make_shared<Title>(title::Stalker,      AchievementType::BackstabsHits, 2, 50,   RoleVec_t { role::Thief }, stats::StatSet(0, 2,  0, 2,  0, 0), 0, 0, 0);
        titleSVec_[title::Creeper]       = std::make_shared<Title>(title::Creeper,      AchievementType::BackstabsHits, 3, 100,  RoleVec_t { role::Thief }, stats::StatSet(0, 3,  0, 3,  0, 0), 0, 0, 0);
        titleSVec_[title::Savage]        = std::make_shared<Title>(title::Savage,       AchievementType::BackstabsHits, 4, 200,  RoleVec_t { role::Thief }, stats::StatSet(0, 4,  0, 4,  0, 0), 0, 0, 1);
        titleSVec_[title::Homicidal]     = std::make_shared<Title>(title::Homicidal,    AchievementType::BackstabsHits, 5, 300,  RoleVec_t { role::Thief }, stats::StatSet(0, 5,  0, 5,  0, 0), 0, 0, 2);
        titleSVec_[title::Bloodthirsty]  = std::make_shared<Title>(title::Bloodthirsty, AchievementType::BackstabsHits, 6, 400,  RoleVec_t { role::Thief }, stats::StatSet(0, 6,  0, 6,  0, 0), 0, 0, 5);
        titleSVec_[title::ShadowWalker]  = std::make_shared<Title>(title::ShadowWalker, AchievementType::BackstabsHits, 7, 500,  RoleVec_t { role::Thief }, stats::StatSet(0, 7,  0, 7,  0, 0), 1, 0, 10);
        titleSVec_[title::Assassin]      = std::make_shared<Title>(title::Assassin,     AchievementType::BackstabsHits, 8, 1000, RoleVec_t { role::Thief }, stats::StatSet(0, 10, 0, 10, 0, 0), 2, 0, 20);

        titleSVec_[title::KnightOfTheClashingBlade]  = std::make_shared<Title>(title::KnightOfTheClashingBlade, AchievementType::BattlesSurvived, 1, 10, RoleVec_t{ role::Knight }, stats::StatSet(1, 1, 0, 0, 0, 0), 0, 0, 5);
        titleSVec_[title::KnightOfAHundredBattles]   = std::make_shared<Title>(title::KnightOfAHundredBattles, AchievementType::BattlesSurvived, 2, 100, RoleVec_t{ role::Knight }, stats::StatSet(2, 2, 0, 0, 0, 0), 0, 0, 10);
        titleSVec_[title::KnightOfThePiercingBlade]  = std::make_shared<Title>(title::KnightOfThePiercingBlade, AchievementType::BattlesSurvived, 3, 200, RoleVec_t{ role::Knight }, stats::StatSet(3, 3, 0, 0, 0, 0), 0, 0, 15);
        titleSVec_[title::KnightOfTheUndauntedBlade] = std::make_shared<Title>(title::KnightOfTheUndauntedBlade, AchievementType::BattlesSurvived, 4, 500, RoleVec_t{ role::Knight }, stats::StatSet(4, 4, 0, 0, 0, 0), 0, 0, 20);
        titleSVec_[title::KnightOfAThousandBattles]  = std::make_shared<Title>(title::KnightOfAThousandBattles, AchievementType::BattlesSurvived, 5, 1000, RoleVec_t{ role::Knight }, stats::StatSet(5, 5, 0, 0, 0, 0), 1, 0, 25);
        titleSVec_[title::KnightOfTheFearedBlade]    = std::make_shared<Title>(title::KnightOfTheFearedBlade, AchievementType::BattlesSurvived, 6, 2000, RoleVec_t{ role::Knight }, stats::StatSet(6, 6, 0, 0, 0, 0), 1, 0, 30);
        titleSVec_[title::KnightOfTheRealm]          = std::make_shared<Title>(title::KnightOfTheRealm, AchievementType::BattlesSurvived, 7, 3000, RoleVec_t{ role::Knight }, stats::StatSet(7, 7, 0, 0, 0, 0), 1, 0, 35);
        titleSVec_[title::KnightOfEtan]              = std::make_shared<Title>(title::KnightOfEtan,                AchievementType::BattlesSurvived, 8, 5000, RoleVec_t{ role::Knight }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50);

        titleSVec_[title::Mender]                = std::make_shared<Title>(title::Mender,                AchievementType::HealthGiven, 1, 50, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1, 0, 0, 1), 0, 0, 2);
        titleSVec_[title::Healer]                = std::make_shared<Title>(title::Healer,                AchievementType::HealthGiven, 2, 200, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2, 0, 0, 2), 0, 0, 5);
        titleSVec_[title::BlessedHands]          = std::make_shared<Title>(title::BlessedHands,          AchievementType::HealthGiven, 3, 500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3, 0, 0, 3), 0, 0, 10);
        titleSVec_[title::WitchDoctor]           = std::make_shared<Title>(title::WitchDoctor,           AchievementType::HealthGiven, 4, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4, 0, 0, 4), 1, 0, 15);
        titleSVec_[title::LightOfTheCure]        = std::make_shared<Title>(title::LightOfTheCure,        AchievementType::HealthGiven, 5, 2000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5, 0, 0, 5), 1, 0, 20);
        titleSVec_[title::DivineTouch]           = std::make_shared<Title>(title::DivineTouch,           AchievementType::HealthGiven, 6, 3000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6, 0, 0, 6), 1, 0, 25);
        titleSVec_[title::OrderOfTheWhiteSage]   = std::make_shared<Title>(title::OrderOfTheWhiteSage,   AchievementType::HealthGiven, 7, 5000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 8, 0, 0, 8), 1, 0, 35);
        titleSVec_[title::ClericOfTheCovenant]   = std::make_shared<Title>(title::ClericOfTheCovenant,   AchievementType::HealthGiven, 8, 10000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 50);

        titleSVec_[title::HandsOfCharity        ] = std::make_shared<Title>(title::HandsOfCharity,          AchievementType::HealthTraded, 1, 50,   RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 1,  0, 0, 1),  0, 0, 2);
        titleSVec_[title::HandsOfLove           ] = std::make_shared<Title>(title::HandsOfLove,             AchievementType::HealthTraded, 2, 100,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 2,  0, 0, 2),  0, 0, 4);
        titleSVec_[title::HandsOfDevotion       ] = std::make_shared<Title>(title::HandsOfDevotion,         AchievementType::HealthTraded, 3, 200,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 3,  0, 0, 3),  0, 0, 8);
        titleSVec_[title::HandsOfNobility       ] = std::make_shared<Title>(title::HandsOfNobility,         AchievementType::HealthTraded, 4, 400,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 4,  0, 0, 4),  0, 0, 12);
        titleSVec_[title::HandsOfSacrifice      ] = std::make_shared<Title>(title::HandsOfSacrifice,        AchievementType::HealthTraded, 5, 800,  RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 5,  0, 0, 5),  0, 0, 16);
        titleSVec_[title::TheSelflessHand       ] = std::make_shared<Title>(title::TheSelflessHand,         AchievementType::HealthTraded, 6, 1000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 6,  0, 0, 6),  1, 0, 20);
        titleSVec_[title::TheIncorruptibleHand  ] = std::make_shared<Title>(title::TheIncorruptibleHand,    AchievementType::HealthTraded, 7, 2500, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 7,  0, 0, 7),  1, 0, 24);
        titleSVec_[title::OrderOfTheBleedingPalm] = std::make_shared<Title>(title::OrderOfTheBleedingPalm,  AchievementType::HealthTraded, 8, 4000, RoleVec_t{ role::Cleric }, stats::StatSet(0, 0, 10, 0, 0, 10), 2, 0, 30);

        titleSVec_[title::HowlingStray    ] = std::make_shared<Title>(title::HowlingStray,     AchievementType::BeastRoars, 1, 10,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(1,  0, 0, 0, 0, 0), 0, 0, 2);
        titleSVec_[title::HowlingBully    ] = std::make_shared<Title>(title::HowlingBully,     AchievementType::BeastRoars, 2, 30,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(2,  0, 0, 0, 0, 0), 0, 0, 4);
        titleSVec_[title::HowlingWildling ] = std::make_shared<Title>(title::HowlingWildling,  AchievementType::BeastRoars, 3, 50,   RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(3,  0, 0, 0, 0, 0), 0, 0, 8);
        titleSVec_[title::SnarlingCreature] = std::make_shared<Title>(title::SnarlingCreature, AchievementType::BeastRoars, 4, 100,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(4,  0, 0, 0, 0, 0), 0, 0, 12);
        titleSVec_[title::SnarlingBeast   ] = std::make_shared<Title>(title::SnarlingBeast,    AchievementType::BeastRoars, 5, 200,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(5,  0, 0, 0, 0, 0), 0, 0, 16);
        titleSVec_[title::SnarlingFiend   ] = std::make_shared<Title>(title::SnarlingFiend,    AchievementType::BeastRoars, 6, 400,  RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(6,  0, 0, 0, 0, 0), 0, 0, 20);
        titleSVec_[title::RagingMonster   ] = std::make_shared<Title>(title::RagingMonster,    AchievementType::BeastRoars, 7, 1000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(8,  0, 0, 0, 0, 0), 1, 0, 24);
        titleSVec_[title::RagingHorror    ] = std::make_shared<Title>(title::RagingHorror,     AchievementType::BeastRoars, 8, 2000, RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin }, stats::StatSet(10, 0, 0, 0, 0, 0), 2, 0, 30);

        titleSVec_[title::Prowler    ] = std::make_shared<Title>(title::Prowler,     AchievementType::LocksPicked, 1, 10,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 2);
        titleSVec_[title::PickPocket ] = std::make_shared<Title>(title::PickPocket,  AchievementType::LocksPicked, 2, 50,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 4);
        titleSVec_[title::CatBurglar ] = std::make_shared<Title>(title::CatBurglar,  AchievementType::LocksPicked, 3, 100,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 8);
        titleSVec_[title::Clincher   ] = std::make_shared<Title>(title::Clincher,    AchievementType::LocksPicked, 4, 200,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 12);
        titleSVec_[title::Picker     ] = std::make_shared<Title>(title::Picker,      AchievementType::LocksPicked, 5, 500,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 16);
        titleSVec_[title::SafeCracker] = std::make_shared<Title>(title::SafeCracker, AchievementType::LocksPicked, 6, 1000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 20);
        titleSVec_[title::LockTickler] = std::make_shared<Title>(title::LockTickler, AchievementType::LocksPicked, 7, 2000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 30);
        titleSVec_[title::LockBane   ] = std::make_shared<Title>(title::LockBane,    AchievementType::LocksPicked, 8, 5000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 50);

        titleSVec_[title::Versifier      ] = std::make_shared<Title>(title::Versifier,       AchievementType::SongsPlayed, 1, 10,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  1,  0, 1),  0, 0, 5);
        titleSVec_[title::Player         ] = std::make_shared<Title>(title::Player,          AchievementType::SongsPlayed, 2, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  1,  0, 2),  0, 0, 10);
        titleSVec_[title::Balladeer      ] = std::make_shared<Title>(title::Balladeer,       AchievementType::SongsPlayed, 3, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  2,  0, 3),  0, 0, 15);
        titleSVec_[title::Minstrel       ] = std::make_shared<Title>(title::Minstrel,        AchievementType::SongsPlayed, 4, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  2,  0, 4),  0, 0, 20);
        titleSVec_[title::Instrumentalist] = std::make_shared<Title>(title::Instrumentalist, AchievementType::SongsPlayed, 5, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  3,  0, 5),  0, 0, 25);
        titleSVec_[title::Soloist        ] = std::make_shared<Title>(title::Soloist,         AchievementType::SongsPlayed, 6, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  3,  0, 6),  1, 0, 30);
        titleSVec_[title::Artiste        ] = std::make_shared<Title>(title::Artiste,         AchievementType::SongsPlayed, 7, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 8,  5,  0, 8),  1, 0, 50);
        titleSVec_[title::Virtuoso       ] = std::make_shared<Title>(title::Virtuoso,        AchievementType::SongsPlayed, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 10, 0, 10), 2, 0, 60);

        titleSVec_[title::BardOfTheTrippingToes  ] = std::make_shared<Title>(title::BardOfTheTrippingToes,   AchievementType::SpiritsLifted, 1, 50,   RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 1,  0, 0, 1 ), 0, 0, 2 );
        titleSVec_[title::BardOfTheMerryMelody   ] = std::make_shared<Title>(title::BardOfTheMerryMelody,    AchievementType::SpiritsLifted, 2, 100,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 2,  0, 0, 2 ), 0, 0, 4 );
        titleSVec_[title::BardOfTheWhimsicalWord ] = std::make_shared<Title>(title::BardOfTheWhimsicalWord,  AchievementType::SpiritsLifted, 3, 200,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 3,  1, 0, 3 ), 0, 0, 8 );
        titleSVec_[title::BardOfTheCarolingChorus] = std::make_shared<Title>(title::BardOfTheCarolingChorus, AchievementType::SpiritsLifted, 4, 500,  RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 4,  1, 0, 4 ), 0, 0, 12);
        titleSVec_[title::BardOfTheBouncingBallad] = std::make_shared<Title>(title::BardOfTheBouncingBallad, AchievementType::SpiritsLifted, 5, 1000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 5,  2, 0, 5 ), 0, 0, 16);
        titleSVec_[title::BardOfTheDancingDitty  ] = std::make_shared<Title>(title::BardOfTheDancingDitty,   AchievementType::SpiritsLifted, 6, 2000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 6,  2, 0, 6 ), 0, 0, 20);
        titleSVec_[title::BardOfTheEnchantedEar  ] = std::make_shared<Title>(title::BardOfTheEnchantedEar,   AchievementType::SpiritsLifted, 7, 3000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 7,  3, 0, 7 ), 1, 0, 24);
        titleSVec_[title::BardOfTheAracneAria    ] = std::make_shared<Title>(title::BardOfTheAracneAria,     AchievementType::SpiritsLifted, 8, 5000, RoleVec_t{ role::Bard }, stats::StatSet(0, 0, 10, 5, 0, 10), 2, 0, 50);

        titleSVec_[title::Targeter      ] = std::make_shared<Title>(title::Targeter,        AchievementType::ProjectileHits, 1, 20,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  1,  0, 1,  0, 0), 0, 0, 0);
        titleSVec_[title::SteadfastFlyer] = std::make_shared<Title>(title::SteadfastFlyer,  AchievementType::ProjectileHits, 2, 50,     RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(1,  2,  0, 2,  0, 0), 0, 0, 0);
        titleSVec_[title::DeadEye       ] = std::make_shared<Title>(title::DeadEye,         AchievementType::ProjectileHits, 3, 100,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  3,  0, 3,  0, 0), 0, 0, 0);
        titleSVec_[title::DeadCenter    ] = std::make_shared<Title>(title::DeadCenter,      AchievementType::ProjectileHits, 4, 500,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(2,  4,  0, 4,  0, 0), 0, 0, 0);
        titleSVec_[title::SureSail      ] = std::make_shared<Title>(title::SureSail,        AchievementType::ProjectileHits, 5, 1000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  5,  0, 5,  0, 0), 0, 0, 0);
        titleSVec_[title::SureShot      ] = std::make_shared<Title>(title::SureShot,        AchievementType::ProjectileHits, 6, 2000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(3,  6,  0, 6,  0, 0), 0, 0, 0);
        titleSVec_[title::PerfectHit    ] = std::make_shared<Title>(title::PerfectHit,      AchievementType::ProjectileHits, 7, 3000,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(4,  8,  0, 8,  0, 0), 0, 0, 0);
        titleSVec_[title::EnchantedAim  ] = std::make_shared<Title>(title::EnchantedAim,    AchievementType::ProjectileHits, 8, 50000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief, role::Cleric, role::Sorcerer }, stats::StatSet(10, 10, 0, 10, 0, 0), 1, 0, 0);

        titleSVec_[title::Magus]     = std::make_shared<Title>(title::Magus,     AchievementType::SpellsCast, 1, 20,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 1,  0, 1),  0, 0, 2);
        titleSVec_[title::Charmer]   = std::make_shared<Title>(title::Charmer,   AchievementType::SpellsCast, 2, 50,     RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 2,  0, 2),  0, 0, 4);
        titleSVec_[title::Diviner]   = std::make_shared<Title>(title::Diviner,   AchievementType::SpellsCast, 3, 100,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 3,  0, 3),  0, 0, 8);
        titleSVec_[title::Enchanter] = std::make_shared<Title>(title::Enchanter, AchievementType::SpellsCast, 4, 200,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 4,  0, 4),  0, 0, 12);
        titleSVec_[title::Conjurer]  = std::make_shared<Title>(title::Conjurer,  AchievementType::SpellsCast, 5, 500,    RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 5,  0, 5),  0, 0, 16);
        titleSVec_[title::Shaman]    = std::make_shared<Title>(title::Shaman,    AchievementType::SpellsCast, 6, 1000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 6,  0, 6),  1, 0, 20);
        titleSVec_[title::Wizard]    = std::make_shared<Title>(title::Wizard,    AchievementType::SpellsCast, 7, 2000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 8,  0, 8),  1, 0, 30);
        titleSVec_[title::Warlock]   = std::make_shared<Title>(title::Warlock,   AchievementType::SpellsCast, 8, 5000,   RoleVec_t{ role::Sylavin, role::Firebrand, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(0, 0, 0, 10, 0, 10), 2, 0, 50);

        titleSVec_[title::Undaunted]     = std::make_shared<Title>(title::Undaunted,     AchievementType::EnemiesFaced, 1, 20,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0,  0, 0, 1,  0, 0),  0, 0,  5);
        titleSVec_[title::Daring]        = std::make_shared<Title>(title::Daring,        AchievementType::EnemiesFaced, 2, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(1,  0, 0, 2,  0, 1),  0, 0, 10);
        titleSVec_[title::Bold]          = std::make_shared<Title>(title::Bold,          AchievementType::EnemiesFaced, 3, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(2,  0, 0, 3,  0, 2),  0, 0, 15);
        titleSVec_[title::Brave]         = std::make_shared<Title>(title::Brave,         AchievementType::EnemiesFaced, 4, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(3,  0, 0, 4,  0, 3),  0, 0, 20);
        titleSVec_[title::Valorous]      = std::make_shared<Title>(title::Valorous,      AchievementType::EnemiesFaced, 5, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(4,  0, 0, 5,  0, 4),  0, 0, 25);
        titleSVec_[title::Valiant]       = std::make_shared<Title>(title::Valiant,       AchievementType::EnemiesFaced, 6, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(5,  0, 0, 6,  0, 5),  0, 0, 30);
        titleSVec_[title::Lionhearted]   = std::make_shared<Title>(title::Lionhearted,   AchievementType::EnemiesFaced, 7, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(6,  0, 0, 7,  0, 6),  0, 0, 35);
        titleSVec_[title::Heroic]        = std::make_shared<Title>(title::Heroic,        AchievementType::EnemiesFaced, 8, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40);
        titleSVec_[title::Fearless]      = std::make_shared<Title>(title::Fearless,      AchievementType::EnemiesFaced, 9, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50);

        titleSVec_[title::PawOfTheLunarPup]          = std::make_shared<Title>(title::PawOfTheLunarPup,       AchievementType::MoonHowls, 1, 10,  RoleVec_t{ role::Wolfen }, stats::StatSet(1,  0, 0, 1,  0, 0), 0, 0, 5);
        titleSVec_[title::PawOfTheYoungHowler]       = std::make_shared<Title>(title::PawOfTheYoungHowler,    AchievementType::MoonHowls, 2, 20,  RoleVec_t{ role::Wolfen }, stats::StatSet(2,  0, 0, 2,  0, 0), 0, 0, 10);
        titleSVec_[title::PawOfTheNightSky]          = std::make_shared<Title>(title::PawOfTheNightSky,       AchievementType::MoonHowls, 3, 50,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  0, 0, 3,  0, 0), 0, 0, 15);
        titleSVec_[title::PawOfTheMidnightWail]      = std::make_shared<Title>(title::PawOfTheMidnightWail,   AchievementType::MoonHowls, 4, 100, RoleVec_t{ role::Wolfen }, stats::StatSet(4,  0, 0, 4,  0, 0), 0, 0, 20);
        titleSVec_[title::PawOfTheTwilightMoon]      = std::make_shared<Title>(title::PawOfTheTwilightMoon,   AchievementType::MoonHowls, 5, 200, RoleVec_t{ role::Wolfen }, stats::StatSet(5,  0, 0, 5,  0, 0), 1, 0, 25);
        titleSVec_[title::PawOfTheCelestialCry]      = std::make_shared<Title>(title::PawOfTheCelestialCry,   AchievementType::MoonHowls, 6, 300, RoleVec_t{ role::Wolfen }, stats::StatSet(6,  0, 0, 6,  0, 0), 1, 0, 30);
        titleSVec_[title::PawOfTheGlowingOrb]        = std::make_shared<Title>(title::PawOfTheGlowingOrb,     AchievementType::MoonHowls, 7, 400, RoleVec_t{ role::Wolfen }, stats::StatSet(8,  0, 0, 8,  0, 0), 1, 0, 40);
        titleSVec_[title::PawOfTheCrescentLegion]    = std::make_shared<Title>(title::PawOfTheCrescentLegion, AchievementType::MoonHowls, 8, 500, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 0, 0, 10, 0, 0), 2, 0, 50);

        titleSVec_[title::WindGlider]    = std::make_shared<Title>(title::WindGlider,    AchievementType::TurnsInFlight, 1, 50,   RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 1,  1,  0), 0, 0, 0);
        titleSVec_[title::LightFeather]  = std::make_shared<Title>(title::LightFeather,  AchievementType::TurnsInFlight, 2, 100,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 2,  2,  0), 0, 0, 0);
        titleSVec_[title::FreeFlyer]     = std::make_shared<Title>(title::FreeFlyer,     AchievementType::TurnsInFlight, 3, 200,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 3,  3,  0), 0, 0, 0);
        titleSVec_[title::SteadyWing]    = std::make_shared<Title>(title::SteadyWing,    AchievementType::TurnsInFlight, 4, 500,  RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 4,  4,  0), 0, 0, 0);
        titleSVec_[title::EverSoar]      = std::make_shared<Title>(title::EverSoar,      AchievementType::TurnsInFlight, 5, 1000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 5,  5,  0), 0, 0, 0);
        titleSVec_[title::SkySail]       = std::make_shared<Title>(title::SkySail,       AchievementType::TurnsInFlight, 6, 2000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 6,  6,  0), 1, 0, 0);
        titleSVec_[title::Aerialist]     = std::make_shared<Title>(title::Aerialist,     AchievementType::TurnsInFlight, 7, 3000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 8,  8,  0), 1, 0, 0);
        titleSVec_[title::FeatherDancer] = std::make_shared<Title>(title::FeatherDancer, AchievementType::TurnsInFlight, 8, 5000, RoleVec_t{ Role::ROLEVEC_ALL_PLAYER_ROLES_ }, stats::StatSet(0, 0, 0, 10, 10, 0), 2, 0, 0);

        titleSVec_[title::PackFollower]  = std::make_shared<Title>(title::PackFollower,  AchievementType::PackActions, 1, 20,   RoleVec_t{ role::Wolfen }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5);
        titleSVec_[title::PackMember]    = std::make_shared<Title>(title::PackMember,    AchievementType::PackActions, 2, 50,   RoleVec_t{ role::Wolfen }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10);
        titleSVec_[title::PackTasker]    = std::make_shared<Title>(title::PackTasker,    AchievementType::PackActions, 3, 100,  RoleVec_t{ role::Wolfen }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15);
        titleSVec_[title::PackCharger]   = std::make_shared<Title>(title::PackCharger,   AchievementType::PackActions, 4, 200,  RoleVec_t{ role::Wolfen }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20);
        titleSVec_[title::PackNobel]     = std::make_shared<Title>(title::PackNobel,     AchievementType::PackActions, 5, 300,  RoleVec_t{ role::Wolfen }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25);
        titleSVec_[title::PackLeader]    = std::make_shared<Title>(title::PackLeader,    AchievementType::PackActions, 6, 400,  RoleVec_t{ role::Wolfen }, stats::StatSet(6,  6,  0, 0, 0, 0), 0, 0, 30);
        titleSVec_[title::PackAlpha]     = std::make_shared<Title>(title::PackAlpha,     AchievementType::PackActions, 7, 500,  RoleVec_t{ role::Wolfen }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 40);
        titleSVec_[title::PackElder]     = std::make_shared<Title>(title::PackElder,     AchievementType::PackActions, 8, 1000, RoleVec_t{ role::Wolfen }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 50);

        titleSVec_[title::CritterClairvoyant]    = std::make_shared<Title>(title::CritterClairvoyant, AchievementType::BeastMindLinks, 1, 20,   RoleVec_t{ role::Beastmaster }, stats::StatSet(1,  0, 0, 1,  0, 1),  0, 0, 5);
        titleSVec_[title::AnimalAnimator]        = std::make_shared<Title>(title::AnimalAnimator,     AchievementType::BeastMindLinks, 2, 50,   RoleVec_t{ role::Beastmaster }, stats::StatSet(2,  0, 0, 2,  0, 2),  0, 0, 10);
        titleSVec_[title::FriendOfTheFeral]      = std::make_shared<Title>(title::FriendOfTheFeral,   AchievementType::BeastMindLinks, 3, 100,  RoleVec_t{ role::Beastmaster }, stats::StatSet(3,  0, 0, 3,  0, 3),  0, 0, 15);
        titleSVec_[title::WillOfTheWild]         = std::make_shared<Title>(title::WillOfTheWild,      AchievementType::BeastMindLinks, 4, 200,  RoleVec_t{ role::Beastmaster }, stats::StatSet(4,  0, 0, 4,  0, 4),  0, 0, 20);
        titleSVec_[title::CreatureChanneler]     = std::make_shared<Title>(title::CreatureChanneler,  AchievementType::BeastMindLinks, 5, 300,  RoleVec_t{ role::Beastmaster }, stats::StatSet(5,  0, 0, 5,  0, 5),  0, 0, 25);
        titleSVec_[title::BeastTaskmaster]       = std::make_shared<Title>(title::BeastTaskmaster,    AchievementType::BeastMindLinks, 6, 400,  RoleVec_t{ role::Beastmaster }, stats::StatSet(6,  0, 0, 6,  0, 6),  1, 0, 30);
        titleSVec_[title::MonsterManipulator]    = std::make_shared<Title>(title::MonsterManipulator, AchievementType::BeastMindLinks, 7, 500,  RoleVec_t{ role::Beastmaster }, stats::StatSet(8,  0, 0, 8,  0, 8),  1, 0, 40);
        titleSVec_[title::MammalianMaster]       = std::make_shared<Title>(title::MammalianMaster,    AchievementType::BeastMindLinks, 8, 1000, RoleVec_t{ role::Beastmaster }, stats::StatSet(10, 0, 0, 10, 0, 10), 2, 0, 50);

        titleSVec_[title::Brawler]               = std::make_shared<Title>(title::Brawler,               AchievementType::MeleeHits, 1, 20,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(1,  1,  0, 0, 0, 0), 0, 0, 5  );
        titleSVec_[title::WildSwing]             = std::make_shared<Title>(title::WildSwing,             AchievementType::MeleeHits, 2, 50,    RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(2,  2,  0, 0, 0, 0), 0, 0, 10 );
        titleSVec_[title::HavocSmasher]          = std::make_shared<Title>(title::HavocSmasher,          AchievementType::MeleeHits, 3, 100,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(3,  3,  0, 0, 0, 0), 0, 0, 15 );
        titleSVec_[title::SureStroke]            = std::make_shared<Title>(title::SureStroke,            AchievementType::MeleeHits, 4, 200,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(4,  4,  0, 0, 0, 0), 0, 0, 20 );
        titleSVec_[title::Warrior]               = std::make_shared<Title>(title::Warrior,               AchievementType::MeleeHits, 5, 500,   RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(5,  5,  0, 0, 0, 0), 0, 0, 25 );
        titleSVec_[title::EverMark]              = std::make_shared<Title>(title::EverMark,              AchievementType::MeleeHits, 6, 1000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(6,  6,  0, 0, 0, 0), 1, 0, 30 );
        titleSVec_[title::MeleeMaster]           = std::make_shared<Title>(title::MeleeMaster,           AchievementType::MeleeHits, 7, 2000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(7,  7,  0, 0, 0, 0), 1, 0, 40 );
        titleSVec_[title::SteadyStriker]         = std::make_shared<Title>(title::SteadyStriker,         AchievementType::MeleeHits, 8, 3000,  RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(8,  8,  0, 0, 0, 0), 1, 0, 50 );
        titleSVec_[title::OrderOfTheSteadyBlade] = std::make_shared<Title>(title::OrderOfTheSteadyBlade, AchievementType::MeleeHits, 9, 50000, RoleVec_t{ role::Knight, role::Archer, role::Beastmaster, role::Bard, role::Thief }, stats::StatSet(10, 10, 0, 0, 0, 0), 2, 0, 100);

        titleSVec_[title::DragonOfTheUnblinkingEye]      = std::make_shared<Title>(title::DragonOfTheUnblinkingEye,      AchievementType::FlyingAttackHits, 1, 20,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(1,  1,  0, 0, 1,  0),  0, 0, 0);
        titleSVec_[title::DragonOfTheCurlingClaw]        = std::make_shared<Title>(title::DragonOfTheCurlingClaw,        AchievementType::FlyingAttackHits, 2, 50,   RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(2,  2,  0, 0, 2,  1),  0, 0, 0);
        titleSVec_[title::DragonOfTheDaggerTooth]        = std::make_shared<Title>(title::DragonOfTheDaggerTooth,        AchievementType::FlyingAttackHits, 3, 100,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(3,  3,  0, 0, 3,  2),  0, 0, 0);
        titleSVec_[title::DragonOfTheForebodingGaze]     = std::make_shared<Title>(title::DragonOfTheForebodingGaze,     AchievementType::FlyingAttackHits, 4, 200,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(4,  4,  0, 0, 4,  3),  0, 0, 0);
        titleSVec_[title::DragonOfTheUnbreakableScale]   = std::make_shared<Title>(title::DragonOfTheUnbreakableScale,   AchievementType::FlyingAttackHits, 5, 500,  RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(5,  5,  0, 0, 5,  4),  0, 0, 0);
        titleSVec_[title::DragonOfTheDreadedBreath]      = std::make_shared<Title>(title::DragonOfTheDreadedBreath,      AchievementType::FlyingAttackHits, 6, 1000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(6,  6,  0, 0, 6,  5),  0, 0, 1);
        titleSVec_[title::DragonOfTheArcaneWing]         = std::make_shared<Title>(title::DragonOfTheArcaneWing,         AchievementType::FlyingAttackHits, 7, 2000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(8,  8,  0, 0, 8,  8),  0, 0, 1);
        titleSVec_[title::DragonOfTheNightmareSky]       = std::make_shared<Title>(title::DragonOfTheNightmareSky,       AchievementType::FlyingAttackHits, 8, 5000, RoleVec_t{ role::Firebrand, role::Sylavin }, stats::StatSet(10, 10, 0, 0, 10, 10), 0, 0, 2);
    }


    TitlePtr_t Warehouse::Get(const title::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(((E >= 0) || (E < title::Count)),                  "game::creature::title::Warehouse::Get(enum=" << E << ")_InvalidValueError.");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < titleSVec_.size()), "game::creature::title::Warehouse::Get(\"" << title::ToString(E) << "\") enum given was " << E << " but that was greater than (or equal to) the vec size of " << titleSVec_.size() << ".");
        M_ASSERT_OR_LOGANDTHROW_SS((titleSVec_.empty() == false),                     "game::creature::title::Warehouse::Get(\"" << title::ToString(E) << "\") called before vec was populated by Setup().");
        const TitlePtr_t TITLE_PTR(titleSVec_[E].get());
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),                            "game::creature::title::Warehouse::Get(\"" << title::ToString(E) << "\") found a nullptr in the vector.");
        return TITLE_PTR;
    }


    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend("game::creature::title::Warehouse::Test() Starting Tests...");
        }

        static auto titleIndex{ 0 };
        if (titleIndex < title::Count)
        {
            auto const NEXT_ENUM(static_cast<title::Enum>(titleIndex));
            auto TITLE_PTR{ Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR != nullptr),                          "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Desc().empty() == false),            "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->LongDesc().empty() == false),        "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->ImageFilename().empty() == false),   "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->RolesCopy().empty() == false),       "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty RolesVec().");
            M_ASSERT_OR_LOGANDTHROW_SS((TITLE_PTR->Which() == NEXT_ENUM),               "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in a Title with a different tile::Enum (\"" << title::ToString(TITLE_PTR->Which()) << "\")");
            auto const TEXTURE_SPTR{ sfml_util::gui::TitleImageManager::Instance()->Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr),                 "game::creature::title::Warehouse::Test(\"" << title::ToString(NEXT_ENUM) << "\") resulted in an empty image/texture.");
            ++titleIndex;
            LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            LoopManager::Instance()->TestingStrIncrement("Title Test #");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::creature::title::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }

}
}
}
