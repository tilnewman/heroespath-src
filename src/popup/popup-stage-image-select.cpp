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


namespace popup
{

    const float PopupStageImageSelect::IMAGE_SLIDER_SPEED_{ 4.0f };


    PopupStageImageSelect::PopupStageImageSelect(const PopupInfo & POPUP_INFO)
    :
        PopupStageBase(POPUP_INFO),
        isImageProcAllowed_(false),
        isInitialAnimation_(true),
        willShowImageCount_(false),
        textureCurr_(),
        texturePrev_(),
        imageSpriteCurr_(),
        imageSpritePrev_(),
        areImagesMoving_(false),
        areImagesMovingLeft_(false),
        imagesRect_(),
        imageWrnTextRegionUPtr_(),
        imageNumTextRegionUPtr_(),
        imageIndex_(0),
        imageIndexLastSoundOn_(0),
        imageIndexLastSoundOff_(0),
        imageCurrTargetScale_(1.0f),
        imagePrevStartScale_(1.0f),
        imagePrevStartPosX_(0.0f),
        imageCurrTravelDist_(0.0f),
        imagePrevTravelDist_(0.0f),
        imageMoveQueue_(),
        imageSlider_(IMAGE_SLIDER_SPEED_),
        imagePosTop_(0.0f)
    {}


    PopupStageImageSelect::~PopupStageImageSelect()
    {}


