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
// popup-stage.cpp
//
#include "popup-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-rendering.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/song-image-manager.hpp"

#include "game/game.hpp"
#include "game/stats/trait-enum.hpp"
#include "game/state/game-state.hpp"
#include "game/player/party.hpp"
#include "game/loop-manager.hpp"
#include "game/log-macros.hpp"
#include "game/creature/name-info.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/song/song.hpp"
#include "game/game-data-file.hpp"
#include "game/item/item-profile-warehouse.hpp"

#include "popup/popup-manager.hpp"

#include "misc/random.hpp"
#include "misc/boost-string-includes.hpp"

#include <sstream>
#include <random>
#include <algorithm>


namespace popup
{

    const int       PopupStage::NUMBER_SELECT_INVALID_          { -1 };//any negative
    const float     PopupStage::BEFORE_FADE_STARTS_DELAY_SEC_   { 2.0f };
    

    PopupStage::PopupStage(const PopupInfo & POPUP_INFO)
    :
        PopupStageBase              (POPUP_INFO),
        elapsedTimeCounter_         (0.0f),
        secondCounter_              (10),
        willSliderbarUpdate_        (true),
        willTextBoxUpdate_          (true),
        textureCurr_                (),
        texturePrev_                (),
        infoTextRegionUPtr_         (),
        textEntryBoxSPtr_           (),
        imageSpriteCurr_            (),
        imageSpritePrev_            (),
        beforeFadeTimerSec_         (0.0f),
        fadeAlpha_                  (0.0f),
        combatBgTexture_            (),
        combatBgSprite_             (),
        titleUPtr_                  (),
        descUPtr_                   (),
        drawCountdown_              (3)
    {}


    PopupStage::~PopupStage()
    {}


    bool PopupStage::HandleCallback(
        const sfml_util::gui::callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if ((popupInfo_.Type() == Popup::NumberSelection))
        {
            const float CURR_RATIO(PACKAGE.PTR_->GetCurrentValue());

            const std::size_t CURR_VAL(popupInfo_.NumberSelMin() + static_cast<std::size_t>(
                CURR_RATIO * static_cast<float>(popupInfo_.NumberSelMax() -
                    popupInfo_.NumberSelMin())));

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

            if ((textEntryBoxSPtr_.get() != nullptr) && willSliderbarUpdate_)
            {
                willTextBoxUpdate_ = false;
                textEntryBoxSPtr_->SetText(numStr);
                willTextBoxUpdate_ = true;
                SetupInfoText("");
            }

            return true;
        }

        return false;
    }


    bool PopupStage::HandleCallback(
        const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &)
    {
        if (popupInfo_.Type() == Popup::NumberSelection)
        {
            const int NUM(GetSelectNumber());
            selection_ = NUM;

            if (willTextBoxUpdate_)
            {
                ProcessSelectNumber();
            }

            return true;
        }
        else
        {
            return false;
        }
    }


    void PopupStage::Setup()
    {
        PopupStageBase::Setup();
        
        if (popupInfo_.Type() == Popup::NumberSelection)
        {
            SetupNumberSelectionPopup();
        }
        else if (popupInfo_.Type() == Popup::ImageFade)
        {
            SetupImageFadePopup();
        }
        else if (popupInfo_.Type() == Popup::CombatOver)
        {
            SetupCombatOverPopup();
        }
        else if (popupInfo_.Type() == Popup::SystemError)
        {
            SetupSystemErrorPopup();
        }
    }


    void PopupStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        if (popupInfo_.Type() == Popup::ItemProfilePleaseWait)
        {
            if (drawCountdown_ > 0)
            {
                if (0 == --drawCountdown_)
                {
                    ItemProfileSetup();
                }
            }
        }

