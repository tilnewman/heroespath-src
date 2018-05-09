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
// item-profiles-reporter.cpp
//
#include "item-profiles-reporter.hpp"

#include "item-profile-warehouse.hpp"
#include "log/log-macros.hpp"

#include <algorithm>

namespace heroespath
{
namespace item
{
    void ItemProfilesReporter::LogReport()
    {
        std::ostringstream ss;
        ss << "\n\n*** Item Profile Report ***\n";

        auto const & NORMAL_PROFILES{ ItemProfileWarehouse::Instance()->GetNormalProfiles() };

        auto const NORMAL_PROFILES_COUNT{ NORMAL_PROFILES.size() };

        auto const NORMAL_PROFILES_SCORE_SUM{ std::accumulate(
            std::begin(NORMAL_PROFILES),
            std::end(NORMAL_PROFILES),
            0_score,
            [](auto const SUBTOTAL, auto const & PROFILE) {
                return SUBTOTAL + PROFILE.TreasureScore();
            }) };

        auto const & RELIGIOUS_PROFILES{ ItemProfileWarehouse::Instance()->GetReligiousProfiles() };

        auto const RELIGIOUS_PROFILES_COUNT{ RELIGIOUS_PROFILES.size() };

        auto const RELIGIOUS_PROFILES_SCORE_SUM{ std::accumulate(
            std::begin(RELIGIOUS_PROFILES),
            std::end(RELIGIOUS_PROFILES),
            0_score,
            [](auto const SUBTOTAL, auto const & PROFILE) {
                return SUBTOTAL + PROFILE.ReligiousScore();
            }) };

        auto const ALL_PROFILES_COUNT{ NORMAL_PROFILES_COUNT + RELIGIOUS_PROFILES_COUNT };

        auto const ALL_PROFILES_SCORE_SUM{ NORMAL_PROFILES_SCORE_SUM
                                           + RELIGIOUS_PROFILES_SCORE_SUM };

        ss << "total_count=" << ALL_PROFILES_COUNT << ", total_score_sum=" << ALL_PROFILES_SCORE_SUM
           << ", total_score_avg="
           << DivideAsDouble(ALL_PROFILES_SCORE_SUM.As<std::size_t>(), ALL_PROFILES_COUNT)
           << ", rel_count=" << RELIGIOUS_PROFILES_COUNT << "("
           << DivideAsDouble(RELIGIOUS_PROFILES_COUNT, ALL_PROFILES_COUNT) << "%)"
           << ", rel_score_sum=" << RELIGIOUS_PROFILES_SCORE_SUM << ", rel_score_avg="
           << DivideAsDouble(
                  RELIGIOUS_PROFILES_SCORE_SUM.As<std::size_t>(), RELIGIOUS_PROFILES_COUNT)
           << "\n";

        auto const SOURCE_PROFILES{ SourceProfiles(
            ALL_PROFILES_COUNT,
            RELIGIOUS_PROFILES_COUNT,
            ALL_PROFILES_SCORE_SUM.As<std::size_t>(),
            RELIGIOUS_PROFILES_SCORE_SUM.As<std::size_t>()) };

        Report allReport("all");

        // these three should include all profiles in a mutually exclusive way
        WeaponOrArmorReport weaponReport(true, "-non-misc");
        WeaponOrArmorReport armorReport(false, "-non-misc");
        Report miscReport("misc");
        //
        WeaponOrArmorReport standardWeaponReport(true, "-standard");
        WeaponOrArmorReport standardArmorReport(false, "-standard");
        Report ordinaryMiscReport("ordinary_misc");
        StandardSetReport weaponOrArmorMisc("weapon_or_armor_misc");
        StandardSetReport magicalReport("magical");
        Report summoningReport("summoning");
        Report uniqueReport("unique");
        StandardSetReport setReport("set");
        StandardSetReport namedReport("named");
        StandardSetReport elementReport("elemental");
        Report religiousReport("religious");
        StandardSetReport pixieReport("pixie");

        auto appendProfileToAllRequiredReports{ [&](const ItemProfile & PROFILE) {
            allReport.Add(PROFILE, true);

            if (weaponReport.Add(PROFILE, (PROFILE.IsWeapon() && (PROFILE.IsMisc() == false)))
                == false)
            {
                if (armorReport.Add(PROFILE, (PROFILE.IsArmor() && (PROFILE.IsMisc() == false)))
                    == false)
                {
                    if (miscReport.Add(PROFILE, PROFILE.IsMisc()) == false)
                    {
                        M_HP_LOG_ERR(
                            "item::ItemProfileWarehouse::LogReportOnAllProfiles()  A profile "
                            "was "
                            "not either non-misc-weapon, non-misc-armor, or "
                            "misc.  profile={"
                            << PROFILE.ToString() << "}");
                    }
                }
            }

            weaponOrArmorMisc.Add(
                PROFILE, (PROFILE.IsMisc() && (PROFILE.IsWeapon() || PROFILE.IsArmor())));

            standardWeaponReport.Add(
                PROFILE, (PROFILE.IsNonMagicalWeaponOrArmor() && PROFILE.IsWeapon()));

            standardArmorReport.Add(
                PROFILE, (PROFILE.IsNonMagicalWeaponOrArmor() && PROFILE.IsArmor()));

            ordinaryMiscReport.Add(PROFILE, misc_type::IsOrdinary(PROFILE.MiscType()));
            magicalReport.Add(PROFILE, PROFILE.IsMagical());
            summoningReport.Add(PROFILE, PROFILE.SummonInfo().CanSummon());
            uniqueReport.Add(PROFILE, PROFILE.IsUnique());
            setReport.Add(PROFILE, PROFILE.IsSet());
            namedReport.Add(PROFILE, PROFILE.IsNamed());
            elementReport.Add(PROFILE, PROFILE.IsElemental());
            religiousReport.Add(PROFILE, PROFILE.IsReligious());
            pixieReport.Add(PROFILE, PROFILE.IsPixie());
        } };

        for (auto const & PROFILE : NORMAL_PROFILES)
        {
            appendProfileToAllRequiredReports(PROFILE);
        }

        for (auto const & PROFILE : RELIGIOUS_PROFILES)
        {
            appendProfileToAllRequiredReports(PROFILE);
        }

        ss << "non_misc_weapon_count=" << weaponReport.Count()
           << PercentToString(weaponReport.Count(), ALL_PROFILES_COUNT)
           << ", non_misc_armor=" << armorReport.Count()
           << PercentToString(armorReport.Count(), ALL_PROFILES_COUNT)
           << ", misc_maybe_weapon_or_armor=" << miscReport.Count()
           << PercentToString(miscReport.Count(), ALL_PROFILES_COUNT) << '\n';

        auto const SPLIT_COUNT{ (weaponReport.Count() + armorReport.Count() + miscReport.Count()) };
        if (SPLIT_COUNT > ALL_PROFILES_COUNT)
        {
            M_HP_LOG_ERR(
                "item::ItemProfileWarehouse::LogReportOnAllProfiles() The three mutually "
                "exclusive "
                "profile categories added up ("
                << SPLIT_COUNT << ") to more than the total count of all profiles ("
                << ALL_PROFILES_COUNT << ").  It was " << (SPLIT_COUNT - ALL_PROFILES_COUNT)
                << " over.");
        }
        else if (SPLIT_COUNT < ALL_PROFILES_COUNT)
        {
            M_HP_LOG_ERR(
                "item::ItemProfileWarehouse::LogReportOnAllProfiles() The three mutually "
                "exclusive "
                "profile categories did not add up ("
                << SPLIT_COUNT << ") to the total count of all profiles (" << ALL_PROFILES_COUNT
                << ").  It was " << (ALL_PROFILES_COUNT - SPLIT_COUNT) << " under.");
        }

        ss << allReport.ToString(SOURCE_PROFILES) << weaponReport.ToString(SOURCE_PROFILES)
           << standardWeaponReport.ToString(SOURCE_PROFILES)
           << armorReport.ToString(SOURCE_PROFILES) << standardArmorReport.ToString(SOURCE_PROFILES)
           << miscReport.ToString(SOURCE_PROFILES) << ordinaryMiscReport.ToString(SOURCE_PROFILES)
           << weaponOrArmorMisc.ToString(SOURCE_PROFILES) << magicalReport.ToString(SOURCE_PROFILES)
           << summoningReport.ToString(SOURCE_PROFILES) << uniqueReport.ToString(SOURCE_PROFILES)
           << setReport.ToString(SOURCE_PROFILES) << namedReport.ToString(SOURCE_PROFILES)
           << elementReport.ToString(SOURCE_PROFILES) << religiousReport.ToString(SOURCE_PROFILES)
           << pixieReport.ToString(SOURCE_PROFILES);

        const std::size_t MIN_MAX_DISPLAY_COUNT{ 10 };
        {
            // min scores
            {
                ss << MIN_MAX_DISPLAY_COUNT << " Normal Items with the lowest scores:";
                for (std::size_t i(0); i < MIN_MAX_DISPLAY_COUNT; ++i)
                {
                    ss << "\n\t" << NORMAL_PROFILES.at(i).TreasureScore() << " {"
                       << NORMAL_PROFILES.at(i).ToString() << "}";
                }
            }

            ss << "\n\n";

            // max scores
            {
                ss << MIN_MAX_DISPLAY_COUNT << " Normal Items with the highest scores:";
                for (std::size_t i(NORMAL_PROFILES.size() - (MIN_MAX_DISPLAY_COUNT + 1));
                     i < NORMAL_PROFILES.size();
                     ++i)
                {
                    ss << "\n\t" << NORMAL_PROFILES.at(i).TreasureScore() << " {"
                       << NORMAL_PROFILES.at(i).ToString() << "}";
                }
            }
        }

        ss << "\n\n";

        {
            // min scores
            {
                ss << MIN_MAX_DISPLAY_COUNT << " Religious Items with the lowest scores:";
                for (std::size_t i(0); i < MIN_MAX_DISPLAY_COUNT; ++i)
                {
                    ss << "\n\t" << RELIGIOUS_PROFILES.at(i).TreasureScore() << " {"
                       << RELIGIOUS_PROFILES.at(i).ToString() << "}";
                }
            }

            ss << "\n\n";

            // max scores
            {
                ss << MIN_MAX_DISPLAY_COUNT << " Religious Items with the highest scores:";
                for (std::size_t i(RELIGIOUS_PROFILES.size() - (MIN_MAX_DISPLAY_COUNT + 1));
                     i < RELIGIOUS_PROFILES.size();
                     ++i)
                {
                    ss << "\n\t" << RELIGIOUS_PROFILES.at(i).TreasureScore() << " {"
                       << RELIGIOUS_PROFILES.at(i).ToString() << "}";
                }
            }
        }

        ss << "\n\n*** Item Profile Report ***\n\n";

        M_HP_LOG(ss.str());
    }

