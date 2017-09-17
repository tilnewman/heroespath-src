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
#ifndef SFMLUTIL_POPUPSTAGEBASE_HPP_INCLUDED
#define SFMLUTIL_POPUPSTAGEBASE_HPP_INCLUDED
//
// popup-stage-base.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-window.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/text-button.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/popup-info.hpp"


namespace sfml_util
{

    //Responsible for encapsulating all state and operations common to popup windows.
    class PopupStageBase
    :
        public Stage,
        public gui::callback::ISliderBarCallbackHandler_t,
        public gui::callback::ITextButtonCallbackHandler_t
    {
        //prevent copy construction
        PopupStageBase(const PopupStageBase &) =delete;

        //prevent copy assignment
        PopupStageBase & operator=(const PopupStageBase &) =delete;

    public:
        PopupStageBase(
            const game::PopupInfo & POPUP_INFO,
            const sf::FloatRect & REGION,
            const sf::FloatRect & INNER_REGION,
            const sf::Texture & BG_TEXTURE = sf::Texture());

        virtual ~PopupStageBase();

        inline virtual const std::string HandlerName() const override { return GetStageName(); }
        virtual bool HandleCallback(const gui::callback::SliderBarCallbackPackage_t &) override;
        virtual bool HandleCallback(const gui::callback::TextButtonCallbackPackage_t &) override;

        virtual void Setup() override;

        virtual void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

    protected:
        //Must call Setup() before these functions or else they will return zero.
        inline float ButtonTextHeight() const { return buttonTextHeight_; }
        
        bool HandleSelect();

    private:
        void SetupVariousButtonPositionValues();
        void SetupButtons();
        void SetupTextRegion();
        void SetupText();
        void SetupGradient();
        void SetupAccentSprite();
        void SetupSliderbar();

    private:
        static const sf::Uint8 ACCENT_IMAGE_ALPHA_;

    protected:
        const game::PopupInfo POPUP_INFO_;
        const sf::FloatRect INNER_REGION_;
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        gui::TextRegionUPtr_t textRegionUPtr_;
        sf::FloatRect textRegion_;
        gui::TextButtonUPtr_t buttonSelectUPtr_;
        gui::TextButtonUPtr_t buttonYesUPtr_;
        gui::TextButtonUPtr_t buttonNoUPtr_;
        gui::TextButtonUPtr_t buttonCancelUPtr_;
        gui::TextButtonUPtr_t buttonContinueUPtr_;
        gui::TextButtonUPtr_t buttonOkayUPtr_;
        gui::SliderBarUPtr_t sliderbarUPtr_;
        sf::Texture accentTexture1_;
        sf::Sprite accentSprite1_;
        sf::Texture accentTexture2_;
        sf::Sprite accentSprite2_;
        float sliderbarPosTop_;
        int selection_;
    private:
        gui::box::Box box_;
        GradientRect gradient_;
        float buttonTextHeight_;
        float buttonVertPos_;
    };

}

#endif //SFMLUTIL_POPUPSTAGEBASE_HPP_INCLUDED
