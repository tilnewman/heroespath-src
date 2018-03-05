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
#ifndef HEROESPATH_CREATURE_Title_HPP_INCLUDED
#define HEROESPATH_CREATURE_Title_HPP_INCLUDED
//
// title.hpp
//  Code that grants a player status and bonuses after a number of Achievements.
//
#include "creature/achievement-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/title-enum.hpp"
#include "misc/types.hpp"
#include "stats/stat-set.hpp"

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
    using CreaturePtr_t = Creature *;

    // A Title is earned after a certain number of Achievements.  (see ACHIEVEMENT_COUNT)
    // Titles can also be earned by specific quest events.  (see Titles::ProtectorOfThornberry)
    // Each Title is one in an ordered number.  (see Titles::Enum and ACHIEVEMENT_INDEX)
    // Titles hold the enum of the Achievement that earns them.  (see ACHIEVEMENT_TYPE)
    // Titles hold a vec of all the roles that are allowed to earn them. (see ROLES_VEC)
    // Once earned, a Title changes the player that earned it.  (see ..._BONUS)
    class Title
    {
        Title(const Title &) = delete;
        Title & operator=(const Title &) = delete;

    public:
        explicit Title(
            const Titles::Enum TITLE = Titles::Count,
            const AchievementType::Enum ACHIEVEMENT_TYPE = AchievementType::None,
            const Index_t & ACHIEVEMENT_INDEX = 0_index, // 0 is invalid
            const Count_t & ACHIEVEMENT_COUNT = 0_count,
            const RoleVec_t & ROLES_VEC = RoleVec_t(),
            const stats::StatSet & STATS_BONUS = stats::StatSet(),
            const Rank_t & RANK_BONUS = 0_rank,
            const Experience_t & EXPERIENCE_BONUS = 0_exp,
            const Health_t & HEALTH_BONUS = 0_health);

        virtual ~Title();

        inline virtual const std::string Name() const { return Titles::Name(title_); }
        inline virtual const std::string Desc() const { return Titles::Desc(title_); }
        inline virtual Titles::Enum Which() const { return title_; }
        inline virtual const stats::StatSet StatBonus() const { return statBonus_; }
        inline virtual AchievementType::Enum GetAchievementType() const { return achievementType_; }
        inline virtual Count_t AchievementCount() const { return achievementCount_; }
        inline virtual Index_t AchievementIndex() const { return achievementIndex_; }
        inline virtual void Roles(RoleVec_t & rolesVec_OutParam) const
        {
            rolesVec_OutParam = rolesVec_;
        }
        inline virtual const RoleVec_t RolesCopy() const { return rolesVec_; }
        inline virtual bool IsRoleInList(const role::Enum E) const
        {
            return (std::find(rolesVec_.begin(), rolesVec_.end(), E) != rolesVec_.end());
        }
        inline virtual const std::string ImageFilename() const { return fileName_; }
        inline virtual Rank_t RankBonus() const { return rankBonus_; }
        inline virtual Experience_t ExpBonus() const { return expBonus_; }
        inline virtual Health_t HealthBonus() const { return healthBonus_; }

        virtual const std::string ToString() const;
        virtual const std::string LongDesc() const;

        virtual void Change(Creature * const) const;
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
        stats::StatSet statBonus_;
        std::string fileName_;
        Health_t healthBonus_;
    };

    using TitlePtr_t = Title *;
    using TitlePtrC_t = Title * const;
    using TitleCPtrC_t = const Title * const;
    using TitlePVec_t = std::vector<TitlePtr_t>;

    bool operator<(const Title & L, const Title & R);

    bool operator==(const Title & L, const Title & R);

    inline bool operator!=(const Title & L, const Title & R) { return !(L == R); }

    // responsible for wrapping all the information about a title transition
    struct TitleTransition
    {
        TitleTransition(
            const CreaturePtr_t CREATURE_PTR,
            const TitlePtr_t FROM_TITLE_PTR,
            const TitlePtr_t TO_TITLE_PTR)
            : creaturePtr(CREATURE_PTR)
            , fromTitlePtr(FROM_TITLE_PTR)
            , toTitlePtr(TO_TITLE_PTR)
        {}

        CreaturePtr_t creaturePtr;
        TitlePtr_t fromTitlePtr;
        TitlePtr_t toTitlePtr;

        friend bool operator==(const TitleTransition &, const TitleTransition &);
    };

    inline bool operator==(const TitleTransition & L, const TitleTransition & R)
    {
        return std::tie(L.creaturePtr, L.fromTitlePtr, L.toTitlePtr)
            == std::tie(R.creaturePtr, R.fromTitlePtr, R.toTitlePtr);
    }

    using TitleTransitionVec_t = std::vector<TitleTransition>;
}
}

#endif // HEROESPATH_CREATURE_Title_HPP_INCLUDED
