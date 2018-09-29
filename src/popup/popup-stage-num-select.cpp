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

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "misc/boost-string-includes.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/gui-images.hpp"
#include "sfml-util/sound-manager.hpp"

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
        const sfml_util::SliderBar::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        auto const CURR_RATIO { PACKET_PTR->PositionRatio() };

        auto const CURR_VAL { popupInfo_.NumberSelMin()
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

    bool PopupStageNumberSelect::HandleCallback(
        const sfml_util::TextEntryBox::Callback_t::PacketPtr_t &)
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

        const sfml_util::TextInfo MSG_TEXT_INFO { " ",
                                                  sfml_util::GuiFont::Default,
                                                  sfml_util::FontManager::Instance()->Size_Small(),
                                                  PopupManager::Color_Font(),
                                                  sfml_util::Justified::Center };

        msgTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "PopupStage'sInfo", MSG_TEXT_INFO, sf::FloatRect());

        EntityAdd(msgTextRegionUPtr_.get());
        SetupInfoText("(type a number or use the slider below)");

        auto const TEXTENTRY_BOX_WIDTH { textRegion_.width * 0.45f };

        auto const TEXTENTRY_BOX_POS_LEFT { textRegion_.left
                                            + ((textRegion_.width - TEXTENTRY_BOX_WIDTH) * 0.5f) };

        // this 115 spacer value of 115 found to look good by experiment
        auto const TEXTENTRY_BOX_POS_TOP { msgTextRegionUPtr_->GetEntityPos().y - 115.0f };

        // this textbox's min height, and fits pretty well here with the font size being "large"
        auto const TEXTENTRY_BOX_HEIGHT { 55.0f };

        const sf::FloatRect TEXTENTRY_REGION(
            TEXTENTRY_BOX_POS_LEFT,
            TEXTENTRY_BOX_POS_TOP,
            TEXTENTRY_BOX_WIDTH,
            TEXTENTRY_BOX_HEIGHT);

        std::ostringstream minNumSS;
        minNumSS << popupInfo_.NumberSelMin();

        const sfml_util::TextInfo TEXTENTRY_TEXT_INFO(
            minNumSS.str(),
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Large(),
            sf::Color::White,
            sfml_util::Justified::Left);

        sfml_util::BoxEntityInfo boxInfo;
        boxInfo.SetupImage(sfml_util::CachedTexture("media-images-backgrounds-tile-wood"));
        boxInfo.SetupBorder(true);
        boxInfo.focus_colors = sfml_util::FocusColors(sf::Color::White);

        textEntryBoxUPtr_ = std::make_unique<sfml_util::TextEntryBox>(
            sfml_util::TextEntryBox::Callback_t::IHandlerPtr_t(this),
            "PopupStage's",
            TEXTENTRY_REGION,
            TEXTENTRY_TEXT_INFO,
            sfml_util::color::Light,
            boxInfo);

        textEntryBoxUPtr_->SetText(minNumSS.str());
        EntityAdd(textEntryBoxUPtr_.get());

        RemoveFocus();
        SetFocus(textEntryBoxUPtr_.get());
        textEntryBoxUPtr_->SetHasFocus(true);
    }

    void PopupStageNumberSelect::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);
        Stage::Draw(target, STATES);
    }

    bool PopupStageNumberSelect::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::S) || (KEY_EVENT.code == sf::Keyboard::Return))
        {
            if (ProcessSelectNumber())
            {
                PlayValidKeypressSoundEffect();

                game::LoopManager::Instance()->PopupWaitEnd(
                    ResponseTypes::Select, static_cast<std::size_t>(GetSelectNumber()));

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
        auto const TEXT_TO_USE { (TEXT.empty()) ? " " : TEXT };

        const sfml_util::TextInfo INFO_TEXT_INFO(
            TEXT_TO_USE,
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            sfml_util::Justified::Center);

        EntityRemove(msgTextRegionUPtr_.get());

        msgTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect());

        EntityAdd(msgTextRegionUPtr_.get());

        auto const INFO_TEXT_POS_LEFT { (textRegion_.left + (textRegion_.width * 0.5f))
                                        - (msgTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const INFO_TEXT_POS_TOP { sliderbarPosTop_
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

        auto const TEXT { boost::algorithm::trim_copy(textEntryBoxUPtr_->GetText()) };

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
        auto const ORIG_SELECTION_NUM { GetSelectNumber() };
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

        auto const SLIDERBAR_LENGTH { textRegion_.width * 0.75f }; // found by experiment

        auto const SLIDERBAR_POS_LEFT { textRegion_.left
                                        + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f) };

        sliderbarUPtr_ = std::make_unique<sfml_util::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            sfml_util::SliderStyle(sfml_util::Orientation::Horiz),
            sfml_util::SliderBar::Callback_t::IHandlerPtr_t(this));

        EntityAdd(sliderbarUPtr_.get());
    }

} // namespace popup
} // namespace heroespath
