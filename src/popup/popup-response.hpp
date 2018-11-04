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
#include "popup/popup-response-enum.hpp"

#include <sstream>
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
            std ::ostringstream ss;

            ss << "PopupResonse(\"" << curently_open_popup_name << "\", "
               << popup::ResponseTypes::ToStringNoThrow(type) << ", selected_index=";

            if (selection_opt)
            {
                ss << selection_opt.value();
            }
            else
            {
                ss << "(none)";
            }

            ss << ")";

            return ss.str();
        }

        std::string curently_open_popup_name;
        ResponseTypes::Enum type = ResponseTypes::None;
        SizetOpt_t selection_opt = boost::none;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
