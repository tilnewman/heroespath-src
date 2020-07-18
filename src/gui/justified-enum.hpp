// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_JUSTIFIED_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_JUSTIFIED_ENUM_HPP_INCLUDED
//
// justified-enum.hpp
//
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace gui
{

    struct Justified : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Left = 0,
            Right,
            Center,
            Count
        };
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_JUSTIFIED_ENUM_HPP_INCLUDED
