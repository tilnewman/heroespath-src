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
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "popup/popup-manager.hpp"

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

    bool PopupStageNumberSelect::HandleCallback(
        const gui::SliderBar::Callback_t::PacketPtr_t PACKET_PTR)
    {
        const auto CURR_RATIO { PACKET_PTR->PositionRatio() };

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
            SetupInfoText("");
        }

        return true;
    }

    bool PopupStageNumberSelect::HandleCallback(const gui::TextEntryBox::Callback_t::PacketPtr_t)
    {
        selection_ = GetSelectNumber();

        if (willTextBoxUpdate_)
        {
            return ProcessSelectNumber();
        }

        return true;
    }

    void PopupStageNumberSelect::Setup()
    {
        PopupStageBase::Setup();

        const gui::TextInfo MSG_TEXT_INFO { " ",
                                            gui::GuiFont::Default,
                                            gui::FontManager::Instance()->Size_Small(),
                                            PopupManager::Color_Font(),
                                            gui::Justified::Center };

        msgTextRegionUPtr_
            = std::make_unique<gui::TextRegion>("PopupStage'sInfo", MSG_TEXT_INFO, sf::FloatRect());

        EntityAdd(msgTextRegionUPtr_);
        SetupInfoText("(type a number or use the slider below)");

        const auto TEXTENTRY_BOX_WIDTH { textRegion_.width * 0.45f };

        const auto TEXTENTRY_BOX_POS_LEFT { textRegion_.left
                                            + ((textRegion_.width - TEXTENTRY_BOX_WIDTH) * 0.5f) };

        // this 115 spacer value of 115 found to look good by experiment
        const auto TEXTENTRY_BOX_POS_TOP { msgTextRegionUPtr_->GetEntityPos().y - 115.0f };

        // this textbox's min height, and fits pretty well here with the font size being "large"
        const auto TEXTENTRY_BOX_HEIGHT { 55.0f };

        const sf::FloatRect TEXTENTRY_REGION(
            TEXTENTRY_BOX_POS_LEFT,
            TEXTENTRY_BOX_POS_TOP,
            TEXTENTRY_BOX_WIDTH,
            TEXTENTRY_BOX_HEIGHT);

        std::ostringstream minNumSS;
        minNumSS << popupInfo_.NumberSelMin();

        const gui::TextInfo TEXTENTRY_TEXT_INFO(
            minNumSS.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Large(),
            sf::Color::White,
            gui::Justified::Left);

        gui::BoxEntityInfo boxInfo;
        boxInfo.SetupImage(gui::CachedTexture("media-images-backgrounds-tile-wood"));
        boxInfo.SetupBorder(true);
        boxInfo.focus_colors = gui::FocusColors(sf::Color::White);

        textEntryBoxUPtr_ = std::make_unique<gui::TextEntryBox>(
            gui::TextEntryBox::Callback_t::IHandlerPtr_t(this),
            "PopupStage's",
            TEXTENTRY_REGION,
            TEXTENTRY_TEXT_INFO,
            sfutil::color::Light,
            boxInfo);

        textEntryBoxUPtr_->SetText(minNumSS.str());
        EntityAdd(textEntryBoxUPtr_);

        RemoveFocus();
        SetFocus(textEntryBoxUPtr_);
        textEntryBoxUPtr_->SetHasFocus(true);
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

                RemovePopup(ResponseTypes::Select, static_cast<std::size_t>(GetSelectNumber()));

                return true;
            }
            else
            {
                return false;
            }
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }

    void PopupStageNumberSelect::SetupInfoText(const std::string & TEXT)
    {
        const auto TEXT_TO_USE { (TEXT.empty()) ? " " : TEXT };

        const gui::TextInfo INFO_TEXT_INFO(
            TEXT_TO_USE,
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            gui::Justified::Center);

        EntityRemove(msgTextRegionUPtr_);

        msgTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect());

        EntityAdd(msgTextRegionUPtr_);

        const auto INFO_TEXT_POS_LEFT { (textRegion_.left + (textRegion_.width * 0.5f))
                                        - (msgTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        const auto INFO_TEXT_POS_TOP { sliderbarPosTop_
                                       - (2.0f * msgTextRegionUPtr_->GetEntityRegion().height) };

        msgTextRegionUPtr_->SetEntityPos(
            INFO_TEXT_POS_LEFT + static_cast<float>(TEXT.size() / 2), INFO_TEXT_POS_TOP);
    }

    int PopupStageNumberSelect::GetSelectNumber() const
    {
        if (!textEntryBoxUPtr_)
        {
            return NUMBER_SELECT_INVALID_;
        }

        const auto TEXT { boost::algorithm::trim_copy(textEntryBoxUPtr_->GetText()) };

        int num(NUMBER_SELECT_INVALID_);
        try
        {
            num = boost::lexical_cast<int>(TEXT);
        }
        catch (...)
        {
            num = NUMBER_SELECT_INVALID_;
        }

        return num;
    }

    bool PopupStageNumberSelect::ProcessSelectNumber()
    {
        const auto ORIG_SELECTION_NUM { GetSelectNumber() };
        selection_ = ORIG_SELECTION_NUM;

        if (ORIG_SELECTION_NUM < 0)
        {
            selection_ = NUMBER_SELECT_INVALID_;
            std::ostringstream ss;

            ss << "(invalid, type a number between " << popupInfo_.NumberSelMin() << " and "
               << popupInfo_.NumberSelMax() << ")";

            SetupInfoText(ss.str());
            return false;
        }
        if (ORIG_SELECTION_NUM < static_cast<int>(popupInfo_.NumberSelMin()))
        {
            selection_ = NUMBER_SELECT_INVALID_;
            std::ostringstream ss;

            ss << "(the number is too small, the minimum is " << popupInfo_.NumberSelMin() << ")";

            SetupInfoText(ss.str());
            return false;
        }
        else if (ORIG_SELECTION_NUM > static_cast<int>(popupInfo_.NumberSelMax()))
        {
            selection_ = NUMBER_SELECT_INVALID_;
            std::ostringstream ss;

            ss << "(the number is too large, the maximum is " << popupInfo_.NumberSelMax() << ")";

            SetupInfoText(ss.str());
            return false;
        }
        else
        {
            SetupInfoText("");

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
        PopupStageBase::SetupSliderbar();

        const auto SLIDERBAR_LENGTH { textRegion_.width * 0.75f }; // found by experiment

        const auto SLIDERBAR_POS_LEFT { textRegion_.left
                                        + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f) };

        sliderbarUPtr_ = std::make_unique<gui::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            gui::SliderStyle(gui::Orientation::Horiz),
            gui::SliderBar::Callback_t::IHandlerPtr_t(this));

        EntityAdd(sliderbarUPtr_);
    }

} // namespace popup
} // namespace heroespath
