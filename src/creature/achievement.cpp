// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// achievement.cpp
//
#include "achievement.hpp"

#include "creature/creature.hpp"
#include "creature/title-holder.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <limits>
#include <sstream>

namespace heroespath
{
namespace creature
{

    Achievement::Achievement(
        const AchievementType::Enum WHICH, const TitleCountMap_t & TITLE_COUNT_MAP)
        : which_(WHICH)
        , count_(0)
        , titleCountMap_(TITLE_COUNT_MAP)
    {}

    // uh-oh, this function makes no sense to me, and it is clear it needs to have common code with
    // GetNextTitle() refactored out... 2018-3-23
    TitlePtrOpt_t Achievement::GetCurrentTitle() const
    {
        TitlePtrOpt_t titlePtrOpt { boost::none };

        if (titleCountMap_.Empty() == false)
        {
            titlePtrOpt = title::Holder::Get(titleCountMap_.begin()->second);

            for (const auto & NEXT_COUNTTITLE_PAIR : titleCountMap_)
            {
                const auto NEXT_TITLE_PTR { title::Holder::Get(NEXT_COUNTTITLE_PAIR.second) };

                if ((NEXT_TITLE_PTR->AchievementCount() < count_)
                    && (NEXT_TITLE_PTR->AchievementCount()
                        >= titlePtrOpt.value()->AchievementCount()))
                {
                    titlePtrOpt = NEXT_TITLE_PTR;
                }
            }
        }

        if (titlePtrOpt && (titlePtrOpt.value()->AchievementCount() > count_))
        {
            return boost::none;
        }
        else
        {
            return titlePtrOpt;
        }
    }

    TitlePtrOpt_t Achievement::GetNextTitle() const
    {
        TitlePtrOpt_t titlePtrOpt { boost::none };

        if (titleCountMap_.Empty() == false)
        {
            titlePtrOpt = title::Holder::Get(titleCountMap_.begin()->second);

            for (const auto & NEXT_COUNTTITLE_PAIR : titleCountMap_)
            {
                const auto NEXT_TITLE_PTR { title::Holder::Get(NEXT_COUNTTITLE_PAIR.second) };

                if ((NEXT_TITLE_PTR->AchievementCount() > count_)
                    && (NEXT_TITLE_PTR->AchievementCount()
                        <= titlePtrOpt.value()->AchievementCount()))
                {
                    titlePtrOpt = NEXT_TITLE_PTR;
                }
            }
        }

        return titlePtrOpt;
    }

    const std::string Achievement::ToString() const
    {
        std::ostringstream ss;
        ss << Name() << "s current count=" << count_
           << ", and has the following achievable titles: ";

        const auto SEP_STR { ", " };
        for (const auto & NEXT_TITLE_COUNT_PAIR : titleCountMap_)
        {
            ss << Titles::Name(NEXT_TITLE_COUNT_PAIR.second) << " at count "
               << NEXT_TITLE_COUNT_PAIR.first << SEP_STR;
        }

        return boost::algorithm::erase_last_copy(ss.str(), SEP_STR);
    }

    bool Achievement::IsRoleInList(const role::Enum ENUM) const
    {
        return title::Holder::Get(titleCountMap_.begin()->second)->IsRoleInList(ENUM);
    }

    TitlePtrOpt_t Achievement::Increment(const creature::role::Enum ROLE_ENUM)
    {
        // Keep incrementing past the count of the final Title so the player can track
        // progress even if there are no more Titles to earn.  Don't stop until the
        // count reaches a large recognizable limit...something with lots of 9's...
        if (999999_count <= count_)
        {
            return boost::none;
        }

        ++count_;

        for (const auto & NEXT_TITLE_COUNT_PAIR : titleCountMap_)
        {
            const auto NEXT_TITLE_PTR { title::Holder::Get(NEXT_TITLE_COUNT_PAIR.second) };

            if ((NEXT_TITLE_PTR->AchievementCount() == count_)
                && (NEXT_TITLE_PTR->IsRoleInList(ROLE_ENUM)))
            {
                return NEXT_TITLE_PTR;
            }
        }

        return boost::none;
    }

} // namespace creature
} // namespace heroespath
