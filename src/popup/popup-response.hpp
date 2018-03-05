///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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

        inline const PopupInfo Info() const { return info_; }
        inline ResponseTypes::Enum Response() const { return response_; }
        inline std::size_t Selection() const { return selection_; }

    private:
        PopupInfo info_;
        ResponseTypes::Enum response_;
        std::size_t selection_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPRESPONSE_HPP_INCLUDED
