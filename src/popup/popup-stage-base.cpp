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

#include "gui/display.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/random.hpp"
#include "popup/popup-manager.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    // This value was found by experiment to be faded enough to avoid interfering with foreground.
    const sf::Uint8 PopupStageBase::ACCENT_IMAGE_ALPHA_ { 20 };

    PopupStageBase::PopupStageBase(const PopupInfo & POPUP_INFO)
        : StageBase(
            POPUP_INFO.Name() + "_PopupStage", { gui::GuiFont::Handwriting, gui::GuiFont::Number })
        , popupInfo_(POPUP_INFO)
        , BACKGROUND_IMAGE_CONFIG_KEY_(PopupImage::ImageConfigKey(POPUP_INFO.Image()))
        , TEXT_BUTTON_VERT_PAD_()
        , TEXT_BUTTON_REGION_HEIGHT_(+(2.0f * TEXT_BUTTON_VERT_PAD_))
        , sliderbarUPtr_()
        , accent1CachedTextureOpt_()
        , accentSprite1_()
        , accent2CachedTextureOpt_()
        , accentSprite2_()
        , selection_(-1) // any negative value will work here
        , xSymbolSprite_()
        , willShowXImage_(false)
        , textRegionUPtr_()
        , backgroundTexture_(BACKGROUND_IMAGE_CONFIG_KEY_)
        , backgroundSprite_(
              backgroundTexture_.Get(),
              misc::ConfigFile::Instance()->GetTextureRect(BACKGROUND_IMAGE_CONFIG_KEY_))
        , contentRegion_()
        , buttonRegion_()
        , buttonSelectUPtr_()
        , buttonYesUPtr_()
        , buttonNoUPtr_()
        , buttonCancelUPtr_()
        , buttonContinueUPtr_()
        , buttonOkayUPtr_()
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
            ClosePopup(PopupButtons::Yes);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "yes text button clicked");
        }
        else if (&PACKET == buttonNoUPtr_.get())
        {
            ClosePopup(PopupButtons::No);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "no text button clicked");
        }
        else if (&PACKET == buttonCancelUPtr_.get())
        {
            ClosePopup(PopupButtons::Cancel);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "cancel text button clicked");
        }
        else if (&PACKET == buttonContinueUPtr_.get())
        {
            ClosePopup(PopupButtons::Continue);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "continue text button clicked");
        }
        else if (&PACKET == buttonOkayUPtr_.get())
        {
            ClosePopup(PopupButtons::Okay);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "okay text button clicked");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "text button callback NOT HANDLED");
    }

    void PopupStageBase::Setup()
    {
        // TODO move this sfx play elsewhere so there isn't so much of a delay
        gui::SoundManager::Instance()->SoundEffectPlay(popupInfo_.SoundEffect());

        SetupBackgroundImage();
        SetupRegions();
        SetupButtons();
        SetupText();
        SetupAccentSprite();
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

        if (willShowXImage_)
        {
            target.draw(xSymbolSprite_, states);
        }
    }

    bool PopupStageBase::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (popupInfo_.Buttons() & PopupButtons::Continue)
        {
            if ((KEY_EVENT.code == sf::Keyboard::C) || (KEY_EVENT.code == sf::Keyboard::Space)
                || (KEY_EVENT.code == sf::Keyboard::Return)
                || (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                ClosePopup(PopupButtons::Continue);
                return true;
            }
        }

        if (popupInfo_.Buttons() & PopupButtons::Okay)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Space) || (KEY_EVENT.code == sf::Keyboard::O)
                || (KEY_EVENT.code == sf::Keyboard::Return)
                || (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                ClosePopup(PopupButtons::Okay);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & PopupButtons::Yes) && (KEY_EVENT.code == sf::Keyboard::Y))
        {
            ClosePopup(PopupButtons::Yes);
            return true;
        }

        if ((popupInfo_.Buttons() & PopupButtons::No)
            && ((KEY_EVENT.code == sf::Keyboard::N) || (KEY_EVENT.code == sf::Keyboard::Escape)))
        {
            ClosePopup(PopupButtons::No);
            return true;
        }

        if (popupInfo_.Buttons() & PopupButtons::Cancel)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape)
                || ((KEY_EVENT.code == sf::Keyboard::C) && WillPressingCKeyClosePopup())
                || ((KEY_EVENT.code == sf::Keyboard::Return)
                    && (popupInfo_.Buttons() == PopupButtons::Cancel)))
            {
                ClosePopup(PopupButtons::Cancel);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & PopupButtons::Select)
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
                ClosePopup(PopupButtons::Continue, 0, false);
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
            ClosePopup(PopupButtons::Select, static_cast<std::size_t>(selection_));
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

    const gui::ImageOptions PopupStageBase::AccentImageOptions() const
    {
        return gui::ImageOptions(
            ((misc::RandomBool()) ? gui::ImageOpt::Default : gui::ImageOpt::FlipHoriz));
    }

    const sf::Vector2f PopupStageBase::BackgroundImageLocalSize() const
    {
        return sf::Vector2f { backgroundTexture_.Get().getSize() };
    }

    void PopupStageBase::SetupBackgroundImage()
    {
        const auto HEGHT { sfutil::ScreenRatioToPixelsVert(
            PopupImage::ScreenSizeRatio(popupInfo_.Image())) };

        const auto SCALE { (HEGHT / backgroundSprite_.getLocalBounds().height) };

        backgroundSprite_.setScale(SCALE, SCALE);
        sfutil::Center(backgroundSprite_);
    }

    void PopupStageBase::SetupRegions()
    {
        StageRegion(backgroundSprite_.getGlobalBounds());

        contentRegion_ = MakeBackgroundImageContentRegion(popupInfo_.Image(), false, StageRegion());

        const auto BUTTON_HEIGHT_MAX { CalcTextButtonMaxHeight() };
        const auto TEXT_TO_BUTTON_VERT_PAD { sfutil::ScreenRatioToPixelsVert(0.006f) };
        buttonRegion_.height = (BUTTON_HEIGHT_MAX + TEXT_TO_BUTTON_VERT_PAD);
        contentRegion_.height -= buttonRegion_.height;

        buttonRegion_.top = sfutil::Bottom(contentRegion_);
        buttonRegion_.left = contentRegion_.left;
        buttonRegion_.width = contentRegion_.width;
    }

    const sf::FloatRect PopupStageBase::MakeBackgroundImageContentRegion(
        const PopupImage::Enum IMAGE,
        const bool IS_LEFT_IMAGE,
        const sf::FloatRect & BG_REGION) const
    {
        const auto BG_SIZE_V { sfutil::Size(BG_REGION) };

        const auto BG_CONTENT_REGION_RATIOS { PopupImage::ContentRegionRatios(
            IMAGE, IS_LEFT_IMAGE) };

        sf::FloatRect rect;
        rect.left = BG_REGION.left + (BG_SIZE_V.x * BG_CONTENT_REGION_RATIOS.left);
        rect.top = BG_REGION.top + (BG_SIZE_V.y * BG_CONTENT_REGION_RATIOS.top);
        rect.width = (BG_SIZE_V.x * BG_CONTENT_REGION_RATIOS.width);
        rect.height = (BG_SIZE_V.y * BG_CONTENT_REGION_RATIOS.height);

        return rect;
    }

    void PopupStageBase::SetupRegionsForFullScreen(const float BORDER_RATIO)
    {
        const auto REGION { gui::Margins(
                                sfutil::ScreenRatioToPixelsHoriz(BORDER_RATIO),
                                sfutil::ScreenRatioToPixelsVert(BORDER_RATIO))
                                .ApplyShrinkCopy(gui::Display::Instance()->FullScreenRect()) };

        StageRegion(REGION);
        contentRegion_ = REGION;

        buttonRegion_.left = REGION.left;
        buttonRegion_.top = sfutil::Bottom(REGION);
        buttonRegion_.width = REGION.width;
        buttonRegion_.height = 0.0f;

        sfutil::SetSizeAndPos(backgroundSprite_, REGION);
    }

    void PopupStageBase::SetupButtons()
    {
        // this is used for horizontal positioning
        const auto TWELF_CONTENT_WIDTH { (contentRegion_.width * 0.0833f) };

        if (popupInfo_.Buttons() & PopupButtons::Yes)
        {
            MakeTextButton(
                buttonYesUPtr_, "Yes", (contentRegion_.left + (TWELF_CONTENT_WIDTH * 3.0f)));
        }

        if (popupInfo_.Buttons() & PopupButtons::No)
        {
            MakeTextButton(
                buttonNoUPtr_, "No", (contentRegion_.left + (TWELF_CONTENT_WIDTH * 5.0f)));
        }

        if (popupInfo_.Buttons() & PopupButtons::Cancel)
        {
            MakeTextButton(
                buttonCancelUPtr_, "Cancel", (contentRegion_.left + (TWELF_CONTENT_WIDTH * 8.5f)));
        }

        if (popupInfo_.Buttons() & PopupButtons::Continue)
        {
            MakeTextButton(
                buttonContinueUPtr_,
                "Continue",
                (contentRegion_.left + (TWELF_CONTENT_WIDTH * 6.5f)));
        }

        if (popupInfo_.Buttons() & PopupButtons::Okay)
        {
            MakeTextButton(buttonOkayUPtr_, "Okay", -1.0f); // any negative will work here (center)
        }

        if (popupInfo_.Buttons() & PopupButtons::Select)
        {
            MakeTextButton(buttonSelectUPtr_, "Select", -1.0f);
        }
    }

    void PopupStageBase::SetupText()
    {
        textRegionUPtr_ = std::make_unique<gui::TextRegion>(
            GetStageName() + "'s",
            popupInfo_.TextInfo(),
            contentRegion_,
            gui::BoxEntityInfo(),
            stage::IStagePtr_t(this));
    }

    void PopupStageBase::SetupAccentSprite()
    {
        const auto ACCENT_IMAGE_PATH { PopupManager::Instance()->RandomAccentImagePath() };

        if (popupInfo_.WillAddRandImage() && (ACCENT_IMAGE_PATH.empty() == false))
        {
            accent1CachedTextureOpt_
                = gui::CachedTexture(PathWrapper(ACCENT_IMAGE_PATH), AccentImageOptions());

            accentSprite1_.setTexture(accent1CachedTextureOpt_->Get(), true);

            sfutil::FitAndCenterTo(accentSprite1_, contentRegion_);

            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }

    void PopupStageBase::SetupRedXImage()
    {
        xSymbolCachedTextureOpt_ = gui::CachedTexture("media-image-misc-x");
        xSymbolSprite_.setTexture(xSymbolCachedTextureOpt_->Get());
        xSymbolSprite_.setColor(sf::Color(255, 0, 0, 127));
    }

    void PopupStageBase::ClosePopup(
        const PopupButtons::Enum BUTTON, const std::size_t SELECTION, const bool WILL_PLAY_SFX)
    {
        EndKeepAliveTimer();

        if (WILL_PLAY_SFX)
        {
            PlayValidKeypressSoundEffect();
        }

        RemovePopup(BUTTON, SELECTION);
    }

    void PopupStageBase::MakeTextButton(
        gui::TextButtonUPtr_t & buttonUPtr, const std::string & NAME, const float POS_LEFT_ORIG)
    {
        buttonUPtr = std::make_unique<gui::TextButton>(
            GetStageName() + "'s_" + NAME,
            sf::Vector2f(),
            gui::MouseTextInfo::Make_PopupButtonSet(NAME, popupInfo_.ButtonColor()),
            gui::TextButton::Callback_t::IHandlerPtr_t(this));

        auto posLeft { POS_LEFT_ORIG };
        if (posLeft < 0.0f)
        {
            posLeft
                = (sfutil::CenterOfHoriz(buttonRegion_)
                   - (buttonUPtr->GetEntityRegion().width * 0.5f));
        }

        const auto POS_TOP { (
            sfutil::CenterOfVert(buttonRegion_) - (buttonUPtr->GetEntityRegion().height * 0.5f)) };

        buttonUPtr->SetEntityPos(posLeft, POS_TOP);

        EntityAdd(buttonUPtr);
    }

    const sf::FloatRect PopupStageBase::ActualTextRegion() const
    {
        if (textRegionUPtr_)
        {
            return textRegionUPtr_->ActualTextRegion();
        }
        else
        {
            return sf::FloatRect();
        }
    }

    float PopupStageBase::CalcTextButtonMaxHeight() const
    {
        const auto MOUSE_TEXT_INFO { gui::MouseTextInfo::Make_PopupButtonSet(
            "Y", popupInfo_.ButtonColor()) };

        const gui::Text TEMP_TEXT_OBJ { MOUSE_TEXT_INFO.up.text,
                                        MOUSE_TEXT_INFO.up.font_letters,
                                        MOUSE_TEXT_INFO.up.size };

        return TEMP_TEXT_OBJ.getGlobalBounds().height;
    }

} // namespace popup
} // namespace heroespath
