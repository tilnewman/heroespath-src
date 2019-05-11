// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SIZE_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_SIZE_ENUM_HPP_INCLUDED
//
// size-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct Size : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Small = 0,
            Medium,
            Large,
            Count
        };
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SIZE_ENUM_HPP_INCLUDED
