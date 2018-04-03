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

#include "creature/name-info.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

#include "popup/popup-manager.hpp"

#include "misc/random.hpp"

namespace heroespath
{
namespace popup
{

    // This value was found by experiment to be faded enough to avoid interfering with foreground.
    const sf::Uint8 PopupStageBase::ACCENT_IMAGE_ALPHA_{ 19 };

    // This value was found by experiment to make a good looking empty border around the image.
    const float PopupStageBase::ACCENT_IMAGE_SCALEDOWN_RATIO_{ 0.85f };

    PopupStageBase::PopupStageBase(const PopupInfo & POPUP_INFO)
        : Stage(POPUP_INFO.Name() + "_PopupStage", false)
        , popupInfo_(POPUP_INFO)
        , innerRegion_()
        , backgroundTexture_()
        , backgroundSprite_()
        , textRegionUPtr_()
        , textRegion_()
        , buttonSelectUPtr_()
        , buttonYesUPtr_()
        , buttonNoUPtr_()
        , buttonCancelUPtr_()
        , buttonContinueUPtr_()
        , buttonOkayUPtr_()
        , sliderbarUPtr_()
        , accentTexture1_()
        , accentSprite1_()
        , accentTexture2_()
        , accentSprite2_()
        , sliderbarPosTop_(0.0f)
        , selection_(-1)
        , // any negative value will work here
        xSymbolSprite_()
        , willShowXImage_(false)
        , box_("PopupWindow's", sfml_util::gui::box::Info())
        , gradient_()
        , buttonTextHeight_(0.0f)
        , buttonVertPos_(0.0f)
        , xSymbolTexture_()
        , keepAliveTimerSec_(POPUP_INFO.KeepAliveSec())
    {}

    PopupStageBase::~PopupStageBase() { Stage::ClearAllEntities(); }

