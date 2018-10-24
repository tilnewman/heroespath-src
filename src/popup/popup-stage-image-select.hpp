// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
//
// popup-stage-image-select.hpp
//
#include "popup/popup-info.hpp"
#include "popup/popup-stage-base.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/text-region.hpp"

#include <queue>
#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Image Select Popup Stage.
    class PopupStageImageSelect : public PopupStageBase
    {
        PopupStageImageSelect(const PopupStageImageSelect &);
        PopupStageImageSelect & operator=(const PopupStageImageSelect &);

    public:
        explicit PopupStageImageSelect(const PopupInfo &);
        virtual ~PopupStageImageSelect();

        using PopupStageBase::HandleCallback;

        bool HandleCallback(const sfml_util::SliderBar::Callback_t::PacketPtr_t &) override;

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
        virtual void SetupContent(const bool) {}
        virtual const sfml_util::CachedTexture &
            GetCurrentCachedTexture(const std::size_t IMAGE_INDEX);

        bool KeyReleaseHandleNumbers(const sf::Event::KeyEvent &);
        bool KeyReleaseHandleLeft();
        bool KeyReleaseHandeRight();

        void SetupSliderbar() override;

    protected:
        void EnqueueImagesFromCurrentToTarget(
            const std::size_t CURRENT_INDEX, const std::size_t TARGET_INDEX);

        bool HandleSelect() override;

    protected:
        static const float IMAGE_SLIDER_SPEED_;

        bool isChangingImageAllowed_;
        bool isInitialAnimComplete_;
        bool willShowImageCount_;
        sfml_util::CachedTextureOpt_t cachedTexturePrevOpt_;
        sf::Sprite spriteCurr_;
        sf::Sprite spritePrev_;
        bool areImagesMoving_;
        bool areImagesMovingLeft_;
        sf::FloatRect imagesRect_;
        sfml_util::TextRegionUPtr_t imageWrnTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t imageNumTextRegionUPtr_;
        std::size_t imageIndex_;
        std::size_t imageIndexLastSoundOn_;
        std::size_t imageIndexLastSoundOff_;
        float targetScaleCurr_;
        float startScalePrev_;
        float startPosXPrev_;
        float travelDistCurr_;
        float travelDistPrev_;
        std::queue<std::size_t> imageMoveQueue_;
        sfml_util::SliderZeroToOne imageSlider_;
        float imagePosTop_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
