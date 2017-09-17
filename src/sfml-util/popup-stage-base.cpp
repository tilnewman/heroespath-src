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
// popup-stage-base.cpp
//
#include "popup-stage-base.hpp"

#include "game/loop-manager.hpp"
#include "game/creature/name-info.hpp"

#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include "misc/random.hpp"


namespace sfml_util
{

    const sf::Uint8 PopupStageBase::ACCENT_IMAGE_ALPHA_{ 32 };


    PopupStageBase::PopupStageBase(
        const game::PopupInfo & POPUP_INFO,
        const sf::FloatRect & REGION,
        const sf::FloatRect & INNER_REGION,
        const sf::Texture & BG_TEXTURE)
    :
        Stage(POPUP_INFO.Name() + "_PopupStage", REGION, false),
        POPUP_INFO_(POPUP_INFO),
        INNER_REGION_(INNER_REGION),
        backgroundTexture_(BG_TEXTURE),
        backgroundSprite_(backgroundTexture_),
        textRegionUPtr_(),
        textRegion_(),
        buttonSelectUPtr_(),
        buttonYesUPtr_(),
        buttonNoUPtr_(),
        buttonCancelUPtr_(),
        buttonContinueUPtr_(),
        buttonOkayUPtr_(),
        sliderbarUPtr_(),
        accentTexture1_(),
        accentSprite1_(),
        accentTexture2_(),
        accentSprite2_(),
        sliderbarPosTop_(0.0f),
        selection_(-1),//any negative value will work here
        box_("PopupWindow's", gui::box::Info()),
        gradient_(),
        buttonTextHeight_(0.0f),
        buttonVertPos_(0.0f)
    {}


    PopupStageBase::~PopupStageBase()
    {}


    bool PopupStageBase::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
    {
        M_HP_LOG_DBG("\t ~~~~~~~~~~ PopupStageBase::HandleCallback(SliderBarCallbackPackage_t)  begin/end");
        return false;
    }


