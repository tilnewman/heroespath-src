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
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace stage
{

    struct Stage : public EnumBaseCounting<EnumNameOfZeroIsNone>
    {
        enum Enum : EnumUnderlying_t
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
            Previous,
            Next,
            Save,
            Help,
            Count
        };

        static constexpr bool IsPlayable(const Stage::Enum STAGE) noexcept
        {
            return !(
                (STAGE == Stage::Next) || (STAGE == Stage::Previous) || (STAGE == Stage::Help)
                || (STAGE == Stage::None) || (STAGE == Stage::Count));
        }

        static constexpr bool IsPlayableAndNotPopup(const Stage::Enum STAGE) noexcept
        {
            return (IsPlayable(STAGE) && (STAGE != Stage::Popup));
        }
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_STAGE_ENUM_HPP_INCLUDED
