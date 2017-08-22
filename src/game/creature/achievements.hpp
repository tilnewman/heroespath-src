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
#ifndef GAME_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
#define GAME_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
//
// achievements.hpp
//  A class that manages all the achievements (title statistics) for a creature
//
#include "misc/boost-serialize-includes.hpp"

#include "game/creature/achievement.hpp"
#include "game/creature/title-enum.hpp"
#include "game/creature/role-enum.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <map>


namespace game
{
namespace creature
{

    //handy types
    using AchievementMap_t = std::map<AchievementType::Enum, Achievement>;
    using AchievementMapIter_t = AchievementMap_t::iterator;
    using AchievementMapCIter_t = AchievementMap_t::const_iterator;


    //Responsible for managing all the title/achievement statistics for a creature.
    //Each Achievements object contains a list (map) of all possible Achievements.
    //Each of those Achievements contains a list of all achieved Titles for that Achievement.
    //Each creature keeps an Achievements object that manages all earned Titles.
    class Achievements
    {
    public:
        explicit Achievements(
            const std::string &          OWNING_CREATURE_NAME = "",
            const creature::role::Enum & OWNING_CREATURE_ROLE = creature::role::Count);

        //these functions will throw on invalid enum or if a valid enum was not found in the map
        const Achievement & Get(const AchievementType::Enum E) const;
        TitlePtr_t Increment(const AchievementType::Enum E);
        TitlePtr_t GetCurrentTitle(const AchievementType::Enum E) const;
        TitlePtr_t GetNextTitle(const AchievementType::Enum E) const;

        friend bool operator<(const Achievements & L, const Achievements & R);
        friend bool operator==(const Achievements & L, const Achievements & R);

    private:
        void AchievementMapInsertPair(const AchievementType::Enum ACHV_TYPE,
                                      const Titles::Enum          TITLE_FIRST,
                                      const Titles::Enum          TITLE_LAST);

    private:
        std::string name_;
        creature::role::Enum role_;
        AchievementMap_t map_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name_;
            ar & role_;
            ar & map_;
        }
    };


    inline bool operator<(const Achievements & L, const Achievements & R)
    {
        return std::tie(L.name_, L.role_, L.map_) < std::tie(R.name_, R.role_, R.map_);
    }


    inline bool operator==(const Achievements & L, const Achievements & R)
    {
        return std::tie(L.name_, L.role_, L.map_) == std::tie(R.name_, R.role_, R.map_);
    }


    inline bool operator!=(const Achievements & L, const Achievements & R)
    {
        return ! (L == R);
    }

}
}


#endif //GAME_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
