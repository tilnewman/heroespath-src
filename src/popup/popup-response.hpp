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

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for wrapping everything about the player's response to a popup
    struct PopupResponse
    {
        PopupResponse()
            : name()
            , type(ResponseTypes::None)
            , selection_opt(boost::none)
        {
            Reset();
        }

        PopupResponse(
            const std::string & NAME,
            const ResponseTypes::Enum RESPONSE_TYPE,
            const SizetOpt_t SELECTION_OPT)
            : name(NAME)
            , type(RESPONSE_TYPE)
            , selection_opt(SELECTION_OPT)
        {}

        PopupResponse(const PopupResponse &) = default;
        PopupResponse(PopupResponse &&) = default;
        PopupResponse & operator=(const PopupResponse &) = default;
        PopupResponse & operator=(PopupResponse &&) = default;

        void Reset(const std::string & NAME = "")
        {
            name = NAME;
            type = ResponseTypes::None;
            selection_opt = boost::none;
        }

        std::string name;
        ResponseTypes::Enum type;
        SizetOpt_t selection_opt;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
