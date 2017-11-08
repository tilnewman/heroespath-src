// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// popup-stage-image-select.cpp
//
#include "popup-stage-image-select.hpp"

#include "game/loop-manager.hpp"

#include "sfml-util/sound-manager.hpp"

#include <algorithm>


namespace heroespath
{
namespace popup
{

    const float PopupStageImageSelect::IMAGE_SLIDER_SPEED_{ 4.0f };


    PopupStageImageSelect::PopupStageImageSelect(const PopupInfo & POPUP_INFO)
    :
        PopupStageBase(POPUP_INFO),
        isChangingImageAllowed_(false),
        isInitialAnimComplete_(false),
        willShowImageCount_(false),
        textureCurr_(),
        texturePrev_(),
        spriteCurr_(),
        spritePrev_(),
        areImagesMoving_(false),
        areImagesMovingLeft_(false),
        imagesRect_(),
        imageWrnTextRegionUPtr_(),
        imageNumTextRegionUPtr_(),
        imageIndex_(0),
        imageIndexLastSoundOn_(0),
        imageIndexLastSoundOff_(0),
        targetScaleCurr_(1.0f),
        startScalePrev_(1.0f),
        startPosXPrev_(0.0f),
        travelDistCurr_(0.0f),
        travelDistPrev_(0.0f),
        imageMoveQueue_(),
        imageSlider_(IMAGE_SLIDER_SPEED_),
        imagePosTop_(0.0f)
    {}


    PopupStageImageSelect::~PopupStageImageSelect()
    {}


    bool PopupStageImageSelect::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
    {
        if (isChangingImageAllowed_)
        {
            auto const COUNT_MAX{ CountMax() };
            auto index{ static_cast<std::size_t>(
                sliderbarUPtr_->GetCurrentValue() * static_cast<float>(COUNT_MAX)) };

            if (index >= COUNT_MAX)
            {
                index = COUNT_MAX - 1;
            }

            imageMoveQueue_.push(index);
        }

        return true;
    }


    void PopupStageImageSelect::Setup()
    {
        PopupStageBase::Setup();

        imagesRect_ = textRegion_;

        //added is a pad so the text does not touch the images
        imagesRect_.top = textRegionUPtr_->GetEntityPos().y +
            sfml_util::MapByRes(70.0f, 200.0f);

        imagesRect_.height =
            (sliderbarPosTop_ - (ButtonTextHeight() * 2.0f)) - imagesRect_.top;

        imagePosTop_ = (imagesRect_.top + (imagesRect_.height * 0.5f));

        isChangingImageAllowed_ = (popupInfo_.ImagesCount() != 0);

        imageMoveQueue_.push(0);
        EnqueueImagesFromCurrentToTarget(imageIndex_, popupInfo_.InitialSelection());

        if (popupInfo_.ImagesCount() == 1)
        {
            const sfml_util::gui::TextInfo TEXT_INFO("(there is only one image)",
                sfml_util::FontManager::Instance()->Font_Default1(),
                sfml_util::FontManager::Instance()->Size_Small(),
                PopupManager::Color_Font(),
                sfml_util::Justified::Center);

            sf::FloatRect region(textRegion_);
            region.top = sliderbarPosTop_;
            region.height = 0.0f;
            imageWrnTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "PopupStage'sImageSelectionion", TEXT_INFO, region);

            EntityAdd(imageWrnTextRegionUPtr_.get());
            if (sliderbarUPtr_.get() != nullptr)
            {
                EntityRemove(sliderbarUPtr_.get());
            }
        }
    }


    void PopupStageImageSelect::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        target.draw(spriteCurr_, STATES);

        if (areImagesMoving_ && (isInitialAnimComplete_))
        {
            target.draw(spritePrev_, STATES);
        }

