//
// achievement.cpp
//
#include "achievement.hpp"

#include "game/creature/creature.hpp"
#include "game/creature/titles.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <sstream>


namespace game
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
        M_ASSERT_OR_LOGANDTHROW_SS((WHICH != AchievementType::None),   "game::creature::Achievement::Achievement(which=AchievementType::None, title_count_map_size=" << TITLE_COUNT_MAP.size() << ") was given an invalid AchievementType.");
        M_ASSERT_OR_LOGANDTHROW_SS((TITLE_COUNT_MAP.empty() == false), "game::creature::Achievement::Achievement(which=" << AchievementType::ToString(WHICH) << ", title_count_map_size=" << TITLE_COUNT_MAP.size() << ") was given an empty title count map.");
    }


    TitlePtr_t Achievement::GetCurrentTitle() const
    {
        TitlePtr_t titlePtr{ nullptr };

        if (titleCountMap_.empty() == false)
        {
            titlePtr = title::Warehouse::Get(titleCountMap_.begin()->second);
            for (auto const & NEXT_COUNTTITLE_PAIR : titleCountMap_)
            {
                auto const NEXT_TITLE_PTR{ title::Warehouse::Get(NEXT_COUNTTITLE_PAIR.second) };
                if ((NEXT_TITLE_PTR->AchievementCount() < count_) &&
                    (NEXT_TITLE_PTR->AchievementCount() >= titlePtr->AchievementCount()))
                {
                    titlePtr = NEXT_TITLE_PTR;
                }
            }
        }

        if ((nullptr != titlePtr) && (titlePtr->AchievementCount() > count_))
        {
            return nullptr;
        }
        else
        {
            return titlePtr;
        }
    }


    TitlePtr_t Achievement::GetNextTitle() const
    {
        TitlePtr_t titlePtr{ nullptr };

        if (titleCountMap_.empty() == false)
        {
            titlePtr = title::Warehouse::Get( titleCountMap_.begin()->second );
            for (auto const & NEXT_COUNTTITLE_PAIR : titleCountMap_)
            {
                auto const NEXT_TITLE_PTR{ title::Warehouse::Get(NEXT_COUNTTITLE_PAIR.second) };
                if ((NEXT_TITLE_PTR->AchievementCount() > count_) &&
                    (NEXT_TITLE_PTR->AchievementCount() <= titlePtr->AchievementCount()))
                {
                    titlePtr = NEXT_TITLE_PTR;
                }
            }
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
            ss << Titles::Name(NEXT_TITLE_COUNT_PAIR.second) << " at count " << NEXT_TITLE_COUNT_PAIR.first << SEP_STR;

        return boost::algorithm::erase_last_copy(ss.str(), SEP_STR);
    }


    bool Achievement::IsRoleInList(const role::Enum E) const
    {
        return title::Warehouse::Get(titleCountMap_.begin()->second)->IsRoleInList(E);
    }


    TitlePtr_t Achievement::Increment(const CreaturePtr_t CREATURE_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr), "game::creature::Achievement::Increment() was given a null CREATURE_PTR.");

        ++count_;

        for (auto const & NEXT_TITLE_COUNT_PAIR : titleCountMap_)
        {
            auto const NEXT_TITLE_PTR{ title::Warehouse::Get(NEXT_TITLE_COUNT_PAIR.second) };
            if (NEXT_TITLE_PTR->AchievementCount() == count_)
                if (NEXT_TITLE_PTR->IsRoleInList(CREATURE_PTR->Role().Which()))
                    return NEXT_TITLE_PTR;
        }

        return nullptr;
    }

}
}