    bool ItemProfilesReporter::Report::Add(const ItemProfile & PROFILE, const bool WILL_ADD)
    {
        if (WILL_ADD)
        {
            auto const NORMAL_SCORE{ PROFILE.TreasureScore().As<std::size_t>() };
            auto const RELIGIOIUS_SCORE{ PROFILE.ReligiousScore().As<std::size_t>() };

            scores_.emplace_back(NORMAL_SCORE);

            if (PROFILE.IsReligious())
            {
                religiousScores_.emplace_back(RELIGIOIUS_SCORE);
            }

            if (PROFILE.IsMisc())
            {
                ++miscCount_;
            }

            if (PROFILE.ElementType() != element_type::None)
            {
                ++elementalCount;
                elementScoreMap_[PROFILE.ElementType()].emplace_back(NORMAL_SCORE);
            }

            if (PROFILE.IsSet())
            {
                scoresSet_.emplace_back(NORMAL_SCORE);
            }
            else if (PROFILE.IsNamed())
            {
                scoresNamed_.emplace_back(NORMAL_SCORE);
            }
            else if (PROFILE.IsUnique())
            {
                scoresUnique_.emplace_back(NORMAL_SCORE);
            }
        }

        return WILL_ADD;
    }

    const std::string ItemProfilesReporter::Report::ToString(const SourceProfiles & SOURCE_PROFILES)
    {
        std::ostringstream ss;
        ss << "\n---------------" << description_ << "\n";

        if (Count() <= 0)
        {
            ss << "\t*** EMPTY REPORT ERROR ***";

            M_HP_LOG_ERR(
                "item::ItemProfilesReporter::Report::ToString() for report \"" << description_
                                                                               << "\" was empty.");
        }
        else
        {
            std::sort(std::begin(scores_), std::end(scores_));
            std::sort(std::begin(religiousScores_), std::end(religiousScores_));
            ss << Make(SOURCE_PROFILES);
        }

        ss << "\n---------------\n\n\n";
        return ss.str();
    }

