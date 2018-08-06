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
    class PopupResponse
    {
    public:
        PopupResponse(
            const std::string & POPUP_NAME,
            const ResponseTypes::Enum RESPONSE,
            const SizetOpt_t SELECTION_OPT)
            : name_(POPUP_NAME)
            , response_(RESPONSE)
            , selectionOpt_(SELECTION_OPT)
        {}

        PopupResponse(const PopupResponse &) = default;
        PopupResponse(PopupResponse &&) = default;
        PopupResponse & operator=(const PopupResponse &) = default;
        PopupResponse & operator=(PopupResponse &&) = default;

        const std::string Name() const { return name_; }
        ResponseTypes::Enum Response() const { return response_; }
        SizetOpt_t SelectionOpt() const { return selectionOpt_; }

    private:
        std::string name_;
        ResponseTypes::Enum response_;
        SizetOpt_t selectionOpt_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
