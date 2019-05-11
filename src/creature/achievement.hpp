// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ACHIEVEMENT_HPP_INCLUDED
#define HEROESPATH_CREATURE_ACHIEVEMENT_HPP_INCLUDED
//
// achievement.hpp
//  A class that manages all the information about a particular creature Achievement.
//
#include "creature/achievement-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/title.hpp"
#include "game/strong-types.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/vector-map.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{

    // Note:  Must store Titles::Enums and not TitlePtr_ts here because
    //       Achievements are serialized, and Titles cannot be serialized.
    using TitleCountMap_t = misc::VectorMap<std::size_t, Titles::Enum>;

    // Responsible for storing all the information about an achievement.
    // An Achievement is a re-occurring event that is summed up until a
    // Title is earned.
    class Achievement
    {
    public:
        explicit Achievement(
            const AchievementType::Enum WHICH = AchievementType::None,
            const TitleCountMap_t & TITLE_COUNT_MAP = TitleCountMap_t());

        AchievementType::Enum Which() const { return which_; }
        std::size_t Count() const { return count_; }
        const std::string Name() const { return std::string(AchievementType::Name(which_)); }
        const TitleCountMap_t & TitleCountMap() const { return titleCountMap_; }

        // These functions return pointers instead of enums because they
        // need a way for them to return nothing.
        TitlePtrOpt_t GetCurrentTitle() const;
        TitlePtrOpt_t GetNextTitle() const;
        TitlePtrOpt_t Increment(const creature::role::Enum ROLE_ENUM);

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
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & which_;
            ar & count_;
            ar & titleCountMap_;
        }
    };

    inline bool operator<(const Achievement & L, const Achievement & R)
    {
        return std::tie(L.which_, L.count_, L.titleCountMap_)
            < std::tie(R.which_, R.count_, R.titleCountMap_);
    }

    inline bool operator==(const Achievement & L, const Achievement & R)
    {
        return std::tie(L.which_, L.count_, L.titleCountMap_)
            == std::tie(R.which_, R.count_, R.titleCountMap_);
    }

    inline bool operator!=(const Achievement & L, const Achievement & R) { return !(L == R); }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ACHIEVEMENT_HPP_INCLUDED
