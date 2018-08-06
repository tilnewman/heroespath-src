// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
#define HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
//
// popup-enums.hpp
//
#include "misc/enum-util.hpp"
#include "popup/popup-response-enum.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    struct PopupButtons
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Okay = ResponseTypes::Okay,
            Select = ResponseTypes::Select,
            Cancel = ResponseTypes::Cancel,
            SelectCancel = ResponseTypes::Select | ResponseTypes::Cancel,
            Continue = ResponseTypes::Continue,
            YesNo = ResponseTypes::Yes | ResponseTypes::No,
            YesNoCancel = ResponseTypes::Yes | ResponseTypes::No | ResponseTypes::Cancel
        };

        static const std::string ToString(const PopupButtons::Enum E);
        static bool IsValid(const PopupButtons::Enum E);
    };

    struct PopupButtonColor
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Light = 0,
            Dark,
            Count
        };

        static const std::string ToString(const PopupButtonColor::Enum);
        static bool IsValid(const PopupButtonColor::Enum);
    };

    struct PopupImage
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Banner = 0,
            Regular,
            RegularSidebar,
            Large,
            LargeSidebar,
            Spellbook,
            MusicSheet,
            Count
        };

        static const std::string ToString(const PopupImage::Enum);
        static bool IsValid(const PopupImage::Enum);
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
