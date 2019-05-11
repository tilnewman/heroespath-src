// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// title-holder.cpp
//
#include "title-holder.hpp"

#include "creature/role-enum.hpp"
#include "creature/title.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/enum-util.hpp"
#include "stage/i-stage.hpp"

#include <memory>
#include <sstream>
#include <stdexcept>

namespace heroespath
{
namespace creature
{
    namespace title
    {

        TitleUVec_t Holder::titleUVec_;

        void Holder::Fill()
        {
            // Note:  Keep order in sync with creature::Titles::Enum

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::ProtectorOfThornberry,
                AchievementType::None,
                1,
                1,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(),
                0_rank,
                500_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Dodger,
                AchievementType::DodgedStanding,
                1,
                50,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 0_lck, 1_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FastFoot,
                AchievementType::DodgedStanding,
                2,
                100,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 0_lck, 2_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::NimbleFoot,
                AchievementType::DodgedStanding,
                3,
                200,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 0_lck, 3_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SwiftFoot,
                AchievementType::DodgedStanding,
                4,
                500,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 1_lck, 4_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeMover,
                AchievementType::DodgedStanding,
                5,
                1000,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 2_lck, 5_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeGrace,
                AchievementType::DodgedStanding,
                6,
                2000,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 3_lck, 6_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeDancer,
                AchievementType::DodgedStanding,
                7,
                5000,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 4_lck, 7_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeShadow,
                AchievementType::DodgedStanding,
                8,
                10000,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 7_lck, 10_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyDodger,
                AchievementType::DodgedFlying,
                1,
                20,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 0_lck, 1_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FastWing,
                AchievementType::DodgedFlying,
                2,
                40,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 0_lck, 2_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::NimbleWing,
                AchievementType::DodgedFlying,
                3,
                80,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 0_lck, 3_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SwiftWing,
                AchievementType::DodgedFlying,
                4,
                120,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 1_lck, 4_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyMover,
                AchievementType::DodgedFlying,
                5,
                200,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 2_lck, 5_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::GraceWing,
                AchievementType::DodgedFlying,
                6,
                300,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 3_lck, 6_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyDancer,
                AchievementType::DodgedFlying,
                7,
                450,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 4_lck, 7_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyShadow,
                AchievementType::DodgedFlying,
                8,
                650,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                StatSet(0_str, 0_acc, 0_cha, 7_lck, 10_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Sneak,
                AchievementType::BackstabHits,
                1,
                10,
                RoleVec_t { role::Thief },
                StatSet(0_str, 1_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Stalker,
                AchievementType::BackstabHits,
                2,
                50,
                RoleVec_t { role::Thief },
                StatSet(0_str, 2_acc, 0_cha, 2_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Creeper,
                AchievementType::BackstabHits,
                3,
                100,
                RoleVec_t { role::Thief },
                StatSet(0_str, 3_acc, 0_cha, 3_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Savage,
                AchievementType::BackstabHits,
                4,
                200,
                RoleVec_t { role::Thief },
                StatSet(0_str, 4_acc, 0_cha, 4_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                1_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Homicidal,
                AchievementType::BackstabHits,
                5,
                300,
                RoleVec_t { role::Thief },
                StatSet(0_str, 5_acc, 0_cha, 5_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Bloodthirsty,
                AchievementType::BackstabHits,
                6,
                400,
                RoleVec_t { role::Thief },
                StatSet(0_str, 6_acc, 0_cha, 6_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::ShadowWalker,
                AchievementType::BackstabHits,
                7,
                500,
                RoleVec_t { role::Thief },
                StatSet(0_str, 7_acc, 0_cha, 7_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Assassin,
                AchievementType::BackstabHits,
                8,
                1000,
                RoleVec_t { role::Thief },
                StatSet(0_str, 10_acc, 0_cha, 10_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheClashingBlade,
                AchievementType::BattlesSurvived,
                1,
                10,
                RoleVec_t { role::Knight },
                StatSet(1_str, 1_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfAHundredBattles,
                AchievementType::BattlesSurvived,
                2,
                100,
                RoleVec_t { role::Knight },
                StatSet(2_str, 2_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfThePiercingBlade,
                AchievementType::BattlesSurvived,
                3,
                200,
                RoleVec_t { role::Knight },
                StatSet(3_str, 3_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheUndauntedBlade,
                AchievementType::BattlesSurvived,
                4,
                500,
                RoleVec_t { role::Knight },
                StatSet(4_str, 4_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfAThousandBattles,
                AchievementType::BattlesSurvived,
                5,
                1000,
                RoleVec_t { role::Knight },
                StatSet(5_str, 5_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheFearedBlade,
                AchievementType::BattlesSurvived,
                6,
                2000,
                RoleVec_t { role::Knight },
                StatSet(6_str, 6_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheRealm,
                AchievementType::BattlesSurvived,
                7,
                3000,
                RoleVec_t { role::Knight },
                StatSet(7_str, 7_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                35_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfEtan,
                AchievementType::BattlesSurvived,
                8,
                5000,
                RoleVec_t { role::Knight },
                StatSet(10_str, 10_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Mender,
                AchievementType::HealthGiven,
                1,
                50,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 1_cha, 0_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Healer,
                AchievementType::HealthGiven,
                2,
                200,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 2_cha, 0_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BlessedHands,
                AchievementType::HealthGiven,
                3,
                500,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 3_cha, 0_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WitchDoctor,
                AchievementType::HealthGiven,
                4,
                1000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 4_cha, 0_lck, 0_spd, 4_int),
                1_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LightOfTheCure,
                AchievementType::HealthGiven,
                5,
                2000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 5_cha, 0_lck, 0_spd, 5_int),
                1_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DivineTouch,
                AchievementType::HealthGiven,
                6,
                3000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 6_cha, 0_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::OrderOfTheWhiteSage,
                AchievementType::HealthGiven,
                7,
                5000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 8_cha, 0_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                35_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::ClericOfTheCovenant,
                AchievementType::HealthGiven,
                8,
                10000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 10_cha, 0_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfCharity,
                AchievementType::HealthTraded,
                1,
                50,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 1_cha, 0_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfLove,
                AchievementType::HealthTraded,
                2,
                100,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 2_cha, 0_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                4_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfDevotion,
                AchievementType::HealthTraded,
                3,
                200,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 3_cha, 0_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                8_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfNobility,
                AchievementType::HealthTraded,
                4,
                400,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 4_cha, 0_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                12_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfSacrifice,
                AchievementType::HealthTraded,
                5,
                800,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 5_cha, 0_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                16_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::TheSelflessHand,
                AchievementType::HealthTraded,
                6,
                1000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 6_cha, 0_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::TheIncorruptibleHand,
                AchievementType::HealthTraded,
                7,
                2500,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 7_cha, 0_lck, 0_spd, 7_int),
                1_rank,
                0_exp,
                24_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::OrderOfTheBleedingPalm,
                AchievementType::HealthTraded,
                8,
                4000,
                RoleVec_t { role::Cleric },
                StatSet(0_str, 0_acc, 10_cha, 0_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HowlingStray,
                AchievementType::BeastRoars,
                1,
                10,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(1_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HowlingBully,
                AchievementType::BeastRoars,
                2,
                30,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(2_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                4_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HowlingWildling,
                AchievementType::BeastRoars,
                3,
                50,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(3_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                8_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SnarlingCreature,
                AchievementType::BeastRoars,
                4,
                100,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(4_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                12_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SnarlingBeast,
                AchievementType::BeastRoars,
                5,
                200,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(5_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                16_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SnarlingFiend,
                AchievementType::BeastRoars,
                6,
                400,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(6_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::RagingMonster,
                AchievementType::BeastRoars,
                7,
                1000,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(8_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                24_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::RagingHorror,
                AchievementType::BeastRoars,
                8,
                2000,
                RoleVec_t { role::Wolfen, role::Firebrand, role::Sylavin },
                StatSet(10_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Prowler,
                AchievementType::LocksPicked,
                1,
                10,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 1_lck, 1_spd, 0_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PickPocket,
                AchievementType::LocksPicked,
                2,
                50,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 2_lck, 2_spd, 0_int),
                0_rank,
                0_exp,
                4_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::CatBurglar,
                AchievementType::LocksPicked,
                3,
                100,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 3_lck, 3_spd, 0_int),
                0_rank,
                0_exp,
                8_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Clincher,
                AchievementType::LocksPicked,
                4,
                200,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 4_lck, 4_spd, 0_int),
                0_rank,
                0_exp,
                12_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Picker,
                AchievementType::LocksPicked,
                5,
                500,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 5_lck, 5_spd, 0_int),
                0_rank,
                0_exp,
                16_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SafeCracker,
                AchievementType::LocksPicked,
                6,
                1000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 6_lck, 6_spd, 0_int),
                1_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LockTickler,
                AchievementType::LocksPicked,
                7,
                2000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 8_lck, 8_spd, 0_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LockBane,
                AchievementType::LocksPicked,
                8,
                5000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 10_lck, 10_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Versifier,
                AchievementType::SongsPlayed,
                1,
                10,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 1_cha, 1_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Player,
                AchievementType::SongsPlayed,
                2,
                50,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 2_cha, 1_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Balladeer,
                AchievementType::SongsPlayed,
                3,
                100,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 3_cha, 2_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Minstrel,
                AchievementType::SongsPlayed,
                4,
                200,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 4_cha, 2_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Instrumentalist,
                AchievementType::SongsPlayed,
                5,
                500,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 5_cha, 3_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Soloist,
                AchievementType::SongsPlayed,
                6,
                1000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 6_cha, 3_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Artiste,
                AchievementType::SongsPlayed,
                7,
                2000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 8_cha, 5_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Virtuoso,
                AchievementType::SongsPlayed,
                8,
                5000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 10_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                60_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheTrippingToes,
                AchievementType::SpiritsLifted,
                1,
                50,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 1_cha, 0_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheMerryMelody,
                AchievementType::SpiritsLifted,
                2,
                100,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 2_cha, 0_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                4_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheWhimsicalWord,
                AchievementType::SpiritsLifted,
                3,
                200,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 3_cha, 1_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                8_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheCarolingChorus,
                AchievementType::SpiritsLifted,
                4,
                500,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 4_cha, 1_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                12_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheBouncingBallad,
                AchievementType::SpiritsLifted,
                5,
                1000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 5_cha, 2_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                16_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheDancingDitty,
                AchievementType::SpiritsLifted,
                6,
                2000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 6_cha, 2_lck, 0_spd, 6_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheEnchantedEar,
                AchievementType::SpiritsLifted,
                7,
                3000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 7_cha, 3_lck, 0_spd, 7_int),
                1_rank,
                0_exp,
                24_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheAracneAria,
                AchievementType::SpiritsLifted,
                8,
                5000,
                RoleVec_t { role::Bard },
                StatSet(0_str, 0_acc, 10_cha, 5_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Targeter,
                AchievementType::ProjectileHits,
                1,
                20,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(1_str, 1_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SteadfastFlyer,
                AchievementType::ProjectileHits,
                2,
                50,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(1_str, 2_acc, 0_cha, 2_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DeadEye,
                AchievementType::ProjectileHits,
                3,
                100,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(2_str, 3_acc, 0_cha, 3_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DeadCenter,
                AchievementType::ProjectileHits,
                4,
                500,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(2_str, 4_acc, 0_cha, 4_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SureSail,
                AchievementType::ProjectileHits,
                5,
                1000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(3_str, 5_acc, 0_cha, 5_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SureShot,
                AchievementType::ProjectileHits,
                6,
                2000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(3_str, 6_acc, 0_cha, 6_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PerfectHit,
                AchievementType::ProjectileHits,
                7,
                3000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(4_str, 8_acc, 0_cha, 8_lck, 0_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::EnchantedAim,
                AchievementType::ProjectileHits,
                8,
                5000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(10_str, 10_acc, 0_cha, 10_lck, 0_spd, 0_int),
                1_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Magus,
                AchievementType::SpellsCast,
                1,
                20,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 1_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Charmer,
                AchievementType::SpellsCast,
                2,
                50,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 2_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                4_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Diviner,
                AchievementType::SpellsCast,
                3,
                100,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 3_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                8_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Enchanter,
                AchievementType::SpellsCast,
                4,
                200,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 4_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                12_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Conjurer,
                AchievementType::SpellsCast,
                5,
                500,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 5_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                16_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Shaman,
                AchievementType::SpellsCast,
                6,
                1000,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 6_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Wizard,
                AchievementType::SpellsCast,
                7,
                2000,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 8_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Warlock,
                AchievementType::SpellsCast,
                8,
                5000,
                RoleVec_t { role::Sylavin,
                            role::Firebrand,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Cleric,
                            role::Sorcerer },
                StatSet(0_str, 0_acc, 0_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Undaunted,
                AchievementType::EnemiesFaced,
                1,
                20,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Daring,
                AchievementType::EnemiesFaced,
                2,
                50,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(1_str, 0_acc, 0_cha, 2_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Bold,
                AchievementType::EnemiesFaced,
                3,
                100,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(2_str, 0_acc, 0_cha, 3_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Brave,
                AchievementType::EnemiesFaced,
                4,
                200,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(3_str, 0_acc, 0_cha, 4_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Valorous,
                AchievementType::EnemiesFaced,
                5,
                500,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(4_str, 0_acc, 0_cha, 5_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Valiant,
                AchievementType::EnemiesFaced,
                6,
                1000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(5_str, 0_acc, 0_cha, 6_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Lionhearted,
                AchievementType::EnemiesFaced,
                7,
                2000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(6_str, 0_acc, 0_cha, 7_lck, 0_spd, 6_int),
                0_rank,
                0_exp,
                35_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Heroic,
                AchievementType::EnemiesFaced,
                8,
                3000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(8_str, 0_acc, 0_cha, 8_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                40_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Fearless,
                AchievementType::EnemiesFaced,
                9,
                5000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(10_str, 0_acc, 0_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheLunarPup,
                AchievementType::MoonHowls,
                1,
                10,
                RoleVec_t { role::Wolfen },
                StatSet(1_str, 0_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheYoungHowler,
                AchievementType::MoonHowls,
                2,
                20,
                RoleVec_t { role::Wolfen },
                StatSet(2_str, 0_acc, 0_cha, 2_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheNightSky,
                AchievementType::MoonHowls,
                3,
                50,
                RoleVec_t { role::Wolfen },
                StatSet(3_str, 0_acc, 0_cha, 3_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheMidnightWail,
                AchievementType::MoonHowls,
                4,
                100,
                RoleVec_t { role::Wolfen },
                StatSet(4_str, 0_acc, 0_cha, 4_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheTwilightMoon,
                AchievementType::MoonHowls,
                5,
                200,
                RoleVec_t { role::Wolfen },
                StatSet(5_str, 0_acc, 0_cha, 5_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheCelestialCry,
                AchievementType::MoonHowls,
                6,
                300,
                RoleVec_t { role::Wolfen },
                StatSet(6_str, 0_acc, 0_cha, 6_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheGlowingOrb,
                AchievementType::MoonHowls,
                7,
                500,
                RoleVec_t { role::Wolfen },
                StatSet(8_str, 0_acc, 0_cha, 8_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                40_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheCrescentLegion,
                AchievementType::MoonHowls,
                8,
                800,
                RoleVec_t { role::Wolfen },
                StatSet(10_str, 0_acc, 0_cha, 10_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WindGlider,
                AchievementType::TurnsInFlight,
                1,
                50,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 1_lck, 1_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LightFeather,
                AchievementType::TurnsInFlight,
                2,
                100,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 2_lck, 2_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FreeFlyer,
                AchievementType::TurnsInFlight,
                3,
                200,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 3_lck, 3_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SteadyWing,
                AchievementType::TurnsInFlight,
                4,
                500,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 4_lck, 4_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::EverSoar,
                AchievementType::TurnsInFlight,
                5,
                1000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 5_lck, 5_spd, 0_int),
                0_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkySail,
                AchievementType::TurnsInFlight,
                6,
                2000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 6_lck, 6_spd, 0_int),
                1_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Aerialist,
                AchievementType::TurnsInFlight,
                7,
                3000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 8_lck, 8_spd, 0_int),
                1_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FeatherDancer,
                AchievementType::TurnsInFlight,
                8,
                5000,
                RoleVec_t { Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                StatSet(0_str, 0_acc, 0_cha, 10_lck, 10_spd, 0_int),
                2_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackFollower,
                AchievementType::PackActions,
                1,
                20,
                RoleVec_t { role::Wolfen },
                StatSet(1_str, 1_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackMember,
                AchievementType::PackActions,
                2,
                50,
                RoleVec_t { role::Wolfen },
                StatSet(2_str, 2_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackTasker,
                AchievementType::PackActions,
                3,
                100,
                RoleVec_t { role::Wolfen },
                StatSet(3_str, 3_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackCharger,
                AchievementType::PackActions,
                4,
                200,
                RoleVec_t { role::Wolfen },
                StatSet(4_str, 4_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackNobel,
                AchievementType::PackActions,
                5,
                300,
                RoleVec_t { role::Wolfen },
                StatSet(5_str, 5_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackLeader,
                AchievementType::PackActions,
                6,
                400,
                RoleVec_t { role::Wolfen },
                StatSet(6_str, 6_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackAlpha,
                AchievementType::PackActions,
                7,
                500,
                RoleVec_t { role::Wolfen },
                StatSet(8_str, 8_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                40_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackElder,
                AchievementType::PackActions,
                8,
                1000,
                RoleVec_t { role::Wolfen },
                StatSet(10_str, 10_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::CritterClairvoyant,
                AchievementType::BeastMindLinks,
                1,
                20,
                RoleVec_t { role::Beastmaster },
                StatSet(1_str, 0_acc, 0_cha, 1_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::AnimalAnimator,
                AchievementType::BeastMindLinks,
                2,
                50,
                RoleVec_t { role::Beastmaster },
                StatSet(2_str, 0_acc, 0_cha, 2_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FriendOfTheFeral,
                AchievementType::BeastMindLinks,
                3,
                100,
                RoleVec_t { role::Beastmaster },
                StatSet(3_str, 0_acc, 0_cha, 3_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WillOfTheWild,
                AchievementType::BeastMindLinks,
                4,
                200,
                RoleVec_t { role::Beastmaster },
                StatSet(4_str, 0_acc, 0_cha, 4_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::CreatureChanneler,
                AchievementType::BeastMindLinks,
                5,
                300,
                RoleVec_t { role::Beastmaster },
                StatSet(5_str, 0_acc, 0_cha, 5_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BeastTaskmaster,
                AchievementType::BeastMindLinks,
                6,
                400,
                RoleVec_t { role::Beastmaster },
                StatSet(6_str, 0_acc, 0_cha, 6_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MonsterManipulator,
                AchievementType::BeastMindLinks,
                7,
                500,
                RoleVec_t { role::Beastmaster },
                StatSet(8_str, 0_acc, 0_cha, 8_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                40_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MammalianMaster,
                AchievementType::BeastMindLinks,
                8,
                1000,
                RoleVec_t { role::Beastmaster },
                StatSet(10_str, 0_acc, 0_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Brawler,
                AchievementType::MeleeHits,
                1,
                20,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(1_str, 1_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WildSwing,
                AchievementType::MeleeHits,
                2,
                50,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(2_str, 2_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HavocSmasher,
                AchievementType::MeleeHits,
                3,
                100,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(3_str, 3_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SureStroke,
                AchievementType::MeleeHits,
                4,
                200,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(4_str, 4_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Warrior,
                AchievementType::MeleeHits,
                5,
                500,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(5_str, 5_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                25_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::EverMark,
                AchievementType::MeleeHits,
                6,
                1000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(6_str, 6_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeMaster,
                AchievementType::MeleeHits,
                7,
                2000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(7_str, 7_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                40_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SteadyStriker,
                AchievementType::MeleeHits,
                8,
                3000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(8_str, 8_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::OrderOfTheEverWarrior,
                AchievementType::MeleeHits,
                9,
                5000,
                RoleVec_t { role::Knight,
                            role::Archer,
                            role::Beastmaster,
                            role::Bard,
                            role::Thief,
                            role::Firebrand,
                            role::Sylavin },
                StatSet(10_str, 10_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                100_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheUnblinkingEye,
                AchievementType::FlyingAttackHits,
                1,
                20,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(1_str, 1_acc, 0_cha, 0_lck, 1_spd, 0_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheCurlingClaw,
                AchievementType::FlyingAttackHits,
                2,
                50,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(2_str, 2_acc, 0_cha, 0_lck, 2_spd, 1_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheDaggerTooth,
                AchievementType::FlyingAttackHits,
                3,
                100,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(3_str, 3_acc, 0_cha, 0_lck, 3_spd, 2_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheForebodingGaze,
                AchievementType::FlyingAttackHits,
                4,
                200,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(4_str, 4_acc, 0_cha, 0_lck, 4_spd, 3_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheUnbreakableScale,
                AchievementType::FlyingAttackHits,
                5,
                500,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(5_str, 5_acc, 0_cha, 0_lck, 5_spd, 4_int),
                0_rank,
                0_exp,
                0_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheDreadedBreath,
                AchievementType::FlyingAttackHits,
                6,
                1000,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(6_str, 6_acc, 0_cha, 0_lck, 6_spd, 5_int),
                0_rank,
                0_exp,
                1_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheArcaneWing,
                AchievementType::FlyingAttackHits,
                7,
                2000,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(8_str, 8_acc, 0_cha, 0_lck, 8_spd, 8_int),
                0_rank,
                0_exp,
                1_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheNightmareSky,
                AchievementType::FlyingAttackHits,
                8,
                5000,
                RoleVec_t { role::Firebrand, role::Sylavin },
                StatSet(10_str, 10_acc, 0_cha, 0_lck, 10_spd, 10_int),
                0_rank,
                0_exp,
                2_health));
        }

        void Holder::Empty() { titleUVec_.clear(); }

        TitlePtr_t Holder::Get(const Titles::Enum ENUM)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (titleUVec_.empty() == false),
                "creature::Titles::Holder::Get(\"" << NAMEOF_ENUM(ENUM)
                                                   << "\") called when the Holder was empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (EnumUtil<Titles>::IsValid(ENUM)),
                "creature::Titles::Holder::Get(enum=" << NAMEOF_ENUM(ENUM)
                                                      << ") but that enum value is invalid.");

            const auto INDEX { static_cast<std::size_t>(ENUM) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (INDEX < titleUVec_.size()),
                "creature::Titles::Holder::Get(\""
                    << NAMEOF_ENUM(ENUM) << "\") enum given was " << ENUM
                    << " but that was greater than (or equal to) "
                    << "the vec size of " << titleUVec_.size() << ".");

            return TitlePtr_t { titleUVec_[INDEX].get() };
        }

        bool Holder::Test(stage::IStagePtr_t iStagePtr)
        {
            static auto hasInitialPrompt { false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                iStagePtr->TestingStrAppend("creature::Titles::Holder::Test() Starting Tests...");
            }

            static EnumUnderlying_t titleIndex { 0 };
            if (titleIndex < Titles::Count)
            {
                const auto NEXT_ENUM(static_cast<Titles::Enum>(titleIndex));
                auto TITLE_PTR { Get(NEXT_ENUM) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (TITLE_PTR->Desc().empty() == false),
                    "creature::Titles::Holder::Test(\"" << NAMEOF_ENUM(NEXT_ENUM)
                                                        << "\") resulted in an empty Desc().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (TITLE_PTR->LongDesc().empty() == false),
                    "creature::Titles::Holder::Test(\"" << NAMEOF_ENUM(NEXT_ENUM)
                                                        << "\") resulted in an empty LongDesc().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (TITLE_PTR->ImageFilename().empty() == false),
                    "creature::Titles::Holder::Test(\""
                        << NAMEOF_ENUM(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (TITLE_PTR->RolesCopy().empty() == false),
                    "creature::Titles::Holder::Test(\"" << NAMEOF_ENUM(NEXT_ENUM)
                                                        << "\") resulted in an empty RolesVec().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (TITLE_PTR->Which() == NEXT_ENUM),
                    "creature::Titles::Holder::Test(\""
                        << NAMEOF_ENUM(NEXT_ENUM)
                        << "\") resulted in a Title with a different tile::Enum (\""
                        << NAMEOF_ENUM(TITLE_PTR->Which()) << "\")");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (TITLE_PTR->Name() == Titles::Name(NEXT_ENUM)),
                    "creature::Titles::Holder::Test(\"" << NAMEOF_ENUM(NEXT_ENUM)
                                                        << "\") Title is out of order.");

                iStagePtr->TestingStrIncrement("Title Test #");
                ++titleIndex;
                return false;
            }

            iStagePtr->TestingStrAppend("creature::Titles::Holder::Test()  ALL TESTS PASSED.");
            return true;
        }

    } // namespace title
} // namespace creature
} // namespace heroespath
