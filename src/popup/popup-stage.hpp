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
#ifndef POPUP_POPUPSTAGE_HPP_INCLUDED
#define POPUP_POPUPSTAGE_HPP_INCLUDED
//
// popup-stage.hpp
//  This class encapsulates a popup window stage on screen.
//
#include "sfml-util/sliders.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-entry-box.hpp"
#include "sfml-util/gui/background-info.hpp"

#include "popup/popup-stage-base.hpp"

#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <utility>


namespace sfml_util
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
}
}
namespace popup
{
    //A base class for all Popup Window Stages
    class PopupStage
    :
        public PopupStageBase,
        public sfml_util::gui::callback::ITextEntryBoxCallbackHandler_t
    {
        //prevent copy construction
        PopupStage(const PopupStage &) =delete;

        //prevent copy assignment
        PopupStage & operator=(const PopupStage &) =delete;

    public:
        explicit PopupStage(const PopupInfo & POPUP_INFO);

        virtual ~PopupStage();

        inline virtual const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &) override;
        using PopupStageBase::HandleCallback;
        bool HandleCallback(const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &) override;
        
        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) override;

        inline std::size_t CurrentSelection() const { return imageIndex_; }

    private:
        void SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED);
        void SetupInfoText(const std::string &);
        int GetSelectNumber() const;

        //returns true of the select number is valid
        bool ProcessSelectNumber();

        void SetupCharacterSelectDetailText(const bool WILL_ERASE);
        void SetupCharacterSelectionRejectImage(const bool WILL_ERASE);
        void ItemProfileSetup();

        void SetupNumberSelectionPopup();
        void SetupCharacterSelectionPopup();
        void SetupImageSelectionPopup();
        void SetupImageFadePopup();
        void SetupCombatOverPopup();
        void SetupSystemErrorPopup();

    private:
        static const float IMAGE_SLIDER_SPEED_;
        static const int NUMBER_SELECT_INVALID_;
        static const float BEFORE_FADE_STARTS_DELAY_SEC_;
        
        //members that support multiple popups
        sfml_util::gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        
        //members that support the resoution change popup
        float elapsedTimeCounter_;
        std::size_t secondCounter_;
        
        //members supporting the image select sliderbar
        bool willSliderbarUpdate_;
        bool willTextBoxUpdate_;
        sf::Texture textureCurr_;
        sf::Texture texturePrev_;

        //number selection members
        sfml_util::gui::TextRegionUPtr_t infoTextRegionUPtr_;
        sfml_util::gui::TextEntryBoxSPtr_t textEntryBoxSPtr_;

        //members needed to animate image select
        bool                  isImageProcAllowed_;
        bool                  isInitialAnimation_;
        bool                  willShowImageCount_;
        sf::Sprite            imageSpriteCurr_;//these two sprites also used for fading
        sf::Sprite            imageSpritePrev_;
        bool                  areImagesMoving_;
        bool                  areImagesMovingLeft_;
        sf::FloatRect         imagesRect_;
        sfml_util::gui::TextRegionUPtr_t imageWrnTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t imageNumTextRegionUPtr_;
        std::size_t           imageIndex_;
        std::size_t           imageIndexLastSoundOn_;
        std::size_t           imageIndexLastSoundOff_;
        float                 imageCurrTargetScale_;
        float                 imagePrevStartScale_;
        float                 imagePrevStartPosX_;
        float                 imageCurrTravelDist_;
        float                 imagePrevTravelDist_;
        std::queue<std::size_t> imageMoveQueue_;
        sfml_util::sliders::ZeroSliderOnce<float> imageSlider_;
        float                 imagePosTop_;

        //members used to fade two images
        //imageSpritePrev_ and imageSpriteCurr_ are used to display the two fading images
        //POPUP_INFO_.Images() holds the two sf::Textures of the two fading images
        float beforeFadeTimerSec_;
        float fadeAlpha_;

        //members supporting CombatOver
        sf::Texture combatBgTexture_;
        sf::Sprite combatBgSprite_;
        sfml_util::gui::TextRegionUPtr_t titleUPtr_;
        sfml_util::gui::TextRegionUPtr_t descUPtr_;

        //members supporting ItemProfilePleaseWait
        int drawCountdown_;
    };

}
#endif //POPUP_POPUPSTAGE_HPP_INCLUDED
