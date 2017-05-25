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

#include <memory>
#include <string>
#include <map>


namespace game
{
namespace creature
{

    //forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;


    //Responsible for managing all the title/achievement statistics for a creature
    class Achievements
    {
    public:
        Achievements();

        //these functions will throw on invalid enum or if a valid enum was not found in the map
        const Achievement AchievementCopy(const AchievementType::Enum E) const;
        TitlePtr_t Increment(const AchievementType::Enum E, const CreaturePtr_t);
        TitlePtr_t GetCurrentTitle(const AchievementType::Enum E) const;
        TitlePtr_t GetNextTitle(const AchievementType::Enum E) const;

        friend bool operator<(const Achievements & L, const Achievements & R);
        friend bool operator==(const Achievements & L, const Achievements & R);

    private:
        void AchievementMapInsertPair(const AchievementType::Enum ACHV_TYPE,
                                      const Titles::Enum          TITLE_FIRST,
                                      const Titles::Enum          TITLE_LAST);

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
