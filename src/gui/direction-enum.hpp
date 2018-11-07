// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_DIRECTION_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_DIRECTION_ENUM_HPP_INCLUDED
//
// direction-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct Direction : public misc::EnumBaseCounting<Direction, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Left = 0,
            Right,
            Up,
            Down,
            Count
        };

        static const std::string ToString(const Direction::Enum);
        static gui::Direction::Enum Opposite(const Direction::Enum);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_DIRECTION_ENUM_HPP_INCLUDED