    bool PopupStageImageSelect::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
    {
        if (isImageProcAllowed_)
        {
            auto const COUNT_MAX{ CountMax() };
            auto index{ static_cast<std::size_t>(sliderbarUPtr_->GetCurrentValue() * COUNT_MAX) };

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

        isImageProcAllowed_ = (popupInfo_.ImagesCount() != 0);

        imageMoveQueue_.push(0);

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

        target.draw(imageSpriteCurr_, STATES);

        if (areImagesMoving_ && (false == isInitialAnimation_))
        {
            target.draw(imageSpritePrev_, STATES);
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
                areImagesMoving_ = ! imageSlider_.GetIsDone();

                if (false == areImagesMoving_)
                {
                    SetupContent(false);
                    willShowImageCount_ = true;

                    if (imageIndexLastSoundOff_ != imageIndex_)
                    {
                        sfml_util::SoundManager::Instance()->Getsound_effect_set(
                            sfml_util::sound_effect_set::TickOff).PlayRandom();
                    }

                    imageIndexLastSoundOff_ = imageIndex_;
                }

                if (isInitialAnimation_)
                {
                    isInitialAnimation_ = areImagesMoving_;
                }
            }

            if ((false == areImagesMoving_) && (imageMoveQueue_.empty() == false))
            {
                areImagesMoving_ = true;
                willShowImageCount_ = false;

                if (imageIndexLastSoundOn_ != imageMoveQueue_.front())
                {
                    sfml_util::SoundManager::Instance()->Getsound_effect_set(
                        sfml_util::sound_effect_set::TickOn).PlayRandom();
                }

                imageIndexLastSoundOn_ = imageMoveQueue_.front();

                SetupSelectImage(
                    imageMoveQueue_.front(),
                    IMAGE_SLIDER_SPEED_ + ((static_cast<float>(imageMoveQueue_.size()) * 0.25f)) );

                imageMoveQueue_.pop();
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);

                SetupContent(true);
            }

            const float CURR_SCALE(imageCurrTargetScale_ * currRatio);
            imageSpriteCurr_.setScale(CURR_SCALE, CURR_SCALE);

            const float PREV_SCALE(imagePrevStartScale_ * (1.0f - currRatio));
            imageSpritePrev_.setScale(PREV_SCALE, PREV_SCALE);

            const float CURR_POS_TOP(imagePosTop_ -
                (imageSpriteCurr_.getGlobalBounds().height * 0.5f));

            const float PREV_POS_TOP(imagePosTop_ -
                (imageSpritePrev_.getGlobalBounds().height * 0.5f));

            if (willShowImageCount_)
            {
                imageNumTextRegionUPtr_->SetEntityPos(imageNumTextRegionUPtr_->GetEntityPos().x,
                    CURR_POS_TOP - imageNumTextRegionUPtr_->GetEntityRegion().height);
            }

            if (areImagesMovingLeft_)
            {
                const float CURR_POS_LEFT((imagesRect_.left + imagesRect_.width) -
                    (currRatio * imageCurrTravelDist_));

                imageSpriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const float PREV_POS_LEFT(imagePrevStartPosX_ - (currRatio * imagePrevTravelDist_));
                imageSpritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
            else
            {
                const float CURR_POS_LEFT(imagesRect_.left + (currRatio * imageCurrTravelDist_));
                imageSpriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const float PREV_POS_LEFT(imagePrevStartPosX_ + (currRatio * imagePrevTravelDist_));
                imageSpritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
        }
    }


    bool PopupStageImageSelect::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code >= sf::Keyboard::Num1) &&
            (KEY_EVENT.code <= sf::Keyboard::Num9))
        {
            return KeyReleaseHandleNumbers(KEY_EVENT);
        }
        else if (KEY_EVENT.code == sf::Keyboard::Left)
        {
            return KeyReleaseHandleLeft();
        }
        else if (KEY_EVENT.code == sf::Keyboard::Right)
        {
            return KeyReleaseHandeRight();
        }

        if (HandleNumberKeys(KEY_EVENT) == true)
        {
            return true;
        }

        if ((popupInfo_.Buttons() & PopupButtons::Select) &&
            (KEY_EVENT.code == sf::Keyboard::S))
        {
            InvalidateSelectionIfCurrentIsInvalid();
            return HandleSelect();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStageImageSelect::SetupSelectImage(
        const std::size_t NEW_IMAGE_INDEX,
        const float SLIDER_SPEED)
    {
        if (false == isImageProcAllowed_)
        {
            return;
        }

        auto const COUNT_MAX{ CountMax() };

        std::size_t newIndex(NEW_IMAGE_INDEX);
        if (newIndex >= COUNT_MAX)
        {
            newIndex = COUNT_MAX - 1;
        }

        if ((imageIndex_ == newIndex) && (isInitialAnimation_ == false))
        {
            return;
        }

        areImagesMovingLeft_ = (imageIndex_ < newIndex);
        imageIndex_ = newIndex;
        selection_ = static_cast<int>(imageIndex_);

        std::ostringstream ss;
        ss << imageIndex_ + 1 << "/" << COUNT_MAX;

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

        imageSlider_.Reset(SLIDER_SPEED);

        imageSpritePrev_ = imageSpriteCurr_;
        texturePrev_ = textureCurr_;
        imageSpritePrev_.setTexture(texturePrev_, true);

        SetCurrentTexture(imageIndex_);

        if (popupInfo_.AreImagesCreatures())
        {
            sfml_util::Invert(textureCurr_);
            sfml_util::Mask(textureCurr_, sf::Color::White);
        }

        imageSpriteCurr_.setTexture(textureCurr_, true);
        imageSpriteCurr_.setScale(1.0f, 1.0f);

        auto const POS_LEFT{
            textRegion_.left + ((areImagesMovingLeft_) ? textRegion_.width : 0.0f) };

        auto const POS_TOP{ imagePosTop_ };

        imageSpriteCurr_.setPosition(POS_LEFT, POS_TOP);

        //establish target scale of new image
        auto const RESIZE_RATIO{ 0.8f };
        auto const SPRITE_TARGET_WIDTH{ imagesRect_.width * RESIZE_RATIO };
        auto const SPRITE_TARGET_HEIGHT{ imagesRect_.height * RESIZE_RATIO };

        auto const SCALE_HORIZ{
            SPRITE_TARGET_WIDTH / imageSpriteCurr_.getGlobalBounds().width };

        auto const SCALE_VERT{
            SPRITE_TARGET_HEIGHT / imageSpriteCurr_.getGlobalBounds().height };

        imageCurrTargetScale_ = std::min(SCALE_HORIZ, SCALE_VERT);
        imageSpriteCurr_.setScale(0.0f, 0.0f);

        imagePrevStartScale_ = imageSpritePrev_.getScale().x;
        imagePrevStartPosX_ = imageSpritePrev_.getPosition().x;

        //establish the distance selectSpriteCurr, the new index sprite, will travel
        {
            sf::Sprite s(imageSpriteCurr_);
            s.setScale(imageCurrTargetScale_, imageCurrTargetScale_);

            auto const POS_LEFT_CENTERED{
                (textRegion_.left + (textRegion_.width * 0.5f)) -
                (s.getGlobalBounds().width * 0.5f) };

            if (areImagesMovingLeft_)
            {
                imageCurrTravelDist_ = (textRegion_.left + textRegion_.width) - POS_LEFT_CENTERED;
            }
            else
            {
                imageCurrTravelDist_ = POS_LEFT_CENTERED - textRegion_.left;
            }
        }

        //establish the distance selectSpritePrev, the old index sprite, will travel
        if (areImagesMovingLeft_)
        {
            imagePrevTravelDist_ = imageSpritePrev_.getGlobalBounds().left - imagesRect_.left;
        }
        else
        {
            imagePrevTravelDist_ = (imagesRect_.left + imagesRect_.width) -
                imageSpritePrev_.getGlobalBounds().left;
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

        isImageProcAllowed_ = false;
        sliderbarUPtr_->SetCurrentValue(static_cast<float>(targetIndex) * ONE_OVER_COUNT);
        isImageProcAllowed_ = true;

        std::size_t i{ imageIndex_ };
        while (true)
        {
            if (targetIndex < imageIndex_)
            {
                imageMoveQueue_.push(--i);
            }
            else
            {
                imageMoveQueue_.push(++i);
            }

            if (i == targetIndex)
            {
                break;
            }
        }

        return true;
    }


    bool PopupStageImageSelect::KeyReleaseHandleLeft()
    {
        if (imageIndex_ > 0)
        {
            auto const NEW_INDEX{ imageIndex_ - 1 };
            isImageProcAllowed_ = false;

            sliderbarUPtr_->SetCurrentValue(
                static_cast<float>(NEW_INDEX) / static_cast<float>(CountMax()) );

            isImageProcAllowed_ = true;
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

            isImageProcAllowed_ = false;

            sliderbarUPtr_->SetCurrentValue(
                static_cast<float>(NEW_INDEX) / static_cast<float>(COUNT_MAX));

            isImageProcAllowed_ = true;

            //if already at the end, then make sure the sliderbar is
            //all the way to the right
            if (NEW_INDEX >= (COUNT_MAX - 1))
            {
                //prevent processing and adding to the imageMoveQueue_ or calling
                //SetupSelectImage() when setting the sliderbar value here.
                isImageProcAllowed_ = false;
                sliderbarUPtr_->SetCurrentValue(1.0f);
                isImageProcAllowed_ = true;
            }

            imageMoveQueue_.push(NEW_INDEX);
            return true;
        }
        else
        {
            return false;
        }
    }

}
