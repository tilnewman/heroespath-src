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
#ifndef POPUP_POPUPSTAGEITEMPROFILEWAIT_HPP_INCLUDED
#define POPUP_POPUPSTAGEITEMPROFILEWAIT_HPP_INCLUDED
//
// popup-stage-item-profile-wait.hpp
//
#include "popup/popup-stage-base.hpp"


namespace popup
{

    //Responsible for implementing the System Error Popup Stage.
    class PopupStageItemProfileWait : public PopupStageBase
    {
        PopupStageItemProfileWait(const PopupStageItemProfileWait &) =delete;
        PopupStageItemProfileWait & operator=(const PopupStageItemProfileWait &) =delete;

    public:
        explicit PopupStageItemProfileWait(const PopupInfo &);
        virtual ~PopupStageItemProfileWait();

        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

    private:
        int drawCountdown_;
    };

}

#endif //POPUP_POPUPSTAGEITEMPROFILEWAIT_HPP_INCLUDED
