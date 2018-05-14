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
// title.hpp
//
#include "title.hpp"

#include "creature/creature.hpp"
#include "creature/role-enum.hpp"
#include "creature/title.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/gui/title-image-loader.hpp"

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace creature
{

    RoleVec_t Title::ROLEVEC_ALL_PLAYER_ROLES_
        = { role::Knight,   role::Archer, role::Bard,    role::Thief,     role::Beastmaster,
            role::Sorcerer, role::Cleric, role::Sylavin, role::Firebrand, role::Wolfen };

    Title::Title(
        const Titles::Enum TITLE,
        const AchievementType::Enum ACHIEVEMENT_TYPE,
        const Index_t & ACHIEVEMENT_INDEX,
        const Count_t & ACHIEVEMENT_COUNT,
        const RoleVec_t & ROLES_VEC,
        const stats::StatSet & STATS_BONUS,
        const Rank_t & RANK_BONUS,
        const Experience_t & EXPERIENCE_BONUS,
        const Health_t & HEALTH_BONUS)
        : title_(TITLE)
        , achievementType_(ACHIEVEMENT_TYPE)
        , achievementCount_(ACHIEVEMENT_COUNT)
        , achievementIndex_(ACHIEVEMENT_INDEX)
        , rolesVec_(ROLES_VEC)
        , rankBonus_(RANK_BONUS)
        , expBonus_(EXPERIENCE_BONUS)
        , statBonus_(STATS_BONUS)
        , fileName_("")
        , healthBonus_(HEALTH_BONUS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (TITLE != Titles::Count),
            "creature::Title::Title(title_enum=role::Count"
                << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT
                << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS
                << ") was given an invalid TITLE enum.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (Titles::ToString(TITLE).empty() == false),
            "creature::Title::Title(title_enum="
                << TITLE << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT
                << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS
                << ") was given an invalid TITLE.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ROLES_VEC.empty() == false),
            "creature::Title::Title(title="
                << Titles::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE
                << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS
                << ", exp_bonus=" << EXPERIENCE_BONUS << ") was given an empty role vector.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ACHIEVEMENT_TYPE != AchievementType::Count),
            "creature::Title::Title(title=" << Titles::ToString(TITLE)
                                            << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count="
                                            << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS
                                            << ", exp_bonus=" << EXPERIENCE_BONUS
                                            << ") was given an AchievementType of 'Count'.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (((ACHIEVEMENT_TYPE != AchievementType::None) && ACHIEVEMENT_COUNT.IsZero()) == false),
            "creature::Title::Title(title="
                << Titles::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE
                << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS
                << ") was given a valid AchievementType but the achievement count was zero.");

        std::ostringstream ss;

        if ((ACHIEVEMENT_TYPE == AchievementType::Count)
            || (ACHIEVEMENT_TYPE == AchievementType::None))
        {
            ss << Titles::ToString(TITLE);
        }
        else
        {
            ss << AchievementType::ToString(ACHIEVEMENT_TYPE);

            if (ACHIEVEMENT_INDEX > 0_index)
            {
                ss << ACHIEVEMENT_INDEX;
            }
        }

        ss << ".png";
        fileName_ = ss.str();
    }

    const std::string Title::ToString() const
    {
        const std::string SEP_STR(", ");

        std::ostringstream ss;
        ss << Titles::Name(title_);

        if (ROLEVEC_ALL_PLAYER_ROLES_ == rolesVec_)
        {
            ss << ", for all roles";
        }
        else
        {
            ss << ", for role" << ((rolesVec_.size() == 1) ? "" : "s") << ": ";

            std::ostringstream rolesSS;
            for (auto const & NEXT_ROLE : rolesVec_)
            {
                rolesSS << role::Name(NEXT_ROLE) << SEP_STR;
            }

            ss << boost::algorithm::erase_last_copy(rolesSS.str(), SEP_STR);
        }

        ss << SEP_STR;

        if (AchievementType::None != achievementType_)
        {
            ss << "granted after " << achievementCount_ << " "
               << AchievementType::Name(achievementType_) << SEP_STR;
        }

        if (0_rank != rankBonus_)
        {
            ss << "rank_bonus=" << rankBonus_ << SEP_STR;
        }

        if (0_exp != expBonus_)
        {
            ss << "exp_bonus=" << expBonus_ << SEP_STR;
        }

        if (0_health != healthBonus_)
        {
            ss << "health_bonus=" << healthBonus_ << SEP_STR;
        }

        const std::string STATS_STR(statBonus_.ToString(true));
        if (STATS_STR.empty() == false)
        {
            ss << "stat_bonuses=" << STATS_STR;
        }

        if (boost::algorithm::ends_with(ss.str(), SEP_STR))
        {
            return boost::algorithm::erase_last_copy(ss.str(), SEP_STR) + ".";
        }
        else
        {
            ss << ".";
            return ss.str();
        }
    }

    const std::string Title::LongDesc() const
    {
        std::ostringstream ss;
        ss << Desc();

        ss << "  This title can be earned by ";

        if (ROLEVEC_ALL_PLAYER_ROLES_ == rolesVec_)
        {
            ss << "ALL roles";
        }
        else
        {
            ss << "the following role" << ((rolesVec_.size() == 1) ? "" : "s") << ": ";

            std::ostringstream rolesSS;
            for (auto const & NEXT_ROLE : rolesVec_)
            {
                rolesSS << role::Name(NEXT_ROLE) << ", ";
            }

            ss << rolesSS.str(); // keep the last ", " intentionally
        }

        if (AchievementType::None != achievementType_)
        {
            ss << ", and is granted after " << achievementCount_ << " "
               << AchievementType::Name(achievementType_);
        }

        ss << ".  ";

        if (0_rank == rankBonus_)
        {
            ss << "This does not increase your rank";
        }
        else
        {
            ss << "This increases your rank by " << rankBonus_;
        }

        ss << ", your health by " << healthBonus_ << ", ";

        if (0_exp == expBonus_)
        {
            ss << "and does not increase your experience";
        }
        else
        {
            ss << "and increases your experience by " << expBonus_;
        }

        ss << ".  ";

        const std::string STATS_STR(statBonus_.ToString(true));
        if (STATS_STR.empty())
        {
            ss << "Stats are not modified by this title.";
        }
        else
        {
            ss << "Stats undergo the following changes:  " << STATS_STR << ".";
        }

        return ss.str();
    }

    bool operator<(const Title & L, const Title & R)
    {
        if (std::tie(
                L.title_,
                L.achievementCount_,
                L.achievementIndex_,
                L.rankBonus_,
                L.expBonus_,
                L.statBonus_,
                L.fileName_)
            < std::tie(
                  R.title_,
                  R.achievementCount_,
                  R.achievementIndex_,
                  R.rankBonus_,
                  R.expBonus_,
                  R.statBonus_,
                  R.fileName_))
        {
            return true;
        }
        else
        {
            return misc::Vector::OrderlessCompareLess(L.rolesVec_, R.rolesVec_);
        }
    }

    bool operator==(const Title & L, const Title & R)
    {
        if (std::tie(
                L.title_,
                L.achievementCount_,
                L.achievementIndex_,
                L.rankBonus_,
                L.expBonus_,
                L.statBonus_,
                L.fileName_)
            != std::tie(
                   R.title_,
                   R.achievementCount_,
                   R.achievementIndex_,
                   R.rankBonus_,
                   R.expBonus_,
                   R.statBonus_,
                   R.fileName_))
        {
            return false;
        }
        else
        {
            return misc::Vector::OrderlessCompareEqual(L.rolesVec_, R.rolesVec_);
        }
    }

    void Title::Change(const CreaturePtr_t CREATURE_PTR) const
    {
        // titles are permenant, so they effect both the normal and current stat values
        CREATURE_PTR->StatTraitsModify(statBonus_);
        CREATURE_PTR->IncreaseRank(rankBonus_);
        CREATURE_PTR->IncreaseExp(expBonus_);
        CREATURE_PTR->HealthNormalAdj(healthBonus_);
        CREATURE_PTR->HealthCurrentAdj(healthBonus_);
    }

} // namespace creature
} // namespace heroespath
