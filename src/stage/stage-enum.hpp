// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_STAGE_ENUM_HPP_INCLUDED
#define HEROESPATH_STAGE_STAGE_ENUM_HPP_INCLUDED
//
// stage-enum.hpp
//  An enum describing the game loop state.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace stage
{

    struct Stage : public misc::EnumBaseCounting<Stage, misc::EnumFirstValue::None>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Intro,
            Menu,
            Load,
            Character,
            Party,
            Adventure,
            Combat,
            Camp,
            Inn,
            Popup,
            Credits,
            Settings,
            Exit,
            Inventory,
            Treasure,
            Test,
            Previous,
            Next,
            Save,
            Help,
            Count
        };

        static const std::string ToString(const Enum);
        static bool HasFadedImage(const Enum);
        static bool IsPlayable(const Enum);
        static bool IsPlayableAndNotPopup(const Enum);
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_STAGE_ENUM_HPP_INCLUDED
