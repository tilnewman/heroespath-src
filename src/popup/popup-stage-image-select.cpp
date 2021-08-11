// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-image-select.cpp
//
#include "popup-stage-image-select.hpp"

#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/sound-manager.hpp"
#include "popup/popup-manager.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/primitives.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace heroespath
{
namespace popup
{

    const float PopupStageImageSelect::IMAGE_SLIDER_SPEED_ { 4.0f };

    PopupStageImageSelect::PopupStageImageSelect(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , IMAGE_COUNT_(POPUP_INFO.ImagesCount())
        , isChangingImageAllowed_(false)
        , isInitialAnimComplete_(false)
        , willShowImageCount_(false)
        , spriteCurr_()
        , spritePrev_()
        , areImagesMoving_(false)
        , areImagesMovingLeft_(false)
        , imageWrnTextRegionUPtr_()
        , imageNumTextRegionUPtr_()
        , imageIndexLastSoundOn_(0)
        , imageIndexLastSoundOff_(0)
        , targetScaleCurr_(1.0f)
        , startScalePrev_(1.0f)
        , startPosXPrev_(0.0f)
        , travelDistCurr_(0.0f)
        , travelDistPrev_(0.0f)
        , imageSlider_(IMAGE_SLIDER_SPEED_)
        , imagePosTop_(0.0f)
        , imagesRect_()
        , imageIndex_(0)
        , imageMoveQueue_()
    {}

    PopupStageImageSelect::~PopupStageImageSelect() = default;

    const std::string PopupStageImageSelect::HandleCallback(
        const gui::SliderBar::Callback_t::Packet_t &, const std::string &)
    {
        if (isChangingImageAllowed_)
        {
            auto index { static_cast<std::size_t>(
                sliderbarUPtr_->PositionRatio() * static_cast<float>(IMAGE_COUNT_)) };

            if (index >= IMAGE_COUNT_)
            {
                index = (IMAGE_COUNT_ - 1);
            }

            imageMoveQueue_.push(index);
        }

        return "";
    }

    void PopupStageImageSelect::Setup()
    {
        PopupStageBase::Setup();

        SetupImagesRect();
        SetupImagePosTop();
        SetupSliderbar();

        isChangingImageAllowed_ = (IMAGE_COUNT_ > 0);

        imageIndex_ = 0;
        imageMoveQueue_.push(imageIndex_);
        EnqueueImagesFromCurrentToTarget(imageIndex_, popupInfo_.InitialSelection());

        if (IMAGE_COUNT_ == 1)
        {
            SetupWarnText();

            if (sliderbarUPtr_)
            {
                EntityRemove(sliderbarUPtr_);
            }
        }
    }

    void PopupStageImageSelect::SetupWarnText()
    {
        const gui::TextInfo TEXT_INFO(
            "(there is only one image)",
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            gui::Justified::Center);

        sf::FloatRect region { ContentRegion() };
        region.top = sliderbarUPtr_->GetEntityPos().y;
        region.height = 0.0f;

        imageWrnTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            GetStageName() + "'sImageSelectionion", TEXT_INFO, region);

        EntityAdd(imageWrnTextRegionUPtr_);
    }

    void PopupStageImageSelect::SetupImagesRect()
    {
        imagesRect_ = ContentRegion();
        imagesRect_.height = (ContentRegion().height * 0.5f);
        imagesRect_.top = sfutil::CenterOfVert(ContentRegion()) - (imagesRect_.height * 0.5f);
    }

    void PopupStageImageSelect::SetupImagePosTop()
    {
        imagePosTop_ = sfutil::CenterOfVert(imagesRect_);
    }

    void PopupStageImageSelect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);

        target.draw(spriteCurr_, states);

        if (areImagesMoving_ && isInitialAnimComplete_)
        {
            target.draw(spritePrev_, states);
        }

        if (willShowImageCount_ && imageNumTextRegionUPtr_)
        {
            imageNumTextRegionUPtr_->draw(target, states);
        }

        StageBase::draw(target, states);
    }

    void PopupStageImageSelect::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        if (areImagesMoving_ || (imageMoveQueue_.empty() == false))
        {
            auto currRatio { 0.0f };

            if (areImagesMoving_)
            {
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);
                areImagesMoving_ = !imageSlider_.IsStopped();

                if (false == areImagesMoving_)
                {
                    SetupContentForNewImage(false);
                    willShowImageCount_ = true;

                    if (imageIndexLastSoundOff_ != imageIndex_)
                    {
                        gui::SoundManager::Instance()->PlaySfx_TickOff();
                    }

                    imageIndexLastSoundOff_ = imageIndex_;
                }

                if (false == isInitialAnimComplete_)
                {
                    isInitialAnimComplete_ = !areImagesMoving_;
                }
            }

            if ((false == areImagesMoving_) && (imageMoveQueue_.empty() == false))
            {
                areImagesMoving_ = true;
                willShowImageCount_ = false;

                if (imageIndexLastSoundOn_ != imageMoveQueue_.front())
                {
                    gui::SoundManager::Instance()->PlaySfx_TickOn();
                }

                imageIndexLastSoundOn_ = imageMoveQueue_.front();

                SetupSelectImage(
                    imageMoveQueue_.front(),
                    IMAGE_SLIDER_SPEED_ + ((static_cast<float>(imageMoveQueue_.size()) * 0.25f)));

                imageMoveQueue_.pop();
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);

                SetupContentForNewImage(true);
            }

            const auto CURR_SCALE { targetScaleCurr_ * currRatio };
            spriteCurr_.setScale(CURR_SCALE, CURR_SCALE);

            const auto PREV_SCALE { startScalePrev_ * (1.0f - currRatio) };
            spritePrev_.setScale(PREV_SCALE, PREV_SCALE);

            const auto CURR_POS_TOP { imagePosTop_
                                      - (spriteCurr_.getGlobalBounds().height * 0.5f) };

            const auto PREV_POS_TOP { imagePosTop_
                                      - (spritePrev_.getGlobalBounds().height * 0.5f) };

            if (areImagesMovingLeft_)
            {
                const auto CURR_POS_LEFT { sfutil::Right(imagesRect_)
                                           - (currRatio * travelDistCurr_) };

                spriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const auto PREV_POS_LEFT { startPosXPrev_ - (currRatio * travelDistPrev_) };
                spritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
            else
            {
                const auto CURR_POS_LEFT { imagesRect_.left + (currRatio * travelDistCurr_) };
                spriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const auto PREV_POS_LEFT { startPosXPrev_ + (currRatio * travelDistPrev_) };
                spritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
        }
    }

    bool PopupStageImageSelect::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code >= sf::Keyboard::Num1) && (KEY_EVENT.code <= sf::Keyboard::Num9))
        {
            PlayValidKeypressSoundEffect();
            return KeyReleaseHandleNumbers(KEY_EVENT);
        }
        else if (KEY_EVENT.code == sf::Keyboard::Left)
        {
            PlayValidKeypressSoundEffect();
            return KeyReleaseHandleLeft();
        }
        else if (KEY_EVENT.code == sf::Keyboard::Right)
        {
            PlayValidKeypressSoundEffect();
            return KeyReleaseHandeRight();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }

    void PopupStageImageSelect::SetupSelectImage(
        const std::size_t NEW_IMAGE_INDEX_PARAM, const float SLIDER_SPEED)
    {
        if (false == isChangingImageAllowed_)
        {
            return;
        }

        const auto NEW_IMAGE_INDEX_TO_USE { [&]() {
            if (NEW_IMAGE_INDEX_PARAM >= IMAGE_COUNT_)
            {
                return (IMAGE_COUNT_ - 1);
            }
            else
            {
                return NEW_IMAGE_INDEX_PARAM;
            }
        }() };

        if ((NEW_IMAGE_INDEX_TO_USE != imageIndex_) || (isInitialAnimComplete_ == false))
        {
            imageSlider_ = gui::SliderZeroToOne(SLIDER_SPEED);

            SetupSelectImage_SetupNewImageIndex(NEW_IMAGE_INDEX_TO_USE);
            SetupSelectImage_SetupImageResources();
            SetupSelectImage_SetupNumberText(imageIndex_);

            startScalePrev_ = spritePrev_.getScale().x;
            startPosXPrev_ = spritePrev_.getPosition().x;

            travelDistCurr_ = SetupSelectImage_CalcTravelDistanceCurrent();
            travelDistPrev_ = SetupSelectImage_CalcTravelDistancePrev();
        }
    }

    void PopupStageImageSelect::SetupSelectImage_SetupNewImageIndex(
        const std::size_t IMAGE_INDEX_NEW)
    {
        areImagesMovingLeft_ = (imageIndex_ < IMAGE_INDEX_NEW);
        imageIndex_ = IMAGE_INDEX_NEW;
        selection_ = static_cast<int>(imageIndex_);
    }

    void PopupStageImageSelect::SetupSelectImage_SetupImageResources()
    {
        if (spriteCurr_.getTexture() != nullptr)
        {
            spritePrev_.setTexture(*spriteCurr_.getTexture(), true);
        }

        spriteCurr_.setTexture(popupInfo_.ImagesAt(imageIndex_).Get(), true);
        spriteCurr_.setScale(1.0f, 1.0f);

        const auto POS_LEFT { (
            (areImagesMovingLeft_) ? sfutil::Right(imagesRect_) : sfutil::Left(imagesRect_)) };

        const auto POS_TOP { imagePosTop_ };

        spriteCurr_.setPosition(POS_LEFT, POS_TOP);

        // shrink images down a bit so they do not touch the top or bottom of imagesRect_ which
        // would touch other things like text and the sliderbar
        const auto SHRINK_RATIO { 0.8f };
        const auto TARGET_HEIGHT { (imagesRect_.height * SHRINK_RATIO) };
        const auto SCALE { (TARGET_HEIGHT / spriteCurr_.getGlobalBounds().height) };
        targetScaleCurr_ = SCALE;

        spriteCurr_.setScale(0.0f, 0.0f);
    }

    void PopupStageImageSelect::SetupSelectImage_SetupNumberText(const std::size_t IMAGE_INDEX)
    {
        std::ostringstream ss;
        ss << IMAGE_INDEX + 1 << "/" << IMAGE_COUNT_;

        const gui::TextInfo TEXT_INFO(
            ss.str(),
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Smallish(),
            PopupManager::Color_Font(),
            gui::Justified::Center);

        imageNumTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            GetStageName() + "'sSelectNumber", TEXT_INFO, imagesRect_);

        imageNumTextRegionUPtr_->MoveEntityPos(
            0.0f, (imageNumTextRegionUPtr_->ActualTextRegion().height * -1.0f));
    }

    float PopupStageImageSelect::SetupSelectImage_CalcTravelDistanceCurrent()
    {
        // after call to SetupSelectImage_SetupImageResources() above the spriteCurr_ is not at
        // targetScaleCurr_, so this code predicts what it's width will be once it reaches that
        // target scale
        const auto CURR_SPRITE_WIDTH_AT_TARGET_SCALE { (
            spriteCurr_.getLocalBounds().width * targetScaleCurr_) };

        const auto POS_LEFT_CENTERED { (
            sfutil::CenterOfHoriz(imagesRect_) - (CURR_SPRITE_WIDTH_AT_TARGET_SCALE * 0.5f)) };

        if (areImagesMovingLeft_)
        {
            return (sfutil::Right(imagesRect_) - POS_LEFT_CENTERED);
        }
        else
        {
            return (POS_LEFT_CENTERED - imagesRect_.left);
        }
    }

    float PopupStageImageSelect::SetupSelectImage_CalcTravelDistancePrev()
    {
        if (areImagesMovingLeft_)
        {
            return (spritePrev_.getGlobalBounds().left - imagesRect_.left);
        }
        else
        {
            return (sfutil::Right(imagesRect_) - spritePrev_.getGlobalBounds().left);
        }
    }

    bool PopupStageImageSelect::KeyReleaseHandleNumbers(const sf::Event::KeyEvent & KEY_EVENT)
    {
        std::size_t targetIndex { 0 };

        if (KEY_EVENT.code == sf::Keyboard::Num2)
        {
            targetIndex = 1;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num3)
        {
            targetIndex = 2;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num4)
        {
            targetIndex = 3;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num5)
        {
            targetIndex = 4;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num6)
        {
            targetIndex = 5;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num7)
        {
            targetIndex = 6;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num8)
        {
            targetIndex = 7;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Num9)
        {
            targetIndex = 8;
        }

        const auto ONE_OVER_COUNT { 1.0f / static_cast<float>(IMAGE_COUNT_) };

        if ((imageIndex_ == targetIndex) || (targetIndex > (IMAGE_COUNT_ - 1)))
        {
            return false;
        }

        isChangingImageAllowed_ = false;
        sliderbarUPtr_->PositionRatio(static_cast<float>(targetIndex) * ONE_OVER_COUNT);
        isChangingImageAllowed_ = true;

        EnqueueImagesFromCurrentToTarget(imageIndex_, targetIndex);
        return true;
    }

    bool PopupStageImageSelect::KeyReleaseHandleLeft()
    {
        if (imageIndex_ > 0)
        {
            const auto NEW_INDEX { imageIndex_ - 1 };
            isChangingImageAllowed_ = false;

            sliderbarUPtr_->PositionRatio(
                static_cast<float>(NEW_INDEX) / static_cast<float>(IMAGE_COUNT_));

            isChangingImageAllowed_ = true;
            imageMoveQueue_.push(NEW_INDEX);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool PopupStageImageSelect::KeyReleaseHandeRight()
    {
        if (imageIndex_ < (IMAGE_COUNT_ - 1))
        {
            const auto NEW_INDEX { imageIndex_ + 1 };

            isChangingImageAllowed_ = false;

            sliderbarUPtr_->PositionRatio(
                static_cast<float>(NEW_INDEX) / static_cast<float>(IMAGE_COUNT_));

            isChangingImageAllowed_ = true;

            // if already at the end, then make sure the sliderbar is
            // all the way to the right
            if (NEW_INDEX >= (IMAGE_COUNT_ - 1))
            {
                // prevent processing and adding to the imageMoveQueue_ or calling
                // SetupSelectImage() when setting the sliderbar value here.
                isChangingImageAllowed_ = false;
                sliderbarUPtr_->PositionRatio(1.0f);
                isChangingImageAllowed_ = true;
            }

            imageMoveQueue_.push(NEW_INDEX);
            return true;
        }
        else
        {
            return false;
        }
    }

    void PopupStageImageSelect::SetupSliderbar()
    {
        const auto MOVE_RATIO_MIN { (
            (IMAGE_COUNT_ >= 2) ? (1.0f / static_cast<float>(IMAGE_COUNT_)) : 0.0f) };

        const auto SLIDERBAR_LENGTH { (ContentRegion().width * 0.75f) };

        sliderbarUPtr_ = std::make_unique<gui::SliderBar>(
            GetStageName() + "'s",
            0.0f,
            0.0f,
            SLIDERBAR_LENGTH,
            gui::SliderStyle(gui::Orientation::Horiz),
            gui::SliderBar::Callback_t::IHandlerPtr_t(this),
            0.0f,
            false,
            MOVE_RATIO_MIN);

        const auto POS_LEFT { sfutil::DisplayCenterHoriz(sliderbarUPtr_->GetEntityRegion().width) };

        const auto POS_TOP { sfutil::Bottom(imagesRect_)
                             + ((sfutil::Bottom(ContentRegion()) - sfutil::Bottom(imagesRect_))
                                * 0.5f) };

        sliderbarUPtr_->SetEntityPos(POS_LEFT, POS_TOP);

        sliderbarUPtr_->WillPlaySfx(false);

        EntityAdd(sliderbarUPtr_);
    }

    void PopupStageImageSelect::EnqueueImagesFromCurrentToTarget(
        const std::size_t CURRENT_INDEX, const std::size_t TARGET_INDEX)
    {
        std::size_t index { CURRENT_INDEX };

        while (index != TARGET_INDEX)
        {
            if (TARGET_INDEX < CURRENT_INDEX)
            {
                imageMoveQueue_.push(--index);
            }
            else
            {
                imageMoveQueue_.push(++index);
            }
        }
    }

    bool PopupStageImageSelect::HandleSelect()
    {
        // attempt to select the value most recently enqueued
        if (imageMoveQueue_.empty() == false)
        {
            selection_ = static_cast<int>(imageMoveQueue_.back());
        }
        else
        {
            selection_ = static_cast<int>(imageIndex_);
        }

        return PopupStageBase::HandleSelect();
    }

} // namespace popup
} // namespace heroespath
