// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-enums.cpp
//
#include "popup-enums.hpp"

#include "misc/enum-util.hpp"

namespace heroespath
{
namespace popup
{

    const std::string
        PopupButtons::ToString(const PopupButtons::Enum BUTTONS, const EnumStringHow & HOW)
    {
        return EnumUtil<PopupButtons>::ToString(BUTTONS, HOW);
    }

    const std::string PopupButtons::ToStringPopulate(
        const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        std::string str;
        str.reserve(16);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::None, "None", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Okay, "Okay", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Continue, "Continue", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Yes, "Yes", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::No, "No", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Cancel, "Cancel", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Select, "Select", SEPARATOR);
        return str;
    }

} // namespace popup
} // namespace heroespath
