//
// achievements.cpp
//
#include "achievements.hpp"

#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/creature/titles.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace creature
{

    Achievements::Achievements()
    :
        map_()
    {
        AchievementMapInsertPair(AchievementType::DodgedStanding, title::Dodger, title::MeleeShadow);
        AchievementMapInsertPair(AchievementType::DodgedFlying, title::SkyDodger, title::SkyShadow);
        AchievementMapInsertPair(AchievementType::BackstabsHits, title::Sneak, title::Assassin);
        AchievementMapInsertPair(AchievementType::BattlesSurvived, title::KnightOfTheClashingBlade, title::KnightOfEtan);
        AchievementMapInsertPair(AchievementType::HealthGiven, title::Mender, title::ClericOfTheCovenant);
        AchievementMapInsertPair(AchievementType::HealthTraded, title::HandsOfCharity, title::OrderOfTheBleedingPalm);
        AchievementMapInsertPair(AchievementType::BeastRoars, title::HowlingStray, title::RagingHorror);
        AchievementMapInsertPair(AchievementType::LocksPicked, title::Prowler, title::LockBane);
        AchievementMapInsertPair(AchievementType::SongsPlayed, title::Versifier, title::Virtuoso);
        AchievementMapInsertPair(AchievementType::SpiritsLifted, title::BardOfTheTrippingToes, title::BardOfTheAracneAria);
        AchievementMapInsertPair(AchievementType::ProjectileHits, title::Targeter, title::EnchantedAim);
        AchievementMapInsertPair(AchievementType::SpellsCast, title::Magus, title::Warlock);
        AchievementMapInsertPair(AchievementType::EnemiesFaced, title::Undaunted, title::Fearless);
        AchievementMapInsertPair(AchievementType::MoonHowls, title::PawOfTheLunarPup, title::PawOfTheCrescentLegion);
        AchievementMapInsertPair(AchievementType::PackActions, title::PackFollower, title::PackElder);
        AchievementMapInsertPair(AchievementType::TurnsInFlight, title::WindGlider, title::FeatherDancer);
        AchievementMapInsertPair(AchievementType::BeastMindLinks, title::CritterClairvoyant, title::MammalianMaster);
        AchievementMapInsertPair(AchievementType::MeleeHits, title::Brawler, title::OrderOfTheSteadyBlade);
        AchievementMapInsertPair(AchievementType::FlyingAttackHits, title::DragonOfTheUnblinkingEye, title::DragonOfTheNightmareSky);
    }


    const Achievement Achievements::AchievementCopy(const AchievementType::Enum E) const
    {
        const AchievementMapCIter_t CITER(map_.find(E));
        if (CITER == map_.end())
        {
            std::ostringstream ss;
            ss << "heroespath::creature::Achievements::AcievementCopy(" << E << ")_InvalidValueError.";
            throw std::runtime_error(ss.str());
        }
        else
            return CITER->second;
    }


    TitleCPtrC_t Achievements::Increment(const AchievementType::Enum E, const CreatureSPtr_t & CREATURE_SPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_SPTR.get() != nullptr), "heroespath::creature::Achievements::Increment(which_enum=" << E << ", creature_sptr=" << CREATURE_SPTR.get() << ") was given a CREATURE_SPTR that was null.");

        const AchievementMapIter_t ITER(map_.find(E));
        if (ITER == map_.end())
        {
            std::ostringstream ss;
            ss << "heroespath::creature::Achievements::Increment(which_enum=" << E << ", creature_name=" << CREATURE_SPTR->Name() << ") was given an invalid AchievementType::Enum.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return ITER->second.Increment(CREATURE_SPTR);
        }
    }


    TitleCPtrC_t Achievements::GetCurrentTitle(const AchievementType::Enum E) const
    {
        const AchievementMapCIter_t CITER(map_.find(E));
        if (CITER == map_.end())
        {
            return TitlePtr_t(nullptr);
        }
        else
        {
            return CITER->second.GetCurrentTitle();
        }
    }


    TitleCPtrC_t Achievements::GetNextTitle(const AchievementType::Enum E) const
    {
        const AchievementMapCIter_t CITER(map_.find(E));
        if (CITER == map_.end())
        {
            return TitlePtr_t(nullptr);
        }
        else
        {
            return CITER->second.GetNextTitle();
        }
    }


    void Achievements::AchievementMapInsertPair(const AchievementType::Enum ACHV_TYPE,
                                                const title::Enum           TITLE_FIRST,
                                                const title::Enum           TITLE_LAST)
    {
        TitleCountMap_t titleCountMap;

        for (int t(TITLE_FIRST); t < TITLE_LAST; ++t)
        {
            const TitlePtr_t NEXT_TITLE_SPTR(title::TitleWarehouse::Get(static_cast<title::Enum>(t)));
            titleCountMap.insert( std::make_pair(NEXT_TITLE_SPTR->AchievementCount(), NEXT_TITLE_SPTR) );
        }

        map_.insert(std::make_pair(ACHV_TYPE, Achievement(ACHV_TYPE, titleCountMap)));
    }

}
}
