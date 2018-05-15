// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// corner.cpp
//
#include "corner-enum.hpp"

namespace heroespath
{
namespace sfml_util
{
    void Corner::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Corner::TopLeft, "TopLeft", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Corner::TopRight, "TopRight", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Corner::BottomLeft, "BottomLeft", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Corner::BottomRight, "BottomRight", SEPARATOR);
    }

} // namespace sfml_util
} // namespace heroespath
