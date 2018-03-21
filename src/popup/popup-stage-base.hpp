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
#ifndef HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
//
// popup-stage-base.hpp
//
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/text-button.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-window.hpp"
#include "sfml-util/stage.hpp"

#include "popup/popup-info.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for encapsulating all state and operations common to popup windows.
    class PopupStageBase
        : public sfml_util::Stage
        , public sfml_util::gui::callback::ISliderBarCallbackHandler_t
        , public sfml_util::gui::callback::ITextButtonCallbackHandler_t
    {
    public:
        PopupStageBase(const PopupStageBase &) = delete;
        PopupStageBase(const PopupStageBase &&) = delete;
        PopupStageBase & operator=(const PopupStageBase &) = delete;
        PopupStageBase & operator=(const PopupStageBase &&) = delete;

        explicit PopupStageBase(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageBase();

        inline const std::string HandlerName() const override { return GetStageName(); }

        virtual bool
            HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &) override;

        virtual bool
            HandleCallback(const sfml_util::gui::callback::TextButtonCallbackPackage_t &) override;

        void Setup() override;

        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

        void DrawRedX(sf::RenderTarget &, const sf::RenderStates &);

        bool KeyRelease(const sf::Event::KeyEvent &) override;

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    protected:
        inline float ButtonTextHeight() const { return buttonTextHeight_; }

        virtual bool HandleSelect();

        virtual void SetupOuterAndInnerRegion();

        void SetupFullscreenRegionsAndBackgroundImage(const sf::FloatRect &);

        virtual void SetupSliderbar();

        void PlayValidKeypressSoundEffect() const;
        void PlayInvalidKeypressSoundEffect() const;

        void EndKeepAliveTimer();

    private:
        void SetupBackgroundImage();
        void SetupVariousButtonPositionValues();
        void SetupButtons();
        void SetupTextRegion();
        void SetupText();
        void SetupGradient();
        void SetupAccentSprite();
        void SetupRedXImage();

        const sf::IntRect BackgroundImageRect(const PopupImage::Enum PI, const float SCALE) const;

        virtual inline bool WillPressingCKeyClosePopup() const { return false; }

        float calcBackgroundImageScale(const PopupImage::Enum) const;

    protected:
        static const sf::Uint8 ACCENT_IMAGE_ALPHA_;
        static const float ACCENT_IMAGE_SCALEDOWN_RATIO_;

    protected:
        PopupInfo popupInfo_;
        sf::FloatRect innerRegion_;
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        sfml_util::gui::TextRegionUPtr_t textRegionUPtr_;
        sf::FloatRect textRegion_;
        sfml_util::gui::TextButtonUPtr_t buttonSelectUPtr_;
        sfml_util::gui::TextButtonUPtr_t buttonYesUPtr_;
        sfml_util::gui::TextButtonUPtr_t buttonNoUPtr_;
        sfml_util::gui::TextButtonUPtr_t buttonCancelUPtr_;
        sfml_util::gui::TextButtonUPtr_t buttonContinueUPtr_;
        sfml_util::gui::TextButtonUPtr_t buttonOkayUPtr_;
        sfml_util::gui::SliderBarUPtr_t sliderbarUPtr_;
        sf::Texture accentTexture1_;
        sf::Sprite accentSprite1_;
        sf::Texture accentTexture2_;
        sf::Sprite accentSprite2_;
        float sliderbarPosTop_;
        int selection_;
        sf::Sprite xSymbolSprite_;
        bool willShowXImage_;

    private:
        sfml_util::gui::box::Box box_;
        sfml_util::GradientRect gradient_;
        float buttonTextHeight_;
        float buttonVertPos_;
        sf::Texture xSymbolTexture_;
        float keepAliveTimerSec_;
    };
}
}

#endif // HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
