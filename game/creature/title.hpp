#ifndef GAME_CREATURE_Title_INCLUDED
#define GAME_CREATURE_Title_INCLUDED
//
// title.hpp
//  Code that places a Creature under a permenant condition.
//
#include "utilz/boost-serialize-includes.hpp"

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
        Title(const Title &);

        //prevent copy assignment
        Title & operator=(const Title &);

    public:
        explicit Title(const title::Enum           TITLE             = title::Count,
                       const AchievementType::Enum ACHIEVEMENT_TYPE  = AchievementType::None,
                       const std::size_t           ACHIEVEMENT_INDEX = 0, //0 is invalid, valid indexs always start at one
                       const std::size_t           ACHIEVEMENT_COUNT = 0,
                       const RoleVec_t &           ROLES_VEC         = RoleVec_t(),
                       const stats::StatSet &      STATS_BONUS       = stats::StatSet(),
                       const stats::Rank_t         RANK_BONUS        = 0,
                       const stats::Exp_t          EXPERIENCE_BONUS  = 0,
                       const stats::Health_t       HEALTH_BONUS      = 0);

        virtual ~Title();

        inline virtual const std::string Name() const                   { return title::Name(title_); }
        inline virtual const std::string Desc() const                   { return title::Desc(title_); }
        inline virtual title::Enum Which() const                        { return title_; }
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
        title::Enum           title_;
        AchievementType::Enum achievementType_;
        std::size_t           achievementCount_;
        std::size_t           achievementIndex_;
        RoleVec_t             rolesVec_;
        stats::Rank_t         rankBonus_;
        stats::Exp_t          expBonus_;
        stats::StatSet        statBonus_;
        std::string           fileName_;
        stats::Health_t       healthBonus_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & title_;
            ar & achievementType_;
            ar & achievementCount_;
            ar & achievementIndex_;
            ar & rolesVec_;
            ar & rankBonus_;
            ar & expBonus_;
            ar & statBonus_;
            ar & fileName_;
            ar & healthBonus_;
        }
    };


    using TitlePtr_t   = Title *;
    using TitlePtrC_t  = Title * const;
    using TitleCPtrC_t = const Title * const;
    using TitlePVec_t  = std::vector<TitlePtr_t>;

    using TitleUPtr_t = std::unique_ptr<Title>;
    using TitleUVec_t = std::vector<TitleUPtr_t>;


    bool operator<(const Title & L, const Title & R);

    bool operator==(const Title & L, const Title & R);

    inline bool operator!=(const Title & L, const Title & R)
    {
        return ! (L == R);
    }

}
}
#endif //GAME_CREATURE_Title_INCLUDED
