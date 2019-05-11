// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
        const std::size_t & ACHIEVEMENT_INDEX,
        const std::size_t & ACHIEVEMENT_COUNT,
        const RoleVec_t & ROLES_VEC,
        const StatSet & STATS_BONUS,
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
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (TITLE != Titles::Count),
            "creature::Title::Title(title_enum=role::Count"
                << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT
                << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS
                << ") was given an invalid TITLE enum.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (NAMEOF_ENUM(TITLE).empty() == false),
            "creature::Title::Title(title_enum="
                << TITLE << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT
                << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS
                << ") was given an invalid TITLE.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ROLES_VEC.empty() == false),
            "creature::Title::Title(title="
                << NAMEOF_ENUM(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE
                << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS
                << ", exp_bonus=" << EXPERIENCE_BONUS << ") was given an empty role vector.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ACHIEVEMENT_TYPE != AchievementType::Count),
            "creature::Title::Title(title=" << NAMEOF_ENUM(TITLE)
                                            << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count="
                                            << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS
                                            << ", exp_bonus=" << EXPERIENCE_BONUS
                                            << ") was given an AchievementType of 'Count'.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (!((ACHIEVEMENT_TYPE != AchievementType::None) && (ACHIEVEMENT_COUNT == 0))),
            "creature::Title::Title(title="
                << NAMEOF_ENUM(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE
                << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS
                << ") was given a valid AchievementType but the achievement count was zero.");

        fileName_.reserve(128);

        if ((ACHIEVEMENT_TYPE == AchievementType::Count)
            || (ACHIEVEMENT_TYPE == AchievementType::None))
        {
            fileName_ += NAMEOF_ENUM(TITLE);
        }
        else
        {
            fileName_ += NAMEOF_ENUM(ACHIEVEMENT_TYPE);

            if (ACHIEVEMENT_INDEX > 0)
            {
                fileName_ += std::to_string(ACHIEVEMENT_INDEX);
            }
        }

        fileName_ += ".png";
    }

    const std::string Title::ToString() const
    {
        const auto SEP_STR(", ");

        std::string str;
        str.reserve(255);

        str += Titles::Name(title_);

        if (ROLEVEC_ALL_PLAYER_ROLES_ == rolesVec_)
        {
            str += ", for all roles";
            str += SEP_STR;
        }
        else
        {
            str += ", for role";
            str += std::string((rolesVec_.size() == 1) ? "" : "s");
            str += ": ";

            for (const auto & NEXT_ROLE : rolesVec_)
            {
                str += role::Name(NEXT_ROLE);
                str += SEP_STR;
            }
        }

        if (AchievementType::None != achievementType_)
        {
            str += "granted after ";
            str += std::to_string(achievementCount_);
            str += ' ';
            str += AchievementType::Name(achievementType_);
            str += SEP_STR;
        }

        if (0_rank != rankBonus_)
        {
            str += "rank_bonus=";
            str += rankBonus_.ToString();
            str += SEP_STR;
        }

        if (0_exp != expBonus_)
        {
            str += "exp_bonus=";
            str += expBonus_.ToString();
            str += SEP_STR;
        }

        if (0_health != healthBonus_)
        {
            str += "health_bonus=";
            str += healthBonus_.ToString();
            str += SEP_STR;
        }

        const std::string STATS_STR(statBonus_.ToString(true));
        if (!STATS_STR.empty())
        {
            str += "stat_bonuses=";
            str += STATS_STR;
        }

        str += '.';
        return str;
    }

    const std::string Title::LongDesc() const
    {
        const std::string SEP_STR(", ");

        std::string str(Desc());
        str.reserve(str.size() + 255);

        str += "  This title can be earned by ";

        if (ROLEVEC_ALL_PLAYER_ROLES_ == rolesVec_)
        {
            str += "ALL roles";
            str += SEP_STR;
        }
        else
        {
            str += "the following role";
            str += std::string((rolesVec_.size() == 1) ? "" : "s");
            str += ": ";

            for (const auto & NEXT_ROLE : rolesVec_)
            {
                str += role::Name(NEXT_ROLE);
                str += SEP_STR;
            }
        }

        if (AchievementType::None != achievementType_)
        {
            str += "and is granted after ";
            str += std::to_string(achievementCount_);
            str += ' ';
            str += AchievementType::Name(achievementType_);
        }

        str += ".  ";

        if (0_rank == rankBonus_)
        {
            str += "This does not increase your rank";
        }
        else
        {
            str += "This increases your rank by ";
            str += rankBonus_.ToString();
        }

        str += ", your health by ";
        str += healthBonus_.ToString();
        str += SEP_STR;

        if (0_exp == expBonus_)
        {
            str += "and does not increase your experience";
        }
        else
        {
            str += "and increases your experience by ";
            str += expBonus_.ToString();
        }

        str += ".  ";

        const std::string STATS_STR(statBonus_.ToString(true));
        if (STATS_STR.empty())
        {
            str += "Stats are not modified by this title.";
        }
        else
        {
            str += "Stats undergo the following changes:  ";
            str += STATS_STR;
            str += '.';
        }

        return str;
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
        // titles are permanent, so they effect both the normal and current stat values
        CREATURE_PTR->StatTraitsModify(statBonus_);
        CREATURE_PTR->IncreaseRank(rankBonus_);
        CREATURE_PTR->IncreaseExp(expBonus_);
        CREATURE_PTR->HealthNormalAdj(healthBonus_);
        CREATURE_PTR->HealthCurrentAdj(healthBonus_);
    }

} // namespace creature
} // namespace heroespath
