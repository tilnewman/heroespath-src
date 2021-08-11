// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MOVING_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_MOVING_ENUM_HPP_INCLUDED
//
// moving-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct Moving : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Still = 0,
            Toward,
            Away,
            Count
        };

        static const std::string ToString(const Moving::Enum);

        static bool IsMoving(const Moving::Enum ENUM)
        {
            return ((ENUM == Toward) || (ENUM == Away));
        }
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MOVING_ENUM_HPP_INCLUDED
