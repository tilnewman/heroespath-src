// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
#define HEROESPATH_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
//
// achievements.hpp
//  A class that manages all the achievements (title statistics) for a creature
//
#include "creature/achievement.hpp"
#include "creature/role-enum.hpp"
#include "creature/title-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/vector-map.hpp"

#include <memory>
#include <string>
#include <tuple>

namespace heroespath
{
namespace creature
{

    // handy types
    using AchievementMap_t = misc::VectorMap<AchievementType::Enum, Achievement>;

    // Responsible for managing all the title/achievement statistics for a creature.
    // Each Achievements object contains a list (map) of all possible Achievements.
    // Each of those Achievements contains a list of all achieved Titles for that Achievement.
    // Each creature keeps an Achievements object that manages all earned Titles.
    class Achievements
    {
    public:
        explicit Achievements(
            const std::string & OWNING_CREATURE_NAME = "",
            const creature::role::Enum & OWNING_CREATURE_ROLE = creature::role::Count);

        // these functions will throw on invalid enum or if a valid enum was not found in the map
        const Achievement Get(const AchievementType::Enum E) const;
        TitlePtrOpt_t Increment(const AchievementType::Enum E);
        TitlePtrOpt_t GetCurrentTitle(const AchievementType::Enum E) const;
        TitlePtrOpt_t GetNextTitle(const AchievementType::Enum E) const;

        friend bool operator<(const Achievements & L, const Achievements & R);
        friend bool operator==(const Achievements & L, const Achievements & R);

    private:
        void AchievementMapInsertPair(
            const AchievementType::Enum ACHV_TYPE,
            const Titles::Enum TITLE_FIRST,
            const Titles::Enum TITLE_LAST);

    private:
        std::string name_;
        creature::role::Enum role_;
        AchievementMap_t map_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
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

    inline bool operator!=(const Achievements & L, const Achievements & R) { return !(L == R); }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ACHIEVEMENTS_HPP_INCLUDED
