#ifndef GAME_CREATURE_ACHIEVEMENT_HPP_INCLUDED
#define GAME_CREATURE_ACHIEVEMENT_HPP_INCLUDED
//
// achievement.hpp
//  A class that manages all the information about a particular creature Achievement.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/creature/achievement-enum.hpp"
#include "game/creature/title.hpp"
#include "game/creature/role.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <tuple>


namespace game
{
namespace creature
{

    //forward declarations
    class Creature;
    using CreatureSPtr_t = std::shared_ptr<Creature>;


    using TitleCountMap_t = std::map<std::size_t, TitlePtr_t>;


    //responsible for storing all the information about an achievement
    class Achievement
    {
    public:
        Achievement(const AchievementType::Enum WHICH           = AchievementType::None,
                    const TitleCountMap_t       TITLE_COUNT_MAP = TitleCountMap_t());

        inline AchievementType::Enum Which() const              { return which_; }
        inline std::size_t Count() const                        { return count_; }
        inline const std::string Name() const                   { return AchievementType::Name(which_); }
        inline const TitleCountMap_t TitleCountMapCopy() const  { return titleCountMap_; }

        TitlePtr_t GetCurrentTitle() const;

        TitlePtr_t GetNextTitle() const;

        const std::string ToString() const;

        bool IsRoleInList(const role::Enum) const;

        //Note:  This function does not new up a unique Title object and return it.
        //       All creatures share the same copy of all Title objects.
        //       So this function returns a shared_ptr to the Title object in titlesSVec_.
        TitlePtr_t Increment(const CreatureSPtr_t &);

        friend bool operator<(const Achievement & L, const Achievement & R);
        friend bool operator==(const Achievement & L, const Achievement & R);

    private:
        AchievementType::Enum which_;
        std::size_t count_;
        TitleCountMap_t titleCountMap_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & which_;
            ar & count_;
            ar & titleCountMap_;
        }
    };


    using AchievementMap_t = std::map<AchievementType::Enum, Achievement>;
    using AchievementMapIter_t = AchievementMap_t::iterator;
    using AchievementMapCIter_t = AchievementMap_t::const_iterator;


    inline bool operator<(const Achievement & L, const Achievement & R)
    {
        return  std::tie(L.which_, L.count_, L.titleCountMap_)
                <
                std::tie(R.which_, R.count_, R.titleCountMap_);
    }


    inline bool operator==(const Achievement & L, const Achievement & R)
    {
        return  std::tie(L.which_, L.count_, L.titleCountMap_)
                ==
                std::tie(R.which_, R.count_, R.titleCountMap_);
    }


    inline bool operator!=(const Achievement & L, const Achievement & R)
    {
        return ! (L == R);
    }

}
}


#endif //GAME_CREATURE_ACHIEVEMENT_HPP_INCLUDED
