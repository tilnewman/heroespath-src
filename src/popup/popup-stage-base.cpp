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
#include "gui/display.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/random.hpp"
#include "popup/popup-manager.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    // This value was found by experiment to be faded enough to avoid interfering with foreground.
    const sf::Uint8 PopupStageBase::ACCENT_IMAGE_ALPHA_ { 19 };

    // This value was found by experiment to make a good looking empty border around the image.
    const float PopupStageBase::ACCENT_IMAGE_SCALEDOWN_RATIO_ { 0.85f };

    PopupStageBase::PopupStageBase(const PopupInfo & POPUP_INFO)
        : StageBase(
            POPUP_INFO.Name() + "_PopupStage",
            { gui::GuiFont::Handwriting, gui::GuiFont::Number },
            false)
        , popupInfo_(POPUP_INFO)
        , innerRegion_()
        , backgroundTexture_(
              PathWrapper(PopupManager::Instance()->BackgroundImagePath(POPUP_INFO.Image())))
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

    PopupStageBase::~PopupStageBase() { StageBase::ClearAllEntities(); }

    const std::string PopupStageBase::HandleCallback(
        const gui::TextButton::Callback_t::Packet_t & PACKET,
        const std::string & PACKET_DESCRIPTION)
    {
        if (&PACKET == buttonSelectUPtr_.get())
        {
            if (HandleSelect())
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "select text button clicked with a valid selection and this popup will be "
                    "removed");
            }
            else
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "select text button clicked with an invalid selection so no action taken");
            }
        }
        else if (&PACKET == buttonYesUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::Yes);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "yes text button clicked");
        }
        else if (&PACKET == buttonNoUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::No);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "no text button clicked");
        }
        else if (&PACKET == buttonCancelUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::Cancel);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "cancel text button clicked");
        }
        else if (&PACKET == buttonContinueUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::Continue);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "continue text button clicked");
        }
        else if (&PACKET == buttonOkayUPtr_.get())
        {
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::Okay);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "okay text button clicked");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "text button callback NOT HANDLED");
    }

    void PopupStageBase::Setup()
    {
        gui::SoundManager::Instance()->SoundEffectPlay(popupInfo_.SoundEffect());
        SetupOuterAndInnerRegion();
        SetupVariousButtonPositionValues();
        SetupButtons();
        SetupTextRegion();
        SetupText();
        SetupAccentSprite();
        SetupSliderbar();
        SetupRedXImage();
    }

    void PopupStageBase::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(backgroundSprite_, states);

        if (popupInfo_.WillAddRandImage())
        {
            target.draw(accentSprite1_, states);
        }

        textRegionUPtr_->draw(target, states);
    }

    void PopupStageBase::DrawRedX(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (willShowXImage_)
        {
            target.draw(xSymbolSprite_, states);
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
                RemovePopup(ResponseTypes::Continue);
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
                RemovePopup(ResponseTypes::Okay);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & ResponseTypes::Yes) && (KEY_EVENT.code == sf::Keyboard::Y))
        {
            EndKeepAliveTimer();
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::Yes);
            return true;
        }

        if ((popupInfo_.Buttons() & ResponseTypes::No)
            && ((KEY_EVENT.code == sf::Keyboard::N) || (KEY_EVENT.code == sf::Keyboard::Escape)))
        {
            PlayValidKeypressSoundEffect();
            RemovePopup(ResponseTypes::No);
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
                RemovePopup(ResponseTypes::Cancel);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & ResponseTypes::Select)
            && ((KEY_EVENT.code == sf::Keyboard::Return) || (KEY_EVENT.code == sf::Keyboard::S)))
        {
            // sound effects played by HandleSelect()
            return HandleSelect();
        }

        return StageBase::KeyRelease(KEY_EVENT);
    }

    void PopupStageBase::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        if (keepAliveTimerSec_ > 0.0f)
        {
            keepAliveTimerSec_ -= ELAPSED_TIME_SECONDS;
            if (keepAliveTimerSec_ < 0.0f)
            {
                RemovePopup(ResponseTypes::Continue);
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
            RemovePopup(ResponseTypes::Select, static_cast<std::size_t>(selection_));
            return true;
        }
    }

    void PopupStageBase::PlayValidKeypressSoundEffect() const
    {
        gui::SoundManager::Instance()->PlaySfx_Keypress();
    }

    void PopupStageBase::PlayInvalidKeypressSoundEffect() const
    {
        gui::SoundManager::Instance()->PlaySfx_Reject();
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
        const auto BG_IMAGE_SCALE { calcBackgroundImageScale(popupInfo_.Image()) };
        innerRegion_ = sf::FloatRect(BackgroundImageRect(popupInfo_.Image(), BG_IMAGE_SCALE));
        backgroundSprite_.setScale(BG_IMAGE_SCALE, BG_IMAGE_SCALE);
        sfutil::Center(backgroundSprite_);
        StageRegion(backgroundSprite_.getGlobalBounds());
    }

    void PopupStageBase::SetupForFullScreenWithBorderRatio(const float BORDER_RATIO)
    {
        const auto REGION { gui::Margins(
                                sfutil::ScreenRatioToPixelsHoriz(BORDER_RATIO),
                                sfutil::ScreenRatioToPixelsVert(BORDER_RATIO))
                                .ApplyShrinkCopy(gui::Display::Instance()->FullScreenRect()) };

        StageRegion(REGION);
        innerRegion_ = REGION;
        sfutil::SetSizeAndPos(backgroundSprite_, REGION);
    }

    void PopupStageBase::SetupVariousButtonPositionValues()
    {
        creature::NameInfo creatureNameInfo;

        const auto TEMP_MOUSE_TEXT_INFO { gui::MouseTextInfo::Make_PopupButtonSet(
            creatureNameInfo.LargestLetterString(), popupInfo_.ButtonColor()) };

        const gui::Text TEMP_TEXT_OBJ { TEMP_MOUSE_TEXT_INFO.up.text,
                                        TEMP_MOUSE_TEXT_INFO.up.font_letters,
                                        TEMP_MOUSE_TEXT_INFO.up.size };

        buttonTextHeight_ = TEMP_TEXT_OBJ.getGlobalBounds().height;

        const auto POPUPBUTTON_TEXT_BOTTOM_MARGIN { sfutil::MapByRes(30.0f, 90.0f) };
        const auto BUTTON_HEIGHT { ButtonTextHeight() + POPUPBUTTON_TEXT_BOTTOM_MARGIN };

        buttonVertPos_
            = (StageRegion().top + innerRegion_.top + innerRegion_.height) - BUTTON_HEIGHT;
    }

    void PopupStageBase::SetupButtons()
    {
        if (popupInfo_.Buttons() & ResponseTypes::Yes)
        {
            buttonYesUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sYes",
                StageRegion().left + innerRegion_.left + (innerRegion_.width / 4.0f) - 50.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Yes", popupInfo_.ButtonColor()),
                gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonYesUPtr_);
        }

        if (popupInfo_.Buttons() & ResponseTypes::No)
        {
            buttonNoUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sNo",
                StageRegion().left + innerRegion_.left + (2.0f * (innerRegion_.width / 4.0f))
                    - 40.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("No", popupInfo_.ButtonColor()),
                gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonNoUPtr_);
        }

        if (popupInfo_.Buttons() & ResponseTypes::Cancel)
        {
            buttonCancelUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sCancel",
                (StageRegion().left + innerRegion_.left + innerRegion_.width)
                    - (innerRegion_.width / 3.0f),
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Cancel", popupInfo_.ButtonColor()),
                gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonCancelUPtr_);
        }

        if (popupInfo_.Buttons() & ResponseTypes::Continue)
        {
            const auto MIDDLE { StageRegion().left + innerRegion_.left
                                + (innerRegion_.width * 0.5f) };

            buttonContinueUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sContinue",
                MIDDLE - 30.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Continue", popupInfo_.ButtonColor()),
                gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonContinueUPtr_);
        }

        if (popupInfo_.Buttons() & ResponseTypes::Okay)
        {
            const float MIDDLE(
                StageRegion().left + innerRegion_.left + (innerRegion_.width * 0.5f));

            buttonOkayUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sOkay",
                MIDDLE - 50.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Okay", popupInfo_.ButtonColor()),
                gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonOkayUPtr_);
        }

        if (popupInfo_.Buttons() & ResponseTypes::Select)
        {
            const float MIDDLE(
                StageRegion().left + innerRegion_.left + (innerRegion_.width * 0.5f));

            buttonSelectUPtr_ = std::make_unique<gui::TextButton>(
                "PopupStage'sSelect",
                MIDDLE - 100.0f,
                buttonVertPos_,
                gui::MouseTextInfo::Make_PopupButtonSet("Select", popupInfo_.ButtonColor()),
                gui::TextButton::Callback_t::IHandlerPtr_t(this));

            EntityAdd(buttonSelectUPtr_);
        }
    }

    void PopupStageBase::SetupTextRegion()
    {
        textRegion_.left = StageRegion().left + innerRegion_.left;
        textRegion_.top = StageRegion().top + innerRegion_.top;
        textRegion_.width = innerRegion_.width;

        const auto TEXT_TO_BUTTON_SPACER { 12.0f }; // found by experiment
        textRegion_.height = (innerRegion_.height - ButtonTextHeight()) - TEXT_TO_BUTTON_SPACER;
    }

    void PopupStageBase::SetupText()
    {
        sf::FloatRect tempRect(textRegion_);
        tempRect.height = 0.0f;

        textRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "PopupStage's", popupInfo_.TextInfo(), tempRect, stage::IStagePtr_t(this));

        if ((textRegionUPtr_->GetEntityRegion().top + textRegionUPtr_->GetEntityRegion().height)
            > (textRegion_.top + textRegion_.height))
        {
            tempRect.height = textRegion_.height;

            textRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "PopupStage's", popupInfo_.TextInfo(), tempRect, stage::IStagePtr_t(this));
        }
    }

    void PopupStageBase::SetupAccentSprite()
    {
        if (popupInfo_.WillAddRandImage())
        {
            accent1CachedTextureOpt_ = PopupManager::Instance()->LoadRandomAccentImage();
            accentSprite1_.setTexture(accent1CachedTextureOpt_->Get(), true);

            sfutil::FitAndReCenter(
                accentSprite1_,
                sfutil::ScaleAndReCenterCopy(textRegion_, ACCENT_IMAGE_SCALEDOWN_RATIO_));

            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }

    void PopupStageBase::SetupSliderbar()
    {
        sliderbarPosTop_ = (buttonVertPos_ - (ButtonTextHeight() * 3.0f));
    }

    void PopupStageBase::SetupRedXImage()
    {
        xSymbolCachedTextureOpt_ = gui::CachedTexture("media-images-misc-x");
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
                return sfutil::ScaleRectLinearCopy(PopupManager::Instance()->Rect_Banner(), SCALE);
            }

            case PopupImage::Regular:
            {
                return sfutil::ScaleRectLinearCopy(PopupManager::Instance()->Rect_Regular(), SCALE);
            }

            case PopupImage::RegularSidebar:
            {
                return sfutil::ScaleRectLinearCopy(
                    PopupManager::Instance()->Rect_RegularSidebar(), SCALE);
            }

            case PopupImage::Large:
            {
                return sfutil::ScaleRectLinearCopy(PopupManager::Instance()->Rect_Large(), SCALE);
            }

            case PopupImage::LargeSidebar:
            {
                return sfutil::ScaleRectLinearCopy(
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
                return sfutil::MapByRes(0.7f, 2.0f);
            }

            case PopupImage::Regular:
            case PopupImage::RegularSidebar:
            {
                return sfutil::MapByRes(0.85f, 3.5f);
            }

            case PopupImage::Large:
            case PopupImage::LargeSidebar:
            {
                // If you change this, then make sure the character_selection
                // and image_selection popups still look right.
                return sfutil::MapByRes(1.0f, 4.75f);
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
