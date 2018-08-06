// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-base.cpp
//
#include "popup-stage-base.hpp"

#include "creature/name-info.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "misc/random.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace popup
{

    // This value was found by experiment to be faded enough to avoid interfering with foreground.
    const sf::Uint8 PopupStageBase::ACCENT_IMAGE_ALPHA_ { 19 };

    // This value was found by experiment to make a good looking empty border around the image.
    const float PopupStageBase::ACCENT_IMAGE_SCALEDOWN_RATIO_ { 0.85f };

    PopupStageBase::PopupStageBase(const PopupInfo & POPUP_INFO)
        : Stage(POPUP_INFO.Name() + "_PopupStage", {}, false)
        , popupInfo_(POPUP_INFO)
        , innerRegion_()
        , backgroundTexture_(PopupManager::Instance()->BackgroundImagePath(POPUP_INFO.Image()))
        , textRegionUPtr_()
        , textRegion_()
        , buttonSelectUPtr_()
        , buttonYesUPtr_()
        , buttonNoUPtr_()
        , buttonCancelUPtr_()
        , buttonContinueUPtr_()
        , buttonOkayUPtr_()
        , sliderbarUPtr_()
        , accent1CachedTextureOpt_()
        , accentSprite1_()
        , accent2CachedTextureOpt_()
        , accentSprite2_()
        , sliderbarPosTop_(0.0f)
        , selection_(-1) // any negative value will work here
        , xSymbolSprite_()
        , willShowXImage_(false)
        , backgroundSprite_(backgroundTexture_.Get())
        , buttonTextHeight_(0.0f)
        , buttonVertPos_(0.0f)
        , xSymbolCachedTextureOpt_()
        , keepAliveTimerSec_(POPUP_INFO.KeepAliveSec())
    {}

    PopupStageBase::~PopupStageBase() { Stage::ClearAllEntities(); }

    bool PopupStageBase::HandleCallback(const sfml_util::gui::SliderBar::Callback_t::PacketPtr_t &)
    {
        return false;
    }

    bool PopupStageBase::HandleCallback(
        const sfml_util::gui::TextButton::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        if (PACKET_PTR == buttonSelectUPtr_.get())
        {
            return HandleSelect();
        }
        else if (PACKET_PTR == buttonYesUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Yes);
            return true;
        }
        else if (PACKET_PTR == buttonNoUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::No);
            return true;
        }
        else if (PACKET_PTR == buttonCancelUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Cancel);
            return true;
        }
        else if (PACKET_PTR == buttonContinueUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Continue);
            return true;
        }
        else if (PACKET_PTR == buttonOkayUPtr_.get())
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
        SetupOuterAndInnerRegion();
        SetupVariousButtonPositionValues();
        SetupButtons();
        SetupTextRegion();
        SetupText();
        SetupAccentSprite();
        SetupSliderbar();
        SetupRedXImage();
    }

    void PopupStageBase::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundSprite_, STATES);

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

    void PopupStageBase::SetupOuterAndInnerRegion()
    {
        auto const BG_IMAGE_SCALE { calcBackgroundImageScale(popupInfo_.Image()) };
        innerRegion_ = sf::FloatRect(BackgroundImageRect(popupInfo_.Image(), BG_IMAGE_SCALE));
        backgroundSprite_.setScale(BG_IMAGE_SCALE, BG_IMAGE_SCALE);
        sfml_util::Center(backgroundSprite_);
        StageRegionSet(backgroundSprite_.getGlobalBounds());
    }

    void PopupStageBase::SetupForFullScreenWithBorderRatio(const float BORDER_RATIO)
    {
        const auto REGION { sfml_util::Margins(
                                sfml_util::ScreenRatioToPixelsHoriz(BORDER_RATIO),
                                sfml_util::ScreenRatioToPixelsVert(BORDER_RATIO))
                                .ApplyShrink(sfml_util::Display::Instance()->FullScreenRect()) };

        StageRegionSet(REGION);
        innerRegion_ = REGION;
        sfml_util::SetSizeAndPos(backgroundSprite_, REGION);
    }

    void PopupStageBase::SetupVariousButtonPositionValues()
    {
        creature::NameInfo creatureNameInfo;

        auto const TEMP_MOUSE_TEXT_INFO { sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
            creatureNameInfo.LargestLetterString(), popupInfo_.ButtonColor()) };

        const sf::Text TEMP_TEXT_OBJ { TEMP_MOUSE_TEXT_INFO.up.text,
                                       *TEMP_MOUSE_TEXT_INFO.up.font_ptr_opt.value(),
                                       TEMP_MOUSE_TEXT_INFO.up.char_size };

        buttonTextHeight_ = TEMP_TEXT_OBJ.getGlobalBounds().height;

        auto const POPUPBUTTON_TEXT_BOTTOM_MARGIN { sfml_util::MapByRes(30.0f, 90.0f) };
        auto const BUTTON_HEIGHT { ButtonTextHeight() + POPUPBUTTON_TEXT_BOTTOM_MARGIN };

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
                sfml_util::gui::TextButton::Callback_t::IHandlerPtr_t(this));

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
                sfml_util::gui::TextButton::Callback_t::IHandlerPtr_t(this));

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
                sfml_util::gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonCancelUPtr_.get());
        }

        if (popupInfo_.Buttons() & ResponseTypes::Continue)
        {
            auto const MIDDLE { StageRegionLeft() + innerRegion_.left
                                + (innerRegion_.width * 0.5f) };

            buttonContinueUPtr_ = std::make_unique<sfml_util::gui::TextButton>(
                "PopupStage'sContinue",
                MIDDLE - 30.0f,
                buttonVertPos_,
                sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(
                    "Continue", popupInfo_.ButtonColor()),
                sfml_util::gui::TextButton::Callback_t::IHandlerPtr_t(this));

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
                sfml_util::gui::TextButton::Callback_t::IHandlerPtr_t(this));

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
                sfml_util::gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonSelectUPtr_.get());
        }
    }

    void PopupStageBase::SetupTextRegion()
    {
        textRegion_.left = StageRegionLeft() + innerRegion_.left;
        textRegion_.top = StageRegionTop() + innerRegion_.top;
        textRegion_.width = innerRegion_.width;

        auto const TEXT_TO_BUTTON_SPACER { 12.0f }; // found by experiment
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

    void PopupStageBase::SetupAccentSprite()
    {
        if (popupInfo_.WillAddRandImage())
        {
            accent1CachedTextureOpt_ = PopupManager::Instance()->LoadRandomAccentImage();
            accentSprite1_.setTexture(accent1CachedTextureOpt_->Get(), true);

            sfml_util::FitAndReCenter(
                accentSprite1_,
                sfml_util::ScaleAndReCenterCopy(textRegion_, ACCENT_IMAGE_SCALEDOWN_RATIO_));

            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }

    void PopupStageBase::SetupSliderbar()
    {
        sliderbarPosTop_ = (buttonVertPos_ - (ButtonTextHeight() * 3.0f));
    }

    void PopupStageBase::SetupRedXImage()
    {
        xSymbolCachedTextureOpt_ = sfml_util::CachedTexture("media-images-misc-x");
        xSymbolSprite_.setTexture(xSymbolCachedTextureOpt_->Get());
        xSymbolSprite_.setColor(sf::Color(255, 0, 0, 127));
    }

    const sf::IntRect
        PopupStageBase::BackgroundImageRect(const PopupImage::Enum PI, const float SCALE) const
    {
        switch (PI)
        {
            case PopupImage::Banner:
            {
                return sfml_util::ScaleRectLinearCopy(
                    PopupManager::Instance()->Rect_Banner(), SCALE);
            }

            case PopupImage::Regular:
            {
                return sfml_util::ScaleRectLinearCopy(
                    PopupManager::Instance()->Rect_Regular(), SCALE);
            }

            case PopupImage::RegularSidebar:
            {
                return sfml_util::ScaleRectLinearCopy(
                    PopupManager::Instance()->Rect_RegularSidebar(), SCALE);
            }

            case PopupImage::Large:
            {
                return sfml_util::ScaleRectLinearCopy(
                    PopupManager::Instance()->Rect_Large(), SCALE);
            }

            case PopupImage::LargeSidebar:
            {
                return sfml_util::ScaleRectLinearCopy(
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
