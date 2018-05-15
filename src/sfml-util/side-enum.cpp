// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// side.cpp
//
#include "side-enum.hpp"

namespace heroespath
{
namespace sfml_util
{
    void Side::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Side::Top, "Top", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Side::Bottom, "Bottom", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Side::Left, "Left", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Side::Right, "Right", SEPARATOR);
    }

} // namespace sfml_util
} // namespace heroespath
