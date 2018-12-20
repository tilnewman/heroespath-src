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
#include "gui/cached-texture.hpp"
#include "gui/sliders.hpp"
#include "gui/text-region.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-stage-base.hpp"

#include <queue>
#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Image Select Popup Stage.
    class PopupStageImageSelect : public PopupStageBase
    {
    public:
        explicit PopupStageImageSelect(const PopupInfo &);
        virtual ~PopupStageImageSelect();

        PopupStageImageSelect(const PopupStageImageSelect &) = delete;
        PopupStageImageSelect(PopupStageImageSelect &&) = delete;
        PopupStageImageSelect & operator=(const PopupStageImageSelect &) = delete;
        PopupStageImageSelect & operator=(PopupStageImageSelect &&) = delete;

        using PopupStageBase::HandleCallback;

        const std::string HandleCallback(
            const gui::SliderBar::Callback_t::Packet_t &,
            const std::string & PACKET_DESCRIPTION) override;

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    protected:
        bool HandleSelect() override;

    private:
        void SetupImagesRect();
        void SetupImagePosTop();
        void SetupSliderbar();
        void SetupWarnText();

        void SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED);
        void SetupSelectImage_SetupNewImageIndex(const std::size_t NEW_IMAGE_INDEX);
        void SetupSelectImage_SetupImageResources();
        void SetupSelectImage_SetupNumberText(const std::size_t IMAGE_INDEX);
        float SetupSelectImage_CalcTravelDistanceCurrent();
        float SetupSelectImage_CalcTravelDistancePrev();

        virtual void SetupContentForNewImage(const bool) {}

        bool KeyReleaseHandleNumbers(const sf::Event::KeyEvent &);
        bool KeyReleaseHandleLeft();
        bool KeyReleaseHandeRight();

        void EnqueueImagesFromCurrentToTarget(
            const std::size_t CURRENT_INDEX, const std::size_t TARGET_INDEX);

    private:
        static const float IMAGE_SLIDER_SPEED_;

        const std::size_t IMAGE_COUNT_;
        bool isChangingImageAllowed_;
        bool isInitialAnimComplete_;
        bool willShowImageCount_;
        sf::Sprite spriteCurr_;
        sf::Sprite spritePrev_;
        bool areImagesMoving_;
        bool areImagesMovingLeft_;
        gui::TextRegionUPtr_t imageWrnTextRegionUPtr_;
        gui::TextRegionUPtr_t imageNumTextRegionUPtr_;
        std::size_t imageIndexLastSoundOn_;
        std::size_t imageIndexLastSoundOff_;
        float targetScaleCurr_;
        float startScalePrev_;
        float startPosXPrev_;
        float travelDistCurr_;
        float travelDistPrev_;
        gui::SliderZeroToOne imageSlider_;
        float imagePosTop_;

    protected:
        sf::FloatRect imagesRect_;
        std::size_t imageIndex_;
        std::queue<std::size_t> imageMoveQueue_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEIMAGESELECT_HPP_INCLUDED