    const std::string ItemProfilesReporter::Report::Make(
        const SourceProfiles & SOURCE_PROFILES, const bool WILL_LIMIT_TO_ONE_LINE) const
    {
        std::ostringstream ss;

        if (WILL_LIMIT_TO_ONE_LINE == false)
        {
            ss << std::setw(20) << std::quoted(description_);
        }

        ss << ReportCountPhrase(SOURCE_PROFILES.normal_count)
           << SumPhrase("score", scores_, SOURCE_PROFILES.normal_score_sum);

        if (WILL_LIMIT_TO_ONE_LINE == false)
        {
            const std::size_t CHUNK_COUNT{ 24 };

            auto const CHUNK_SIZE{ (
                (scores_.size() < CHUNK_COUNT) ? 1 : (scores_.size() / CHUNK_COUNT)) };

            ss << "\n---(" << CHUNK_SIZE << ")---";
            for (std::size_t chunkIndex(0); chunkIndex < CHUNK_COUNT; ++chunkIndex)
            {
                const std::size_t MIN_DEFAULT{ 99999999 }; // yeah...i know...
                std::size_t min{ MIN_DEFAULT };
                std::size_t max{ 0 };
                std::size_t sum{ 0 };

                for (std::size_t index(0); index < CHUNK_SIZE; ++index)
                {
                    auto const ACTUAL_INDEX{ (chunkIndex * CHUNK_SIZE) + index };
                    if (ACTUAL_INDEX < scores_.size())
                    {
                        auto const VALUE{ scores_.at(ACTUAL_INDEX) };

                        if (VALUE < min)
                        {
                            min = VALUE;
                        }

                        if (VALUE > max)
                        {
                            max = VALUE;
                        }

                        sum += VALUE;
                    }
                    else
                    {
                        break;
                    }
                }

                auto const AVERAGE{ static_cast<std::size_t>(DivideAsDouble(sum, CHUNK_SIZE)) };

                ss << "\n-\t[" << std::setw(4) << ((MIN_DEFAULT == min) ? 0 : min) << ", "
                   << std::setw(4) << AVERAGE << ", " << std::setw(4) << max
                   << "]\trange=" << (max - min) << "\tavg=" << AVERAGE;
            }
            ss << "\n---(" << CHUNK_SIZE << ")---";

            if (religiousScores_.size() > 0)
            {
                ss << '\n'
                   << CountPhrase(
                          "religious", religiousScores_.size(), SOURCE_PROFILES.religious_count);

                ss << SumPhrase("religious", religiousScores_, SOURCE_PROFILES.religious_score_sum);
            }

            if (miscCount_ > 0)
            {
                ss << '\n' << CountPhrase("misc", miscCount_, Count());
            }

            auto appendScoresSubReport{ [&](const std::string & NAME,
                                            std::vector<std::size_t> & scores) {
                if (scores.empty() == false)
                {
                    std::sort(std::begin(scores), std::end(scores));

                    misc::Vector::MinMaxAvgSum<std::size_t> stats(scores);

                    ss << '\n'
                       << std::setw(10) << NAME << "count=" << std::setw(8) << scores.size()
                       << PercentToString(scores.size(), SOURCE_PROFILES.normal_count, true) << "["
                       << std::setw(4) << stats.min << ", " << std::setw(4) << stats.avg << ", "
                       << std::setw(4) << stats.max << "]("
                       << misc::Vector::StandardDeviation(scores, scores.size(), stats.avg)
                       << "std)";
                }
            } };

            appendScoresSubReport("Set", scoresSet_);
            appendScoresSubReport("Named", scoresNamed_);
            appendScoresSubReport("Unique", scoresUnique_);

            if (elementalCount > 0)
            {
                auto const SCORES_SUM{ misc::Vector::SumVec(scores_) };

                ss << "\nElemental Items  count=" << elementalCount
                   << PercentToString(elementalCount, scores_.size());

                for (auto & elementTypeScoreVecPair : elementScoreMap_)
                {
                    std::sort(
                        std::begin(elementTypeScoreVecPair.second),
                        std::end(elementTypeScoreVecPair.second));

                    ss << "\n\t" << std::setw(20)
                       << element_type::ToString(elementTypeScoreVecPair.first, true, "/")
                       << "\t\tcount=" << std::setw(7) << elementTypeScoreVecPair.second.size()
                       << PercentToString(elementTypeScoreVecPair.second.size(), elementalCount)
                       << SumPhrase("", elementTypeScoreVecPair.second, SCORES_SUM);
                }
            }
        }

        return ss.str();
    }