    bool PopupStageBase::HandleCallback(
        const sfml_util::gui::callback::TextButtonCallbackPackage_t & PACKAGE)
    {
        M_HP_LOG_DBG("\t ~~~~~~~~~~ PopupStageBase::HandleCallback(TextButtonCallbackPackage_t)  begin (no end)");
        if (PACKAGE.PTR_ == buttonSelectUPtr_.get())
        {
            return HandleSelect();
        }
        else if (PACKAGE.PTR_ == buttonYesUPtr_.get())
        {
            SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Yes);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonNoUPtr_.get())
        {
            SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::No);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonCancelUPtr_.get())
        {
            SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Cancel);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonContinueUPtr_.get())
        {
            SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonOkayUPtr_.get())
        {
            SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Okay);
            return true;
        }

        return false;
    }


    void PopupStageBase::Setup()
    {
        SoundManager::Instance()->SoundEffectPlay(POPUP_INFO_.SoundEffect());
        
        //darken the box gold bars a bit
        box_.SetEntityColors(gui::ColorSet(sf::Color(200,200,200)));

        backgroundSprite_.setPosition(StageRegionLeft(), StageRegionTop());

        SetupVariousButtonPositionValues();
        SetupButtons();
        SetupTextRegion();
        SetupText();
        SetupGradient();
        SetupAccentSprite();
        SetupSliderbar();
    }


    void PopupStageBase::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (POPUP_INFO_.Image() == sfml_util::PopupImage::Custom)
        {
            target.draw(box_, STATES);
            target.draw(gradient_, STATES);
        }
        else
        {
            target.draw(backgroundSprite_, STATES);
        }

        if (POPUP_INFO_.WillAddRandImage())
        {
            target.draw(accentSprite1_, STATES);
        }

        textRegionUPtr_->draw(target, STATES);
    }


    bool PopupStageBase::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (POPUP_INFO_.Buttons() & Response::Continue)
        {
            if ((KEY_EVENT.code == sf::Keyboard::C) ||
                (KEY_EVENT.code == sf::Keyboard::Space) ||
                (KEY_EVENT.code == sf::Keyboard::Return) ||
                (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
                return true;
            }
        }

        if (POPUP_INFO_.Buttons() & Response::Okay)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Space) ||
                (KEY_EVENT.code == sf::Keyboard::O) ||
                (KEY_EVENT.code == sf::Keyboard::Return) ||
                (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Okay);
                return true;
            }
        }

        if ((POPUP_INFO_.Buttons() & Response::Yes) && (KEY_EVENT.code == sf::Keyboard::Y))
        {
            SoundManager::Instance()->
                Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(Response::Yes);
            return true;
        }

        if ((POPUP_INFO_.Buttons() & Response::No) &&
            ((KEY_EVENT.code == sf::Keyboard::N) || (KEY_EVENT.code == sf::Keyboard::Escape)))
        {
            SoundManager::Instance()->
                Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(Response::No);
            return true;
        }

        if (POPUP_INFO_.Buttons() & Response::Cancel)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape) ||
                ((KEY_EVENT.code == sf::Keyboard::C) &&
                ((POPUP_INFO_.Type() != game::Popup::ContentSelectionWithItem) &&
                    (POPUP_INFO_.Type() != game::Popup::ContentSelectionWithoutItem))) ||
                ((KEY_EVENT.code == sf::Keyboard::Return) &&
                    (POPUP_INFO_.Buttons() == sfml_util::PopupButtons::Cancel)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Cancel);
                return true;
            }
        }

        if ((POPUP_INFO_.Buttons() & Response::Select) &&
            (KEY_EVENT.code == sf::Keyboard::Return))
        {
            return HandleSelect();
        }

        return Stage::KeyRelease(KEY_EVENT);
    }


    bool PopupStageBase::HandleSelect()
    {
        if (selection_ < 0)
        {
            SoundManager::Instance()->SoundEffectPlay(sound_effect::PromptWarn);
            return false;
        }
        else
        {
            SoundManager::Instance()->Getsound_effect_set(
                sfml_util::sound_effect_set::Thock).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(
                Response::Select, static_cast<std::size_t>(selection_));

            return true;
        }
    }


    void PopupStageBase::SetupVariousButtonPositionValues()
    {
        auto const TEMP_MOUSE_TEXT_INFO{ gui::MouseTextInfo::Make_PopupButtonSet(
            game::creature::NameInfo::Instance()->LargestLetterString(), POPUP_INFO_) };

        const sf::Text TEMP_TEXT_OBJ{
            TEMP_MOUSE_TEXT_INFO.up.text,
            *TEMP_MOUSE_TEXT_INFO.up.fontPtr,
            TEMP_MOUSE_TEXT_INFO.up.charSize };

        buttonTextHeight_ = TEMP_TEXT_OBJ.getGlobalBounds().height;
    
        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN{ MapByRes(30.0f, 90.0f) };
        
        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM{
            (POPUP_INFO_.Image() == PopupImage::Custom) ? 25.0f : 0.0f };

        auto const BUTTON_HEIGHT{
            ButtonTextHeight() +
            POPUPBUTTON_TEXT_BOTTOM_MARGIN +
            POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM };

        buttonVertPos_ = (StageRegionTop() + INNER_REGION_.top + INNER_REGION_.height) -
            BUTTON_HEIGHT;

    }


    void PopupStageBase::SetupButtons()
    {
        if (POPUP_INFO_.Buttons() & Response::Yes)
        {
            buttonYesUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sYes",
                StageRegionLeft() + INNER_REGION_.left + (INNER_REGION_.width / 4.0f) - 50.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Yes", POPUP_INFO_),
                this);

            EntityAdd(buttonYesUPtr_.get());
        }

        if (POPUP_INFO_.Buttons() & Response::No)
        {
            buttonNoUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sNo",
                StageRegionLeft() + INNER_REGION_.left + (2.0f * (INNER_REGION_.width / 4.0f))
                    - 40.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("No", POPUP_INFO_),
                this);

            EntityAdd(buttonNoUPtr_.get());
        }

        if (POPUP_INFO_.Buttons() & Response::Cancel)
        {
            buttonCancelUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sCancel",
                (StageRegionLeft() + INNER_REGION_.left + INNER_REGION_.width) -
                    (INNER_REGION_.width / 3.0f),
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Cancel", POPUP_INFO_),
                this);

            EntityAdd(buttonCancelUPtr_.get());
        }

        if (POPUP_INFO_.Buttons() & Response::Continue)
        {
            auto const MIDDLE{ StageRegionLeft() + INNER_REGION_.left +
                (INNER_REGION_.width * 0.5f) };

            buttonContinueUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sContinue",
                MIDDLE - 30.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Continue", POPUP_INFO_),
                this);

            if (POPUP_INFO_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE -
                    (buttonContinueUPtr_->GetEntityRegion().width * 0.5f) };

                auto const POS_TOP{ buttonContinueUPtr_->GetEntityPos().y };

                buttonContinueUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonContinueUPtr_.get());
        }

        if (POPUP_INFO_.Buttons() & Response::Okay)
        {
            const float MIDDLE(StageRegionLeft() + INNER_REGION_.left +
                (INNER_REGION_.width * 0.5f));

            buttonOkayUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sOkay",
                MIDDLE - 50.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Okay", POPUP_INFO_),
                this);

            if (POPUP_INFO_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE - (buttonOkayUPtr_->GetEntityRegion().width * 0.5f) -
                    10.0f };

                auto const POS_TOP{ buttonOkayUPtr_->GetEntityPos().y };

                buttonOkayUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonOkayUPtr_.get());
        }

        if (POPUP_INFO_.Buttons() & Response::Select)
        {
            const float MIDDLE(StageRegionLeft() + INNER_REGION_.left +
                (INNER_REGION_.width * 0.5f));

            buttonSelectUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sSelect",
                MIDDLE - 100.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Select", POPUP_INFO_),
                this);

            if (POPUP_INFO_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE - (buttonSelectUPtr_->GetEntityRegion().width * 0.5f) -
                    10.0f };

                auto const POS_TOP{ buttonSelectUPtr_->GetEntityPos().y };

                buttonSelectUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonSelectUPtr_.get());
        }
    }


    void PopupStageBase::SetupText()
    {
        sf::FloatRect tempRect(textRegion_);
        tempRect.height = 0.0f;

        textRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "PopupStage's",
            POPUP_INFO_.TextInfo(),
            tempRect,
            this);

        if ((textRegionUPtr_->GetEntityRegion().top + textRegionUPtr_->GetEntityRegion().height) >
            (textRegion_.top + textRegion_.height))
        {
            tempRect.height = textRegion_.height;

            textRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "PopupStage's",
                POPUP_INFO_.TextInfo(),
                tempRect,
                this);
        }
    }


    void PopupStageBase::SetupTextRegion()
    {
        textRegion_.left = StageRegionLeft() + INNER_REGION_.left;
        textRegion_.top = StageRegionTop() + INNER_REGION_.top;
        textRegion_.width = INNER_REGION_.width;

        auto const TEXT_TO_BUTTON_SPACER{ 12.0f }; //found by experiment
        textRegion_.height = (INNER_REGION_.height - ButtonTextHeight()) - TEXT_TO_BUTTON_SPACER;
    }


    void PopupStageBase::SetupGradient()
    {
        const sf::FloatRect GRADIENT_RECT(
            StageRegionLeft(),
            textRegion_.top + textRegion_.height,
            StageRegionWidth() - 1.0f,
            (StageRegionHeight() - textRegion_.height) + 2.0f);

        gradient_.Setup(
            GRADIENT_RECT,
            GradientInfo(sf::Color::Black, 0, Side::Top, sf::Color(0, 0, 0, 20)));

    }


    void PopupStageBase::SetupAccentSprite()
    {
        if (POPUP_INFO_.WillAddRandImage())
        {
            gui::PopupManager::Instance()->LoadRandomAccentImage(accentTexture1_);
            accentSprite1_.setTexture(accentTexture1_);

            auto const SIZE_RATIO{ misc::random::Float(0.65f, 0.85f) };//found by experiment

            auto const SCALE_VERT{
                (textRegion_.height * SIZE_RATIO) / accentSprite1_.getLocalBounds().height };

            accentSprite1_.setScale(SCALE_VERT, SCALE_VERT);

            if (accentSprite1_.getGlobalBounds().width > (textRegion_.width * SIZE_RATIO))
            {
                auto const SCALE_HORIZ{
                    (textRegion_.width * SIZE_RATIO) / accentSprite1_.getLocalBounds().width };

                if (SCALE_HORIZ < SCALE_VERT)
                {
                    accentSprite1_.setScale(SCALE_HORIZ, SCALE_HORIZ);
                }
            }

            //always center the accent sprite image
            auto const ACCENT_POS_LEFT{
                (StageRegionLeft() + (StageRegionWidth() * 0.5f)) -
                    (accentSprite1_.getGlobalBounds().width * 0.5f) };

            auto const ACCENT_POS_TOP{
                (StageRegionTop() + (StageRegionHeight() * 0.5f)) -
                    (accentSprite1_.getGlobalBounds().height * 0.5f) };

            accentSprite1_.setPosition(ACCENT_POS_LEFT, ACCENT_POS_TOP);
            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }


    void PopupStageBase::SetupSliderbar()
    {
        sliderbarPosTop_ = (buttonVertPos_ - (ButtonTextHeight() * 3.0f));

        if ((POPUP_INFO_.Type() == game::Popup::ImageSelection) ||
            (POPUP_INFO_.Type() == game::Popup::NumberSelection))
        {
            auto const SLIDERBAR_LENGTH{ textRegion_.width * 0.75f };//found by experiment

            auto const SLIDERBAR_POS_LEFT{
                textRegion_.left + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f) };

            sliderbarUPtr_ = std::make_unique<gui::SliderBar>(
                "PopupStage's",
                SLIDERBAR_POS_LEFT,
                sliderbarPosTop_,
                SLIDERBAR_LENGTH,
                gui::SliderStyle(Orientation::Horiz),
                this);

            EntityAdd(sliderbarUPtr_.get());
        }
    }

}
