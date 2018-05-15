// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPRESPONSEENUM_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPRESPONSEENUM_HPP_INCLUDED
//
// popup-response-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    struct ResponseTypes : misc::EnumBaseBitField<ResponseTypes>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Error = 0,
            None = Error,
            Okay = 1 << 0,
            Continue = 1 << 1,
            Yes = 1 << 2,
            No = 1 << 3,
            Cancel = 1 << 4,
            Select = 1 << 5,
            Last = Select
        };

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR = ", ");

        static bool IsAffirmative(const ResponseTypes::Enum);
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSEENUM_HPP_INCLUDED
