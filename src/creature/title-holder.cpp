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
#include "game/loop-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfml-util/gui/title-image-loader.hpp"

#include <exception>
#include <memory>
#include <sstream>

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
                1_index,
                1_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(),
                0_rank,
                500_exp,
                2_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Dodger,
                AchievementType::DodgedStanding,
                1_index,
                50_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 0_lck, 1_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FastFoot,
                AchievementType::DodgedStanding,
                2_index,
                100_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 0_lck, 2_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::NimbleFoot,
                AchievementType::DodgedStanding,
                3_index,
                200_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 0_lck, 3_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SwiftFoot,
                AchievementType::DodgedStanding,
                4_index,
                500_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 1_lck, 4_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeMover,
                AchievementType::DodgedStanding,
                5_index,
                1000_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 2_lck, 5_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeGrace,
                AchievementType::DodgedStanding,
                6_index,
                2000_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 3_lck, 6_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeDancer,
                AchievementType::DodgedStanding,
                7_index,
                5000_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 4_lck, 7_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeShadow,
                AchievementType::DodgedStanding,
                8_index,
                10000_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 7_lck, 10_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyDodger,
                AchievementType::DodgedFlying,
                1_index,
                20_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 0_lck, 1_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FastWing,
                AchievementType::DodgedFlying,
                2_index,
                40_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 0_lck, 2_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::NimbleWing,
                AchievementType::DodgedFlying,
                3_index,
                80_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 0_lck, 3_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SwiftWing,
                AchievementType::DodgedFlying,
                4_index,
                120_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 1_lck, 4_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyMover,
                AchievementType::DodgedFlying,
                5_index,
                200_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 2_lck, 5_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::GraceWing,
                AchievementType::DodgedFlying,
                6_index,
                300_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 3_lck, 6_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyDancer,
                AchievementType::DodgedFlying,
                7_index,
                450_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 4_lck, 7_spd, 0_int)));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkyShadow,
                AchievementType::DodgedFlying,
                8_index,
                650_count,
                Title::ROLEVEC_ALL_PLAYER_ROLES_,
                stats::StatSet(0_str, 0_acc, 0_cha, 7_lck, 10_spd, 0_int)));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Sneak,
                AchievementType::BackstabsHits,
                1_index,
                10_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 1_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Stalker,
                AchievementType::BackstabsHits,
                2_index,
                50_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 2_acc, 0_cha, 2_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Creeper,
                AchievementType::BackstabsHits,
                3_index,
                100_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 3_acc, 0_cha, 3_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Savage,
                AchievementType::BackstabsHits,
                4_index,
                200_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 4_acc, 0_cha, 4_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                1_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Homicidal,
                AchievementType::BackstabsHits,
                5_index,
                300_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 5_acc, 0_cha, 5_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Bloodthirsty,
                AchievementType::BackstabsHits,
                6_index,
                400_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 6_acc, 0_cha, 6_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::ShadowWalker,
                AchievementType::BackstabsHits,
                7_index,
                500_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 7_acc, 0_cha, 7_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Assassin,
                AchievementType::BackstabsHits,
                8_index,
                1000_count,
                RoleVec_t{ role::Thief },
                stats::StatSet(0_str, 10_acc, 0_cha, 10_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                20_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheClashingBlade,
                AchievementType::BattlesSurvived,
                1_index,
                10_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(1_str, 1_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfAHundredBattles,
                AchievementType::BattlesSurvived,
                2_index,
                100_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(2_str, 2_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfThePiercingBlade,
                AchievementType::BattlesSurvived,
                3_index,
                200_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(3_str, 3_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheUndauntedBlade,
                AchievementType::BattlesSurvived,
                4_index,
                500_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(4_str, 4_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfAThousandBattles,
                AchievementType::BattlesSurvived,
                5_index,
                1000_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(5_str, 5_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheFearedBlade,
                AchievementType::BattlesSurvived,
                6_index,
                2000_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(6_str, 6_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfTheRealm,
                AchievementType::BattlesSurvived,
                7_index,
                3000_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(7_str, 7_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                35_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::KnightOfEtan,
                AchievementType::BattlesSurvived,
                8_index,
                5000_count,
                RoleVec_t{ role::Knight },
                stats::StatSet(10_str, 10_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Mender,
                AchievementType::HealthGiven,
                1_index,
                50_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 1_cha, 0_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Healer,
                AchievementType::HealthGiven,
                2_index,
                200_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 2_cha, 0_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BlessedHands,
                AchievementType::HealthGiven,
                3_index,
                500_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 3_cha, 0_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WitchDoctor,
                AchievementType::HealthGiven,
                4_index,
                1000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 4_cha, 0_lck, 0_spd, 4_int),
                1_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LightOfTheCure,
                AchievementType::HealthGiven,
                5_index,
                2000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 5_cha, 0_lck, 0_spd, 5_int),
                1_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DivineTouch,
                AchievementType::HealthGiven,
                6_index,
                3000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 6_cha, 0_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::OrderOfTheWhiteSage,
                AchievementType::HealthGiven,
                7_index,
                5000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 8_cha, 0_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                35_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::ClericOfTheCovenant,
                AchievementType::HealthGiven,
                8_index,
                10000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 10_cha, 0_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfCharity,
                AchievementType::HealthTraded,
                1_index,
                50_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 1_cha, 0_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfLove,
                AchievementType::HealthTraded,
                2_index,
                100_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 2_cha, 0_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                4_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfDevotion,
                AchievementType::HealthTraded,
                3_index,
                200_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 3_cha, 0_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                8_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfNobility,
                AchievementType::HealthTraded,
                4_index,
                400_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 4_cha, 0_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                12_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HandsOfSacrifice,
                AchievementType::HealthTraded,
                5_index,
                800_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 5_cha, 0_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                16_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::TheSelflessHand,
                AchievementType::HealthTraded,
                6_index,
                1000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 6_cha, 0_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::TheIncorruptibleHand,
                AchievementType::HealthTraded,
                7_index,
                2500_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 7_cha, 0_lck, 0_spd, 7_int),
                1_rank,
                0_exp,
                24_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::OrderOfTheBleedingPalm,
                AchievementType::HealthTraded,
                8_index,
                4000_count,
                RoleVec_t{ role::Cleric },
                stats::StatSet(0_str, 0_acc, 10_cha, 0_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HowlingStray,
                AchievementType::BeastRoars,
                1_index,
                10_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(1_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HowlingBully,
                AchievementType::BeastRoars,
                2_index,
                30_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(2_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                4_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HowlingWildling,
                AchievementType::BeastRoars,
                3_index,
                50_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(3_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                8_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SnarlingCreature,
                AchievementType::BeastRoars,
                4_index,
                100_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(4_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                12_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SnarlingBeast,
                AchievementType::BeastRoars,
                5_index,
                200_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(5_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                16_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SnarlingFiend,
                AchievementType::BeastRoars,
                6_index,
                400_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(6_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::RagingMonster,
                AchievementType::BeastRoars,
                7_index,
                1000_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(8_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                24_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::RagingHorror,
                AchievementType::BeastRoars,
                8_index,
                2000_count,
                RoleVec_t{ role::Wolfen, role::Firebrand, role::Sylavin },
                stats::StatSet(10_str, 0_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                30_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Prowler,
                AchievementType::LocksPicked,
                1_index,
                10_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 1_lck, 1_spd, 0_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PickPocket,
                AchievementType::LocksPicked,
                2_index,
                50_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 2_lck, 2_spd, 0_int),
                0_rank,
                0_exp,
                4_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::CatBurglar,
                AchievementType::LocksPicked,
                3_index,
                100_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 3_lck, 3_spd, 0_int),
                0_rank,
                0_exp,
                8_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Clincher,
                AchievementType::LocksPicked,
                4_index,
                200_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 4_lck, 4_spd, 0_int),
                0_rank,
                0_exp,
                12_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Picker,
                AchievementType::LocksPicked,
                5_index,
                500_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 5_lck, 5_spd, 0_int),
                0_rank,
                0_exp,
                16_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SafeCracker,
                AchievementType::LocksPicked,
                6_index,
                1000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 6_lck, 6_spd, 0_int),
                1_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LockTickler,
                AchievementType::LocksPicked,
                7_index,
                2000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 8_lck, 8_spd, 0_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LockBane,
                AchievementType::LocksPicked,
                8_index,
                5000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 10_lck, 10_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Versifier,
                AchievementType::SongsPlayed,
                1_index,
                10_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 1_cha, 1_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Player,
                AchievementType::SongsPlayed,
                2_index,
                50_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 2_cha, 1_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Balladeer,
                AchievementType::SongsPlayed,
                3_index,
                100_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 3_cha, 2_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Minstrel,
                AchievementType::SongsPlayed,
                4_index,
                200_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 4_cha, 2_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Instrumentalist,
                AchievementType::SongsPlayed,
                5_index,
                500_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 5_cha, 3_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Soloist,
                AchievementType::SongsPlayed,
                6_index,
                1000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 6_cha, 3_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Artiste,
                AchievementType::SongsPlayed,
                7_index,
                2000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 8_cha, 5_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                50_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Virtuoso,
                AchievementType::SongsPlayed,
                8_index,
                5000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 10_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                60_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheTrippingToes,
                AchievementType::SpiritsLifted,
                1_index,
                50_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 1_cha, 0_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheMerryMelody,
                AchievementType::SpiritsLifted,
                2_index,
                100_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 2_cha, 0_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                4_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheWhimsicalWord,
                AchievementType::SpiritsLifted,
                3_index,
                200_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 3_cha, 1_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                8_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheCarolingChorus,
                AchievementType::SpiritsLifted,
                4_index,
                500_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 4_cha, 1_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                12_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheBouncingBallad,
                AchievementType::SpiritsLifted,
                5_index,
                1000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 5_cha, 2_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                16_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheDancingDitty,
                AchievementType::SpiritsLifted,
                6_index,
                2000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 6_cha, 2_lck, 0_spd, 6_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheEnchantedEar,
                AchievementType::SpiritsLifted,
                7_index,
                3000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 7_cha, 3_lck, 0_spd, 7_int),
                1_rank,
                0_exp,
                24_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BardOfTheAracneAria,
                AchievementType::SpiritsLifted,
                8_index,
                5000_count,
                RoleVec_t{ role::Bard },
                stats::StatSet(0_str, 0_acc, 10_cha, 5_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Targeter,
                AchievementType::ProjectileHits,
                1_index,
                20_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(1_str, 1_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SteadfastFlyer,
                AchievementType::ProjectileHits,
                2_index,
                50_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(1_str, 2_acc, 0_cha, 2_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DeadEye,
                AchievementType::ProjectileHits,
                3_index,
                100_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(2_str, 3_acc, 0_cha, 3_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DeadCenter,
                AchievementType::ProjectileHits,
                4_index,
                500_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(2_str, 4_acc, 0_cha, 4_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SureSail,
                AchievementType::ProjectileHits,
                5_index,
                1000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(3_str, 5_acc, 0_cha, 5_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SureShot,
                AchievementType::ProjectileHits,
                6_index,
                2000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(3_str, 6_acc, 0_cha, 6_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PerfectHit,
                AchievementType::ProjectileHits,
                7_index,
                3000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(4_str, 8_acc, 0_cha, 8_lck, 0_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::EnchantedAim,
                AchievementType::ProjectileHits,
                8_index,
                5000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(10_str, 10_acc, 0_cha, 10_lck, 0_spd, 0_int),
                1_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Magus,
                AchievementType::SpellsCast,
                1_index,
                20_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 1_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                2_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Charmer,
                AchievementType::SpellsCast,
                2_index,
                50_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 2_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                4_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Diviner,
                AchievementType::SpellsCast,
                3_index,
                100_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 3_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                8_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Enchanter,
                AchievementType::SpellsCast,
                4_index,
                200_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 4_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                12_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Conjurer,
                AchievementType::SpellsCast,
                5_index,
                500_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 5_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                16_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Shaman,
                AchievementType::SpellsCast,
                6_index,
                1000_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 6_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Wizard,
                AchievementType::SpellsCast,
                7_index,
                2000_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 8_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Warlock,
                AchievementType::SpellsCast,
                8_index,
                5000_count,
                RoleVec_t{ role::Sylavin,
                           role::Firebrand,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Cleric,
                           role::Sorcerer },
                stats::StatSet(0_str, 0_acc, 0_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Undaunted,
                AchievementType::EnemiesFaced,
                1_index,
                20_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Daring,
                AchievementType::EnemiesFaced,
                2_index,
                50_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(1_str, 0_acc, 0_cha, 2_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Bold,
                AchievementType::EnemiesFaced,
                3_index,
                100_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(2_str, 0_acc, 0_cha, 3_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Brave,
                AchievementType::EnemiesFaced,
                4_index,
                200_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(3_str, 0_acc, 0_cha, 4_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Valorous,
                AchievementType::EnemiesFaced,
                5_index,
                500_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(4_str, 0_acc, 0_cha, 5_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Valiant,
                AchievementType::EnemiesFaced,
                6_index,
                1000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(5_str, 0_acc, 0_cha, 6_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Lionhearted,
                AchievementType::EnemiesFaced,
                7_index,
                2000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(6_str, 0_acc, 0_cha, 7_lck, 0_spd, 6_int),
                0_rank,
                0_exp,
                35_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Heroic,
                AchievementType::EnemiesFaced,
                8_index,
                3000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(8_str, 0_acc, 0_cha, 8_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                40_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Fearless,
                AchievementType::EnemiesFaced,
                9_index,
                5000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(10_str, 0_acc, 0_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheLunarPup,
                AchievementType::MoonHowls,
                1_index,
                10_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(1_str, 0_acc, 0_cha, 1_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheYoungHowler,
                AchievementType::MoonHowls,
                2_index,
                20_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(2_str, 0_acc, 0_cha, 2_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheNightSky,
                AchievementType::MoonHowls,
                3_index,
                50_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(3_str, 0_acc, 0_cha, 3_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheMidnightWail,
                AchievementType::MoonHowls,
                4_index,
                100_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(4_str, 0_acc, 0_cha, 4_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheTwilightMoon,
                AchievementType::MoonHowls,
                5_index,
                200_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(5_str, 0_acc, 0_cha, 5_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheCelestialCry,
                AchievementType::MoonHowls,
                6_index,
                300_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(6_str, 0_acc, 0_cha, 6_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheGlowingOrb,
                AchievementType::MoonHowls,
                7_index,
                500_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(8_str, 0_acc, 0_cha, 8_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                40_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PawOfTheCrescentLegion,
                AchievementType::MoonHowls,
                8_index,
                800_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(10_str, 0_acc, 0_cha, 10_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WindGlider,
                AchievementType::TurnsInFlight,
                1_index,
                50_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 1_lck, 1_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::LightFeather,
                AchievementType::TurnsInFlight,
                2_index,
                100_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 2_lck, 2_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FreeFlyer,
                AchievementType::TurnsInFlight,
                3_index,
                200_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 3_lck, 3_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SteadyWing,
                AchievementType::TurnsInFlight,
                4_index,
                500_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 4_lck, 4_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::EverSoar,
                AchievementType::TurnsInFlight,
                5_index,
                1000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 5_lck, 5_spd, 0_int),
                0_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SkySail,
                AchievementType::TurnsInFlight,
                6_index,
                2000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 6_lck, 6_spd, 0_int),
                1_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Aerialist,
                AchievementType::TurnsInFlight,
                7_index,
                3000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 8_lck, 8_spd, 0_int),
                1_rank));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FeatherDancer,
                AchievementType::TurnsInFlight,
                8_index,
                5000_count,
                RoleVec_t{ Title::ROLEVEC_ALL_PLAYER_ROLES_ },
                stats::StatSet(0_str, 0_acc, 0_cha, 10_lck, 10_spd, 0_int),
                2_rank));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackFollower,
                AchievementType::PackActions,
                1_index,
                20_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(1_str, 1_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackMember,
                AchievementType::PackActions,
                2_index,
                50_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(2_str, 2_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackTasker,
                AchievementType::PackActions,
                3_index,
                100_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(3_str, 3_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackCharger,
                AchievementType::PackActions,
                4_index,
                200_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(4_str, 4_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackNobel,
                AchievementType::PackActions,
                5_index,
                300_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(5_str, 5_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackLeader,
                AchievementType::PackActions,
                6_index,
                400_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(6_str, 6_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackAlpha,
                AchievementType::PackActions,
                7_index,
                500_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(8_str, 8_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                40_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::PackElder,
                AchievementType::PackActions,
                8_index,
                1000_count,
                RoleVec_t{ role::Wolfen },
                stats::StatSet(10_str, 10_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::CritterClairvoyant,
                AchievementType::BeastMindLinks,
                1_index,
                20_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(1_str, 0_acc, 0_cha, 1_lck, 0_spd, 1_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::AnimalAnimator,
                AchievementType::BeastMindLinks,
                2_index,
                50_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(2_str, 0_acc, 0_cha, 2_lck, 0_spd, 2_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::FriendOfTheFeral,
                AchievementType::BeastMindLinks,
                3_index,
                100_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(3_str, 0_acc, 0_cha, 3_lck, 0_spd, 3_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WillOfTheWild,
                AchievementType::BeastMindLinks,
                4_index,
                200_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(4_str, 0_acc, 0_cha, 4_lck, 0_spd, 4_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::CreatureChanneler,
                AchievementType::BeastMindLinks,
                5_index,
                300_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(5_str, 0_acc, 0_cha, 5_lck, 0_spd, 5_int),
                0_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::BeastTaskmaster,
                AchievementType::BeastMindLinks,
                6_index,
                400_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(6_str, 0_acc, 0_cha, 6_lck, 0_spd, 6_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MonsterManipulator,
                AchievementType::BeastMindLinks,
                7_index,
                500_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(8_str, 0_acc, 0_cha, 8_lck, 0_spd, 8_int),
                1_rank,
                0_exp,
                40_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MammalianMaster,
                AchievementType::BeastMindLinks,
                8_index,
                1000_count,
                RoleVec_t{ role::Beastmaster },
                stats::StatSet(10_str, 0_acc, 0_cha, 10_lck, 0_spd, 10_int),
                2_rank,
                0_exp,
                50_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Brawler,
                AchievementType::MeleeHits,
                1_index,
                20_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(1_str, 1_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                5_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::WildSwing,
                AchievementType::MeleeHits,
                2_index,
                50_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(2_str, 2_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                10_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::HavocSmasher,
                AchievementType::MeleeHits,
                3_index,
                100_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(3_str, 3_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                15_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SureStroke,
                AchievementType::MeleeHits,
                4_index,
                200_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(4_str, 4_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                20_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::Warrior,
                AchievementType::MeleeHits,
                5_index,
                500_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(5_str, 5_acc, 0_cha, 0_lck, 0_spd, 0_int),
                0_rank,
                0_exp,
                25_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::EverMark,
                AchievementType::MeleeHits,
                6_index,
                1000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(6_str, 6_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                30_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::MeleeMaster,
                AchievementType::MeleeHits,
                7_index,
                2000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(7_str, 7_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                40_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::SteadyStriker,
                AchievementType::MeleeHits,
                8_index,
                3000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(8_str, 8_acc, 0_cha, 0_lck, 0_spd, 0_int),
                1_rank,
                0_exp,
                50_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::OrderOfTheEverWarrior,
                AchievementType::MeleeHits,
                9_index,
                5000_count,
                RoleVec_t{ role::Knight,
                           role::Archer,
                           role::Beastmaster,
                           role::Bard,
                           role::Thief,
                           role::Firebrand,
                           role::Sylavin },
                stats::StatSet(10_str, 10_acc, 0_cha, 0_lck, 0_spd, 0_int),
                2_rank,
                0_exp,
                100_health));

            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheUnblinkingEye,
                AchievementType::FlyingAttackHits,
                1_index,
                20_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(1_str, 1_acc, 0_cha, 0_lck, 1_spd, 0_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheCurlingClaw,
                AchievementType::FlyingAttackHits,
                2_index,
                50_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(2_str, 2_acc, 0_cha, 0_lck, 2_spd, 1_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheDaggerTooth,
                AchievementType::FlyingAttackHits,
                3_index,
                100_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(3_str, 3_acc, 0_cha, 0_lck, 3_spd, 2_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheForebodingGaze,
                AchievementType::FlyingAttackHits,
                4_index,
                200_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(4_str, 4_acc, 0_cha, 0_lck, 4_spd, 3_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheUnbreakableScale,
                AchievementType::FlyingAttackHits,
                5_index,
                500_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(5_str, 5_acc, 0_cha, 0_lck, 5_spd, 4_int),
                0_rank,
                0_exp,
                0_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheDreadedBreath,
                AchievementType::FlyingAttackHits,
                6_index,
                1000_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(6_str, 6_acc, 0_cha, 0_lck, 6_spd, 5_int),
                0_rank,
                0_exp,
                1_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheArcaneWing,
                AchievementType::FlyingAttackHits,
                7_index,
                2000_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(8_str, 8_acc, 0_cha, 0_lck, 8_spd, 8_int),
                0_rank,
                0_exp,
                1_health));
            titleUVec_.emplace_back(std::make_unique<Title>(
                Titles::DragonOfTheNightmareSky,
                AchievementType::FlyingAttackHits,
                8_index,
                5000_count,
                RoleVec_t{ role::Firebrand, role::Sylavin },
                stats::StatSet(10_str, 10_acc, 0_cha, 0_lck, 10_spd, 10_int),
                0_rank,
                0_exp,
                2_health));
        }

        void Holder::Empty() { titleUVec_.clear(); }

        TitlePtr_t Holder::Get(const Titles::Enum E)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (titleUVec_.empty() == false),
                "creature::Titles::Holder::Get(\"" << Titles::ToString(E)
                                                   << "\") called when the Holder was empty.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((E >= 0) && (E < Titles::Count)),
                "creature::Titles::Holder::Get(enum=" << E << ")_InvalidValueError.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (static_cast<std::size_t>(E) < titleUVec_.size()),
                "creature::Titles::Holder::Get(\""
                    << Titles::ToString(E) << "\") enum given was " << E
                    << " but that was greater than (or equal to) "
                    << "the vec size of " << titleUVec_.size() << ".");

            return TitlePtr_t{ titleUVec_[static_cast<std::size_t>(E)].get() };
        }

        bool Holder::Test()
        {
            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "creature::Titles::Holder::Test() Starting Tests...");
            }

            static sfml_util::gui::TitleImageLoader titleImageLoader;

            static misc::EnumUnderlying_t titleIndex{ 0 };
            if (titleIndex < Titles::Count)
            {
                auto const NEXT_ENUM(static_cast<Titles::Enum>(titleIndex));
                auto TITLE_PTR{ Get(NEXT_ENUM) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TITLE_PTR->Desc().empty() == false),
                    "creature::Titles::Holder::Test(\"" << Titles::ToString(NEXT_ENUM)
                                                        << "\") resulted in an empty Desc().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TITLE_PTR->LongDesc().empty() == false),
                    "creature::Titles::Holder::Test(\"" << Titles::ToString(NEXT_ENUM)
                                                        << "\") resulted in an empty LongDesc().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TITLE_PTR->ImageFilename().empty() == false),
                    "creature::Titles::Holder::Test(\""
                        << Titles::ToString(NEXT_ENUM)
                        << "\") resulted in an empty ImageFilename().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TITLE_PTR->RolesCopy().empty() == false),
                    "creature::Titles::Holder::Test(\"" << Titles::ToString(NEXT_ENUM)
                                                        << "\") resulted in an empty RolesVec().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TITLE_PTR->Which() == NEXT_ENUM),
                    "creature::Titles::Holder::Test(\""
                        << Titles::ToString(NEXT_ENUM)
                        << "\") resulted in a Title with a different tile::Enum (\""
                        << Titles::ToString(TITLE_PTR->Which()) << "\")");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (TITLE_PTR->Name() == Titles::Name(NEXT_ENUM)),
                    "creature::Titles::Holder::Test(\"" << Titles::ToString(NEXT_ENUM)
                                                        << "\") Title is out of order.");

                sf::Texture texture;
                titleImageLoader.Get(texture, NEXT_ENUM);
                game::LoopManager::Instance()->TestingImageSet(texture);
                game::LoopManager::Instance()->TestingStrIncrement("Title Test #");
                ++titleIndex;
                return false;
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "creature::Titles::Holder::Test()  ALL TESTS PASSED.");

            return true;
        }

    } // namespace title
} // namespace creature
} // namespace heroespath
