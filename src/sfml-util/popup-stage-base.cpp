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
#include "game/game-data-file.hpp"

#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "misc/random.hpp"


namespace sfml_util
{

    const sf::Uint8 PopupStageBase::ACCENT_IMAGE_ALPHA_{ 32 };


    PopupStageBase::PopupStageBase(const game::PopupInfo & POPUP_INFO)
    :
        Stage(POPUP_INFO.Name() + "_PopupStage", false),
        popupInfo_(POPUP_INFO),
        innerRegion_(),
        backgroundTexture_(),
        backgroundSprite_(),
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
        xSymbolSprite_(),
        willShowXImage_(false),
        box_("PopupWindow's", gui::box::Info()),
        gradient_(),
        buttonTextHeight_(0.0f),
        buttonVertPos_(0.0f),
        xSymbolTexture_()
    {}


    PopupStageBase::~PopupStageBase()
    {
        ClearAllEntities();
    }


    bool PopupStageBase::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
    {
        return false;
    }


    bool PopupStageBase::HandleCallback(
        const sfml_util::gui::callback::TextButtonCallbackPackage_t & PACKAGE)
    {
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
        SoundManager::Instance()->SoundEffectPlay(popupInfo_.SoundEffect());

        SetupBackgroundImage();
        SetupOuterAndInnerRegion();
        
        //darken the box gold bars a bit
        box_.SetEntityColors( gui::ColorSet(sf::Color(200,200,200)) );

        //this call must occur after SetupOuterAndInnerRegion()
        backgroundSprite_.setPosition(StageRegionLeft(), StageRegionTop());

        SetupVariousButtonPositionValues();
        SetupButtons();
        SetupTextRegion();
        SetupText();
        SetupGradient();
        SetupAccentSprite();
        SetupSliderbar();
        SetupRedXImage();
    }


    void PopupStageBase::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (popupInfo_.Image() == sfml_util::PopupImage::Custom)
        {
            target.draw(box_, STATES);
            target.draw(gradient_, STATES);
        }
        else
        {
            target.draw(backgroundSprite_, STATES);
        }

        if (popupInfo_.WillAddRandImage())
        {
            target.draw(accentSprite1_, STATES);
        }

        textRegionUPtr_->draw(target, STATES);

