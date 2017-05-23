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
    using CreaturePtr_t = Creature *;


    //Note:  Must store TItles::Enums and not TitlePtr_ts here because
    //       Achievements are serialized, and Titles cannot be serialized.
    using TitleCountMap_t = std::map<std::size_t, Titles::Enum>;


    //responsible for storing all the information about an achievement
    class Achievement
    {
    public:
        Achievement(const AchievementType::Enum WHICH           = AchievementType::None,
                    const TitleCountMap_t &     TITLE_COUNT_MAP = TitleCountMap_t());

        inline AchievementType::Enum Which() const              { return which_; }
        inline std::size_t Count() const                        { return count_; }
        inline const std::string Name() const                   { return AchievementType::Name(which_); }
        inline const TitleCountMap_t TitleCountMapCopy() const  { return titleCountMap_; }

        //These functions return pointers instead of enums because they
        //need a way for them to return nothing.
        TitlePtr_t GetCurrentTitle() const;
        TitlePtr_t GetNextTitle() const;
        TitlePtr_t Increment(const CreaturePtr_t);

        const std::string ToString() const;

        bool IsRoleInList(const role::Enum) const;

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
