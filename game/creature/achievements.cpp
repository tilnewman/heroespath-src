//
// achievements.cpp
//
#include "achievements.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/titles.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace creature
{

    Achievements::Achievements()
    :
        map_()
    {
        AchievementMapInsertPair(AchievementType::DodgedStanding,   Titles::Dodger,                     Titles::MeleeShadow);
        AchievementMapInsertPair(AchievementType::DodgedFlying,     Titles::SkyDodger,                  Titles::SkyShadow);
        AchievementMapInsertPair(AchievementType::BackstabsHits,    Titles::Sneak,                      Titles::Assassin);
        AchievementMapInsertPair(AchievementType::BattlesSurvived,  Titles::KnightOfTheClashingBlade,   Titles::KnightOfEtan);
        AchievementMapInsertPair(AchievementType::HealthGiven,      Titles::Mender,                     Titles::ClericOfTheCovenant);
        AchievementMapInsertPair(AchievementType::HealthTraded,     Titles::HandsOfCharity,             Titles::OrderOfTheBleedingPalm);
        AchievementMapInsertPair(AchievementType::BeastRoars,       Titles::HowlingStray,               Titles::RagingHorror);
        AchievementMapInsertPair(AchievementType::LocksPicked,      Titles::Prowler,                    Titles::LockBane);
        AchievementMapInsertPair(AchievementType::SongsPlayed,      Titles::Versifier,                  Titles::Virtuoso);
        AchievementMapInsertPair(AchievementType::SpiritsLifted,    Titles::BardOfTheTrippingToes,      Titles::BardOfTheAracneAria);
        AchievementMapInsertPair(AchievementType::ProjectileHits,   Titles::Targeter,                   Titles::EnchantedAim);
        AchievementMapInsertPair(AchievementType::SpellsCast,       Titles::Magus,                      Titles::Warlock);
        AchievementMapInsertPair(AchievementType::EnemiesFaced,     Titles::Undaunted,                  Titles::Fearless);
        AchievementMapInsertPair(AchievementType::MoonHowls,        Titles::PawOfTheLunarPup,           Titles::PawOfTheCrescentLegion);
        AchievementMapInsertPair(AchievementType::PackActions,      Titles::PackFollower,               Titles::PackElder);
        AchievementMapInsertPair(AchievementType::TurnsInFlight,    Titles::WindGlider,                 Titles::FeatherDancer);
        AchievementMapInsertPair(AchievementType::BeastMindLinks,   Titles::CritterClairvoyant,         Titles::MammalianMaster);
        AchievementMapInsertPair(AchievementType::MeleeHits,        Titles::Brawler,                    Titles::OrderOfTheSteadyBlade);
        AchievementMapInsertPair(AchievementType::FlyingAttackHits, Titles::DragonOfTheUnblinkingEye,   Titles::DragonOfTheNightmareSky);
    }


    const Achievement Achievements::AchievementCopy(const AchievementType::Enum E) const
    {
        const AchievementMapCIter_t CITER(map_.find(E));
        if (CITER == map_.end())
        {
            std::ostringstream ss;
            ss << "game::creature::Achievements::AcievementCopy(" << E << ")_InvalidValueError.";
            throw std::runtime_error(ss.str());
        }
        else
            return CITER->second;
    }


    TitlePtr_t Achievements::Increment(const AchievementType::Enum E, const CreaturePtr_t CREATURE_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr), "game::creature::Achievements::Increment(which_enum=" << E << ", creature_ptr=" << CREATURE_PTR << ") was given a CREATURE_PTR that was null.");

        const AchievementMapIter_t ITER(map_.find(E));
        if (ITER == map_.end())
        {
            std::ostringstream ss;
            ss << "game::creature::Achievements::Increment(which_enum=" << E << ", creature_name=" << CREATURE_PTR->Name() << ") was given an invalid AchievementType::Enum.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return ITER->second.Increment(CREATURE_PTR);
        }
    }


    TitlePtr_t Achievements::GetCurrentTitle(const AchievementType::Enum E) const
    {
        const AchievementMapCIter_t CITER(map_.find(E));
        if (CITER == map_.end())
        {
            return nullptr;
        }
        else
        {
            return CITER->second.GetCurrentTitle();
        }
    }


    TitlePtr_t Achievements::GetNextTitle(const AchievementType::Enum E) const
    {
        const AchievementMapCIter_t CITER(map_.find(E));
        if (CITER == map_.end())
        {
            return nullptr;
        }
        else
        {
            return CITER->second.GetNextTitle();
        }
    }


    void Achievements::AchievementMapInsertPair(const AchievementType::Enum ACHV_TYPE,
                                                const Titles::Enum          TITLE_FIRST,
                                                const Titles::Enum          TITLE_LAST)
    {
        TitleCountMap_t titleCountMap;

        for (int t(TITLE_FIRST); t <= TITLE_LAST; ++t)
        {
            auto const NEXT_ENUM{ static_cast<Titles::Enum>(t) };
            auto const NEXT_TITLE_PTR(title::Warehouse::Get(NEXT_ENUM));
            titleCountMap.insert( std::make_pair(NEXT_TITLE_PTR->AchievementCount(), NEXT_ENUM) );
        }

        map_.insert(std::make_pair(ACHV_TYPE, Achievement(ACHV_TYPE, titleCountMap)));
    }

}
}
