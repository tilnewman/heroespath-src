// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// gui-event-enum.cpp
//
#include "gui-event-enum.hpp"

#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string GuiEvent::ToString(const Enum ENUM, const EnumStringHow & HOW)
    {
        return EnumUtil<GuiEvent>::ToString(ENUM, HOW);
    }

    const std::string
        GuiEvent::ToStringPopulate(const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        std::string str;
        AppendNameIfBitIsSet(str, ENUM_VALUE, GuiEvent::Click, "Click", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, GuiEvent::DoubleClick, "DoubleClick", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, GuiEvent::MouseWheel, "MouseWheel", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, GuiEvent::SelectionChange, "SelectionChange", SEPARATOR);

        AppendNameIfBitIsSet(str, ENUM_VALUE, GuiEvent::Keypress, "Keypress", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, GuiEvent::FocusChange, "FocusChange", SEPARATOR);
        return str;
    }

} // namespace gui
} // namespace heroespath
