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
// popup-stage-num-select.cpp
//
#include "popup-stage-num-select.hpp"

#include "game/loop-manager.hpp"

#include "popup/popup-manager.hpp"

#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/sound-manager.hpp"

#include "misc/boost-string-includes.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    const int PopupStageNumberSelect::NUMBER_SELECT_INVALID_{ -1 }; // any negative will work here

    PopupStageNumberSelect::PopupStageNumberSelect(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , msgTextRegionUPtr_()
        , textEntryBoxUPtr_()
        , willSliderbarUpdate_(true)
        , willTextBoxUpdate_(true)
    {}

    PopupStageNumberSelect::~PopupStageNumberSelect() = default;

    bool PopupStageNumberSelect::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        auto const CURR_RATIO{ PACKAGE.PTR_->GetCurrentValue() };

        auto const CURR_VAL{ popupInfo_.NumberSelMin()
                             + static_cast<std::size_t>(
                                   CURR_RATIO
                                   * static_cast<float>(
                                         popupInfo_.NumberSelMax() - popupInfo_.NumberSelMin())) };

        selection_ = static_cast<int>(CURR_VAL);

        std::ostringstream minNumSS;
        minNumSS << popupInfo_.NumberSelMin();
        std::string numStr(minNumSS.str());

        try
        {
            numStr = boost::lexical_cast<std::string>(CURR_VAL);
        }
        catch (...)
        {
            numStr = minNumSS.str();
        }

        if ((textEntryBoxUPtr_.get() != nullptr) && willSliderbarUpdate_)
        {
            willTextBoxUpdate_ = false;
            textEntryBoxUPtr_->SetText(numStr);
            willTextBoxUpdate_ = true;
            SetupInfoText("");
        }

        return true;
    }

    bool PopupStageNumberSelect::HandleCallback(
        const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &)
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

        const sfml_util::gui::TextInfo MSG_TEXT_INFO{
            " ",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            sfml_util::Justified::Center
        };

        msgTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStage'sInfo", MSG_TEXT_INFO, sf::FloatRect());

        EntityAdd(msgTextRegionUPtr_.get());
        SetupInfoText("(type a number or use the slider below)");

        auto const TEXTENTRY_BOX_WIDTH{ textRegion_.width * 0.45f };

        auto const TEXTENTRY_BOX_POS_LEFT{ textRegion_.left
                                           + ((textRegion_.width - TEXTENTRY_BOX_WIDTH) * 0.5f) };

        // this 115 spacer value of 115 found to look good by experiment
        auto const TEXTENTRY_BOX_POS_TOP{ msgTextRegionUPtr_->GetEntityPos().y - 115.0f };

        // this textbox's min height, and fits pretty well here with the font size being "large"
        auto const TEXTENTRY_BOX_HEIGHT{ 55.0f };

        const sf::FloatRect TEXTENTRY_REGION(
            TEXTENTRY_BOX_POS_LEFT,
            TEXTENTRY_BOX_POS_TOP,
            TEXTENTRY_BOX_WIDTH,
            TEXTENTRY_BOX_HEIGHT);

        std::ostringstream minNumSS;
        minNumSS << popupInfo_.NumberSelMin();

        const sfml_util::gui::TextInfo TEXTENTRY_TEXT_INFO(
            minNumSS.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sf::Color::White,
            sfml_util::Justified::Left);

        const sfml_util::gui::BackgroundInfo TEXTENTRY_BG_INFO{
            sfml_util::gui::GuiElements::Instance()->GetTextureWood(), TEXTENTRY_REGION
        };

        const sfml_util::gui::box::Info TEXTENTRY_BOX_INFO(
            true, TEXTENTRY_REGION, sfml_util::gui::ColorSet(sf::Color::White), TEXTENTRY_BG_INFO);

        textEntryBoxUPtr_ = std::make_unique<sfml_util::gui::TextEntryBox>(
            "PopupStage's",
            TEXTENTRY_REGION,
            TEXTENTRY_TEXT_INFO,
            sfml_util::FontManager::Color_Light(),
            TEXTENTRY_BOX_INFO,
            this);

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
        auto const TEXT_TO_USE{ (TEXT.empty()) ? " " : TEXT };

        const sfml_util::gui::TextInfo INFO_TEXT_INFO(
            TEXT_TO_USE,
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            sfml_util::Justified::Center);

        EntityRemove(msgTextRegionUPtr_.get());

        msgTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect());

        EntityAdd(msgTextRegionUPtr_.get());

        auto const INFO_TEXT_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f))
                                       - (msgTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const INFO_TEXT_POS_TOP{ sliderbarPosTop_
                                      - (2.0f * msgTextRegionUPtr_->GetEntityRegion().height) };

        msgTextRegionUPtr_->SetEntityPos(
            INFO_TEXT_POS_LEFT + static_cast<float>(TEXT.size() / 2), INFO_TEXT_POS_TOP);
    }

    int PopupStageNumberSelect::GetSelectNumber() const
    {
        if (textEntryBoxUPtr_.get() == nullptr)
        {
            return NUMBER_SELECT_INVALID_;
        }

        auto const TEXT{ boost::algorithm::trim_copy(textEntryBoxUPtr_->GetText()) };

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
        auto const ORIG_SELECTION_NUM{ GetSelectNumber() };
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

            if (sliderbarUPtr_.get() != nullptr)
            {
                willSliderbarUpdate_ = false;

                if (ORIG_SELECTION_NUM == static_cast<int>(popupInfo_.NumberSelMin()))
                {
                    sliderbarUPtr_->SetCurrentValue(0.0f);
                }
                else
                {
                    sliderbarUPtr_->SetCurrentValue(
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

        auto const SLIDERBAR_LENGTH{ textRegion_.width * 0.75f }; // found by experiment

        auto const SLIDERBAR_POS_LEFT{ textRegion_.left
                                       + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f) };

        sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz),
            this);

        EntityAdd(sliderbarUPtr_.get());
    }
} // namespace popup
} // namespace heroespath
