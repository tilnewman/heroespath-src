//
// title.hpp
//
#include "title.hpp"

#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/creature/title.hpp"
#include "heroespath/creature/role.hpp"

#include "sfml-util/gui/title-image-manager.hpp"

#include <sstream>
#include <tuple>


namespace heroespath
{
namespace creature
{

    Title::Title(const title::Enum           TITLE,
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
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE != title::Count),                                                               "heroespath::creature::Title::Title(title_enum=role::Count"           << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an invalid TITLE enum.");
        M_ASSERT_OR_LOGANDTHROW_SS((title::ToString(TITLE).empty() == false),                                            "heroespath::creature::Title::Title(title_enum=" << TITLE             << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an invalid TITLE.");
        M_ASSERT_OR_LOGANDTHROW_SS((ROLES_VEC.empty() == false),                                                         "heroespath::creature::Title::Title(title=" << title::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an empty role vector.");
        M_ASSERT_OR_LOGANDTHROW_SS((ACHIEVEMENT_TYPE != AchievementType::Count),                                         "heroespath::creature::Title::Title(title=" << title::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given an AchievementType of 'Count'.");
        M_ASSERT_OR_LOGANDTHROW_SS((((ACHIEVEMENT_TYPE != AchievementType::None) && (ACHIEVEMENT_COUNT == 0)) == false), "heroespath::creature::Title::Title(title=" << title::ToString(TITLE) << ", ach_enum=" << ACHIEVEMENT_TYPE << ", ach_count=" << ACHIEVEMENT_COUNT << ", rank_bonus=" << RANK_BONUS << ", exp_bonus=" << EXPERIENCE_BONUS << ", stat_bonus=" << STATS_BONUS.ToStringCurrent(true, true, true) << ", roles=" << Role::CharacterRoleString(ROLES_VEC) << ") was given a valid AchievementType but the achievement count was zero.");

        //construct filename
        std::ostringstream ss;

        if ((ACHIEVEMENT_TYPE == AchievementType::Count) || (ACHIEVEMENT_TYPE == AchievementType::None))
        {
            ss << title::ToString(TITLE);
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
        ss << title::Name(title_);

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
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtr != nullptr), "Title::Change(creaturePtr==nullptr)  title=\"" << title::ToString(title_) << "\"  was given a null creaturePtr.");

        //titles are permenant, so they effect both the normal and current stat values
        creaturePtr->Stats().ModifyNormal(statBonus_);
        creaturePtr->Stats().ModifyCurrent(statBonus_);

        creaturePtr->IncreaseRank(rankBonus_);
        creaturePtr->IncreaseExp(expBonus_);

        creaturePtr->HealthNormalAdj(healthBonus_);
    }

}
}
