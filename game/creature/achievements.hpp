#ifndef GAME_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
#define GAME_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
//
// achievements.hpp
//  A class that manages all the achievements (title statistics) for a creature
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/creature/achievement.hpp"
#include "game/creature/title.hpp"

#include <memory>
#include <string>
#include <map>


namespace game
{
namespace creature
{

    //forward declarations
    class Creature;
    using CreatureSPtr_t = std::shared_ptr<Creature>;


    //Responsible for managing all the title/achievement statistics for a creature
    class Achievements
    {
    public:
        Achievements();

        //these functions will throw on invalid enum or if a valid enum was not found in the map
        const Achievement AchievementCopy(const AchievementType::Enum E) const;
        TitlePtr_t Increment(const AchievementType::Enum E, const CreatureSPtr_t &);
        TitlePtr_t GetCurrentTitle(const AchievementType::Enum E) const;
        TitlePtr_t GetNextTitle(const AchievementType::Enum E) const;

        friend bool operator<(const Achievements & L, const Achievements & R);
        friend bool operator==(const Achievements & L, const Achievements & R);

    private:
        void AchievementMapInsertPair(const AchievementType::Enum ACHV_TYPE,
                                      const title::Enum           TITLE_FIRST,
                                      const title::Enum           TITLE_LAST);

    private:
        AchievementMap_t map_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & map_;
        }
    };

    inline bool operator<(const Achievements & L, const Achievements & R)  { return L.map_ < R.map_; }
    inline bool operator==(const Achievements & L, const Achievements & R) { return L.map_ == R.map_; }
    inline bool operator!=(const Achievements & L, const Achievements & R) { return ! (L == R); }

}
}


#endif //GAME_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
