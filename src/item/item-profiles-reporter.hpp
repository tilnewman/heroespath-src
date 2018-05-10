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
#ifndef HEROESPATH_ITEM_ITEM_PROFILES_REPORTER_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_PROFILES_REPORTER_HPP_INCLUDED
//
// item-profiles-reporter.hpp
//
#include "item/item-profile.hpp"
#include "misc/types.hpp"
#include "misc/vector-map.hpp"
#include "misc/vectors.hpp"

#include <iomanip>
#include <numeric>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    // Responsible for logging details about the kinds of items generated by ItemProfileWarehouse.
    struct ItemProfilesReporter
    {
        static void LogReport();

    private:
        struct SourceProfiles
        {
            SourceProfiles(
                const std::size_t SOURCE_PROFILES_COUNT,
                const std::size_t SOURCE_RELIGIOUS_PROFILES_COUNT,
                const std::size_t SOURCE_PROFILES_SCORE_SUM,
                const std::size_t SOURCE_RELIGIOUS_PROFILES_SCORE_SUM)
                : normal_count(SOURCE_PROFILES_COUNT)
                , religious_count(SOURCE_RELIGIOUS_PROFILES_COUNT)
                , normal_score_sum(SOURCE_PROFILES_SCORE_SUM)
                , religious_score_sum(SOURCE_RELIGIOUS_PROFILES_SCORE_SUM)
            {}

            std::size_t normal_count;
            std::size_t religious_count;
            std::size_t normal_score_sum;
            std::size_t religious_score_sum;
        };

        // Responsible for holding a specific set of profiles and for generating a report on them.
        class Report
        {
        public:
            Report(const std::string & DESCRIPTION = "")
                : description_(DESCRIPTION)
                , scores_()
                , religiousScores_()
                , miscCount_(0)
                , elementalCount(0)
                , scoresSet_()
                , scoresNamed_()
                , scoresUnique_()
                , scoresPixie_()
                , elementScoreMap_()
            {}

            virtual ~Report() = default;

            void SetDescription(const std::string & DESCRIPTION) { description_ = DESCRIPTION; }

            virtual bool Add(const ItemProfile & PROFILE, const bool WILL_ADD);

            std::size_t Count() const { return scores_.size(); }

            const std::string ToString(const SourceProfiles &);

            virtual const std::string
                Make(const SourceProfiles &, const bool WILL_LIMIT_TO_ONE_LINE = false) const;

            const std::string ReportCountPhrase(const std::size_t COUNT_COMPARED_WITH) const;

        protected:
            std::string description_;
            std::vector<std::size_t> scores_;
            std::vector<std::size_t> religiousScores_;
            std::size_t miscCount_{ 0 };
            std::size_t elementalCount{ 0 };
            mutable std::vector<std::size_t> scoresSet_;
            mutable std::vector<std::size_t> scoresNamed_;
            mutable std::vector<std::size_t> scoresUnique_;
            mutable std::vector<std::size_t> scoresPixie_;
            mutable misc::VectorMap<element_type::Enum, std::vector<std::size_t>> elementScoreMap_;
        };

        struct SpecificItemInfo
        {
            std::vector<std::size_t> scores;
            MaterialPairVec_t material_pairs;
        };

        class WeaponOrArmorReport : public Report
        {
        public:
            WeaponOrArmorReport(
                const bool IS_WEAPON_REPORT, const std::string & DESCRIPTION_POSTFIX = "")
                : Report(((IS_WEAPON_REPORT) ? "weapons" : "armor") + DESCRIPTION_POSTFIX)
            {
                isWeaponReport_ = IS_WEAPON_REPORT;
            }

            bool Add(const ItemProfile & PROFILE, const bool WILL_ADD) override;

            const std::string
                Make(const SourceProfiles &, const bool WILL_LIMIT_TO_ONE_LINE) const override;

        protected:
            bool isWeaponReport_;
            mutable misc::VectorMap<std::string, Report> reportMap_;
            mutable misc::VectorMap<std::string, SpecificItemInfo> specificMap_;
        };

        class StandardSetReport : public Report
        {
        public:
            StandardSetReport(const std::string & DESCRIPTION)
                : Report(DESCRIPTION)
            {}

            bool Add(const ItemProfile & PROFILE, const bool WILL_ADD) override;

            const std::string
                Make(const SourceProfiles &, const bool WILL_LIMIT_TO_ONE_LINE) const override;

        protected:
            mutable misc::VectorMap<std::string, Report> reportMap_;
        };

        template <typename T, typename U>
        static double DivideAsDouble(const T NUMERATOR, const U DENOMINATOR)
        {
            if (static_cast<double>(DENOMINATOR) > 0.0)
            {
                return (static_cast<double>(NUMERATOR) / static_cast<double>(DENOMINATOR));
            }
            else
            {
                return -0.0;
            }
        }

        template <typename T, typename U>
        static const std::string RatioToString(
            const T NUMERATOR,
            const U DENOMINATOR,
            const bool WILL_WRAP = false,
            const std::string POSTFIX = "",
            const int DIGITS = 4)
        {
            if (DENOMINATOR > 0)
            {
                std::ostringstream ss;
                ss << ((WILL_WRAP) ? "(" : "") << std::setw(DIGITS + 2) << std::setprecision(DIGITS)
                   << ((POSTFIX == "%") ? 100.0 : 1.0) * DivideAsDouble(NUMERATOR, DENOMINATOR)
                   << std::setprecision(0) << POSTFIX << ((WILL_WRAP) ? ")" : "");

                return ss.str();
            }
            else
            {
                return "(divide_by_zero_error)";
            }
        }

        template <typename T, typename U>
        static const std::string
            PercentToString(const T NUMERATOR, const U DENOMINATOR, const bool WILL_WRAP = true)
        {
            if (DENOMINATOR > 0)
            {
                return RatioToString(NUMERATOR, DENOMINATOR, WILL_WRAP, "%", 3);
            }
            else
            {
                return "100%";
            }
        }

        // NAME_count=#(#%)
        template <typename T, typename U>
        static const std::string
            CountPhrase(const std::string & NAME, const T COUNT, const U COUNT_COMPARED_WITH)
        {
            std::ostringstream ss;
            ss << ", " << NAME << "_count=" << COUNT;

            if (COUNT_COMPARED_WITH > 0)
            {
                ss << PercentToString(COUNT, COUNT_COMPARED_WITH);
            }

            return ss.str();
        }

        // NAME=[min, avg, max](#sum)(#std_dev)(#%)
        template <typename T, typename U>
        static const std::string SumPhrase(
            const std::string & NAME, const std::vector<T> & VALUES, const U SUM_COMPARED_WITH = 0)
        {
            auto const MIN_MAX_AVG_SUM{ misc::Vector::MinMaxAvgSum<T>(VALUES) };

            std::ostringstream ss;

            ss << ", " << std::setw(12) << NAME << "=[" << std::setw(4) << MIN_MAX_AVG_SUM.min
               << ", " << std::setw(4) << MIN_MAX_AVG_SUM.avg << ", " << std::setw(4)
               << MIN_MAX_AVG_SUM.max << "](" << std::setw(9) << MIN_MAX_AVG_SUM.sum << "sum)("
               << misc::Vector::StandardDeviation(VALUES, VALUES.size(), MIN_MAX_AVG_SUM.avg)
               << "std)";

            if (SUM_COMPARED_WITH > 0)
            {
                ss << PercentToString(MIN_MAX_AVG_SUM.sum, SUM_COMPARED_WITH);
            }

            return ss.str();
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILES_REPORTER_HPP_INCLUDED
