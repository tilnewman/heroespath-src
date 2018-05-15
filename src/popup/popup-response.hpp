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
//  Encapsulates everything about a popup window's result with a response from the user
//
#include "popup/popup-info.hpp"
#include "popup/popup-response-enum.hpp"

namespace heroespath
{
namespace popup
{

    // encapsulates everything about a popup window's response
    class PopupResponse
    {
    public:
        PopupResponse(
            const PopupInfo & POPUP_INFO,
            const ResponseTypes::Enum RESPONSE,
            const std::size_t SELECTION)
            : info_(POPUP_INFO)
            , response_(RESPONSE)
            , selection_(SELECTION)
        {}

        const PopupInfo Info() const { return info_; }
        ResponseTypes::Enum Response() const { return response_; }
        std::size_t Selection() const { return selection_; }

    private:
        PopupInfo info_;
        ResponseTypes::Enum response_;
        std::size_t selection_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
