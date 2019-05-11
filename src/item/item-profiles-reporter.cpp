// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profiles-reporter.cpp
//
#include "item-profiles-reporter.hpp"

#include "item-profile-warehouse.hpp"
#include "misc/log-macros.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace item
{

    std::string ItemProfilesReporter::toNumberSpecialForType(const double NUMBER, const int WIDTH)
    {
        std::ostringstream ss;

        if (WIDTH != 0)
        {
            ss << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(WIDTH);
        }

        ss << NUMBER;

        if (WIDTH != 0)
        {
            ss << std::setfill(' ') << std::setw(0) << std::setprecision(0);
        }

        return ss.str();
    }

    std::string
        ItemProfilesReporter::toNumberSpecialForType(const long long NUMBER, const int WIDTH)
    {
        std::ostringstream ss;

        if (WIDTH != 0)
        {
            ss << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(WIDTH);
        }

        ss << NUMBER;

        if (WIDTH != 0)
        {
            ss << std::setfill(' ') << std::setw(0) << std::setprecision(0);
        }

        return ss.str();
    }

    void ItemProfilesReporter::LogReport()
    {
        std::ostringstream ss;
        ss << "\n\n*** Item Profile Report ***\n";

        const auto & NORMAL_PROFILES { ItemProfileWarehouse::Instance()->GetNormalProfiles() };

        const auto NORMAL_PROFILES_COUNT { NORMAL_PROFILES.size() };

        const auto NORMAL_PROFILES_SCORE_SUM { std::accumulate(
            std::begin(NORMAL_PROFILES),
            std::end(NORMAL_PROFILES),
            0_score,
            [](const auto SUBTOTAL, const auto & PROFILE) {
                return SUBTOTAL + PROFILE.TreasureScore();
            }) };

        const auto & RELIGIOUS_PROFILES {
            ItemProfileWarehouse::Instance()->GetReligiousProfiles()
        };

        const auto RELIGIOUS_PROFILES_COUNT { RELIGIOUS_PROFILES.size() };

        const auto RELIGIOUS_PROFILES_SCORE_SUM { std::accumulate(
            std::begin(RELIGIOUS_PROFILES),
            std::end(RELIGIOUS_PROFILES),
            0_score,
            [](const auto SUBTOTAL, const auto & PROFILE) {
                return SUBTOTAL + PROFILE.ReligiousScore();
            }) };

        const auto ALL_PROFILES_COUNT { NORMAL_PROFILES_COUNT + RELIGIOUS_PROFILES_COUNT };

        const auto ALL_PROFILES_SCORE_SUM { NORMAL_PROFILES_SCORE_SUM
                                            + RELIGIOUS_PROFILES_SCORE_SUM };

        ss << "total_count=" << ALL_PROFILES_COUNT << ", total_score_sum=" << ALL_PROFILES_SCORE_SUM
           << ", total_score_avg="
           << DivideAsDouble(ALL_PROFILES_SCORE_SUM.GetAs<std::size_t>(), ALL_PROFILES_COUNT)
           << ", rel_count=" << RELIGIOUS_PROFILES_COUNT << "("
           << DivideAsDouble(RELIGIOUS_PROFILES_COUNT, ALL_PROFILES_COUNT) << "%)"
           << ", rel_score_sum=" << RELIGIOUS_PROFILES_SCORE_SUM << ", rel_score_avg="
           << DivideAsDouble(
                  RELIGIOUS_PROFILES_SCORE_SUM.GetAs<std::size_t>(), RELIGIOUS_PROFILES_COUNT)
           << "\n";

        const auto SOURCE_PROFILES { SourceProfiles(
            ALL_PROFILES_COUNT,
            RELIGIOUS_PROFILES_COUNT,
            ALL_PROFILES_SCORE_SUM.GetAs<std::size_t>(),
            RELIGIOUS_PROFILES_SCORE_SUM.GetAs<std::size_t>()) };

        Report allReport("all");

        // these three should include all profiles in a mutually exclusive way
        WeaponOrArmorReport weaponReport(true, "-non-misc");
        WeaponOrArmorReport armorReport(false, "-non-misc");
        Report miscReport("misc");
        //
        WeaponOrArmorReport standardWeaponReport(true, "-standard");
        WeaponOrArmorReport standardArmorReport(false, "-standard");
        StandardSetReport weaponOrArmorMisc("weapon_or_armor_misc");
        StandardSetReport magicalReport("magical");
        Report summoningReport("summoning");
        Report uniqueReport("unique");
        StandardSetReport setReport("set");
        StandardSetReport namedReport("named");
        StandardSetReport elementReport("elemental");
        Report religiousReport("religious");
        StandardSetReport pixieReport("pixie");

        auto appendProfileToAllRequiredReports { [&](const ItemProfile & PROFILE) {
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

            magicalReport.Add(PROFILE, PROFILE.IsMagical());
            summoningReport.Add(PROFILE, PROFILE.SummonInfo().CanSummon());
            uniqueReport.Add(PROFILE, PROFILE.IsUnique());
            setReport.Add(PROFILE, PROFILE.IsSet());
            namedReport.Add(PROFILE, PROFILE.IsNamed());
            elementReport.Add(PROFILE, PROFILE.IsElemental());
            religiousReport.Add(PROFILE, PROFILE.IsReligious());
            pixieReport.Add(PROFILE, PROFILE.IsPixie());
        } };

        for (const auto & PROFILE : NORMAL_PROFILES)
        {
            appendProfileToAllRequiredReports(PROFILE);
        }

        for (const auto & PROFILE : RELIGIOUS_PROFILES)
        {
            appendProfileToAllRequiredReports(PROFILE);
        }

        ss << "non_misc_weapon_count=" << weaponReport.Count()
           << PercentToString(weaponReport.Count(), ALL_PROFILES_COUNT)
           << ", non_misc_armor=" << armorReport.Count()
           << PercentToString(armorReport.Count(), ALL_PROFILES_COUNT)
           << ", misc_maybe_weapon_or_armor=" << miscReport.Count()
           << PercentToString(miscReport.Count(), ALL_PROFILES_COUNT) << '\n';

        const auto SPLIT_COUNT { (
            weaponReport.Count() + armorReport.Count() + miscReport.Count()) };
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
           << miscReport.ToString(SOURCE_PROFILES) << weaponOrArmorMisc.ToString(SOURCE_PROFILES)
           << magicalReport.ToString(SOURCE_PROFILES) << summoningReport.ToString(SOURCE_PROFILES)
           << uniqueReport.ToString(SOURCE_PROFILES) << setReport.ToString(SOURCE_PROFILES)
           << namedReport.ToString(SOURCE_PROFILES) << elementReport.ToString(SOURCE_PROFILES)
           << religiousReport.ToString(SOURCE_PROFILES) << pixieReport.ToString(SOURCE_PROFILES);

        const std::size_t MIN_MAX_DISPLAY_COUNT { 10 };
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
            const auto NORMAL_SCORE { PROFILE.TreasureScore().GetAs<std::size_t>() };
            const auto RELIGIOIUS_SCORE { PROFILE.ReligiousScore().GetAs<std::size_t>() };

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

            if (PROFILE.IsPixie())
            {
                scoresPixie_.emplace_back(NORMAL_SCORE);
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
            const std::size_t CHUNK_COUNT { 24 };

            const auto CHUNK_SIZE { (
                (scores_.size() < CHUNK_COUNT) ? 1 : (scores_.size() / CHUNK_COUNT)) };

            ss << "\n---(" << CHUNK_SIZE << ")---";
            for (std::size_t chunkIndex(0); chunkIndex < CHUNK_COUNT; ++chunkIndex)
            {
                const std::size_t MIN_DEFAULT { std::numeric_limits<std::size_t>::max() };
                std::size_t min { MIN_DEFAULT };
                std::size_t max { 0 };
                std::size_t sum { 0 };

                for (std::size_t index(0); index < CHUNK_SIZE; ++index)
                {
                    const auto ACTUAL_INDEX { (chunkIndex * CHUNK_SIZE) + index };
                    if (ACTUAL_INDEX < scores_.size())
                    {
                        const auto VALUE { scores_.at(ACTUAL_INDEX) };

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

                const auto AVERAGE { static_cast<std::size_t>(DivideAsDouble(sum, CHUNK_SIZE)) };

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

            auto appendScoresSubReport { [&](const std::string & NAME,
                                             std::vector<std::size_t> & scores) {
                if (scores.empty() == false)
                {
                    std::sort(std::begin(scores), std::end(scores));

                    const auto STATS { MinMaxAvgStdDev<std::size_t>(scores) };

                    ss << '\n'
                       << std::setw(10) << NAME << "count=" << std::setw(8) << STATS.count
                       << PercentToString(scores.size(), SOURCE_PROFILES.normal_count, true)
                       << STATS.ToString(false, false, 4);
                }
            } };

            appendScoresSubReport("Set", scoresSet_);
            appendScoresSubReport("Named", scoresNamed_);
            appendScoresSubReport("Unique", scoresUnique_);
            appendScoresSubReport("Pixie", scoresPixie_);

            if (elementalCount > 0)
            {
                const auto SCORES_SUM { misc::Vector::Sum(scores_) };

                ss << "\nElemental Items  count=" << elementalCount
                   << PercentToString(elementalCount, scores_.size());

                for (auto & elementTypeScoreVecPair : elementScoreMap_)
                {
                    std::sort(
                        std::begin(elementTypeScoreVecPair.second),
                        std::end(elementTypeScoreVecPair.second));

                    ss << "\n\t" << std::setw(20)
                       << element_type::ToString(
                              elementTypeScoreVecPair.first,
                              EnumStringHow(Wrap::Yes, "/", NoneEmpty::No))
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

                auto & specificItemInfo { specificMap_[PROFILE.WeaponInfo().SpecificName()] };

                specificItemInfo.scores.emplace_back(PROFILE.TreasureScore().GetAs<std::size_t>());

                specificItemInfo.material_pairs.emplace_back(
                    PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());
            }
            else if ((false == isWeaponReport_) && PROFILE.IsArmor())
            {
                Report::Add(PROFILE, true);
                reportMap_[PROFILE.ArmorInfo().GeneralName()].Add(PROFILE, true);

                auto & specificItemInfo { specificMap_[PROFILE.ArmorInfo().SpecificName()] };

                specificItemInfo.scores.emplace_back(PROFILE.TreasureScore().GetAs<std::size_t>());

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

        const auto SUB_REPORT_SOURCES { SourceProfiles(
            scores_.size(),
            religiousScores_.size(),
            misc::Vector::Sum(scores_),
            misc::Vector::Sum(religiousScores_)) };

        for (auto & stringReportPair : reportMap_)
        {
            stringReportPair.second.SetDescription(stringReportPair.first);
            ss << '\n' << stringReportPair.second.Make(SUB_REPORT_SOURCES, true);
        }

        ss << "\n\n";

        for (auto & specificNameScoresPair : specificMap_)
        {
            const auto NAME { specificNameScoresPair.first };

            auto & scores { specificNameScoresPair.second.scores };
            std::sort(std::begin(scores), std::end(scores));

            ss << "\n\t" << std::setw(20) << NAME << " count=" << std::setw(7) << scores.size()
               << std::setw(6) << PercentToString(scores.size(), SUB_REPORT_SOURCES.normal_count)
               << SumPhrase("", scores, SUB_REPORT_SOURCES.normal_score_sum);

            auto & materialPairs { specificNameScoresPair.second.material_pairs };

            misc::VectorMap<material::Enum, MaterialVec_t> primaryToSecondariesMap;

            for (const auto & MATERIAL_PAIR : materialPairs)
            {
                primaryToSecondariesMap[MATERIAL_PAIR.first].emplace_back(MATERIAL_PAIR.second);
            }

            for (auto & primaryToSecondariesPair : primaryToSecondariesMap)
            {
                auto & secondaries { primaryToSecondariesPair.second };

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
                auto & primaries { secondariesToPrimariesPair.second };

                std::sort(std::begin(primaries), std::end(primaries));

                primaries.erase(
                    std::unique(std::begin(primaries), std::end(primaries)), std::end(primaries));
            }

            auto materialVecToString { [](const MaterialVec_t & MATERIALS) {
                std::ostringstream matVecSS;
                matVecSS << "(";
                for (const auto MATERIAL : MATERIALS)
                {
                    matVecSS << NAMEOF_ENUM(MATERIAL) << ",";
                }
                matVecSS << ")";
                return matVecSS.str();
            } };

            for (const auto & SECONDARIES_PRIMARIES_PAIR : secondariesToPrimariesMap)
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

        const auto SUB_REPORT_SOURCES { SourceProfiles(
            scores_.size(),
            religiousScores_.size(),
            misc::Vector::Sum(scores_),
            misc::Vector::Sum(religiousScores_)) };

        for (auto & stringReportPair : reportMap_)
        {
            stringReportPair.second.SetDescription(stringReportPair.first);
            ss << '\n' << stringReportPair.second.Make(SUB_REPORT_SOURCES, true);
        }

        return ss.str();
    }

    const std::string ItemProfilesReporter::RatioToStringStringMaker(
        const int DIGITS,
        const std::string & POSTFIX,
        const double AFTER_DIVISION,
        const bool WILL_WRAP)
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "(" : "") << std::setw(DIGITS + 2) << std::setprecision(DIGITS)
           << ((POSTFIX == "%") ? 100.0 : 1.0) * AFTER_DIVISION << std::setprecision(0) << POSTFIX
           << ((WILL_WRAP) ? ")" : "");

        return ss.str();
    }

    const std::string ItemProfilesReporter::SumPhraseStringMaker(
        const std::string & NAME, const std::string & STATS_STR)
    {
        std::ostringstream ss;
        ss << ", " << std::setw(12) << NAME << std::setw(0) << "=" << STATS_STR;
        return ss.str();
    }

} // namespace item
} // namespace heroespath
