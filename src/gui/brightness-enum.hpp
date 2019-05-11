// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_BRIGHTNESS_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_BRIGHTNESS_ENUM_HPP_INCLUDED
//
// brightness.hpp
//
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace gui
{

    struct Brightness : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Bright = 0, // Default
            Medium,
            Dark,
            Count
        };
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_BRIGHTNESS_ENUM_HPP_INCLUDED
