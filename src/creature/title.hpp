// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_Title_HPP_INCLUDED
#define HEROESPATH_CREATURE_Title_HPP_INCLUDED
//
// title.hpp
//  Code that grants a player status and bonuses after a number of Achievements.
//
#include "creature/achievement-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/stat-set.hpp"
#include "creature/title-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{

    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    // A Title is earned after a certain number of Achievements.  (see ACHIEVEMENT_COUNT)
    // Titles can also be earned by specific quest events.  (see Titles::ProtectorOfThornberry)
    // Each Title is one in an ordered number.  (see Titles::Enum and ACHIEVEMENT_INDEX)
    // Titles hold the enum of the Achievement that earns them.  (see ACHIEVEMENT_TYPE)
    // Titles hold a vec of all the roles that are allowed to earn them. (see ROLES_VEC)
    // Once earned, a Title changes the player that earned it.  (see ..._BONUS)
    class Title
    {
    public:
        Title(const Title &) = delete;
        Title(Title &&) = delete;
        Title & operator=(const Title &) = delete;
        Title & operator=(Title &&) = delete;

        explicit Title(
            const Titles::Enum TITLE = Titles::Count,
            const AchievementType::Enum ACHIEVEMENT_TYPE = AchievementType::None,
            const Index_t & ACHIEVEMENT_INDEX = 0_index, // 0 is invalid
            const Count_t & ACHIEVEMENT_COUNT = 0_count,
            const RoleVec_t & ROLES_VEC = RoleVec_t(),
            const StatSet & STATS_BONUS = StatSet(),
            const Rank_t & RANK_BONUS = 0_rank,
            const Experience_t & EXPERIENCE_BONUS = 0_exp,
            const Health_t & HEALTH_BONUS = 0_health);

        const std::string Name() const { return Titles::Name(title_); }
        const std::string Desc() const { return Titles::Desc(title_); }
        Titles::Enum Which() const { return title_; }
        const StatSet StatBonus() const { return statBonus_; }
        AchievementType::Enum GetAchievementType() const { return achievementType_; }
        Count_t AchievementCount() const { return achievementCount_; }
        Index_t AchievementIndex() const { return achievementIndex_; }
        void Roles(RoleVec_t & rolesVec_OutParam) const { rolesVec_OutParam = rolesVec_; }
        const RoleVec_t RolesCopy() const { return rolesVec_; }

        bool IsRoleInList(const role::Enum E) const
        {
            return (std::find(rolesVec_.begin(), rolesVec_.end(), E) != rolesVec_.end());
        }

        const std::string ImageFilename() const { return fileName_; }
        Rank_t RankBonus() const { return rankBonus_; }
        Experience_t ExpBonus() const { return expBonus_; }
        Health_t HealthBonus() const { return healthBonus_; }

        const std::string ToString() const;
        const std::string LongDesc() const;

        void Change(const CreaturePtr_t) const;
        // titles are permenant, so no need for an UndoChange() function here

        friend bool operator<(const Title & L, const Title & R);
        friend bool operator==(const Title & L, const Title & R);

    public:
        static RoleVec_t ROLEVEC_ALL_PLAYER_ROLES_;

    protected:
        Titles::Enum title_;
        AchievementType::Enum achievementType_;
        Count_t achievementCount_;
        Index_t achievementIndex_;
        RoleVec_t rolesVec_;
        Rank_t rankBonus_;
        Experience_t expBonus_;
        StatSet statBonus_;
        std::string fileName_;
        Health_t healthBonus_;
    };

    using TitlePtr_t = misc::NotNull<Title *>;
    using TitlePtrOpt_t = boost::optional<TitlePtr_t>;
    using TitlePVec_t = std::vector<TitlePtr_t>;

    bool operator<(const Title & L, const Title & R);

    bool operator==(const Title & L, const Title & R);

    inline bool operator!=(const Title & L, const Title & R) { return !(L == R); }

    // responsible for wrapping all the information about a title transition
    struct TitleTransition
    {
        TitleTransition(
            const CreaturePtr_t CREATURE_PTR,
            const TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
            const TitlePtr_t TO_TITLE_PTR)
            : creaturePtr(CREATURE_PTR)
            , fromTitlePtrOpt(FROM_TITLE_PTR_OPT)
            , toTitlePtr(TO_TITLE_PTR)
        {}

        CreaturePtr_t creaturePtr;
        TitlePtrOpt_t fromTitlePtrOpt;
        TitlePtr_t toTitlePtr;

        friend bool operator==(const TitleTransition &, const TitleTransition &);
    };

    inline bool operator==(const TitleTransition & L, const TitleTransition & R)
    {
        return std::tie(L.creaturePtr, L.fromTitlePtrOpt, L.toTitlePtr)
            == std::tie(R.creaturePtr, R.fromTitlePtrOpt, R.toTitlePtr);
    }

    using TitleTransitionVec_t = std::vector<TitleTransition>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_Title_HPP_INCLUDED
