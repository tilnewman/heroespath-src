// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// achievements.cpp
//
#include "achievements.hpp"

#include "creature/creature.hpp"
#include "creature/title-holder.hpp"
#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace creature
{

    Achievements::Achievements(
        const std::string & OWNING_CREATURE_NAME, const creature::role::Enum & OWNING_CREATURE_ROLE)
        : name_(OWNING_CREATURE_NAME)
        , role_(OWNING_CREATURE_ROLE)
        , map_()
    {
        map_.Reserve(64);

        AchievementMapInsertPair(
            AchievementType::DodgedStanding, Titles::Dodger, Titles::MeleeShadow);

        AchievementMapInsertPair(
            AchievementType::DodgedFlying, Titles::SkyDodger, Titles::SkyShadow);

        AchievementMapInsertPair(AchievementType::BackstabsHits, Titles::Sneak, Titles::Assassin);

        AchievementMapInsertPair(
            AchievementType::BattlesSurvived,
            Titles::KnightOfTheClashingBlade,
            Titles::KnightOfEtan);

        AchievementMapInsertPair(
            AchievementType::HealthGiven, Titles::Mender, Titles::ClericOfTheCovenant);

        AchievementMapInsertPair(
            AchievementType::HealthTraded, Titles::HandsOfCharity, Titles::OrderOfTheBleedingPalm);

        AchievementMapInsertPair(
            AchievementType::BeastRoars, Titles::HowlingStray, Titles::RagingHorror);

        AchievementMapInsertPair(AchievementType::LocksPicked, Titles::Prowler, Titles::LockBane);

        AchievementMapInsertPair(AchievementType::SongsPlayed, Titles::Versifier, Titles::Virtuoso);

        AchievementMapInsertPair(
            AchievementType::SpiritsLifted,
            Titles::BardOfTheTrippingToes,
            Titles::BardOfTheAracneAria);

        AchievementMapInsertPair(
            AchievementType::ProjectileHits, Titles::Targeter, Titles::EnchantedAim);

        AchievementMapInsertPair(AchievementType::SpellsCast, Titles::Magus, Titles::Warlock);

        AchievementMapInsertPair(
            AchievementType::EnemiesFaced, Titles::Undaunted, Titles::Fearless);

        AchievementMapInsertPair(
            AchievementType::MoonHowls, Titles::PawOfTheLunarPup, Titles::PawOfTheCrescentLegion);

        AchievementMapInsertPair(
            AchievementType::PackActions, Titles::PackFollower, Titles::PackElder);

        AchievementMapInsertPair(
            AchievementType::TurnsInFlight, Titles::WindGlider, Titles::FeatherDancer);

        AchievementMapInsertPair(
            AchievementType::BeastMindLinks, Titles::CritterClairvoyant, Titles::MammalianMaster);

        AchievementMapInsertPair(
            AchievementType::MeleeHits, Titles::Brawler, Titles::OrderOfTheEverWarrior);

        AchievementMapInsertPair(
            AchievementType::FlyingAttackHits,
            Titles::DragonOfTheUnblinkingEye,
            Titles::DragonOfTheNightmareSky);
    }

    const Achievement Achievements::Get(const AchievementType::Enum ENUM) const
    {
        const auto FOUND_ITER { map_.Find(ENUM) };
        if (FOUND_ITER == map_.end())
        {
            std::ostringstream ss;
            ss << "creature::Achievements::Get(" << ENUM << ") not found in the map_.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return FOUND_ITER->second;
        }
    }

    TitlePtrOpt_t Achievements::Increment(const AchievementType::Enum ENUM)
    {
        const auto FOUND_ITER { map_.Find(ENUM) };
        if (FOUND_ITER == map_.end())
        {
            std::ostringstream ss;
            ss << "creature::Achievements::Increment(which_enum=" << ENUM
               << ", creature_name=" << name_
               << ") was given an AchievementType::Enum that was not found in the map.";

            throw std::runtime_error(ss.str());
        }
        else
        {
            return FOUND_ITER->second.Increment(role_);
        }
    }

    TitlePtrOpt_t Achievements::GetCurrentTitle(const AchievementType::Enum ENUM) const
    {
        const auto FOUND_ITER { map_.Find(ENUM) };
        if (FOUND_ITER == map_.end())
        {
            return boost::none;
        }
        else
        {
            return FOUND_ITER->second.GetCurrentTitle();
        }
    }

    TitlePtrOpt_t Achievements::GetNextTitle(const AchievementType::Enum ENUM) const
    {
        const auto FOUND_ITER { map_.Find(ENUM) };
        if (FOUND_ITER == map_.end())
        {
            return boost::none;
        }
        else
        {
            return FOUND_ITER->second.GetNextTitle();
        }
    }

    void Achievements::AchievementMapInsertPair(
        const AchievementType::Enum ACHV_TYPE,
        const Titles::Enum TITLE_FIRST,
        const Titles::Enum TITLE_LAST)
    {
        TitleCountMap_t titleCountMap;

        for (EnumUnderlying_t t(TITLE_FIRST); t <= TITLE_LAST; ++t)
        {
            const auto NEXT_ENUM { static_cast<Titles::Enum>(t) };
            const auto NEXT_TITLE_PTR { title::Holder::Get(NEXT_ENUM) };
            titleCountMap[NEXT_TITLE_PTR->AchievementCount()] = NEXT_ENUM;
        }

        map_[ACHV_TYPE] = Achievement(ACHV_TYPE, titleCountMap);
    }

} // namespace creature
} // namespace heroespath
