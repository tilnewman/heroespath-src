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
#ifndef POPUP_POPUPSTAGENUMSELECT_HPP_INCLUDED
#define POPUP_POPUPSTAGENUMSELECT_HPP_INCLUDED
//
// popup-stage-num-select.hpp
//
#include "popup/popup-stage-base.hpp"

#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/text-entry-box.hpp"

#include <string>


namespace popup
{

    //Responsible for implementing the Number Selection Popup Stage.
    class PopupStageNumberSelect
    :
        public PopupStageBase,
        public sfml_util::gui::callback::ITextEntryBoxCallbackHandler_t
    {
        PopupStageNumberSelect(const PopupStageNumberSelect &) =delete;
        PopupStageNumberSelect & operator=(const PopupStageNumberSelect &) =delete;

    public:
        explicit PopupStageNumberSelect(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageNumberSelect();

        using PopupStageBase::HandleCallback;

        inline virtual const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        virtual bool HandleCallback(
            const sfml_util::gui::callback::SliderBarCallbackPackage_t &) override;

        virtual bool HandleCallback(
            const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &) override;

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void SetupInfoText(const std::string &);
        int GetSelectNumber() const;
        bool ProcessSelectNumber();
        void SetupSliderbar() override;

    private:
        static const int NUMBER_SELECT_INVALID_;
        //
        sfml_util::gui::TextRegionUPtr_t msgTextRegionUPtr_;
        sfml_util::gui::TextEntryBoxUPtr_t textEntryBoxUPtr_;
        bool willSliderbarUpdate_;
        bool willTextBoxUpdate_;
    };

}

#endif //POPUP_POPUPSTAGENUMSELECT_HPP_INCLUDED
