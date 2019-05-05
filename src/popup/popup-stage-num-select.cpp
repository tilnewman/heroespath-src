// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-num-select.cpp
//
#include "popup-stage-num-select.hpp"

#include "gui/font-manager.hpp"
#include "gui/gui-images.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/strings.hpp"
#include "popup/popup-manager.hpp"
#include "sfutil/center.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    const int PopupStageNumberSelect::NUMBER_SELECT_INVALID_ { -1 }; // any negative will work here

    PopupStageNumberSelect::PopupStageNumberSelect(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , msgTextRegionUPtr_()
        , textEntryBoxUPtr_()
        , willSliderbarUpdate_(true)
        , willTextBoxUpdate_(true)
    {}

    PopupStageNumberSelect::~PopupStageNumberSelect() = default;

    const std::string PopupStageNumberSelect::HandleCallback(
        const gui::SliderBar::Callback_t::Packet_t & PACKET, const std::string &)
    {
        const auto CURR_RATIO { PACKET.PositionRatio() };

        const auto CURR_VAL { popupInfo_.NumberSelMin()
                              + static_cast<std::size_t>(
                                    CURR_RATIO
                                    * static_cast<float>(
                                          popupInfo_.NumberSelMax() - popupInfo_.NumberSelMin())) };

        selection_ = static_cast<int>(CURR_VAL);

        if ((textEntryBoxUPtr_) && willSliderbarUpdate_)
        {
            willTextBoxUpdate_ = false;
            textEntryBoxUPtr_->SetText(std::to_string(CURR_VAL));
            willTextBoxUpdate_ = true;
            SetupMessageText("");
        }

        return "";
    }

    const std::string PopupStageNumberSelect::HandleCallback(
        const gui::TextEntryBox::Callback_t::Packet_t &, const std::string &)
    {
        selection_ = GetSelectNumber();

        if (willTextBoxUpdate_)
        {
            ProcessSelectNumber();
        }

        return "";
    }

    void PopupStageNumberSelect::Setup()
    {
        PopupStageBase::Setup();

        SetupMessageText("(type a number or use the slider below)");
        SetupTextEntryBox();
        SetupSliderbar();

        RemoveFocus();
        SetFocus(textEntryBoxUPtr_);
        textEntryBoxUPtr_->SetHasFocus(true);
    }

    void PopupStageNumberSelect::SetupTextEntryBox()
    {
        const auto TEXTENTRY_BOX_WIDTH { ContentRegion().width * 0.45f };

        const auto TEXTENTRY_BOX_POS_LEFT { (
            sfutil::CenterOfHoriz(ContentRegion()) - (TEXTENTRY_BOX_WIDTH * 0.5f)) };

        const auto TEXTENTRY_BOX_POS_TOP { (
            sfutil::Bottom(msgTextRegionUPtr_->GetEntityRegion())
            + sfutil::ScreenRatioToPixelsVert(0.08f)) };

        const auto TEXTENTRY_BOX_HEIGHT { sfutil::ScreenRatioToPixelsVert(0.08f) };

        const sf::FloatRect TEXTENTRY_REGION(
            TEXTENTRY_BOX_POS_LEFT,
            TEXTENTRY_BOX_POS_TOP,
            TEXTENTRY_BOX_WIDTH,
            TEXTENTRY_BOX_HEIGHT);

        const auto NUMBER_SEL_MIN_STR(std::to_string(popupInfo_.NumberSelMin()));

        const gui::TextInfo TEXTENTRY_TEXT_INFO(
            NUMBER_SEL_MIN_STR,
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Large(),
            sf::Color::White,
            gui::Justified::Left);

        gui::BoxEntityInfo boxInfo;
        boxInfo.SetupImage(gui::CachedTexture("media-image-background-tile-wood"));
        boxInfo.SetupBorder(true);
        boxInfo.focus_colors = gui::FocusColors(sf::Color::White);

        textEntryBoxUPtr_ = std::make_unique<gui::TextEntryBox>(
            gui::TextEntryBox::Callback_t::IHandlerPtr_t(this),
            GetStageName() + "'s",
            TEXTENTRY_REGION,
            TEXTENTRY_TEXT_INFO,
            sfutil::color::Light,
            boxInfo);

        textEntryBoxUPtr_->SetText(NUMBER_SEL_MIN_STR);
        EntityAdd(textEntryBoxUPtr_);
    }

    void PopupStageNumberSelect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);
        StageBase::draw(target, states);
    }

    bool PopupStageNumberSelect::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::S) || (KEY_EVENT.code == sf::Keyboard::Return))
        {
            if (ProcessSelectNumber())
            {
                PlayValidKeypressSoundEffect();
                RemovePopup(PopupButtons::Select, static_cast<std::size_t>(GetSelectNumber()));
                return true;
            }
            else
            {
                return false;
            }
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }

    void PopupStageNumberSelect::SetupMessageText(const std::string & MESSAGE)
    {
        const auto MESSAGE_TEXT_TO_USE { ((MESSAGE.empty()) ? " " : MESSAGE) };

        const gui::TextInfo INFO_TEXT_INFO(
            MESSAGE_TEXT_TO_USE,
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            gui::Justified::Center);

        if (msgTextRegionUPtr_)
        {
            EntityRemove(msgTextRegionUPtr_);
        }

        msgTextRegionUPtr_
            = std::make_unique<gui::TextRegion>(GetStageName() + "'sInfo", INFO_TEXT_INFO);

        EntityAdd(msgTextRegionUPtr_);

        const auto POS_LEFT { (
            sfutil::CenterOfHoriz(ContentRegion())
            - (msgTextRegionUPtr_->GetEntityRegion().width * 0.5f)) };

        const auto POS_TOP { (
            sfutil::Bottom(ActualTextRegion()) + sfutil::ScreenRatioToPixelsVert(0.075f)) };

        msgTextRegionUPtr_->SetEntityPos(
            (POS_LEFT + static_cast<float>(MESSAGE.size() / 2)), POS_TOP);
    }

    int PopupStageNumberSelect::GetSelectNumber() const
    {
        if (!textEntryBoxUPtr_)
        {
            return NUMBER_SELECT_INVALID_;
        }

        const auto TEXT { misc::TrimWhitespaceCopy(textEntryBoxUPtr_->GetText()) };
        return misc::ToNumberOr(TEXT, NUMBER_SELECT_INVALID_);
    }

    bool PopupStageNumberSelect::ProcessSelectNumber()
    {
        const auto ORIG_SELECTION_NUM { GetSelectNumber() };
        selection_ = ORIG_SELECTION_NUM;

        if (ORIG_SELECTION_NUM < 0)
        {
            selection_ = NUMBER_SELECT_INVALID_;

            SetupMessageText(
                "(invalid, type a number between " + std::to_string(popupInfo_.NumberSelMin())
                + " and " + std::to_string(popupInfo_.NumberSelMax()) + ")");

            return false;
        }
        else if (ORIG_SELECTION_NUM < static_cast<int>(popupInfo_.NumberSelMin()))
        {
            selection_ = NUMBER_SELECT_INVALID_;

            SetupMessageText(
                "(the number is too small, the minimum is "
                + std::to_string(popupInfo_.NumberSelMin()) + ")");

            return false;
        }
        else if (ORIG_SELECTION_NUM > static_cast<int>(popupInfo_.NumberSelMax()))
        {
            selection_ = NUMBER_SELECT_INVALID_;

            SetupMessageText(
                "(the number is too large, the maximum is "
                + std::to_string(popupInfo_.NumberSelMax()) + ")");

            return false;
        }
        else
        {
            SetupMessageText("");

            if (sliderbarUPtr_)
            {
                willSliderbarUpdate_ = false;

                if (ORIG_SELECTION_NUM == static_cast<int>(popupInfo_.NumberSelMin()))
                {
                    sliderbarUPtr_->PositionRatio(0.0f);
                }
                else
                {
                    sliderbarUPtr_->PositionRatio(
                        static_cast<float>(ORIG_SELECTION_NUM)
                        / static_cast<float>(popupInfo_.NumberSelMax()));
                }

                willSliderbarUpdate_ = true;
            }

            return true;
        }
    }

    void PopupStageNumberSelect::SetupSliderbar()
    {
        const auto SLIDERBAR_LENGTH { (ContentRegion().width * 0.75f) };

        sliderbarUPtr_ = std::make_unique<gui::SliderBar>(
            GetStageName() + "'s",
            0.0f,
            0.0f,
            SLIDERBAR_LENGTH,
            gui::SliderStyle(gui::Orientation::Horiz),
            gui::SliderBar::Callback_t::IHandlerPtr_t(this));

        const auto POS_LEFT { sfutil::DisplayCenterHoriz(sliderbarUPtr_->GetEntityRegion().width) };

        const auto POS_TOP { (
            sfutil::Bottom(textEntryBoxUPtr_->GetEntityRegion())
            + sfutil::ScreenRatioToPixelsVert(0.08f)) };

        sliderbarUPtr_->SetEntityPos(POS_LEFT, POS_TOP);
        EntityAdd(sliderbarUPtr_);
    }

} // namespace popup
} // namespace heroespath
