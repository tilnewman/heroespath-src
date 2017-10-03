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
#ifndef POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
#define POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
//
// popup-stage-char-select.hpp
//
#include "popup/popup-stage-image-select.hpp"
#include "popup/popup-info.hpp"

#include "sfml-util/gui/text-region.hpp"

#include <string>


namespace popup
{

    //Responsible for implementing the Character Select Popup Stage.
    class PopupStageCharacterSelect : public PopupStageImageSelect
    {
        PopupStageCharacterSelect(const PopupStageCharacterSelect &);
        PopupStageCharacterSelect & operator=(const PopupStageCharacterSelect &);

    public:
        explicit PopupStageCharacterSelect(const PopupInfo &);
        virtual ~PopupStageCharacterSelect();

        using PopupStageBase::HandlerName;
        using PopupStageBase::HandleCallback;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;

    protected:
        bool HandleSelect() override;
        
    private:
        void PlayRejectionSfx() const;
        std::size_t CountMax() const override;
        void SetupContent(const bool WILL_ERASE) override;
        void SetupCharacterSelectDetailText(const bool WILL_ERASE);
        void SetupCharacterSelectionRejectImage(const bool WILL_ERASE);
        void SetCurrentTexture(const std::size_t IMAGE_INDEX) override;

    private:
        sfml_util::gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
    };

}

#endif //POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