        if (willShowImageCount_ && (imageNumTextRegionUPtr_.get() != nullptr))
        {
            imageNumTextRegionUPtr_->draw(target, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void PopupStageImageSelect::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (areImagesMoving_ || (imageMoveQueue_.empty() == false))
        {
            auto currRatio{ 0.0f };

            if (areImagesMoving_)
            {
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);
                areImagesMoving_ = ! imageSlider_.IsDone();

                if (false == areImagesMoving_)
                {
                    SetupContent(false);
                    willShowImageCount_ = true;

                    if (imageIndexLastSoundOff_ != imageIndex_)
                    {
                        sfml_util::SoundManager::Instance()->PlaySfx_TickOff();
                    }

                    imageIndexLastSoundOff_ = imageIndex_;
                }

                if (false == isInitialAnimComplete_)
                {
                    isInitialAnimComplete_ = ! areImagesMoving_;
                }
            }

            if ((false == areImagesMoving_) && (imageMoveQueue_.empty() == false))
            {
                areImagesMoving_ = true;
                willShowImageCount_ = false;

                if (imageIndexLastSoundOn_ != imageMoveQueue_.front())
                {
                    sfml_util::SoundManager::Instance()->PlaySfx_TickOn();
                }

                imageIndexLastSoundOn_ = imageMoveQueue_.front();

                SetupSelectImage(
                    imageMoveQueue_.front(),
                    IMAGE_SLIDER_SPEED_ + ((static_cast<float>(imageMoveQueue_.size()) * 0.25f)) );

                imageMoveQueue_.pop();
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);

                SetupContent(true);
            }

            auto const CURR_SCALE{ targetScaleCurr_ * currRatio };
            spriteCurr_.setScale(CURR_SCALE, CURR_SCALE);

            auto const PREV_SCALE{ startScalePrev_ * (1.0f - currRatio) };
            spritePrev_.setScale(PREV_SCALE, PREV_SCALE);

            auto const CURR_POS_TOP{
                imagePosTop_ - (spriteCurr_.getGlobalBounds().height * 0.5f) };

            auto const PREV_POS_TOP{
                imagePosTop_ - (spritePrev_.getGlobalBounds().height * 0.5f) };

            if (willShowImageCount_)
            {
                imageNumTextRegionUPtr_->SetEntityPos(imageNumTextRegionUPtr_->GetEntityPos().x,
                    CURR_POS_TOP - imageNumTextRegionUPtr_->GetEntityRegion().height);
            }

            if (areImagesMovingLeft_)
            {
                auto const CURR_POS_LEFT{ (imagesRect_.left + imagesRect_.width) -
                    (currRatio * travelDistCurr_) };

                spriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                auto const PREV_POS_LEFT{ startPosXPrev_ - (currRatio * travelDistPrev_) };
                spritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
            else
            {
                auto const CURR_POS_LEFT{ imagesRect_.left + (currRatio * travelDistCurr_) };
                spriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                auto const PREV_POS_LEFT{ startPosXPrev_ + (currRatio * travelDistPrev_) };
                spritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
        }
    }


    bool PopupStageImageSelect::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code >= sf::Keyboard::Num1) &&
            (KEY_EVENT.code <= sf::Keyboard::Num9))
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
        const std::size_t NEW_IMAGE_INDEX_PARAM,
        const float SLIDER_SPEED)
    {
        if (false == isChangingImageAllowed_)
        {
            return;
        }

        auto const NEW_IMAGE_INDEX_TO_USE{ [&]()
            {
                auto const COUNT_MAX{ CountMax() };

                if (NEW_IMAGE_INDEX_PARAM >= COUNT_MAX)
                {
                    return COUNT_MAX - 1;
                }
                else
                {
                    return NEW_IMAGE_INDEX_PARAM;
                }
            }() };

        if ((NEW_IMAGE_INDEX_TO_USE != imageIndex_) || (isInitialAnimComplete_ == false))
        {
            imageSlider_.Reset(SLIDER_SPEED);

            SetupSelectImage_SetupNewImageIndex(NEW_IMAGE_INDEX_TO_USE);
            SetupSelectImage_SetupImageResources();
            SetupSelectImage_SetupNumberText(imageIndex_);

            startScalePrev_ = spritePrev_.getScale().x;
            startPosXPrev_ = spritePrev_.getPosition().x;

            travelDistCurr_ = SetupSelectImage_CalcTravelDistanceCurrent();
            travelDistPrev_ = SetupSelectImage_CalcTravelDistancePrev();
        }
    }


    void PopupStageImageSelect::SetupSelectImage_SetupNewImageIndex(const std::size_t IMAGE_INDEX_NEW)
    {
        areImagesMovingLeft_ = (imageIndex_ < IMAGE_INDEX_NEW);
        imageIndex_ = IMAGE_INDEX_NEW;
        selection_ = static_cast<int>(imageIndex_);
    }


    void PopupStageImageSelect::SetupSelectImage_SetupImageResources()
    {
        spritePrev_ = spriteCurr_;
        texturePrev_ = textureCurr_;
        spritePrev_.setTexture(texturePrev_, true);

        SetCurrentTexture(imageIndex_);

        if (popupInfo_.AreImagesCreatures())
        {
            sfml_util::Invert(textureCurr_);
            sfml_util::Mask(textureCurr_, sf::Color::White);
        }

        spriteCurr_.setTexture(textureCurr_, true);
        spriteCurr_.setScale(1.0f, 1.0f);

        auto const POS_LEFT{
            textRegion_.left + ((areImagesMovingLeft_) ? textRegion_.width : 0.0f) };

        auto const POS_TOP{ imagePosTop_ };

        spriteCurr_.setPosition(POS_LEFT, POS_TOP);

        auto const RESIZE_RATIO{ 0.8f };
        auto const SPRITE_TARGET_WIDTH{ imagesRect_.width * RESIZE_RATIO };
        auto const SPRITE_TARGET_HEIGHT{ imagesRect_.height * RESIZE_RATIO };

        auto const SCALE_HORIZ{
            SPRITE_TARGET_WIDTH / spriteCurr_.getGlobalBounds().width };

        auto const SCALE_VERT{
            SPRITE_TARGET_HEIGHT / spriteCurr_.getGlobalBounds().height };

        targetScaleCurr_ = std::min(SCALE_HORIZ, SCALE_VERT);
        spriteCurr_.setScale(0.0f, 0.0f);
    }


    void PopupStageImageSelect::SetupSelectImage_SetupNumberText(const std::size_t IMAGE_INDEX)
    {
        std::ostringstream ss;
        ss << IMAGE_INDEX + 1 << "/" << CountMax();

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            PopupManager::Color_Font(),
            sfml_util::Justified::Center);

        sf::FloatRect imageCountTextRect(textRegion_);
        imageCountTextRect.height = 0.0f;

        imageNumTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStage'sSelectNumber", TEXT_INFO, imageCountTextRect);
    }


    float PopupStageImageSelect::SetupSelectImage_CalcTravelDistanceCurrent()
    {
        sf::Sprite tempSprite(spriteCurr_);
        tempSprite.setScale(targetScaleCurr_, targetScaleCurr_);

        auto const POS_LEFT_CENTERED{
            (textRegion_.left + (textRegion_.width * 0.5f)) -
            (tempSprite.getGlobalBounds().width * 0.5f) };

        if (areImagesMovingLeft_)
        {
            return (textRegion_.left + textRegion_.width) - POS_LEFT_CENTERED;
        }
        else
        {
            return POS_LEFT_CENTERED - textRegion_.left;
        }
    }


    float PopupStageImageSelect::SetupSelectImage_CalcTravelDistancePrev()
    {
        if (areImagesMovingLeft_)
        {
            return spritePrev_.getGlobalBounds().left - imagesRect_.left;
        }
        else
        {
            return (imagesRect_.left + imagesRect_.width) - spritePrev_.getGlobalBounds().left;
        }
    }


    std::size_t PopupStageImageSelect::CountMax() const
    {
        return popupInfo_.ImagesCount();
    }


    void PopupStageImageSelect::SetCurrentTexture(const std::size_t IMAGE_INDEX)
    {
        textureCurr_ = popupInfo_.ImagesAt(IMAGE_INDEX);
    }


    bool PopupStageImageSelect::KeyReleaseHandleNumbers(const sf::Event::KeyEvent & KEY_EVENT)
    {
        std::size_t targetIndex{ 0 };

        if (KEY_EVENT.code == sf::Keyboard::Num2)      targetIndex = 1;
        else if (KEY_EVENT.code == sf::Keyboard::Num3) targetIndex = 2;
        else if (KEY_EVENT.code == sf::Keyboard::Num4) targetIndex = 3;
        else if (KEY_EVENT.code == sf::Keyboard::Num5) targetIndex = 4;
        else if (KEY_EVENT.code == sf::Keyboard::Num6) targetIndex = 5;
        else if (KEY_EVENT.code == sf::Keyboard::Num7) targetIndex = 6;
        else if (KEY_EVENT.code == sf::Keyboard::Num8) targetIndex = 7;
        else if (KEY_EVENT.code == sf::Keyboard::Num9) targetIndex = 8;

        auto const COUNT_MAX{ CountMax() };
        auto const ONE_OVER_COUNT{ 1.0f / static_cast<float>(COUNT_MAX) };

        if ((imageIndex_ == targetIndex) ||
            (targetIndex > (COUNT_MAX - 1)))
        {
            return false;
        }

        isChangingImageAllowed_ = false;
        sliderbarUPtr_->SetCurrentValue(static_cast<float>(targetIndex) * ONE_OVER_COUNT);
        isChangingImageAllowed_ = true;

        EnqueueImagesFromCurrentToTarget(imageIndex_, targetIndex);
        return true;
    }


    bool PopupStageImageSelect::KeyReleaseHandleLeft()
    {
        if (imageIndex_ > 0)
        {
            auto const NEW_INDEX{ imageIndex_ - 1 };
            isChangingImageAllowed_ = false;

            sliderbarUPtr_->SetCurrentValue(
                static_cast<float>(NEW_INDEX) / static_cast<float>(CountMax()) );

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
        auto const COUNT_MAX{ CountMax() };

        if (imageIndex_ < (COUNT_MAX - 1))
        {
            auto const NEW_INDEX{ imageIndex_ + 1 };

            isChangingImageAllowed_ = false;

            sliderbarUPtr_->SetCurrentValue(
                static_cast<float>(NEW_INDEX) / static_cast<float>(COUNT_MAX));

            isChangingImageAllowed_ = true;

            //if already at the end, then make sure the sliderbar is
            //all the way to the right
            if (NEW_INDEX >= (COUNT_MAX - 1))
            {
                //prevent processing and adding to the imageMoveQueue_ or calling
                //SetupSelectImage() when setting the sliderbar value here.
                isChangingImageAllowed_ = false;
                sliderbarUPtr_->SetCurrentValue(1.0f);
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
        PopupStageBase::SetupSliderbar();

        auto const SLIDERBAR_LENGTH{ textRegion_.width * 0.75f };//found by experiment

        auto const SLIDERBAR_POS_LEFT{
            textRegion_.left + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f) };

        sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz),
            this);

        EntityAdd(sliderbarUPtr_.get());
    }


    void PopupStageImageSelect::EnqueueImagesFromCurrentToTarget(
        const std::size_t CURRENT_INDEX,
        const std::size_t TARGET_INDEX)
    {
        std::size_t i{ CURRENT_INDEX };

        while (i != TARGET_INDEX)
        {
            if (TARGET_INDEX < CURRENT_INDEX)
            {
                imageMoveQueue_.push(--i);
            }
            else
            {
                imageMoveQueue_.push(++i);
            }
        }
    }


    bool PopupStageImageSelect::HandleSelect()
    {
        //attempt to select the value most recently enqueued
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

}
}