        if (popupInfo_.Type() == Popup::ImageFade)
        {
            if (popupInfo_.Images().size() > 1)
            {
                target.draw(imageSpritePrev_, STATES);
            }

            target.draw(imageSpriteCurr_, STATES);
            target.draw( * titleUPtr_, STATES);
            target.draw( * descUPtr_, STATES);
        }
        else if (popupInfo_.Type() == Popup::CombatOver)
        {
            target.draw(combatBgSprite_, STATES);
            target.draw( * titleUPtr_, STATES);
            target.draw( * descUPtr_, STATES);
        }
        else if (popupInfo_.Type() == Popup::SystemError)
        {
            //The SystemError popup uses CombatOver popup's texture and sprite.
            target.draw(combatBgSprite_, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void PopupStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if ((popupInfo_.Type() == Popup::ResolutionChange) &&
            (secondCounter_ > 0))
        {
            elapsedTimeCounter_ += ELAPSED_TIME_SECONDS;

            if (elapsedTimeCounter_ > 1.0f)
            {
                elapsedTimeCounter_ = 0.0f;
                --secondCounter_;

                if (secondCounter_ > 0)
                {
                    sfml_util::gui::TextInfo textInfo(popupInfo_.TextInfo());
                    std::ostringstream ss;
                    ss << textInfo.text << "\n" << secondCounter_;
                    textInfo.text = ss.str();

                    textRegionUPtr_->Setup(textInfo,
                        textRegion_,
                        this);
                }
                else
                {
                    game::LoopManager::Instance()->PopupWaitEnd(Response::No);
                }
            }
        }

        if (popupInfo_.Type() == Popup::ImageFade)
        {
            if (beforeFadeTimerSec_ < BEFORE_FADE_STARTS_DELAY_SEC_)
            {
                beforeFadeTimerSec_ += ELAPSED_TIME_SECONDS;
            }
            else
            {
                if (fadeAlpha_ < 255.0f)
                {
                    fadeAlpha_ += ELAPSED_TIME_SECONDS * popupInfo_.ImageFadeSpeed();

                    if (fadeAlpha_ > 255.0f)
                    {
                        fadeAlpha_ = 255.0f;
                    }

                    imageSpriteCurr_.setColor(
                        sf::Color(255, 255, 255, static_cast<sf::Uint8>(fadeAlpha_)));
                }
                else
                {
                    imageSpriteCurr_.setColor(sf::Color::White);
                }
            }
        }
    }


    bool PopupStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (popupInfo_.Type() == Popup::ImageFade)
        {
            beforeFadeTimerSec_ = BEFORE_FADE_STARTS_DELAY_SEC_;
            fadeAlpha_ = 256.0f;//anything greater than 255.0f will work here
        }

        if ((popupInfo_.Type() == Popup::ContentSelectionWithItem) ||
            (popupInfo_.Type() == Popup::ContentSelectionWithoutItem))
        {
            if ((KEY_EVENT.code == sf::Keyboard::I) &&
                (popupInfo_.Type() == Popup::ContentSelectionWithItem))
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_Item());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::C)
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_Coins());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::G)
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_Gems());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::M)
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_MeteorShards());

                return true;
            }
        }

        if ((popupInfo_.Type() == Popup::NumberSelection) &&
            ((KEY_EVENT.code == sf::Keyboard::S) || (KEY_EVENT.code == sf::Keyboard::Return)))
        {
            if (ProcessSelectNumber())
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, static_cast<std::size_t>(GetSelectNumber()));

                return true;
            }
            else
            {
                return false;
            }
        }

        if ((popupInfo_.Buttons() & PopupButtons::Select) &&
            (KEY_EVENT.code == sf::Keyboard::S))
        {
            return HandleSelect();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStage::SetupInfoText(const std::string & TEXT)
    {
        const std::string TEXT_TO_USE((TEXT.empty()) ? " " : TEXT);

        const sfml_util::gui::TextInfo INFO_TEXT_INFO(
            TEXT_TO_USE,
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            sfml_util::Justified::Center);

        EntityRemove(infoTextRegionUPtr_.get());

        infoTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect());

        EntityAdd(infoTextRegionUPtr_.get());

        const float INFO_TEXT_POS_LEFT((textRegion_.left + (textRegion_.width * 0.5f)) -
            (infoTextRegionUPtr_->GetEntityRegion().width * 0.5f));

        const float INFO_TEXT_POS_TOP(sliderbarPosTop_ -
            (2.0f * infoTextRegionUPtr_->GetEntityRegion().height));

        infoTextRegionUPtr_->SetEntityPos(INFO_TEXT_POS_LEFT + static_cast<float>(TEXT.size() / 2),
            INFO_TEXT_POS_TOP);
    }


    int PopupStage::GetSelectNumber() const
    {
        if (textEntryBoxSPtr_.get() == nullptr)
        {
            return NUMBER_SELECT_INVALID_;
        }

        const std::string TEXT(boost::algorithm::trim_copy(textEntryBoxSPtr_->GetText()));

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


    bool PopupStage::ProcessSelectNumber()
    {
        const int NUM(GetSelectNumber());
        selection_ = NUM;

        if (NUM < 0)
        {
            selection_ = NUMBER_SELECT_INVALID_;
            std::ostringstream ss;

            ss << "(invalid, type a number between " << popupInfo_.NumberSelMin() << " and "
                << popupInfo_.NumberSelMax() << ")";

            SetupInfoText(ss.str());
            return false;
        }
        if (NUM < static_cast<int>(popupInfo_.NumberSelMin()))
        {
            selection_ = NUMBER_SELECT_INVALID_;
            std::ostringstream ss;

            ss << "(the number is too small, the minimum is "
                << popupInfo_.NumberSelMin() << ")";

            SetupInfoText(ss.str());
            return false;
        }
        else if (NUM > static_cast<int>(popupInfo_.NumberSelMax()))
        {
            selection_ = NUMBER_SELECT_INVALID_;
            std::ostringstream ss;

            ss << "(the number is too large, the maximum is "
                << popupInfo_.NumberSelMax() << ")";

            SetupInfoText(ss.str());
            return false;
        }
        else
        {
            SetupInfoText("");

            if (sliderbarUPtr_.get() != nullptr)
            {
                willSliderbarUpdate_ = false;

                if (NUM == static_cast<int>(popupInfo_.NumberSelMin()))
                {
                    sliderbarUPtr_->SetCurrentValue(0.0f);
                }
                else
                {
                    sliderbarUPtr_->SetCurrentValue(static_cast<float>(NUM) /
                        static_cast<float>(popupInfo_.NumberSelMax()));
                }

                willSliderbarUpdate_ = true;
            }

            return true;
        }
    }


    void PopupStage::ItemProfileSetup()
    {
        game::item::ItemProfileWarehouse::Instance()->Setup();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
    }


    void PopupStage::SetupNumberSelectionPopup()
    {
        const sfml_util::gui::TextInfo INFO_TEXT_INFO(" ",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            PopupManager::Color_Font(),
            sfml_util::Justified::Center);

        infoTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect());

        EntityAdd(infoTextRegionUPtr_.get());
        SetupInfoText("(type a number or use the slider below)");

        const float TEXTENTRY_BOX_WIDTH(textRegion_.width * 0.45f);

        const float TEXTENTRY_BOX_POS_LEFT(textRegion_.left +
            ((textRegion_.width - TEXTENTRY_BOX_WIDTH) * 0.5f));

        //this 115 spacer value of 115 found to look good by experiment
        const float TEXTENTRY_BOX_POS_TOP(infoTextRegionUPtr_->GetEntityPos().y - 115.0f);

        //this textbox's min height, and fits pretty well here with the font size being "large"
        const float TEXTENTRY_BOX_HEIGHT(55.0f);

        const sf::FloatRect TEXTENTRY_REGION(TEXTENTRY_BOX_POS_LEFT,
            TEXTENTRY_BOX_POS_TOP,
            TEXTENTRY_BOX_WIDTH,
            TEXTENTRY_BOX_HEIGHT);

        std::ostringstream minNumSS;
        minNumSS << popupInfo_.NumberSelMin();

        const sfml_util::gui::TextInfo TEXTENTRY_TEXT_INFO(minNumSS.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sf::Color::White,
            sfml_util::Justified::Left);

        const sfml_util::gui::BackgroundInfo TEXTENTRY_BG_INFO{
            sfml_util::gui::GuiElements::Instance()->GetTextureWood(),
            TEXTENTRY_REGION };

        const sfml_util::gui::box::Info TEXTENTRY_BOX_INFO(true,
            TEXTENTRY_REGION,
            sfml_util::gui::ColorSet(sf::Color::White),
            TEXTENTRY_BG_INFO);

        textEntryBoxSPtr_.reset(new sfml_util::gui::TextEntryBox("PopupStage's",
            TEXTENTRY_REGION,
            TEXTENTRY_TEXT_INFO,
            sfml_util::FontManager::Color_Light(),
            TEXTENTRY_BOX_INFO,
            this));

        textEntryBoxSPtr_->SetText(minNumSS.str());
        EntityAdd(textEntryBoxSPtr_.get());

        RemoveFocus();
        SetFocus(textEntryBoxSPtr_.get());
        textEntryBoxSPtr_->SetHasFocus(true);    
    }


    void PopupStage::SetupImageFadePopup()
    {
        const sfml_util::gui::TextInfo TITLE_TEXTINFO(
            popupInfo_.TitleText(),
            sfml_util::FontManager::Instance()->Font_BigFlavor1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        auto titleRegion{ textRegion_ };
        titleRegion.height = 0.0f;

        titleUPtr_ = std::make_unique<sfml_util::gui::TextRegion>("ImageFadePopupTitle",
            TITLE_TEXTINFO,
            titleRegion);

        const sfml_util::gui::TextInfo DESC_TEXTINFO(
            popupInfo_.DescText(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        auto descRegion{ textRegion_ };
        descRegion.height = 0.0f;

        descUPtr_ = std::make_unique<sfml_util::gui::TextRegion>("ImageFadePopupDesc",
            DESC_TEXTINFO,
            descRegion);

        descRegion = descUPtr_->GetEntityRegion();

        descUPtr_->SetEntityPos(descUPtr_->GetEntityPos().x,
            (textRegion_.top + textRegion_.height) - descRegion.height);

        auto const IMAGE_PAD(sfml_util::MapByRes(15.0f, 35.0f));

        auto const IMAGE_TOP(textRegionUPtr_->GetEntityRegion().top +
            textRegionUPtr_->GetEntityRegion().height + IMAGE_PAD);

        auto const FREE_SPACE_VERT(descUPtr_->GetEntityPos().y - (IMAGE_TOP + IMAGE_PAD));
        auto const FREE_SPACE_HORIZ{ textRegion_.width };
        auto const IMAGE_MAX_DIMM{ sfml_util::MapByRes(256.0f, 768.0f) };
        auto const IMAGE_WIDTH{ std::min(IMAGE_MAX_DIMM, FREE_SPACE_HORIZ) };
        auto const IMAGE_HEIGHT{ std::min(IMAGE_MAX_DIMM, FREE_SPACE_VERT) };

        if (popupInfo_.ImagesCount() == 1)
        {
            imageSpriteCurr_.setTexture(popupInfo_.ImagesAt(0), true);
        }
        else
        {
            imageSpritePrev_.setTexture(popupInfo_.ImagesAt(0), true);
            imageSpriteCurr_.setTexture(popupInfo_.ImagesAt(1), true);
        }

        //scale the image to fit in the free space
        //assume popupInfo_.ImagesAt(0) and popupInfo_.ImagesAt(1) are the same size
        imageSpritePrev_.setScale(1.0f, 1.0f);
        imageSpriteCurr_.setScale(1.0f, 1.0f);
        auto SCALE_VERT{ IMAGE_HEIGHT / imageSpriteCurr_.getGlobalBounds().height };
        imageSpritePrev_.setScale(SCALE_VERT, SCALE_VERT);
        imageSpriteCurr_.setScale(SCALE_VERT, SCALE_VERT);

        if (imageSpriteCurr_.getGlobalBounds().width > FREE_SPACE_HORIZ)
        {
            auto const SCALE_HORIZ{ IMAGE_WIDTH / imageSpriteCurr_.getGlobalBounds().width };
            imageSpritePrev_.setScale(SCALE_HORIZ, SCALE_HORIZ);
            imageSpriteCurr_.setScale(SCALE_HORIZ, SCALE_HORIZ);
        }

        imageSpritePrev_.setPosition((textRegion_.left + (textRegion_.width * 0.5f)) -
            (imageSpritePrev_.getGlobalBounds().width * 0.5f),
            (IMAGE_TOP + (IMAGE_HEIGHT * 0.5f)) -
            (imageSpritePrev_.getGlobalBounds().height * 0.5f));

        imageSpriteCurr_.setPosition((textRegion_.left + (textRegion_.width * 0.5f)) -
            (imageSpriteCurr_.getGlobalBounds().width * 0.5f),
            (IMAGE_TOP + (IMAGE_HEIGHT * 0.5f)) -
            (imageSpriteCurr_.getGlobalBounds().height * 0.5f));

        imageSpriteCurr_.setColor(sf::Color(255, 255, 255, 0));
    }


    void PopupStage::SetupCombatOverPopup()
    {   
        switch (popupInfo_.HowCombatEnded())
        {
            case game::combat::CombatEnd::Win:
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::CombatWin).PlayRandom();

                sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-combat-crossswords"));

                break;
            }

            case game::combat::CombatEnd::Lose:
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::CombatLose).PlayRandom();

                sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-combat-crossbones"));

                break;
            }

            case game::combat::CombatEnd::Ran:
            case game::combat::CombatEnd::Count:
            default:
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::CombatLose).PlayRandom();

                sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-combat-run"));

                break;
            }
        }

        sfml_util::Invert(combatBgTexture_);
        sfml_util::Mask(combatBgTexture_, sf::Color::White);
        combatBgSprite_.setTexture(combatBgTexture_, true);
        combatBgSprite_.setColor( sf::Color(255, 255, 255, 32) );

        auto const HORIZ_RESCALE{ textRegion_.width /
            static_cast<float>(combatBgTexture_.getSize().x) };

        combatBgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (combatBgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE{ textRegion_.height /
                static_cast<float>(combatBgTexture_.getSize().y) };

            combatBgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP{ (textRegion_.top + (textRegion_.height * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().height * 0.5f) };

        combatBgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);

        auto const TITLE_TEXT{ [&]()
            {
                switch (popupInfo_.HowCombatEnded())
                {
                    case game::combat::CombatEnd::Win:  { return "Victory!";}
                    case game::combat::CombatEnd::Lose: { return "Death Strikes!"; }
                    case game::combat::CombatEnd::Ran:
                    case game::combat::CombatEnd::Count:
                    default:                            { return "Defeat!"; }
                }
            }() };

        const sfml_util::gui::TextInfo COMBAT_TITLE_TEXTINFO(
            TITLE_TEXT,
            sfml_util::FontManager::Instance()->Font_BigFlavor1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        titleUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatOverPopupTitle",
            COMBAT_TITLE_TEXTINFO,
            sf::FloatRect());

        auto const TITLE_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f)) -
            (titleUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TITLE_POS_TOP{ textRegion_.top + sfml_util::MapByRes(20.0f, 60.0f) };

        titleUPtr_->SetEntityPos(TITLE_POS_LEFT, TITLE_POS_TOP);

        auto const DESC_TEXT{ [&]()
            {
                std::ostringstream descSS;
                switch (popupInfo_.HowCombatEnded())
                {
                    case game::combat::CombatEnd::Win:
                    {
                        descSS << "Congratulations, your party has beaten all enemies on the "
                            << "field of battle and emerged victorious.\n\nWill you search "
                            << "for loot?";
                        break;
                    }
                    case game::combat::CombatEnd::Lose:
                    {
                        descSS << "The dangers of Etan have claimed another party of "
                            << "adventurers.  All of your characters have been "
                            << "incapacitated or killed, but all is not lost.  Your "
                            << "saved games remain and can be loaded at any time.\n\n"
                            << "Click YES to restart from your last save, or click NO "
                            << "to quit.";
                        break;
                    }
                    case game::combat::CombatEnd::Ran:
                    case game::combat::CombatEnd::Count:
                    default:
                    {
                        descSS << "You have run from battle, and as a consequence, "
                                << "you will earn no experience and loot no treasure.";
                        break;
                    }
                }
                return descSS.str();
            }() };

        const sfml_util::gui::TextInfo COMBAT_DESC_TEXTINFO(
            DESC_TEXT,
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        const sf::FloatRect COMBAT_DESC_RECT(
            textRegion_.left,
            titleUPtr_->GetEntityRegion().top +
                titleUPtr_->GetEntityRegion().height +
                sfml_util::MapByRes(20.0f, 60.0f),
            textRegion_.width,
            textRegion_.height -
                (titleUPtr_->GetEntityRegion().height +
                    (sfml_util::MapByRes(20.0f, 60.0f) * 2.0f)));

        descUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatOverPopupDesc",
            COMBAT_DESC_TEXTINFO,
            COMBAT_DESC_RECT);
    }


    void PopupStage::SetupSystemErrorPopup()
    {
        sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-misc-error"));

        sfml_util::Mask(combatBgTexture_, sf::Color::White);
        combatBgSprite_.setTexture(combatBgTexture_, true);
        combatBgSprite_.setColor( sf::Color(255, 255, 255, 32) );

        auto const HORIZ_RESCALE{ textRegion_.width /
            static_cast<float>(combatBgTexture_.getSize().x) };

        combatBgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (combatBgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE{ textRegion_.height /
                static_cast<float>(combatBgTexture_.getSize().y) };

            combatBgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP{ (textRegion_.top + (textRegion_.height * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().height * 0.5f) };

        combatBgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);
    }

}
