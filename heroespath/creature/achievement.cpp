//
// achievement.cpp
//
#include "achievement.hpp"

#include "heroespath/creature/creature.hpp"
#include "heroespath/creature/titles.hpp"
#include "heroespath/assertlogandthrow.hpp"

#include <sstream>


namespace heroespath
{
namespace creature
{

    Achievement::Achievement(const AchievementType::Enum WHICH,
                             const TitleCountMap_t       TITLE_COUNT_MAP)
    :
        which_        (WHICH),
        count_        (0),
        titleCountMap_(TITLE_COUNT_MAP)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((WHICH != AchievementType::None),   "heroespath::creature::Achievement::Achievement(which=AchievementType::None, title_count_map_size=" << TITLE_COUNT_MAP.size() << ") was given an invalid AchievementType.");
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE_COUNT_MAP.empty() == false), "heroespath::creature::Achievement::Achievement(which=" << AchievementType::ToString(WHICH) << ", title_count_map_size=" << TITLE_COUNT_MAP.size() << ") was given an empty title count map.");
    }


    TitlePtr_t Achievement::GetCurrentTitle() const
    {
        TitlePtr_t titlePtr{ nullptr };

        if (titleCountMap_.empty() == false)
        {
            titlePtr = titleCountMap_.begin()->second;
            for (auto const & NEXT_COUNTTITLE_PAIR : titleCountMap_)
                if ((NEXT_COUNTTITLE_PAIR.second->AchievementCount() < count_) && (NEXT_COUNTTITLE_PAIR.second->AchievementCount() >= titlePtr->AchievementCount()))
                    titlePtr = NEXT_COUNTTITLE_PAIR.second;
        }

        if (titlePtr->AchievementCount() > count_)
            return nullptr;
        else
            return titlePtr;
    }


    TitlePtr_t Achievement::GetNextTitle() const
    {
        TitlePtr_t titlePtr(nullptr);

        if (titleCountMap_.empty() == false)
        {
            titlePtr = titleCountMap_.begin()->second;
            for (auto const & NEXT_COUNTTITLE_PAIR : titleCountMap_)
                if ((NEXT_COUNTTITLE_PAIR.second->AchievementCount() > count_) && (NEXT_COUNTTITLE_PAIR.second->AchievementCount() <= titlePtr->AchievementCount()))
                    titlePtr = NEXT_COUNTTITLE_PAIR.second;
        }

        return titlePtr;
    }


    const std::string Achievement::ToString() const
    {
        std::ostringstream ss;
        ss << Name()
           << "s current count=" << count_
           << ", and has the following achievable titles: ";

        const std::string SEP_STR(", ");
        for (auto const & NEXT_TITLE_COUNT_PAIR : titleCountMap_)
            ss << NEXT_TITLE_COUNT_PAIR.second->Name() << " at count " << NEXT_TITLE_COUNT_PAIR.first << SEP_STR;

        return boost::algorithm::erase_last_copy(ss.str(), SEP_STR);
    }


    bool Achievement::IsRoleInList(const role::Enum E) const
    {
        return titleCountMap_.begin()->second->IsRoleInList(E);
    }


    TitlePtr_t Achievement::Increment(const CreatureSPtr_t & CREATURE_SPTR)
    {
        ++count_;

        for(auto const & NEXT_TITLE_COUNT_PAIR : titleCountMap_)
            if (NEXT_TITLE_COUNT_PAIR.second->AchievementCount() == count_)
                if (NEXT_TITLE_COUNT_PAIR.second->IsRoleInList(CREATURE_SPTR->Role().Which()))
                    return NEXT_TITLE_COUNT_PAIR.second;

        return nullptr;
    }

}
}