    const std::string
        ItemProfilesReporter::Report::ReportCountPhrase(const std::size_t COUNT_COMPARED_WITH) const
    {
        std::ostringstream ss;
        ss << ", " << std::setw(16) << description_ << "_count=" << std::setw(8) << Count()
           << PercentToString(Count(), COUNT_COMPARED_WITH);

        return ss.str();
    }

    bool ItemProfilesReporter::WeaponOrArmorReport::Add(
        const ItemProfile & PROFILE, const bool WILL_ADD)
    {
        if (WILL_ADD)
        {
            if (isWeaponReport_ && PROFILE.IsWeapon())
            {
                Report::Add(PROFILE, true);
                reportMap_[PROFILE.WeaponInfo().GeneralName()].Add(PROFILE, true);

                auto & specificItemInfo{ specificMap_[PROFILE.WeaponInfo().SpecificName()] };

                specificItemInfo.scores.emplace_back(PROFILE.TreasureScore().As<std::size_t>());

                specificItemInfo.material_pairs.emplace_back(
                    PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());
            }
            else if ((false == isWeaponReport_) && PROFILE.IsArmor())
            {
                Report::Add(PROFILE, true);
                reportMap_[PROFILE.ArmorInfo().GeneralName()].Add(PROFILE, true);

                auto & specificItemInfo{ specificMap_[PROFILE.ArmorInfo().SpecificName()] };

                specificItemInfo.scores.emplace_back(PROFILE.TreasureScore().As<std::size_t>());

                specificItemInfo.material_pairs.emplace_back(
                    PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());
            }
        }

        return WILL_ADD;
    }

