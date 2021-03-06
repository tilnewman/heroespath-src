// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_CORNER_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_CORNER_ENUM_HPP_INCLUDED
//
// corner.hpp
//
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace gui
{

    struct Corner : public EnumBaseBitField<>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            TopLeft = 1 << 0,
            TopRight = 1 << 1,
            BottomLeft = 1 << 2,
            BottomRight = 1 << 3
        };

        static constexpr Enum Last = BottomRight;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CORNER_ENUM_HPP_INCLUDED
