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
#ifndef POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
#define POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
//
// popup-stage-image-select.hpp
//
#include "popup/popup-stage-base.hpp"
#include "popup/popup-info.hpp"

#include "sfml-util/sliders.hpp"
#include "sfml-util/gui/text-region.hpp"

#include <queue>
#include <string>


namespace popup
{

    //Responsible for implementing the Image Select Popup Stage.
    class PopupStageImageSelect : public PopupStageBase
    {
        PopupStageImageSelect(const PopupStageImageSelect &);
        PopupStageImageSelect & operator=(const PopupStageImageSelect &);

    public:
        explicit PopupStageImageSelect(const PopupInfo &);
        virtual ~PopupStageImageSelect();

        using PopupStageBase::HandlerName;
        using PopupStageBase::HandleCallback;

        bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED);
        void SetupSelectImage_SetupNewImageIndex(const std::size_t NEW_IMAGE_INDEX);
        void SetupSelectImage_SetupImageResources();
        void SetupSelectImage_SetupNumberText(const std::size_t IMAGE_INDEX);
        float SetupSelectImage_CalcTravelDistanceCurrent();
        float SetupSelectImage_CalcTravelDistancePrev();

        virtual std::size_t CountMax() const;
        inline virtual void SetupContent(const bool) {}
        inline virtual bool HandleNumberKeys(const sf::Event::KeyEvent &) { return false; }
        inline virtual void InvalidateSelectionIfCurrentIsInvalid() {}
        virtual void SetCurrentTexture(const std::size_t IMAGE_INDEX);

        bool KeyReleaseHandleNumbers(const sf::Event::KeyEvent &);
        bool KeyReleaseHandleLeft();
        bool KeyReleaseHandeRight();

        void SetupSliderbar() override;

    protected:
        void EnqueueImagesFromCurrentToTarget(
            const std::size_t CURRENT_INDEX,
            const std::size_t TARGET_INDEX);

    protected:
        static const float IMAGE_SLIDER_SPEED_;

        bool isChangingImageAllowed_;
        bool isInitialAnimComplete_;
        bool willShowImageCount_;
        sf::Texture textureCurr_;
        sf::Texture texturePrev_;
        sf::Sprite spriteCurr_;
        sf::Sprite spritePrev_;
        bool areImagesMoving_;
        bool areImagesMovingLeft_;
        sf::FloatRect imagesRect_;
        sfml_util::gui::TextRegionUPtr_t imageWrnTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t imageNumTextRegionUPtr_;
        std::size_t imageIndex_;
        std::size_t imageIndexLastSoundOn_;
        std::size_t imageIndexLastSoundOff_;
        float targetScaleCurr_;
        float startScalePrev_;
        float startPosXPrev_;
        float travelDistCurr_;
        float travelDistPrev_;
        std::queue<std::size_t> imageMoveQueue_;
        sfml_util::sliders::ZeroSliderOnce<float> imageSlider_;
        float imagePosTop_;
    };

}

#endif //POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