    const std::string ItemProfilesReporter::WeaponOrArmorReport::Make(
        const SourceProfiles & SOURCE_PROFILES, const bool) const
    {
        std::ostringstream ss;
        ss << Report::Make(SOURCE_PROFILES);

        auto const SUB_REPORT_SOURCES{ SourceProfiles(
            scores_.size(),
            religiousScores_.size(),
            misc::Vector::SumVec(scores_),
            misc::Vector::SumVec(religiousScores_)) };

        for (auto & stringReportPair : reportMap_)
        {
            stringReportPair.second.SetDescription(stringReportPair.first);
            ss << '\n' << stringReportPair.second.Make(SUB_REPORT_SOURCES, true);
        }

        ss << "\n\n";

        for (auto & specificNameScoresPair : specificMap_)
        {
            auto const NAME{ specificNameScoresPair.first };

            auto & scores{ specificNameScoresPair.second.scores };
            std::sort(std::begin(scores), std::end(scores));

            ss << "\n\t" << std::setw(20) << NAME << " count=" << std::setw(7) << scores.size()
               << std::setw(6) << PercentToString(scores.size(), SUB_REPORT_SOURCES.normal_count)
               << SumPhrase("", scores, SUB_REPORT_SOURCES.normal_score_sum);

            auto & materialPairs{ specificNameScoresPair.second.material_pairs };

            misc::VectorMap<material::Enum, MaterialVec_t> primaryToSecondariesMap;

            for (auto const & MATERIAL_PAIR : materialPairs)
            {
                primaryToSecondariesMap[MATERIAL_PAIR.first].emplace_back(MATERIAL_PAIR.second);
            }

            for (auto & primaryToSecondariesPair : primaryToSecondariesMap)
            {
                auto & secondaries{ primaryToSecondariesPair.second };

                std::sort(std::begin(secondaries), std::end(secondaries));

                secondaries.erase(
                    std::unique(std::begin(secondaries), std::end(secondaries)),
                    std::end(secondaries));
            }

            misc::VectorMap<MaterialVec_t, MaterialVec_t> secondariesToPrimariesMap;

            for (auto & primaryToSecondariesPair : primaryToSecondariesMap)
            {
                secondariesToPrimariesMap[primaryToSecondariesPair.second].emplace_back(
                    primaryToSecondariesPair.first);
            }

            for (auto & secondariesToPrimariesPair : secondariesToPrimariesMap)
            {
                auto & primaries{ secondariesToPrimariesPair.second };

                std::sort(std::begin(primaries), std::end(primaries));

                primaries.erase(
                    std::unique(std::begin(primaries), std::end(primaries)), std::end(primaries));
            }

            auto materialVecToString{ [](const MaterialVec_t & MATERIALS) {
                std::ostringstream matVecSS;
                matVecSS << "(";
                for (auto const MATERIAL : MATERIALS)
                {
                    matVecSS << material::ToString(MATERIAL) << ",";
                }
                matVecSS << ")";
                return matVecSS.str();
            } };

            for (auto const & SECONDARIES_PRIMARIES_PAIR : secondariesToPrimariesMap)
            {
                ss << "\n\t\t" << std::setw(30)
                   << materialVecToString(SECONDARIES_PRIMARIES_PAIR.second) << std::setw(10)
                   << "\t\t" << std::setw(30)
                   << materialVecToString(SECONDARIES_PRIMARIES_PAIR.first);
            }
        }

        return ss.str();
    }