    bool
        PopupStageBase::HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
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
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Yes);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonNoUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::No);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonCancelUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Cancel);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonContinueUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Continue);
            return true;
        }
        else if (PACKAGE.PTR_ == buttonOkayUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Okay);
            return true;
        }

        return false;
    }

    void PopupStageBase::Setup()
    {
        sfml_util::SoundManager::Instance()->SoundEffectPlay(popupInfo_.SoundEffect());

        SetupBackgroundImage();
        SetupOuterAndInnerRegion();

        // darken the box gold bars a bit
        box_.SetEntityColors(sfml_util::gui::ColorSet(sf::Color(200, 200, 200)));

        // this call must occur after SetupOuterAndInnerRegion()
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
        if (popupInfo_.Image() == PopupImage::Custom)
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
    }

    void PopupStageBase::DrawRedX(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (willShowXImage_)
        {
            target.draw(xSymbolSprite_, STATES);
        }
    }

    bool PopupStageBase::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (popupInfo_.Buttons() & ResponseTypes::Continue)
        {
            if ((KEY_EVENT.code == sf::Keyboard::C) || (KEY_EVENT.code == sf::Keyboard::Space)
                || (KEY_EVENT.code == sf::Keyboard::Return)
                || (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                EndKeepAliveTimer();
                PlayValidKeypressSoundEffect();
                game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Continue);
                return true;
            }
        }

        if (popupInfo_.Buttons() & ResponseTypes::Okay)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Space) || (KEY_EVENT.code == sf::Keyboard::O)
                || (KEY_EVENT.code == sf::Keyboard::Return)
                || (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                EndKeepAliveTimer();
                PlayValidKeypressSoundEffect();
                game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Okay);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & ResponseTypes::Yes) && (KEY_EVENT.code == sf::Keyboard::Y))
        {
            EndKeepAliveTimer();
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Yes);
            return true;
        }

        if ((popupInfo_.Buttons() & ResponseTypes::No)
            && ((KEY_EVENT.code == sf::Keyboard::N) || (KEY_EVENT.code == sf::Keyboard::Escape)))
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::No);
            return true;
        }

        if (popupInfo_.Buttons() & ResponseTypes::Cancel)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape)
                || ((KEY_EVENT.code == sf::Keyboard::C) && WillPressingCKeyClosePopup())
                || ((KEY_EVENT.code == sf::Keyboard::Return)
                    && (popupInfo_.Buttons() == PopupButtons::Cancel)))
            {
                EndKeepAliveTimer();
                PlayValidKeypressSoundEffect();
                game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Cancel);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & ResponseTypes::Select)
            && ((KEY_EVENT.code == sf::Keyboard::Return) || (KEY_EVENT.code == sf::Keyboard::S)))
        {
            // sound effects played by HandleSelect()
            return HandleSelect();
        }

        return Stage::KeyRelease(KEY_EVENT);
    }

    void PopupStageBase::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (keepAliveTimerSec_ > 0.0f)
        {
            keepAliveTimerSec_ -= ELAPSED_TIME_SECONDS;
            if (keepAliveTimerSec_ < 0.0f)
            {
                game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Continue);
            }
        }
    }

    bool PopupStageBase::HandleSelect()
    {
        if (selection_ < 0)
        {
            PlayInvalidKeypressSoundEffect();
            return false;
        }
        else
        {
            PlayValidKeypressSoundEffect();

            game::LoopManager::Instance()->PopupWaitEnd(
                ResponseTypes::Select, static_cast<std::size_t>(selection_));

            return true;
        }
    }

    void PopupStageBase::PlayValidKeypressSoundEffect() const
    {
        sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
    }

    void PopupStageBase::PlayInvalidKeypressSoundEffect() const
    {
        sfml_util::SoundManager::Instance()->PlaySfx_Reject();
    }

    void PopupStageBase::EndKeepAliveTimer()
    {
        if (keepAliveTimerSec_ > 0.0f)
        {
            keepAliveTimerSec_ = 0.001f;
        }
    }

    void PopupStageBase::SetupBackgroundImage()
    {
        // Custom popups have no background image
        if (popupInfo_.Image() != PopupImage::Custom)
        {
            sfml_util::LoadTexture(
                backgroundTexture_,
                PopupManager::Instance()->BackgroundImagePath(popupInfo_.Image()));

            backgroundSprite_.setTexture(backgroundTexture_, true);
        }
    }

    void PopupStageBase::SetupOuterAndInnerRegion()
    {
        auto const BG_IMAGE_SCALE{ calcBackgroundImageScale(popupInfo_.Image()) };

        if (popupInfo_.Image() == PopupImage::Custom)
        {
            // establish a new popup window region for Custom popups
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

            // re-construct the box info object based on the NEW_REGION
            sfml_util::gui::box::Info newBoxInfo(popupInfo_.BoxInfo());
            newBoxInfo.SetBoxAndBackgroundRegion(NEW_REGION);

            popupInfo_.SetBoxInfo(newBoxInfo);

            // establish inner rect
            innerRegion_ = NEW_REGION;
            auto const PAD{ 20.0f }; // found by experiment to look good in varying resolution
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
            region.left
                = (sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (TEXTURE_WIDTH * 0.5f);

            region.top
                = (sfml_util::Display::Instance()->GetWinHeight() * 0.5f) - (TEXTURE_HEIGHT * 0.5f);

            region.width = TEXTURE_WIDTH;
            region.height = TEXTURE_HEIGHT;

            StageRegionSet(region);

            innerRegion_ = sfml_util::ConvertRect<int, float>(
                BackgroundImageRect(popupInfo_.Image(), BG_IMAGE_SCALE));
        }

        // adjust regions based on the background image
        backgroundSprite_.setScale(BG_IMAGE_SCALE, BG_IMAGE_SCALE);

        // set the stage region to fit the newly sized background paper/whatever image
        auto const BACKGROUND_WIDTH{ backgroundSprite_.getGlobalBounds().width };

        auto const BACKGROUND_HEIGHT{ backgroundSprite_.getGlobalBounds().height };

        auto const BACKGROUND_POS_LEFT{ (sfml_util::Display::Instance()->GetWinWidth() * 0.5f)
                                        - (BACKGROUND_WIDTH * 0.5f) };

        auto const BACKGROUND_POS_TOP{ (sfml_util::Display::Instance()->GetWinHeight() * 0.5f)
                                       - (BACKGROUND_HEIGHT * 0.5f) };

        StageRegionSet(sf::FloatRect(
            BACKGROUND_POS_LEFT, BACKGROUND_POS_TOP, BACKGROUND_WIDTH, BACKGROUND_HEIGHT));
    }

    void PopupStageBase::SetupFullscreenRegionsAndBackgroundImage(const sf::FloatRect & REGION)
    {
        StageRegionSet(REGION);
        innerRegion_ = REGION;

        auto const SCALE{ REGION.width / static_cast<float>(backgroundTexture_.getSize().x) };

        backgroundSprite_.setScale(SCALE, SCALE);
    }

    void PopupStageBase::SetupVariousButtonPositionValues()
    {
        auto const TEMP_MOUSE_TEXT_INFO{ sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
            creature::NameInfo::Instance()->LargestLetterString(), popupInfo_.ButtonColor()) };

        const sf::Text TEMP_TEXT_OBJ{ TEMP_MOUSE_TEXT_INFO.up.text,
                                      *TEMP_MOUSE_TEXT_INFO.up.fontPtrOpt.value(),
                                      TEMP_MOUSE_TEXT_INFO.up.charSize };

        buttonTextHeight_ = TEMP_TEXT_OBJ.getGlobalBounds().height;

        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN{ sfml_util::MapByRes(30.0f, 90.0f) };

        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM{
            (popupInfo_.Image() == PopupImage::Custom) ? 25.0f : 0.0f
        };

        auto const BUTTON_HEIGHT{ ButtonTextHeight() + POPUPBUTTON_TEXT_BOTTOM_MARGIN
                                  + POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM };

        buttonVertPos_
            = (StageRegionTop() + innerRegion_.top + innerRegion_.height) - BUTTON_HEIGHT;
    }

    void PopupStageBase::SetupButtons()
    {
        if (popupInfo_.Buttons() & ResponseTypes::Yes)
        {
            buttonYesUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sYes",
                StageRegionLeft() + innerRegion_.left + (innerRegion_.width / 4.0f) - 50.0f,
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet("Yes", popupInfo_.ButtonColor()),
                this);

            EntityAdd(buttonYesUPtr_.get());
        }

        if (popupInfo_.Buttons() & ResponseTypes::No)
        {
            buttonNoUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sNo",
                StageRegionLeft() + innerRegion_.left + (2.0f * (innerRegion_.width / 4.0f))
                    - 40.0f,
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet("No", popupInfo_.ButtonColor()),
                this);

            EntityAdd(buttonNoUPtr_.get());
        }

        if (popupInfo_.Buttons() & ResponseTypes::Cancel)
        {
            buttonCancelUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sCancel",
                (StageRegionLeft() + innerRegion_.left + innerRegion_.width)
                    - (innerRegion_.width / 3.0f),
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
                    "Cancel", popupInfo_.ButtonColor()),
                this);

            EntityAdd(buttonCancelUPtr_.get());
        }

        if (popupInfo_.Buttons() & ResponseTypes::Continue)
        {
            auto const MIDDLE{ StageRegionLeft() + innerRegion_.left
                               + (innerRegion_.width * 0.5f) };

            buttonContinueUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sContinue",
                MIDDLE - 30.0f,
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
                    "Continue", popupInfo_.ButtonColor()),
                this);

            if (popupInfo_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE
                                     - (buttonContinueUPtr_->GetEntityRegion().width * 0.5f) };

                auto const POS_TOP{ buttonContinueUPtr_->GetEntityPos().y };

                buttonContinueUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonContinueUPtr_.get());
        }

        if (popupInfo_.Buttons() & ResponseTypes::Okay)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left + (innerRegion_.width * 0.5f));

            buttonOkayUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sOkay",
                MIDDLE - 50.0f,
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
                    "Okay", popupInfo_.ButtonColor()),
                this);

            if (popupInfo_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE - (buttonOkayUPtr_->GetEntityRegion().width * 0.5f)
                                     - 10.0f };

                auto const POS_TOP{ buttonOkayUPtr_->GetEntityPos().y };

                buttonOkayUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonOkayUPtr_.get());
        }

        if (popupInfo_.Buttons() & ResponseTypes::Select)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left + (innerRegion_.width * 0.5f));

            buttonSelectUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sSelect",
                MIDDLE - 100.0f,
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
                    "Select", popupInfo_.ButtonColor()),
                this);

            if (popupInfo_.Image() == PopupImage::Custom)
            {
                auto const POS_LEFT{ MIDDLE - (buttonSelectUPtr_->GetEntityRegion().width * 0.5f)
                                     - 10.0f };

                auto const POS_TOP{ buttonSelectUPtr_->GetEntityPos().y };

                buttonSelectUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
            }

            EntityAdd(buttonSelectUPtr_.get());
        }
    }

    void PopupStageBase::SetupTextRegion()
    {
        textRegion_.left = StageRegionLeft() + innerRegion_.left;
        textRegion_.top = StageRegionTop() + innerRegion_.top;
        textRegion_.width = innerRegion_.width;

        auto const TEXT_TO_BUTTON_SPACER{ 12.0f }; // found by experiment
        textRegion_.height = (innerRegion_.height - ButtonTextHeight()) - TEXT_TO_BUTTON_SPACER;
    }

    void PopupStageBase::SetupText()
    {
        sf::FloatRect tempRect(textRegion_);
        tempRect.height = 0.0f;

        textRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStage's", popupInfo_.TextInfo(), tempRect, sfml_util::IStagePtr_t(this));

        if ((textRegionUPtr_->GetEntityRegion().top + textRegionUPtr_->GetEntityRegion().height)
            > (textRegion_.top + textRegion_.height))
        {
            tempRect.height = textRegion_.height;

            textRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "PopupStage's", popupInfo_.TextInfo(), tempRect, sfml_util::IStagePtr_t(this));
        }
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
            sfml_util::GradientInfo(
                sf::Color::Black, 0, sfml_util::Side::Top, sf::Color(0, 0, 0, 20)));
    }

    void PopupStageBase::SetupAccentSprite()
    {
        if (popupInfo_.WillAddRandImage())
        {
            PopupManager::Instance()->LoadRandomAccentImage(accentTexture1_);
            accentSprite1_.setTexture(accentTexture1_, true);

            sfml_util::CenterAndScaleSpriteToFit(
                accentSprite1_, textRegion_, ACCENT_IMAGE_SCALEDOWN_RATIO_);

            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }

    void PopupStageBase::SetupSliderbar()
    {
        sliderbarPosTop_ = (buttonVertPos_ - (ButtonTextHeight() * 3.0f));
    }

    void PopupStageBase::SetupRedXImage()
    {
        sfml_util::LoadTexture(
            xSymbolTexture_, game::GameDataFile::Instance()->GetMediaPath("media-images-misc-x"));

        xSymbolSprite_.setTexture(xSymbolTexture_);

        // this color found by experiment to look good on all popups
        xSymbolSprite_.setColor(sf::Color(255, 0, 0, 127));
    }

    const sf::IntRect
        PopupStageBase::BackgroundImageRect(const PopupImage::Enum PI, const float SCALE) const
    {
        switch (PI)
        {
            case PopupImage::Banner:
            {
                return sfml_util::ScaleRectCopy(PopupManager::Instance()->Rect_Banner(), SCALE);
            }

            case PopupImage::Regular:
            {
                return sfml_util::ScaleRectCopy(PopupManager::Instance()->Rect_Regular(), SCALE);
            }

            case PopupImage::RegularSidebar:
            {
                return sfml_util::ScaleRectCopy(
                    PopupManager::Instance()->Rect_RegularSidebar(), SCALE);
            }

            case PopupImage::Large:
            {
                return sfml_util::ScaleRectCopy(PopupManager::Instance()->Rect_Large(), SCALE);
            }

            case PopupImage::LargeSidebar:
            {
                return sfml_util::ScaleRectCopy(
                    PopupManager::Instance()->Rect_LargeSidebar(), SCALE);
            }

            case PopupImage::Spellbook:
            case PopupImage::MusicSheet:
            {
                std::ostringstream ss;
                ss << "popup::PopupStageBase::BackgroundImageRect(" << PI
                   << ") was given Spellbook or MusicSheet, which are fullscreen"
                   << " popups and have no rects to setup.";

                throw std::invalid_argument(ss.str());
            }
            case PopupImage::Custom:
            case PopupImage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "popup::PopupStageBase::BackgroundImageRect(" << PI << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    float PopupStageBase::calcBackgroundImageScale(const PopupImage::Enum E) const
    {
        // These values found by experiment to look good at various resolutions.
        switch (E)
        {
            case PopupImage::Banner:
            {
                return sfml_util::MapByRes(0.7f, 2.0f);
            }

            case PopupImage::Regular:
            case PopupImage::RegularSidebar:
            {
                return sfml_util::MapByRes(0.85f, 3.5f);
            }

            case PopupImage::Large:
            case PopupImage::LargeSidebar:
            {
                // If you change this, then make sure the character_selection
                // and image_selection popups still look right.
                return sfml_util::MapByRes(1.0f, 4.75f);
            }

            case PopupImage::Custom: // Custom popups are custom sized so no value needed here.
            case PopupImage::Spellbook: // Spellbook and MusicSheet are scale themselves to fit the
                                        // screen.
            case PopupImage::MusicSheet:
            case PopupImage::Count:
            default:
            {
                return 1.0f;
            }
        }
    }
} // namespace popup
} // namespace heroespath
