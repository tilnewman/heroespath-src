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
#ifndef GAME_CREATURE_Title_INCLUDED
#define GAME_CREATURE_Title_INCLUDED
//
// title.hpp
//  Code that places a Creature under a permenant condition.
//
#include "game/stats/stat-set.hpp"
#include "game/creature/role-enum.hpp"
#include "game/creature/title-enum.hpp"
#include "game/creature/achievement-enum.hpp"

#include <string>
#include <vector>
#include <memory>


namespace game
{
namespace creature
{

    class Creature;


    //base class for all conditions
    class Title
    {
        //prevent copy construction
        Title(const Title &) =delete;

        //prevent copy assignment
        Title & operator=(const Title &) =delete;

    public:
        explicit Title(const Titles::Enum          TITLE             = Titles::Count,
                       const AchievementType::Enum ACHIEVEMENT_TYPE  = AchievementType::None,
                       const std::size_t           ACHIEVEMENT_INDEX = 0, //0 is invalid, valid indexs always start at one
                       const std::size_t           ACHIEVEMENT_COUNT = 0,
                       const RoleVec_t &           ROLES_VEC         = RoleVec_t(),
                       const stats::StatSet &      STATS_BONUS       = stats::StatSet(),
                       const stats::Rank_t         RANK_BONUS        = 0,
                       const stats::Exp_t          EXPERIENCE_BONUS  = 0,
                       const stats::Health_t       HEALTH_BONUS      = 0);

        virtual ~Title();

        inline virtual const std::string Name() const                   { return Titles::Name(title_); }
        inline virtual const std::string Desc() const                   { return Titles::Desc(title_); }
        inline virtual Titles::Enum Which() const                       { return title_; }
        inline virtual const stats::StatSet StatBonus() const           { return statBonus_; }
        inline virtual AchievementType::Enum GetAchievementType() const { return achievementType_; }
        inline virtual std::size_t AchievementCount() const             { return achievementCount_; }
        inline virtual std::size_t AchievementIndex() const             { return achievementIndex_; }
        inline virtual void Roles(RoleVec_t & rolesVec_OutParam) const  { rolesVec_OutParam = rolesVec_; }
        inline virtual const RoleVec_t RolesCopy() const                { return rolesVec_; }
        inline virtual bool IsRoleInList(const role::Enum E) const      { return (std::find(rolesVec_.begin(), rolesVec_.end(), E) != rolesVec_.end()); }
        inline virtual const std::string ImageFilename() const          { return fileName_; }
        inline virtual stats::Rank_t RankBonus() const                  { return rankBonus_; }
        inline virtual stats::Exp_t ExpBonus() const                    { return expBonus_; }
        inline virtual stats::Health_t HealthBonus() const              { return healthBonus_; }

        virtual const std::string ToString() const;
        virtual const std::string LongDesc() const;

        virtual void Change(Creature * const) const;
        //titles are permenant, so no need for an UndoChange() function here

        friend bool operator<(const Title & L, const Title & R);
        friend bool operator==(const Title & L, const Title & R);

    protected:
        Titles::Enum          title_;
        AchievementType::Enum achievementType_;
        std::size_t           achievementCount_;
        std::size_t           achievementIndex_;
        RoleVec_t             rolesVec_;
        stats::Rank_t         rankBonus_;
        stats::Exp_t          expBonus_;
        stats::StatSet        statBonus_;
        std::string           fileName_;
        stats::Health_t       healthBonus_;
    };


    using TitlePtr_t   = Title *;
    using TitlePtrC_t  = Title * const;
    using TitleCPtrC_t = const Title * const;
    using TitlePVec_t  = std::vector<TitlePtr_t>;

    using TitleSPtr_t = std::shared_ptr<Title>;
    using TitleSVec_t = std::vector<TitleSPtr_t>;


    bool operator<(const Title & L, const Title & R);

    bool operator==(const Title & L, const Title & R);

    inline bool operator!=(const Title & L, const Title & R)
    {
        return ! (L == R);
    }

}
}
#endif //GAME_CREATURE_Title_INCLUDED
