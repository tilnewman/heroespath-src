// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_GUIEVENT_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_GUIEVENT_ENUM_HPP_INCLUDED
//
// gui-event-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct GuiEvent : misc::EnumBaseBitField<GuiEvent>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Click = 1 << 0,
            DoubleClick = 1 << 1,
            MouseWheel = 1 << 2,
            SelectionChange = 1 << 3,
            Keypress = 1 << 4,
            FocusChange = 1 << 5,
            Last = FocusChange
        };

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_GUIEVENT_ENUM_HPP_INCLUDED
