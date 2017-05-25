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

#include "utilz/assertlogandthrow.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/title.hpp"
#include "game/creature/role.hpp"

#include "sfml-util/gui/title-image-manager.hpp"

#include <sstream>
#include <tuple>


namespace game
{
namespace creature
{

    Title::Title(const Titles::Enum          TITLE,
                 const AchievementType::Enum ACHIEVEMENT_TYPE,
                 const std::size_t           ACHIEVEMENT_INDEX,
                 const std::size_t           ACHIEVEMENT_COUNT,
                 const RoleVec_t &           ROLES_VEC,
                 const stats::StatSet &      STATS_BONUS,
                 const stats::Rank_t         RANK_BONUS,
                 const stats::Exp_t          EXPERIENCE_BONUS,
                 const stats::Health_t       HEALTH_BONUS)
    :
        title_           (TITLE),
        achievementType_ (ACHIEVEMENT_TYPE),
        achievementCount_(ACHIEVEMENT_COUNT),
        achievementIndex_(ACHIEVEMENT_INDEX),
        rolesVec_        (ROLES_VEC),
        rankBonus_       (RANK_BONUS),
        expBonus_        (EXPERIENCE_BONUS),
        statBonus_       (STATS_BONUS),
        fileName_        (""),
        healthBonus_     (HEALTH_BONUS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE != Titles::Count),                                                               "game::creature::Title::Title(title_enum=role::Count"           << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an invalid TITLE enum.");
        M_ASSERT_OR_LOGANDTHROW_SS((Titles::ToString(TITLE).empty() == false),                                            "game::creature::Title::Title(title_enum=" << TITLE             << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an invalid TITLE.");
        M_ASSERT_OR_LOGANDTHROW_SS((ROLES_VEC.empty() == false),                                                         "game::creature::Title::Title(title=" << Titles::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an empty role vector.");
        M_ASSERT_OR_LOGANDTHROW_SS((ACHIEVEMENT_TYPE != AchievementType::Count),                                         "game::creature::Title::Title(title=" << Titles::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an AchievementType of 'Count'.");
        M_ASSERT_OR_LOGANDTHROW_SS((((ACHIEVEMENT_TYPE != AchievementType::None) && (ACHIEVEMENT_COUNT == 0)) == false), "game::creature::Title::Title(title=" << Titles::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given a valid AchievementType but the achievement count was zero.");

        //construct filename
        std::ostringstream ss;

        if ((ACHIEVEMENT_TYPE == AchievementType::Count) || (ACHIEVEMENT_TYPE == AchievementType::None))
        {
            ss << Titles::ToString(TITLE);
        }
        else
        {
            ss << AchievementType::ToString(ACHIEVEMENT_TYPE);

            if (ACHIEVEMENT_INDEX > 0)
                ss << ACHIEVEMENT_INDEX;
        }

        ss << sfml_util::gui::TitleImageManager::FileNameExt();
        fileName_ = ss.str();
    }


    Title::~Title()
    {}


    const std::string Title::ToString() const
    {
        const std::string SEP_STR(", ");

        std::ostringstream ss;
        ss << Titles::Name(title_);

        if (Role::ROLEVEC_ALL_PLAYER_ROLES_ == rolesVec_)
        {
            ss << ", for all roles";
        }
        else
        {
            ss << ", for role" << ((rolesVec_.size() == 1) ? "" : "s") << ": ";

            std::ostringstream rolesSS;
            for (auto const & NEXT_ROLE : rolesVec_)
                rolesSS << role::Name(NEXT_ROLE) << SEP_STR;

            ss << boost::algorithm::erase_last_copy(rolesSS.str(), SEP_STR);
        }

        ss << SEP_STR;

        if (AchievementType::None != achievementType_)
            ss << "granted after " << achievementCount_ << " " << AchievementType::Name(achievementType_) << SEP_STR;

        if (0 != rankBonus_)
            ss << "rank_bonus=" << rankBonus_ << SEP_STR;

        if (0 != expBonus_)
            ss << "exp_bonus=" << expBonus_ << SEP_STR;

        if (0 != healthBonus_)
            ss << "health_bonus=" << healthBonus_ << SEP_STR;

        const std::string STATS_STR(statBonus_.ToStringNormal(true, true, true, true));
        if (STATS_STR.empty() == false)
            ss << "stat_bonuses=" << STATS_STR;

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

        if (Role::ROLEVEC_ALL_PLAYER_ROLES_ == rolesVec_)
        {
            ss << "ALL roles";
        }
        else
        {
            ss << "the following role" << ((rolesVec_.size() == 1) ? "" : "s") << ": ";

            std::ostringstream rolesSS;
            for (auto const & NEXT_ROLE : rolesVec_)
                rolesSS << role::Name(NEXT_ROLE) << ", ";

            ss << rolesSS.str();//keep the last ", " intentionally
        }

        if (AchievementType::None != achievementType_)
            ss << ", and is granted after " << achievementCount_ << " " << AchievementType::Name(achievementType_);

        ss << ".  ";

        if (0 == rankBonus_)
            ss << "This does not increase your rank";
        else
            ss << "This increases your rank by " << rankBonus_;

        ss << ", your health by " << healthBonus_ << ", ";

        if (0 == expBonus_)
            ss << "and does not increase your experience";
        else
            ss << "and increases your experience by " << expBonus_;

        ss << ".  ";

        const std::string STATS_STR(statBonus_.ToStringNormal(false, false, true, true));
        if (STATS_STR.empty())
            ss << "Stats are not modified by this title.";
        else
            ss << "Stats undergo the following changes:  " << STATS_STR << ".";

        return ss.str();
    }


    bool operator<(const Title & L, const Title & R)
    {
        return std::tie(L.title_,
                        L.achievementCount_,
                        L.achievementIndex_,
                        L.rolesVec_,
                        L.rankBonus_,
                        L.expBonus_,
                        L.statBonus_,
                        L.fileName_)
               <
               std::tie(R.title_,
                        R.achievementCount_,
                        R.achievementIndex_,
                        R.rolesVec_,
                        R.rankBonus_,
                        R.expBonus_,
                        R.statBonus_,
                        R.fileName_);
    }


    bool operator==(const Title & L, const Title & R)
    {
        return std::tie(L.title_,
                        L.achievementCount_,
                        L.achievementIndex_,
                        L.rolesVec_,
                        L.rankBonus_,
                        L.expBonus_,
                        L.statBonus_,
                        L.fileName_)
               ==
               std::tie(R.title_,
                        R.achievementCount_,
                        R.achievementIndex_,
                        R.rolesVec_,
                        R.rankBonus_,
                        R.expBonus_,
                        R.statBonus_,
                        R.fileName_);
    }


    void Title::Change(Creature * const creaturePtr) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtr != nullptr), "Title::Change(creaturePtr==nullptr)  title=\"" << Titles::ToString(title_) << "\"  was given a null creaturePtr.");

        //titles are permenant, so they effect both the normal and current stat values
        creaturePtr->Stats().ModifyNormal(statBonus_);
        creaturePtr->Stats().ModifyCurrent(statBonus_);

        creaturePtr->IncreaseRank(rankBonus_);
        creaturePtr->IncreaseExp(expBonus_);

        creaturePtr->HealthNormalAdj(healthBonus_);
    }

}
}