    bool ItemProfilesReporter::StandardSetReport::Add(
        const ItemProfile & PROFILE, const bool WILL_ADD)
    {
        if (WILL_ADD)
        {
            if (PROFILE.IsWeapon())
            {
                Report::Add(PROFILE, true);
                reportMap_["weapon"].Add(PROFILE, true);
            }
            else if (PROFILE.IsArmor())
            {
                Report::Add(PROFILE, true);
                reportMap_["armor"].Add(PROFILE, true);
            }
            else if (PROFILE.IsMisc())
            {
                Report::Add(PROFILE, true);
                reportMap_["misc"].Add(PROFILE, true);
            }
        }

        return WILL_ADD;
    }

    const std::string ItemProfilesReporter::StandardSetReport::Make(
        const SourceProfiles & SOURCE_PROFILES, const bool) const
    {
        std::ostringstream ss;
        ss << Report::Make(SOURCE_PROFILES);

        auto const SUB_REPORT_SOURCES{ SourceProfiles(
            scores_.size(),
            religiousScores_.size(),
            misc::Vector::SumVec(scores_),
            misc::Vector::SumVec(religiousScores_)) };

        for (auto & stringReportPair : reportMap_)
        {
            stringReportPair.second.SetDescription(stringReportPair.first);
            ss << '\n' << stringReportPair.second.Make(SUB_REPORT_SOURCES, true);
        }

        return ss.str();
    }

} // namespace item
} // namespace heroespath
