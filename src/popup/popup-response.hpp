// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
//
// popup-response.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "popup/popup-enums.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for wrapping everything about the player's response to a popup
    struct PopupResponse
    {
        PopupResponse() = default;
        PopupResponse(const PopupResponse &) = default;
        PopupResponse(PopupResponse &&) = default;
        PopupResponse & operator=(const PopupResponse &) = default;
        PopupResponse & operator=(PopupResponse &&) = default;

        const std::string ToString() const
        {
            std::string str;
            str.reserve(32);

            str += "PopupResonse(\"";
            str += curently_open_popup_name;
            str += "\", ";
            str += NAMEOF_ENUM_STR(type);
            str += ", selected_index=";

            if (selection_opt)
            {
                str += std::to_string(selection_opt.value());
            }
            else
            {
                str += "(none)";
            }

            str += ')';

            return str;
        }

        std::string curently_open_popup_name;
        PopupButtons::Enum type = PopupButtons::None;
        SizetOpt_t selection_opt = boost::none;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