        if (willShowXImage_)
        {
            target.draw(xSymbolSprite_, STATES);
        }
    }


    bool PopupStageBase::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (popupInfo_.Buttons() & Response::Continue)
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

        if (popupInfo_.Buttons() & Response::Okay)
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

        if ((popupInfo_.Buttons() & Response::Yes) && (KEY_EVENT.code == sf::Keyboard::Y))
        {
            SoundManager::Instance()->
                Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(Response::Yes);
            return true;
        }

        if ((popupInfo_.Buttons() & Response::No) &&
            ((KEY_EVENT.code == sf::Keyboard::N) || (KEY_EVENT.code == sf::Keyboard::Escape)))
        {
            SoundManager::Instance()->
                Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(Response::No);
            return true;
        }

        if (popupInfo_.Buttons() & Response::Cancel)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape) ||
                ((KEY_EVENT.code == sf::Keyboard::C) &&
                ((popupInfo_.Type() != game::Popup::ContentSelectionWithItem) &&
                    (popupInfo_.Type() != game::Popup::ContentSelectionWithoutItem))) ||
                ((KEY_EVENT.code == sf::Keyboard::Return) &&
                    (popupInfo_.Buttons() == sfml_util::PopupButtons::Cancel)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Cancel);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & Response::Select) &&
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


    void PopupStageBase::SetupBackgroundImage()
    {
        //Custom popups have no background image
        if (popupInfo_.Image() != sfml_util::PopupImage::Custom)
        {
            sfml_util::LoadTexture(
                backgroundTexture_,
                sfml_util::gui::PopupManager::Instance()->BackgroundImagePath(popupInfo_.Image()));

            backgroundSprite_.setTexture(backgroundTexture_, true);
        }
    }


    void PopupStageBase::SetupOuterAndInnerRegion()
    {
        if (popupInfo_.Image() == sfml_util::PopupImage::Custom)
        {
            //establish a new popup window region for Custom popups
            auto const SCREEN_WIDTH(sfml_util::Display::Instance()->GetWinWidth());
            auto const SCREEN_HEIGHT(sfml_util::Display::Instance()->GetWinHeight());
            auto const POPUP_WIDTH(SCREEN_WIDTH * popupInfo_.SizeX());
            auto const POPUP_HEIGHT(SCREEN_HEIGHT * popupInfo_.SizeY());

            const sf::FloatRect NEW_REGION(
                (SCREEN_WIDTH * 0.5f) - (POPUP_WIDTH * 0.5f),
                (SCREEN_HEIGHT * 0.5f) - (POPUP_HEIGHT * 0.5f),
                POPUP_WIDTH,
                POPUP_HEIGHT);

            StageRegionSet(NEW_REGION);

            //re-construct the box info object based on the NEW_REGION
            sfml_util::gui::box::Info newBoxInfo(popupInfo_.BoxInfo());
            newBoxInfo.SetBoxAndBackgroundRegion(NEW_REGION);

            popupInfo_.SetBoxInfo(newBoxInfo);

            //establish inner rect
            innerRegion_ = NEW_REGION;
            auto const PAD{ 20.0f };//found by experiment to look good in varying resolution
            innerRegion_.left = PAD;
            innerRegion_.top = PAD;
            innerRegion_.width -= (PAD * 2.0f);
            innerRegion_.height -= (PAD * 2.0f);
        }
        else
        {
            auto const TEXTURE_WIDTH{ static_cast<float>(backgroundTexture_.getSize().x) };
            auto const TEXTURE_HEIGHT{ static_cast<float>(backgroundTexture_.getSize().y) };

            sf::FloatRect region;
            region.left =
                (sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (TEXTURE_WIDTH * 0.5f);

            region.top =
                (sfml_util::Display::Instance()->GetWinHeight() * 0.5f) - (TEXTURE_HEIGHT * 0.5f);

            region.width = TEXTURE_WIDTH;
            region.height = TEXTURE_HEIGHT;

            StageRegionSet(region);

            innerRegion_ = sfml_util::ConvertRect<int, float>(
                BackgroundImageRect(popupInfo_.Image(), popupInfo_.ImageScale()) );
        }

        //adjust regions based on the background image
        backgroundSprite_.setScale(popupInfo_.ImageScale(), popupInfo_.ImageScale());

        //set the stage region to fit the newly sized background paper/whatever image
        auto const BACKGROUND_WIDTH{ backgroundSprite_.getGlobalBounds().width };

        auto const BACKGROUND_HEIGHT{ backgroundSprite_.getGlobalBounds().height };
            
        auto const BACKGROUND_POS_LEFT{
            (sfml_util::Display::Instance()->GetWinWidth() * 0.5f) -
                (BACKGROUND_WIDTH * 0.5f) };

        auto const BACKGROUND_POS_TOP{
            (sfml_util::Display::Instance()->GetWinHeight() * 0.5f) -
                (BACKGROUND_HEIGHT * 0.5f) };

        StageRegionSet(sf::FloatRect(
            BACKGROUND_POS_LEFT,
            BACKGROUND_POS_TOP,
            BACKGROUND_WIDTH,
            BACKGROUND_HEIGHT));
    }


    void PopupStageBase::SetupFullscreenRegionsAndBackgroundImage(
        const sf::FloatRect & REGION)
    {
        StageRegionSet(REGION);
        innerRegion_ = REGION;

        auto const SCALE{
            REGION.width / static_cast<float>(backgroundTexture_.getSize().x) };

        backgroundSprite_.setScale(SCALE, SCALE);
    }


    void PopupStageBase::SetupVariousButtonPositionValues()
    {
        auto const TEMP_MOUSE_TEXT_INFO{ gui::MouseTextInfo::Make_PopupButtonSet(
            game::creature::NameInfo::Instance()->LargestLetterString(), popupInfo_) };

        const sf::Text TEMP_TEXT_OBJ{
            TEMP_MOUSE_TEXT_INFO.up.text,
            *TEMP_MOUSE_TEXT_INFO.up.fontPtr,
            TEMP_MOUSE_TEXT_INFO.up.charSize };

        buttonTextHeight_ = TEMP_TEXT_OBJ.getGlobalBounds().height;
    
        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN{ MapByRes(30.0f, 90.0f) };
        
        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM{
            (popupInfo_.Image() == PopupImage::Custom) ? 25.0f : 0.0f };

        auto const BUTTON_HEIGHT{
            ButtonTextHeight() +
            POPUPBUTTON_TEXT_BOTTOM_MARGIN +
            POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM };

        buttonVertPos_ = (StageRegionTop() + innerRegion_.top + innerRegion_.height) -
            BUTTON_HEIGHT;

    }


    void PopupStageBase::SetupButtons()
    {
        if (popupInfo_.Buttons() & Response::Yes)
        {
            buttonYesUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sYes",
                StageRegionLeft() + innerRegion_.left + (innerRegion_.width / 4.0f) - 50.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Yes", popupInfo_),
                this);

            EntityAdd(buttonYesUPtr_.get());
        }

        if (popupInfo_.Buttons() & Response::No)
        {
            buttonNoUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sNo",
                StageRegionLeft() + innerRegion_.left + (2.0f * (innerRegion_.width / 4.0f))
                    - 40.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("No", popupInfo_),
                this);

            EntityAdd(buttonNoUPtr_.get());
        }

        if (popupInfo_.Buttons() & Response::Cancel)
        {
            buttonCancelUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sCancel",
                (StageRegionLeft() + innerRegion_.left + innerRegion_.width) -
                    (innerRegion_.width / 3.0f),
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Cancel", popupInfo_),
                this);

            EntityAdd(buttonCancelUPtr_.get());
        }

        if (popupInfo_.Buttons() & Response::Continue)
        {
            auto const MIDDLE{ StageRegionLeft() + innerRegion_.left +
                (innerRegion_.width * 0.5f) };

            buttonContinueUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sContinue",
                MIDDLE - 30.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Continue", popupInfo_),
                this);

            if (popupInfo_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE -
                    (buttonContinueUPtr_->GetEntityRegion().width * 0.5f) };

                auto const POS_TOP{ buttonContinueUPtr_->GetEntityPos().y };

                buttonContinueUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonContinueUPtr_.get());
        }

        if (popupInfo_.Buttons() & Response::Okay)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left +
                (innerRegion_.width * 0.5f));

            buttonOkayUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sOkay",
                MIDDLE - 50.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Okay", popupInfo_),
                this);

            if (popupInfo_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE - (buttonOkayUPtr_->GetEntityRegion().width * 0.5f) -
                    10.0f };

                auto const POS_TOP{ buttonOkayUPtr_->GetEntityPos().y };

                buttonOkayUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonOkayUPtr_.get());
        }

        if (popupInfo_.Buttons() & Response::Select)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left +
                (innerRegion_.width * 0.5f));

            buttonSelectUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sSelect",
                MIDDLE - 100.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Select", popupInfo_),
                this);

            if (popupInfo_.Image() == PopupImage::Custom)
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
            popupInfo_.TextInfo(),
            tempRect,
            this);

        if ((textRegionUPtr_->GetEntityRegion().top + textRegionUPtr_->GetEntityRegion().height) >
            (textRegion_.top + textRegion_.height))
        {
            tempRect.height = textRegion_.height;

            textRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "PopupStage's",
                popupInfo_.TextInfo(),
                tempRect,
                this);
        }
    }


    void PopupStageBase::SetupTextRegion()
    {
        textRegion_.left = StageRegionLeft() + innerRegion_.left;
        textRegion_.top = StageRegionTop() + innerRegion_.top;
        textRegion_.width = innerRegion_.width;

        auto const TEXT_TO_BUTTON_SPACER{ 12.0f }; //found by experiment
        textRegion_.height = (innerRegion_.height - ButtonTextHeight()) - TEXT_TO_BUTTON_SPACER;
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
        if (popupInfo_.WillAddRandImage())
        {
            gui::PopupManager::Instance()->LoadRandomAccentImage(accentTexture1_);
            accentSprite1_.setTexture(accentTexture1_, true);

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
                (textRegion_.left + (textRegion_.width * 0.5f)) -
                    (accentSprite1_.getGlobalBounds().width * 0.5f) };

            auto const ACCENT_POS_TOP{
                (textRegion_.top + (textRegion_.height * 0.5f)) -
                    (accentSprite1_.getGlobalBounds().height * 0.5f) };

            accentSprite1_.setPosition(ACCENT_POS_LEFT, ACCENT_POS_TOP);
            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }


    void PopupStageBase::SetupSliderbar()
    {
        sliderbarPosTop_ = (buttonVertPos_ - (ButtonTextHeight() * 3.0f));

        if ((popupInfo_.Type() == game::Popup::ImageSelection) ||
            (popupInfo_.Type() == game::Popup::NumberSelection))
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


    void PopupStageBase::SetupRedXImage()
    {
        sfml_util::LoadTexture(xSymbolTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-x"));

        xSymbolSprite_.setTexture(xSymbolTexture_);

        //this color found by experiment to look good on all popups
        xSymbolSprite_.setColor(sf::Color(255, 0, 0, 127));
    }


    const sf::IntRect PopupStageBase::BackgroundImageRect(
        const sfml_util::PopupImage::Enum PI,
        const float SCALE) const
    {
        switch (PI)
        {
            case PopupImage::Banner:
            {
                return sfml_util::ScaleRectCopy(
                    sfml_util::gui::PopupManager::Instance()->Rect_Banner(), SCALE);
            }

            case PopupImage::Regular:
            {
                return sfml_util::ScaleRectCopy(
                    sfml_util::gui::PopupManager::Instance()->Rect_Regular(), SCALE);
            }

            case PopupImage::RegularSidebar:
            {
                return sfml_util::ScaleRectCopy(
                    sfml_util::gui::PopupManager::Instance()->Rect_RegularSidebar(), SCALE);
            }

            case PopupImage::Large:
            {
                return sfml_util::ScaleRectCopy(
                    sfml_util::gui::PopupManager::Instance()->Rect_Large(), SCALE);
            }

            case PopupImage::LargeSidebar:
            {
                return sfml_util::ScaleRectCopy(
                    sfml_util::gui::PopupManager::Instance()->Rect_LargeSidebar(), SCALE);
            }

            case PopupImage::Spellbook:
            case PopupImage::MusicSheet:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupStageBase::BackgroundImageRect("
                    << PI << ") was given Spellbook or MusicSheet, which are fullscreen"
                    << " popups and have no rects to setup.";

                throw std::invalid_argument(ss.str());
            }
            case PopupImage::Custom:
            case PopupImage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupStageBase::BackgroundImageRect("
                    << PI << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

}
