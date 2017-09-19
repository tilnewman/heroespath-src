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
#include "sfml-util/gui/popup-manager.hpp"
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

#include "misc/random.hpp"
#include "misc/boost-string-includes.hpp"

#include <sstream>
#include <random>
#include <algorithm>


namespace sfml_util
{

    const int       PopupStage::NUMBER_SELECT_INVALID_                      { -1 };//any negative
    const float     PopupStage::IMAGE_SLIDER_SPEED_                         { 4.0f };
    const float     PopupStage::BEFORE_FADE_STARTS_DELAY_SEC_               { 2.0f };
    const float     PopupStage::MUSICSHEET_POPUP_BACKGROUND_WIDTH_RATIO_    { 0.9f };
    const float     PopupStage::SPELLBOOK_COLOR_FADE_SPEED_                 { 6.0f };
    const sf::Color PopupStage::SPELL_UNABLE_TEXT_COLOR_  { sf::Color(127, 32, 32) };
    const sf::Uint8 PopupStage::SPELLBOOK_IMAGE_ALPHA_                      { 192 };
    const float     PopupStage::SPELL_WARNING_DURATION_SEC_                 { 2.0f };


    PopupStage::PopupStage(const game::PopupInfo & POPUP_INFO)
    :
        PopupStageBase              (POPUP_INFO),
        charDetailsTextRegionUPtr_  (),
        listBoxLabelTextRegionUPtr_ (),
        listBoxSPtr_                (),
        elapsedTimeCounter_         (0.0f),
        secondCounter_              (10),
        willSliderbarUpdate_        (true),
        willTextBoxUpdate_          (true),
        textureCurr_                (),
        texturePrev_                (),
        infoTextRegionUPtr_         (),
        textEntryBoxSPtr_           (),
        isImageProcAllowed_         (false),
        isInitialAnimation_         (true),
        willShowImageCount_         (false),
        imageSpriteCurr_            (),
        imageSpritePrev_            (),
        areImagesMoving_            (false),
        areImagesMovingLeft_        (false),
        imagesRect_                 (),
        imageWrnTextRegionUPtr_     (),
        imageNumTextRegionUPtr_     (),
        imageIndex_                 (0),
        imageIndexLastSoundOn_      (0),
        imageIndexLastSoundOff_     (0),
        imageCurrTargetScale_       (1.0f),
        imagePrevStartScale_        (1.0f),
        imagePrevStartPosX_         (0.0f),
        imageCurrTravelDist_        (0.0f),
        imagePrevTravelDist_        (0.0f),
        imageMoveQueue_             (),
        imageSlider_                (IMAGE_SLIDER_SPEED_),
        imagePosTop_                (0.0f),
        beforeFadeTimerSec_         (0.0f),
        fadeAlpha_                  (0.0f),
        fadeState_                  (FadeState::Initial),
        playerTexture_              (),
        playerSprite_               (),
        pageRectLeft_               (),
        pageRectRight_              (),
        spellColorImageCurrent_     (sf::Color::Transparent),
        spellColorImageStart_       (sf::Color::Transparent),
        spellColorImageEnd_         (sf::Color::Transparent),
        spellColorTextCurrent_      (sf::Color::Transparent),
        spellColorTextStart_        (sf::Color::Transparent),
        spellColorTextEnd_          (sf::Color::Transparent),
        spellTitleTextRegionUPtr_   (),
        spellDetailsTextUPtr_       (),
        spellUnableTextUPtr_        (),
        spellDescTextUPtr_          (),
        spellTexture_               (),
        spellSprite_                (),
        spellUnableTextWillShow_    (false),
        spellWarningTimerSec_       (0.0f),
        spellWarnColorShaker_       (SPELL_UNABLE_TEXT_COLOR_, sf::Color::Transparent, 60.0f),
        LISTBOX_IMAGE_COLOR_        (sf::Color(255, 255, 255, 190)),
        LISTBOX_LINE_COLOR_         (sfml_util::FontManager::Color_GrayDark()),
        LISTBOX_COLOR_FG_           (LISTBOX_LINE_COLOR_),

        LISTBOX_COLOR_BG_           (sfml_util::FontManager::Color_Orange() -
                                     sf::Color(100, 100, 100, 220)),

        LISTBOX_COLORSET_           (LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_),
        LISTBOX_BG_INFO_            (LISTBOX_COLOR_BG_),

        listBoxItemTextInfo_        (" ",
                                     sfml_util::FontManager::Instance()->Font_Default2(),
                                     sfml_util::FontManager::Instance()->Size_Smallish(),
                                     sfml_util::FontManager::Color_GrayDarker(),
                                     sfml_util::Justified::Left),

        songCurrentPtr_             (nullptr),
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
        if ((popupInfo_.Type() == game::Popup::ImageSelection) ||
            (popupInfo_.Type() == game::Popup::CharacterSelection))
        {
            if (isImageProcAllowed_)
            {
                auto const COUNT{ ((popupInfo_.Type() == game::Popup::CharacterSelection) ?
                    game::Game::Instance()->State().Party().Characters().size() :
                    popupInfo_.ImagesCount()) };

                const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f / static_cast<float>(COUNT));

                std::size_t index(static_cast<std::size_t>(sliderbarUPtr_->GetCurrentValue() /
                    SINGLE_IMAGE_SLIDER_WIDTH_RATIO));

                if (index >= COUNT)
                {
                    index = COUNT - 1;
                }

                imageMoveQueue_.push(index);
            }

            return true;
        }
        else if ((popupInfo_.Type() == game::Popup::NumberSelection))
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
        }

        return false;
    }


    bool PopupStage::HandleCallback(
        const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &)
    {
        if (popupInfo_.Type() == game::Popup::NumberSelection)
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


    bool PopupStage::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        if (PACKAGE.package.PTR_ == nullptr)
        {
            return false;
        }

        if ((PACKAGE.gui_event == sfml_util::GuiEvent::Click) ||
            (PACKAGE.gui_event == sfml_util::GuiEvent::SelectionChange) ||
            (PACKAGE.keypress_event.code == sf::Keyboard::Up) ||
            (PACKAGE.keypress_event.code == sf::Keyboard::Down))
        {
            if ((FadeState::Initial != fadeState_) &&
                (PACKAGE.package.PTR_->GetSelected() != nullptr) &&
                (songCurrentPtr_ != PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC))
            {
                if (songCurrentPtr_ != PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC)
                {
                    songCurrentPtr_ = PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC;
                }

                if (FadeState::FadingOut != fadeState_)
                {
                    spellColorImageStart_  = spellColorImageCurrent_;
                    spellColorTextStart_  = spellColorTextCurrent_;
                    spellColorImageEnd_ = sf::Color(255, 255, 255, 0);
                    spellColorTextEnd_ = sf::Color(0, 0, 0, 0);
                    fadeState_ = FadeState::FadingOut;
                    spellColorSlider_.Reset(SPELLBOOK_COLOR_FADE_SPEED_);
                }

                return true;
            }
        }
        else if ((PACKAGE.gui_event == sfml_util::GuiEvent::DoubleClick) ||
                    (PACKAGE.keypress_event.code == sf::Keyboard::Return))
        {
            if (PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC != nullptr)
            {
                return HandleSongPlay();
            }
        }

        return false;
    }


    void PopupStage::Setup()
    {
        PopupStageBase::Setup();
        
        if (popupInfo_.Type() == game::Popup::NumberSelection)
        {
            SetupNumberSelectionPopup();
        }
        if (popupInfo_.Type() == game::Popup::CharacterSelection)
        {
            SetupCharacterSelectionPopup();
        }
        else if (popupInfo_.Type() == game::Popup::ImageSelection)
        {
            SetupImageSelectionPopup();
        }
        else if (popupInfo_.Type() == game::Popup::ImageFade)
        {
            SetupImageFadePopup();
        }
        else if (popupInfo_.Type() == game::Popup::MusicSheet)
        {
            SetupMusicSheetPopup();
        }
        else if (popupInfo_.Type() == game::Popup::CombatOver)
        {
            SetupCombatOverPopup();
        }
        else if (popupInfo_.Type() == game::Popup::SystemError)
        {
            SetupSystemErrorPopup();
        }
    }


    void PopupStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        if (popupInfo_.Type() == game::Popup::ItemProfilePleaseWait)
        {
            if (drawCountdown_ > 0)
            {
                if (0 == --drawCountdown_)
                {
                    ItemProfileSetup();
                }
            }
        }

        if ((popupInfo_.Type() == game::Popup::ImageSelection) ||
            (popupInfo_.Type() == game::Popup::CharacterSelection))
        {
            target.draw(imageSpriteCurr_, STATES);

            if (areImagesMoving_ && (false == isInitialAnimation_))
            {
                target.draw(imageSpritePrev_, STATES);
            }

            if (willShowImageCount_ && (imageNumTextRegionUPtr_.get() != nullptr))
            {
                imageNumTextRegionUPtr_->draw(target, STATES);
            }
        }
        else if (popupInfo_.Type() == game::Popup::ImageFade)
        {
            if (popupInfo_.Images().size() > 1)
            {
                target.draw(imageSpritePrev_, STATES);
            }

            target.draw(imageSpriteCurr_, STATES);
            target.draw( * titleUPtr_, STATES);
            target.draw( * descUPtr_, STATES);
        }
        else if (popupInfo_.Type() == game::Popup::MusicSheet)
        {
            target.draw(accentSprite1_, STATES);
            target.draw(accentSprite2_, STATES);
            target.draw(playerSprite_, STATES);
            target.draw(spellSprite_, STATES);
            charDetailsTextRegionUPtr_->draw(target, STATES);
            listBoxLabelTextRegionUPtr_->draw(target, STATES);
            spellTitleTextRegionUPtr_->draw(target, STATES);
            spellDetailsTextUPtr_->draw(target, STATES);

            if (spellUnableTextWillShow_)
            {
                spellUnableTextUPtr_->draw(target, STATES);
            }

            spellDescTextUPtr_->draw(target, STATES);
        }
        else if (popupInfo_.Type() == game::Popup::CombatOver)
        {
            target.draw(combatBgSprite_, STATES);
            target.draw( * titleUPtr_, STATES);
            target.draw( * descUPtr_, STATES);
        }
        else if (popupInfo_.Type() == game::Popup::SystemError)
        {
            //The SystemError popup uses CombatOver popup's texture and sprite.
            target.draw(combatBgSprite_, STATES);
        }

        if (popupInfo_.Type() == game::Popup::CharacterSelection)
        {
            charDetailsTextRegionUPtr_->draw(target, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void PopupStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (popupInfo_.Type() == game::Popup::MusicSheet)
        {
            if (FadeState::Initial == fadeState_)
            {
                songCurrentPtr_ = listBoxSPtr_->At(0)->SONG_CPTRC;
                SetupSpellbookPageRightForFadeIn();
            }
            else if (FadeState::FadingIn == fadeState_)
            {
                MoveSpellbookPageRightColors(ELAPSED_TIME_SECONDS);
                SetSpellbookPageRightColors();

                if (spellColorSlider_.GetIsDone())
                {
                    fadeState_ = FadeState::Waiting;
                    spellColorSlider_.Reset(SPELLBOOK_COLOR_FADE_SPEED_);
                }
            }
            else if (FadeState::FadingOut == fadeState_)
            {
                MoveSpellbookPageRightColors(ELAPSED_TIME_SECONDS);
                SetSpellbookPageRightColors();

                if (spellColorSlider_.GetIsDone())
                {
                    SetupMusicSheetPageRightText(songCurrentPtr_);
                    SetupSpellbookPageRightForFadeIn();
                }
            }
            else if (FadeState::Warning == fadeState_)
            {
                spellUnableTextUPtr_->SetEntityColorFgBoth(spellWarnColorShaker_.Update(ELAPSED_TIME_SECONDS));
                spellWarningTimerSec_ += ELAPSED_TIME_SECONDS;
                if (spellWarningTimerSec_ > SPELL_WARNING_DURATION_SEC_)
                {
                    spellUnableTextUPtr_->SetEntityColorFgBoth(SPELL_UNABLE_TEXT_COLOR_);
                    spellWarningTimerSec_ = 0.0f;
                    fadeState_ = FadeState::Waiting;
                }
            }

            return;
        }

        if ((popupInfo_.Type() == game::Popup::ResolutionChange) &&
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
                    game::LoopManager::Instance()->PopupWaitEnd(sfml_util::Response::No);
                }
            }
        }

        if (areImagesMoving_ || (imageMoveQueue_.empty() == false))
        {
            float currRatio(0.0f);

            if (areImagesMoving_)
            {
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);
                areImagesMoving_ = ! imageSlider_.GetIsDone();

                if (false == areImagesMoving_)
                {
                    if (popupInfo_.Type() == game::Popup::CharacterSelection)
                    {
                        SetupCharacterSelectDetailText(false);
                        SetupCharacterSelectionRejectImage(false);
                    }

                    willShowImageCount_ = true;

                    if (imageIndexLastSoundOff_ != imageIndex_)
                    {
                        sfml_util::SoundManager::Instance()->Getsound_effect_set(
                            sfml_util::sound_effect_set::TickOff).PlayRandom();
                    }

                    imageIndexLastSoundOff_ = imageIndex_;
                }

                if (isInitialAnimation_)
                {
                    isInitialAnimation_ = areImagesMoving_;
                }
            }

            if ((false == areImagesMoving_) && (imageMoveQueue_.empty() == false))
            {
                areImagesMoving_ = true;
                willShowImageCount_ = false;

                if (imageIndexLastSoundOn_ != imageMoveQueue_.front())
                {
                    sfml_util::SoundManager::Instance()->Getsound_effect_set(
                        sfml_util::sound_effect_set::TickOn).PlayRandom();
                }

                imageIndexLastSoundOn_ = imageMoveQueue_.front();

                SetupSelectImage(imageMoveQueue_.front(),
                    IMAGE_SLIDER_SPEED_ + ((static_cast<float>(imageMoveQueue_.size()) * 0.25f)));

                imageMoveQueue_.pop();
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);

                if (popupInfo_.Type() == game::Popup::CharacterSelection)
                {
                    SetupCharacterSelectDetailText(true);
                    SetupCharacterSelectionRejectImage(true);
                }
            }

            const float CURR_SCALE(imageCurrTargetScale_ * currRatio);
            imageSpriteCurr_.setScale(CURR_SCALE, CURR_SCALE);

            const float PREV_SCALE(imagePrevStartScale_ * (1.0f - currRatio));
            imageSpritePrev_.setScale(PREV_SCALE, PREV_SCALE);

            const float CURR_POS_TOP(imagePosTop_ -
                (imageSpriteCurr_.getGlobalBounds().height * 0.5f));

            const float PREV_POS_TOP(imagePosTop_ -
                (imageSpritePrev_.getGlobalBounds().height * 0.5f));

            if (willShowImageCount_)
            {
                imageNumTextRegionUPtr_->SetEntityPos(imageNumTextRegionUPtr_->GetEntityPos().x,
                    CURR_POS_TOP - imageNumTextRegionUPtr_->GetEntityRegion().height);
            }

            if (areImagesMovingLeft_)
            {
                const float CURR_POS_LEFT((imagesRect_.left + imagesRect_.width) -
                    (currRatio * imageCurrTravelDist_));

                imageSpriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const float PREV_POS_LEFT(imagePrevStartPosX_ - (currRatio * imagePrevTravelDist_));
                imageSpritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
            else
            {
                const float CURR_POS_LEFT(imagesRect_.left + (currRatio * imageCurrTravelDist_));
                imageSpriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const float PREV_POS_LEFT(imagePrevStartPosX_ + (currRatio * imagePrevTravelDist_));
                imageSpritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
        }

        if (popupInfo_.Type() == game::Popup::ImageFade)
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
        if (popupInfo_.Type() == game::Popup::MusicSheet)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape) ||
                (KEY_EVENT.code == sf::Keyboard::Space))
            {
                SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Cancel, 0);
                return true;
            }
            else if ((FadeState::Waiting == fadeState_) &&
                     ((KEY_EVENT.code == sf::Keyboard::Return) ||
                     (KEY_EVENT.code == sf::Keyboard::C)))
            {
                return HandleSongPlay();
            }
        }

        if (popupInfo_.Type() == game::Popup::ImageFade)
        {
            beforeFadeTimerSec_ = BEFORE_FADE_STARTS_DELAY_SEC_;
            fadeAlpha_ = 256.0f;//anything greater than 255.0f will work here
        }

        if ((popupInfo_.Type() == game::Popup::ContentSelectionWithItem) ||
            (popupInfo_.Type() == game::Popup::ContentSelectionWithoutItem))
        {
            if ((KEY_EVENT.code == sf::Keyboard::I) &&
                (popupInfo_.Type() == game::Popup::ContentSelectionWithItem))
            {
                SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Select,
                                                            game::PopupInfo::ContentNum_Item());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::C)
            {
                SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Select,
                                                            game::PopupInfo::ContentNum_Coins());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::G)
            {
                SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Select,
                                                            game::PopupInfo::ContentNum_Gems());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::M)
            {
                SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Select,
                    game::PopupInfo::ContentNum_MeteorShards());
                return true;
            }
        }

        if ((popupInfo_.Type() == game::Popup::NumberSelection) &&
            ((KEY_EVENT.code == sf::Keyboard::S) || (KEY_EVENT.code == sf::Keyboard::Return)))
        {
            if (ProcessSelectNumber())
            {
                SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, static_cast<std::size_t>(GetSelectNumber()));

                return true;
            }
            else
            {
                return false;
            }
        }

        if (((popupInfo_.Type() == game::Popup::ImageSelection) ||
            (popupInfo_.Type() == game::Popup::CharacterSelection)) &&
            ((KEY_EVENT.code == sf::Keyboard::Left) ||
             (KEY_EVENT.code == sf::Keyboard::Right) ||
             ((KEY_EVENT.code >= sf::Keyboard::Num1) && (KEY_EVENT.code <= sf::Keyboard::Num9))))
        {
            auto const COUNT{ ((popupInfo_.Type() == game::Popup::CharacterSelection) ?
                game::Game::Instance()->State().Party().Characters().size() :
                popupInfo_.ImagesCount()) };

            if ((KEY_EVENT.code >= sf::Keyboard::Num1) && (KEY_EVENT.code <= sf::Keyboard::Num9))
            {
                std::size_t targetIndex{ 0 };
                if      (KEY_EVENT.code == sf::Keyboard::Num2) targetIndex = 1;
                else if (KEY_EVENT.code == sf::Keyboard::Num3) targetIndex = 2;
                else if (KEY_EVENT.code == sf::Keyboard::Num4) targetIndex = 3;
                else if (KEY_EVENT.code == sf::Keyboard::Num5) targetIndex = 4;
                else if (KEY_EVENT.code == sf::Keyboard::Num6) targetIndex = 5;
                else if (KEY_EVENT.code == sf::Keyboard::Num7) targetIndex = 6;
                else if (KEY_EVENT.code == sf::Keyboard::Num8) targetIndex = 7;
                else if (KEY_EVENT.code == sf::Keyboard::Num9) targetIndex = 8;

                if ((imageIndex_ == targetIndex) ||
                    (targetIndex > (COUNT - 1)))
                {
                    return false;
                }

                const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f /
                    static_cast<float>(COUNT));

                isImageProcAllowed_ = false;
                sliderbarUPtr_->SetCurrentValue(static_cast<float>(targetIndex) *
                    SINGLE_IMAGE_SLIDER_WIDTH_RATIO);
                isImageProcAllowed_ = true;

                std::size_t i{ imageIndex_ };
                while(true)
                {
                    if (targetIndex < imageIndex_)
                    {
                        imageMoveQueue_.push(--i);
                    }
                    else
                    {
                        imageMoveQueue_.push(++i);
                    }

                    if (i == targetIndex)
                    {
                        break;
                    }
                }

                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::Left)
            {
                if (imageIndex_ > 0)
                {
                    const std::size_t NEW_INDEX(imageIndex_ - 1);

                    const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f /
                        static_cast<float>(COUNT));

                    isImageProcAllowed_ = false;
                    sliderbarUPtr_->SetCurrentValue(static_cast<float>(NEW_INDEX) *
                        SINGLE_IMAGE_SLIDER_WIDTH_RATIO);
                    isImageProcAllowed_ = true;

                    imageMoveQueue_.push(NEW_INDEX);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (KEY_EVENT.code == sf::Keyboard::Right)
            {
                if (imageIndex_ < (COUNT - 1))
                {
                    const std::size_t NEW_INDEX(imageIndex_ + 1);

                    const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f /
                        static_cast<float>(COUNT));

                    isImageProcAllowed_ = false;
                    sliderbarUPtr_->SetCurrentValue(static_cast<float>(NEW_INDEX) *
                        SINGLE_IMAGE_SLIDER_WIDTH_RATIO);
                    isImageProcAllowed_ = true;

                    //if already at the end, then make sure the sliderbar is
                    //all the way to the right
                    if (NEW_INDEX >= (COUNT - 1))
                    {
                        //prevent processing and adding to the imageMoveQueue_ or calling
                        //SetupSelectImage() when setting the sliderbar value here.
                        isImageProcAllowed_ = false;
                        sliderbarUPtr_->SetCurrentValue(1.0f);
                        isImageProcAllowed_ = true;
                    }

                    imageMoveQueue_.push(NEW_INDEX);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }

        if (popupInfo_.Type() == game::Popup::CharacterSelection)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Num1) && (popupInfo_.IsNumberValid(0)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
                
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 0);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num2) && (popupInfo_.IsNumberValid(1)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
                
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 1);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num3) && (popupInfo_.IsNumberValid(2)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
                
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 2);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num4) && (popupInfo_.IsNumberValid(3)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
                
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 3);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num5) && (popupInfo_.IsNumberValid(4)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
                
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 4);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num6) && (popupInfo_.IsNumberValid(5)))
            {
                SoundManager::Instance()->
                    Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 5);
                return true;
            }
        }

        if ((popupInfo_.Buttons() & Response::Select) &&
            (KEY_EVENT.code == sf::Keyboard::S))
        {
            if (popupInfo_.Type() == game::Popup::CharacterSelection)
            {
                auto const TEXT{ popupInfo_.TextVec()[imageIndex_] };
                if (TEXT.empty())
                {
                    selection_ = static_cast<int>(imageIndex_);
                }
                else
                {
                    selection_ = -1;//any negative value will work here
                }
            }

            return HandleSelect();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStage::SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED)
    {
        if (false == isImageProcAllowed_)
        {
            return;
        }

        auto const COUNT{ ((popupInfo_.Type() == game::Popup::CharacterSelection) ?
            game::Game::Instance()->State().Party().Characters().size() :
            popupInfo_.ImagesCount()) };

        std::size_t newIndex(NEW_IMAGE_INDEX);
        if (newIndex >= COUNT)
        {
            newIndex = COUNT - 1;
        }

        if ((imageIndex_ != newIndex) || isInitialAnimation_)
        {
            areImagesMovingLeft_ = (imageIndex_ < newIndex);
            imageIndex_ = newIndex;
            selection_ = static_cast<int>(imageIndex_);

            std::ostringstream ss;
            ss << imageIndex_ + 1 << "/" << COUNT;
            const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                     FontManager::Instance()->Font_Typical(),
                                                     FontManager::Instance()->Size_Smallish(),
                                                     gui::PopupManager::Color_Font(),
                                                     Justified::Center);
            sf::FloatRect imageCountTextRect(textRegion_);
            imageCountTextRect.height = 0.0f;
            imageNumTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "PopupStage'sSelectNumber", TEXT_INFO, imageCountTextRect);

            imageSlider_.Reset(SLIDER_SPEED);

            imageSpritePrev_ = imageSpriteCurr_;
            texturePrev_ = textureCurr_;
            imageSpritePrev_.setTexture(texturePrev_, true);

            if (popupInfo_.Type() == game::Popup::CharacterSelection)
            {
                auto const CREATURE_PTR{
                    game::Game::Instance()->State().Party().GetAtOrderPos(imageIndex_) };

                sfml_util::gui::CreatureImageManager::Instance()->GetImage(
                    textureCurr_,
                    CREATURE_PTR->ImageFilename(),
                    true);
            }
            else
            {
                textureCurr_ = popupInfo_.ImagesAt(imageIndex_);
            }

            if (popupInfo_.AreImagesCreatures())
            {
                sfml_util::Invert(textureCurr_);
                sfml_util::Mask(textureCurr_, sf::Color::White);
            }

            imageSpriteCurr_.setTexture(textureCurr_, true);
            imageSpriteCurr_.setScale(1.0f, 1.0f);

            auto const POS_LEFT{ textRegion_.left +
                ((areImagesMovingLeft_) ? textRegion_.width : 0.0f) };

            auto const POS_TOP{ imagePosTop_ };

            imageSpriteCurr_.setPosition(POS_LEFT, POS_TOP);

            //establish target scale of new image
            const float RESIZE_RATIO(0.8f);
            const float SPRITE_TARGET_WIDTH(imagesRect_.width * RESIZE_RATIO);
            const float SPRITE_TARGET_HEIGHT(imagesRect_.height * RESIZE_RATIO);
            const float SCALE_HORIZ(SPRITE_TARGET_WIDTH / imageSpriteCurr_.getGlobalBounds().width);
            const float SCALE_VERT(SPRITE_TARGET_HEIGHT / imageSpriteCurr_.getGlobalBounds().height);
            imageCurrTargetScale_ = std::min(SCALE_HORIZ, SCALE_VERT);
            imageSpriteCurr_.setScale(0.0f, 0.0f);

            imagePrevStartScale_ = imageSpritePrev_.getScale().x;
            imagePrevStartPosX_ = imageSpritePrev_.getPosition().x;

            //establish the distance selectSpriteCurr, the new index sprite, will travel
            {
                sf::Sprite s(imageSpriteCurr_);
                s.setScale(imageCurrTargetScale_, imageCurrTargetScale_);
                const float POS_LEFT_CENTERED((textRegion_.left + (textRegion_.width * 0.5f)) -
                    (s.getGlobalBounds().width * 0.5f));

                if (areImagesMovingLeft_)
                {
                    imageCurrTravelDist_ = (textRegion_.left + textRegion_.width) - POS_LEFT_CENTERED;
                }
                else
                {
                    imageCurrTravelDist_ = POS_LEFT_CENTERED - textRegion_.left;
                }
            }

            //establish the distance selectSpritePrev, the old index sprite, will travel
            if (areImagesMovingLeft_)
            {
                imagePrevTravelDist_ = imageSpritePrev_.getGlobalBounds().left - imagesRect_.left;
            }
            else
            {
                imagePrevTravelDist_ = (imagesRect_.left + imagesRect_.width) -
                    imageSpritePrev_.getGlobalBounds().left;
            }
        }
    }


    void PopupStage::SetupInfoText(const std::string & TEXT)
    {
        const std::string TEXT_TO_USE((TEXT.empty()) ? " " : TEXT);

        const sfml_util::gui::TextInfo INFO_TEXT_INFO(TEXT_TO_USE,
                                                      FontManager::Instance()->Font_Default1(),
                                                      FontManager::Instance()->Size_Small(),
                                                      gui::PopupManager::Color_Font(),
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


    void PopupStage::SetupMusicSheetPageRightText(const game::song::SongPtrC_t SONG_CPTRC)
    {
        if (SONG_CPTRC == nullptr)
        {
            return;
        }

        //setup song title text
        const sfml_util::gui::TextInfo SONG_TITLE_TEXTINFO(
            SONG_CPTRC->Name(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        const sf::FloatRect SONG_TITLE_TEXTRECT{ pageRectRight_.left,
                                                 pageRectRight_.top,
                                                 pageRectRight_.width,
                                                 0.0f };

        if (spellTitleTextRegionUPtr_.get() == nullptr)
        {
            spellTitleTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "MusicSheetPopupWindowSongTitle",
                SONG_TITLE_TEXTINFO,
                SONG_TITLE_TEXTRECT);
        }
        else
        {
            spellTitleTextRegionUPtr_->SetText(SONG_CPTRC->Name());
        }

        //setup song image
        sfml_util::gui::SongImageManager::Instance()->Get(spellTexture_,
                                                          SONG_CPTRC->Which());

        spellSprite_.setTexture(spellTexture_);
        auto const SPELL_IMAGE_SCALE{ sfml_util::MapByRes(0.75f, 4.0f) };
        spellSprite_.setScale(SPELL_IMAGE_SCALE, SPELL_IMAGE_SCALE);
        spellSprite_.setColor(spellColorImageCurrent_);

        spellSprite_.setPosition((pageRectRight_.left + (pageRectRight_.width * 0.5f)) -
            (spellSprite_.getGlobalBounds().width * 0.5f),
                spellTitleTextRegionUPtr_->GetEntityRegion().top + spellTitleTextRegionUPtr_->
                    GetEntityRegion().height + sfml_util::MapByRes(5.0f, 60.0f));

        auto const SYM_SCALE{ sfml_util::MapByRes(0.75f, 3.75f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        auto const X_SYM_POS_LEFT{ (spellSprite_.getGlobalBounds().left +
            (spellSprite_.getGlobalBounds().width * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        auto const X_SYM_POS_TOP{ (spellSprite_.getGlobalBounds().top +
            (spellSprite_.getGlobalBounds().height * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        //setup spell details text
        std::ostringstream ss;
        ss << "Mana Cost: " << SONG_CPTRC->ManaCost() << "\n"
           << "Rank: " << SONG_CPTRC->Rank() << "\n"
           << "Targets " << game::TargetType::Name(SONG_CPTRC->Target()) << "\n"
           << "Play during " << game::Phase::ToString(SONG_CPTRC->ValidPhases(), false) << "\n";

        const sfml_util::gui::TextInfo SONG_DETAILS_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        auto const SONGDETAILS_TEXTRECT_LEFT   { pageRectRight_.left };

        auto const SONGDETAILS_TEXTRECT_TOP    { spellSprite_.getGlobalBounds().top +
            spellSprite_.getGlobalBounds().height + sfml_util::MapByRes(10.0f, 90.0f) };

        auto const SONGDETAILS_TEXTRECT_WIDTH  { pageRectRight_.width };
        auto const SONGDETAILS_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SONG_DETAILS_TEXTRECT{ SONGDETAILS_TEXTRECT_LEFT,
                                                   SONGDETAILS_TEXTRECT_TOP,
                                                   SONGDETAILS_TEXTRECT_WIDTH,
                                                   SONGDETAILS_TEXTRECT_HEIGHT };

        if (spellDetailsTextUPtr_.get() == nullptr)
        {
            spellDetailsTextUPtr_ = std::make_unique<gui::TextRegion>(
                "MusicsheetPopupWindowSongDetails",
                SONG_DETAILS_TEXTINFO,
                SONG_DETAILS_TEXTRECT);
        }
        else
        {
            spellDetailsTextUPtr_->SetText(ss.str());
        }

        //setup song 'unable to cast' text
        willShowXImage_ = false;
        ss.str(" ");
        spellUnableTextWillShow_ = false;
        if (DoesCharacterHaveEnoughManaToPlaySong(SONG_CPTRC) == false)
        {
            willShowXImage_ = true;
            ss << "Insufficient Mana";
            spellUnableTextWillShow_ = true;
        }
        else if (CanPlaySongInPhase(SONG_CPTRC) == false)
        {
            willShowXImage_ = true;

            auto const CURRENT_PHASE{ game::LoopManager::Instance()->GetPhase() };

            if (CURRENT_PHASE & game::Phase::Combat)
            {
                ss << "Cannot play during combat.";
            }
            else if (CURRENT_PHASE & game::Phase::Conversation)
            {
                ss << "Cannot play while talking.";
            }
            else if (CURRENT_PHASE & game::Phase::Exploring)
            {
                ss << "Cannot play while exploring.";
            }
            else if (CURRENT_PHASE & game::Phase::Inventory)
            {
                ss << "Cannot play from inventory.";
            }
            else
            {
                ss << "Only during "
                    << game::Phase::ToString(SONG_CPTRC->ValidPhases(), false) << ".";
            }

            spellUnableTextWillShow_ = true;
        }

        const sfml_util::gui::TextInfo SONG_UNABLE_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            SPELL_UNABLE_TEXT_COLOR_,
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        auto const VERT_SPACER{ sfml_util::MapByRes(15.0f, 60.0f) };

        auto const SONG_UNABLE_TEXTRECT_LEFT   { pageRectRight_.left };

        auto const SONG_UNABLE_TEXTRECT_TOP    { spellDetailsTextUPtr_->GetEntityRegion().top +
            spellDetailsTextUPtr_->GetEntityRegion().height + VERT_SPACER };

        auto const SONG_UNABLE_TEXTRECT_WIDTH  { pageRectRight_.width };
        auto const SONG_UNABLE_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SONG_UNABLE_TEXTRECT{ SONG_UNABLE_TEXTRECT_LEFT,
                                                   SONG_UNABLE_TEXTRECT_TOP,
                                                   SONG_UNABLE_TEXTRECT_WIDTH,
                                                   SONG_UNABLE_TEXTRECT_HEIGHT };

        spellUnableTextUPtr_ = std::make_unique<gui::TextRegion>(
            "MusicsheetPopupWindowSongUnableToCast",
            SONG_UNABLE_TEXTINFO,
            SONG_UNABLE_TEXTRECT);

        //setup spell description text
        ss.str("");
        ss << SONG_CPTRC->Desc() << "  " << SONG_CPTRC->DescExtra();

        const sfml_util::gui::TextInfo SONG_DESC_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        auto const SONG_DESC_HORIZ_MARGIN{ sfml_util::MapByRes(15.0f, 30.0f) };
        auto const SONG_DESC_TEXTRECT_LEFT{ pageRectRight_.left + SONG_DESC_HORIZ_MARGIN };
        auto spellDescTextRectTop{ 0.0f };
        if (spellUnableTextWillShow_)
        {
            spellDescTextRectTop = spellUnableTextUPtr_->GetEntityRegion().top +
                spellUnableTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        else
        {
            spellDescTextRectTop = spellDetailsTextUPtr_->GetEntityRegion().top +
                spellDetailsTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        auto const SONG_DESC_TEXTRECT_WIDTH{ pageRectRight_.width -
            (SONG_DESC_HORIZ_MARGIN * 2.0f) };

        auto const SONG_DESC_TEXTRECT_HEIGHT{ ((pageRectRight_.top + pageRectRight_.height) -
            spellDescTextRectTop) - VERT_SPACER };

        const sf::FloatRect SONG_DESC_TEXTRECT{ SONG_DESC_TEXTRECT_LEFT,
                                                spellDescTextRectTop,
                                                SONG_DESC_TEXTRECT_WIDTH,
                                                SONG_DESC_TEXTRECT_HEIGHT };

        if (spellDescTextUPtr_.get() == nullptr)
        {
            spellDescTextUPtr_ = std::make_unique<gui::TextRegion>(
                "SpellnbookPopupWindowSpellDescription",
                SONG_DESC_TEXTINFO,
                SONG_DESC_TEXTRECT);
        }
        else
        {
            spellDescTextUPtr_->Setup(SONG_DESC_TEXTINFO, SONG_DESC_TEXTRECT, this);
        }
    }


    void PopupStage::MoveSpellbookPageRightColors(const float ELAPSED_TIME_SECONDS)
    {
        auto const RATIO_COMPLETE{ spellColorSlider_.Update(ELAPSED_TIME_SECONDS) };

        auto const IMAGE_ALPHA_DIFF{
            static_cast<float>(spellColorImageEnd_.a - spellColorImageStart_.a) };

        auto const IMAGE_ALPHA_NEW{ static_cast<sf::Uint8>(IMAGE_ALPHA_DIFF * RATIO_COMPLETE) };
        spellColorImageCurrent_.a = IMAGE_ALPHA_NEW;

        auto const TEXT_ALPHA_DIFF{
            static_cast<float>(spellColorTextEnd_.a - spellColorTextStart_.a) };

        auto const TEXT_ALPHA_NEW{ static_cast<sf::Uint8>(TEXT_ALPHA_DIFF * RATIO_COMPLETE) };
        spellColorTextCurrent_.a = TEXT_ALPHA_NEW;

        //Note: rgb values are all the same between spellColorImageCurrent_ and
        //      spellColorImageTarget_ and are always full/white.
        spellColorImageCurrent_.r = spellColorImageCurrent_.g = spellColorImageCurrent_.b = 255;

        //Note: rgb values are all the same between spellColorTextCurrent_ and
        //      spellColorTextTarget_ and are always black.
        spellColorTextCurrent_.r = spellColorTextCurrent_.g = spellColorTextCurrent_.b = 0;
    }


    void PopupStage::SetupSpellbookPageRightForFadeIn()
    {
        SetupMusicSheetPageRightText(songCurrentPtr_);
        
        spellColorImageStart_ = sf::Color::Transparent;
        spellColorImageCurrent_ = sf::Color::Transparent;
        spellColorTextStart_ = sf::Color::Transparent;
        spellColorTextCurrent_ = sf::Color::Transparent;
        SetSpellbookPageRightColors();

        spellColorImageEnd_ = sf::Color(255, 255, 255, SPELLBOOK_IMAGE_ALPHA_);
        spellColorTextEnd_ = sf::Color(0, 0, 0, 255);

        fadeState_ = FadeState::FadingIn;

        spellColorSlider_.Reset(SPELLBOOK_COLOR_FADE_SPEED_);

        if (songCurrentPtr_ != nullptr)
        {
            if (songCurrentPtr_->Type() == game::song::SongType::Drum)
            {
                SoundManager::Instance()->Getsound_effect_set(sound_effect_set::DrumBlip).PlayRandom();
            }
            else if (songCurrentPtr_->Type() == game::song::SongType::Guitar)
            {
                SoundManager::Instance()->Getsound_effect_set(sound_effect_set::GuitarStrum).PlayRandom();
            }
        }
    }


    void PopupStage::SetSpellbookPageRightColors()
    {
        spellSprite_.setColor(spellColorImageCurrent_);

        const sfml_util::gui::ColorSet TEXT_COLOR_SET(
            spellColorTextCurrent_,
            spellColorTextCurrent_,
            spellColorTextCurrent_,
            spellColorTextCurrent_);

        spellTitleTextRegionUPtr_->SetEntityColors(TEXT_COLOR_SET);
        spellDetailsTextUPtr_->SetEntityColors(TEXT_COLOR_SET);
        spellDescTextUPtr_->SetEntityColors(TEXT_COLOR_SET);

        auto unableTextColor{ SPELL_UNABLE_TEXT_COLOR_ };
        unableTextColor.a = spellColorTextCurrent_.a;
        const sfml_util::gui::ColorSet TUNABLE_EXT_COLOR_SET(
            unableTextColor,
            unableTextColor,
            unableTextColor,
            unableTextColor);

        spellUnableTextUPtr_->SetEntityColors(TUNABLE_EXT_COLOR_SET);
    }


    bool PopupStage::DoesCharacterHaveEnoughManaToPlaySong(
        const game::song::SongPtrC_t SONG_CPTRC) const
    {
        return (popupInfo_.CreaturePtr()->TraitWorking(game::stats::Traits::Mana) >=
            SONG_CPTRC->ManaCost());
    }


    bool PopupStage::CanPlaySongInPhase(const game::song::SongPtrC_t SONG_CPTRC) const
    {
        return (SONG_CPTRC->ValidPhases() & game::LoopManager::Instance()->GetPhase());
    }


    bool PopupStage::CanPlaySong(const game::song::SongPtrC_t SONG_CPTRC) const
    {
        return (DoesCharacterHaveEnoughManaToPlaySong(SONG_CPTRC) &&
            CanPlaySongInPhase(SONG_CPTRC));
    }


    bool PopupStage::HandleSongPlay()
    {
        if (CanPlaySong(listBoxSPtr_->GetSelected()->SONG_CPTRC))
        {
            if (songCurrentPtr_ != nullptr)
            {
                if (songCurrentPtr_->Type() == game::song::SongType::Drum)
                {
                    SoundManager::Instance()->Getsound_effect_set(sound_effect_set::DrumBlip).PlayRandom();
                }
                else if (songCurrentPtr_->Type() == game::song::SongType::Guitar)
                {
                    SoundManager::Instance()->Getsound_effect_set(sound_effect_set::GuitarStrum).PlayRandom();
                }
            }

            game::LoopManager::Instance()->PopupWaitEnd(
                Response::Select, listBoxSPtr_->GetSelectedIndex());

            return true;
        }
        else
        {
            SoundManager::Instance()->Getsound_effect_set(sfml_util::sound_effect_set::Prompt).Play(
                sound_effect::PromptWarn);

            if (FadeState::Waiting == fadeState_)
            {
                fadeState_ = FadeState::Warning;
                spellWarningTimerSec_ = 0.0f;
            }
            return false;
        }
    }


    void PopupStage::SetupCharacterSelectDetailText(const bool WILL_ERASE)
    {
        if (WILL_ERASE)
        {
            charDetailsTextRegionUPtr_->SetText(" ");
            return;
        }

        auto const CREATURE_PTR{
            game::Game::Instance()->State().Party().GetAtOrderPos(imageIndex_) };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name() << "\n"
           << CREATURE_PTR->RaceName();

        if (CREATURE_PTR->IsBeast())
        {
            if (CREATURE_PTR->Race() != game::creature::race::Wolfen)
            {
                ss << ", " << CREATURE_PTR->RoleName();
            }
            ss << " " << CREATURE_PTR->RankClassName() << "\n";
        }
        else
        {
            ss << " " << CREATURE_PTR->RankClassName() << " "
                << CREATURE_PTR->RoleName() << "\n";
        }

        ss << "Health:  " << CREATURE_PTR->HealthCurrent() << "/"
           << CREATURE_PTR->HealthNormal() << "\n"
           << "Condition:  " << CREATURE_PTR->ConditionNames(3) << "\n";

        charDetailsTextRegionUPtr_->SetText(ss.str());
    }


    void PopupStage::SetupCharacterSelectionRejectImage(const bool WILL_ERASE)
    {
        if (imageIndex_ < popupInfo_.TextVec().size())
        {
            auto const TEXT{ popupInfo_.TextVec()[imageIndex_] };

            if (TEXT.empty())
            {
                selection_ = static_cast<int>(imageIndex_);
            }
            else
            {
                selection_ = -1;//any negative value will work here
            }

            if (TEXT.empty() || WILL_ERASE)
            {
                willShowXImage_ = false;
                SetupCharacterSelectDetailText(false);
                charDetailsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::Black);
            }
            else
            {
                willShowXImage_ = true;
                charDetailsTextRegionUPtr_->SetText(TEXT);
                charDetailsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color(100, 32, 32));
            }
        }
    }


    void PopupStage::ItemProfileSetup()
    {
        game::item::ItemProfileWarehouse::Instance()->Setup();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
    }


    void PopupStage::SetupOuterAndInnerRegion()
    {
        if (popupInfo_.Image() == sfml_util::PopupImage::MusicSheet)
        {
            auto const MUSICSHEET_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() *
                PopupStage::MUSICSHEET_POPUP_BACKGROUND_WIDTH_RATIO_ };

            auto const MUSICSHEET_HEIGHT{ (static_cast<float>(backgroundTexture_.getSize().y) *
                MUSICSHEET_WIDTH) / static_cast<float>(backgroundTexture_.getSize().x) };

            sf::FloatRect region;
            region.left =
                (sfml_util::Display::Instance()->GetWinWidth() - MUSICSHEET_WIDTH) * 0.5f;

            region.top =
                (sfml_util::Display::Instance()->GetWinHeight() - MUSICSHEET_HEIGHT) * 0.5f;

            region.width = MUSICSHEET_WIDTH;
            region.height = MUSICSHEET_HEIGHT;

            SetupFullscreenRegionsAndBackgroundImage(region);
        }
        else
        {
            PopupStageBase::SetupOuterAndInnerRegion();
        }
    }


    void PopupStage::SetupNumberSelectionPopup()
    {
        const sfml_util::gui::TextInfo INFO_TEXT_INFO(" ",
            FontManager::Instance()->Font_Default1(),
            FontManager::Instance()->Size_Small(),
            gui::PopupManager::Color_Font(),
            Justified::Center);

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
            FontManager::Instance()->Font_Default1(),
            FontManager::Instance()->Size_Large(),
            sf::Color::White,
            Justified::Left);

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
            FontManager::Color_Light(),
            TEXTENTRY_BOX_INFO,
            this));

        textEntryBoxSPtr_->SetText(minNumSS.str());
        EntityAdd(textEntryBoxSPtr_.get());

        RemoveFocus();
        SetFocus(textEntryBoxSPtr_.get());
        textEntryBoxSPtr_->SetHasFocus(true);    
    }


    void PopupStage::SetupCharacterSelectionPopup()
    {
        imagesRect_ = textRegion_;

        imagesRect_.top = textRegionUPtr_->GetEntityPos().y +
            sfml_util::MapByRes(70.0f, 120.0f);

        imagesRect_.height = 300.0f;

        imagePosTop_ = (imagesRect_.top + (imagesRect_.height * 0.5f));

        auto const SYM_SCALE{ sfml_util::MapByRes(0.5f, 2.0f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        auto const X_SYM_POS_TOP{ (imagesRect_.top + (imagesRect_.height * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        auto const X_SYM_POS_LEFT{ (imagesRect_.left + (imagesRect_.width * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        sf::FloatRect charDetailsTextRegion(textRegion_);

        charDetailsTextRegion.top = imagesRect_.top + imagesRect_.height;

        charDetailsTextRegion.height = 0.0f;

        auto charDetailsTextInfo{ popupInfo_.TextInfo() };
        charDetailsTextInfo.text = " ";

        charDetailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupWindow's_CharacterSelection_DetailsText",
            charDetailsTextInfo,
            charDetailsTextRegion);

        //sliderbar setup
        const float SLIDERBAR_LENGTH(textRegion_.width * 0.5f);
        const float SLIDERBAR_POS_LEFT(textRegion_.left +
            ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f));

        sliderbarPosTop_ = charDetailsTextRegionUPtr_->GetEntityRegion().top +
            charDetailsTextRegionUPtr_->GetEntityRegion().height + 100.0f;

        if (sliderbarUPtr_.get() != nullptr)
        {
            EntityRemove(sliderbarUPtr_.get());
        }

        sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz),
            this);

        EntityAdd(sliderbarUPtr_.get());

        isImageProcAllowed_ =
            (game::Game::Instance()->State().Party().Characters().size() != 0);

        imageMoveQueue_.push(0);
    }


    void PopupStage::SetupImageSelectionPopup()
    {
        imagesRect_ = textRegion_;

        //added is a pad so the text does not touch the images
        imagesRect_.top = textRegionUPtr_->GetEntityPos().y +
            sfml_util::MapByRes(70.0f, 200.0f);

        imagesRect_.height =
            (sliderbarPosTop_ - (ButtonTextHeight() * 2.0f)) - imagesRect_.top;

        imagePosTop_ = (imagesRect_.top + (imagesRect_.height * 0.5f));

        isImageProcAllowed_ = (popupInfo_.ImagesCount() != 0);

        imageMoveQueue_.push(0);

        if (popupInfo_.ImagesCount() == 1)
        {
            const sfml_util::gui::TextInfo TEXT_INFO("(there is only one image)",
                FontManager::Instance()->Font_Default1(),
                FontManager::Instance()->Size_Small(),
                gui::PopupManager::Color_Font(),
                Justified::Center);

            sf::FloatRect region(textRegion_);
            region.top = sliderbarPosTop_;
            region.height = 0.0f;
            imageWrnTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "PopupStage'sImageSelectionion", TEXT_INFO, region);

            EntityAdd(imageWrnTextRegionUPtr_.get());
            if (sliderbarUPtr_.get() != nullptr)
            {
                EntityRemove(sliderbarUPtr_.get());
            }
        }
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

        titleUPtr_ = std::make_unique<gui::TextRegion>("ImageFadePopupTitle",
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

        descUPtr_ = std::make_unique<gui::TextRegion>("ImageFadePopupDesc",
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


    void PopupStage::SetupMusicSheetPopup()
    {
        //setup regions
        auto const LEFT_SIDE_RECT_RAW { sfml_util::ConvertRect<int, float>(
            sfml_util::gui::PopupManager::Rect_MusicSheet_LeftSide()) };

        auto const SCALE(innerRegion_.width /
            static_cast<float>(backgroundTexture_.getSize().x));

        pageRectLeft_.left   = innerRegion_.left + (LEFT_SIDE_RECT_RAW.left * SCALE);
        pageRectLeft_.top    = innerRegion_.top + (LEFT_SIDE_RECT_RAW.top * SCALE);
        pageRectLeft_.width  = LEFT_SIDE_RECT_RAW.width * SCALE;
        pageRectLeft_.height = LEFT_SIDE_RECT_RAW.height * SCALE;

        auto const RIGHT_SIDE_RECT_RAW{ sfml_util::ConvertRect<int, float>(
            sfml_util::gui::PopupManager::Rect_MusicSheet_RightSide()) };

        pageRectRight_.left = innerRegion_.left + (RIGHT_SIDE_RECT_RAW.left * SCALE);
        pageRectRight_.top = innerRegion_.top + (RIGHT_SIDE_RECT_RAW.top * SCALE);
        pageRectRight_.width = RIGHT_SIDE_RECT_RAW.width * SCALE;
        pageRectRight_.height = RIGHT_SIDE_RECT_RAW.height * SCALE;

        //setup the left accent image
        {
            sfml_util::gui::PopupManager::Instance()->LoadRandomAccentImage(accentTexture1_);
            accentSprite1_.setTexture(accentTexture1_);

            auto const SIZE_RATIO{ misc::random::Float(0.65f, 0.85f) };

            const float SCALE_VERT((pageRectLeft_.height * SIZE_RATIO) /
                accentSprite1_.getLocalBounds().height);

            accentSprite1_.setScale(SCALE_VERT, SCALE_VERT);

            if (accentSprite1_.getGlobalBounds().width > (pageRectLeft_.width * SIZE_RATIO))
            {
                const float SCALE_HORIZ((pageRectLeft_.width * SIZE_RATIO) /
                    accentSprite1_.getLocalBounds().width);

                if (SCALE_HORIZ < SCALE_VERT)
                {
                    accentSprite1_.setScale(SCALE_HORIZ, SCALE_HORIZ);
                }
            }

            //always center the accent sprite image
            const float ACCENT1_POS_LEFT((pageRectLeft_.left + (pageRectLeft_.width  * 0.5f)) -
                (accentSprite1_.getGlobalBounds().width * 0.5f));

            const float ACCENT1_POS_TOP ((pageRectLeft_.top  + (pageRectLeft_.height * 0.5f)) -
                (accentSprite1_.getGlobalBounds().height * 0.5f));

            accentSprite1_.setPosition(ACCENT1_POS_LEFT, ACCENT1_POS_TOP);

            accentSprite1_.setColor(sf::Color(255, 255, 255, 16));
        }

        //setup the right accent image
        {
            sfml_util::gui::PopupManager::Instance()->LoadRandomAccentImage(accentTexture2_);
            accentSprite2_.setTexture(accentTexture2_);

            auto const SIZE_RATIO{ misc::random::Float(0.65f, 0.85f) };

            const float SCALE_VERT((pageRectRight_.height * SIZE_RATIO) /
                accentSprite2_.getLocalBounds().height);

            accentSprite2_.setScale(SCALE_VERT, SCALE_VERT);

            if (accentSprite2_.getGlobalBounds().width > (pageRectRight_.width * SIZE_RATIO))
            {
                const float SCALE_HORIZ((pageRectRight_.width * SIZE_RATIO) /
                    accentSprite2_.getLocalBounds().width);

                if (SCALE_HORIZ < SCALE_VERT)
                {
                    accentSprite2_.setScale(SCALE_HORIZ, SCALE_HORIZ);
                }
            }

            //always center the accent sprite image
            auto const ACCENT2_POS_LEFT{ (pageRectRight_.left + (pageRectRight_.width  * 0.5f))
                - (accentSprite2_.getGlobalBounds().width * 0.5f) };

            auto const ACCENT2_POS_TOP{ (pageRectRight_.top + (pageRectRight_.height * 0.5f))
                - (accentSprite2_.getGlobalBounds().height * 0.5f) };

            accentSprite2_.setPosition(ACCENT2_POS_LEFT, ACCENT2_POS_TOP);

            accentSprite2_.setColor(sf::Color(255, 255, 255, 16));
        }

        //setup player image
        sfml_util::gui::CreatureImageManager::Instance()->GetImage(playerTexture_,
            popupInfo_.CreaturePtr()->ImageFilename(), true);

        sfml_util::Invert(playerTexture_);
        sfml_util::Mask(playerTexture_, sf::Color::White);
        //
        playerSprite_.setTexture(playerTexture_ );
        auto const PLAYER_IMAGE_SCALE{ sfml_util::MapByRes(0.55f, 3.5f) };
        playerSprite_.setScale(PLAYER_IMAGE_SCALE, PLAYER_IMAGE_SCALE);
        playerSprite_.setColor(sf::Color(255, 255, 255, 192));
        playerSprite_.setPosition(pageRectLeft_.left, pageRectLeft_.top);

        //setup player details text
        auto cPtr{ popupInfo_.CreaturePtr() };
        std::ostringstream ss;
        ss << cPtr->Name() << "\n";

        if (cPtr->IsBeast())
        {
            ss << cPtr->RaceName();

            if (cPtr->Race() != game::creature::race::Wolfen)
            {
                ss << ", " << cPtr->RoleName();
            }

            ss << " " << cPtr->RankClassName() << "\n";
        }
        else
        {
            ss << cPtr->RankClassName() << " " << cPtr->RoleName() << "\n"
                << cPtr->RaceName() << "\n";
        }

        ss << "Rank:  " << cPtr->Rank() << "\n"
            << "Health:  " << cPtr->HealthCurrent() << "/" << cPtr->HealthNormal() << " "
            << cPtr->HealthPercentStr() << "\n"
            << "Mana:  " << cPtr->TraitWorking(game::stats::Traits::Mana) << "/"
            << cPtr->TraitNormal(game::stats::Traits::Mana) << "\n"
            << "\n";

        const sfml_util::gui::TextInfo DETAILS_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        const sf::FloatRect DETAILS_TEXT_RECT{
            pageRectLeft_.left +
                playerSprite_.getGlobalBounds().width +
                sfml_util::MapByRes(10.0f, 40.0f),
            pageRectLeft_.top + sfml_util::MapByRes(20.0f, 80.0f),
            0.0f,
            0.0f };

        charDetailsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "MusicSheetPopupWindowDetails",
            DETAILS_TEXTINFO,
            DETAILS_TEXT_RECT);

        //spell listbox label
        const sfml_util::gui::TextInfo LISTBOX_LABEL_TEXTINFO(
            "Songs",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        const sf::FloatRect LISTBOX_LABEL_TEXTRECT{
            pageRectLeft_.left + sfml_util::MapByRes(10.0f, 40.0f),
            playerSprite_.getGlobalBounds().top +
                playerSprite_.getGlobalBounds().height +
                sfml_util::MapByRes(20.0f, 80.0f),
            0.0f,
            0.0f };

        listBoxLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "MusicSheetPopupWindowSpellListLabel",
            LISTBOX_LABEL_TEXTINFO,
            LISTBOX_LABEL_TEXTRECT);

        //spell listbox
        auto const LISTBOX_MARGIN     { sfml_util::MapByRes(15.0f, 45.0f) };
        auto const LISTBOX_RECT_LEFT  { pageRectLeft_.left + LISTBOX_MARGIN };

        auto const LISTBOX_RECT_TOP   { listBoxLabelTextRegionUPtr_->GetEntityRegion().top +
            listBoxLabelTextRegionUPtr_->GetEntityRegion().height + LISTBOX_MARGIN };

        auto const LISTBOX_RECT_WIDTH { pageRectLeft_.width - (LISTBOX_MARGIN * 2.0f) };

        auto const LISTBOX_RECT_HEIGHT{ ((pageRectLeft_.top + pageRectLeft_.height) -
            LISTBOX_RECT_TOP) - (LISTBOX_MARGIN * 2.0f) };

        const sf::FloatRect LISTBOX_RECT{
            LISTBOX_RECT_LEFT,
            LISTBOX_RECT_TOP,
            LISTBOX_RECT_WIDTH,
            LISTBOX_RECT_HEIGHT };

        const sfml_util::gui::box::Info LISTBOX_BOX_INFO(
            1,
            true,
            LISTBOX_RECT,
            LISTBOX_COLORSET_,
            LISTBOX_BG_INFO_);

        sfml_util::gui::ListBoxItemSLst_t listBoxItemsSList;
        auto const SONG_PVEC{ cPtr->SongsPVec() };
        for (auto const NEXT_SONG_PTR : SONG_PVEC)
        {
            listBoxItemTextInfo_.text = NEXT_SONG_PTR->Name();

            auto const LISTBOXITEM_SPTR( std::make_shared<gui::ListBoxItem>(
                NEXT_SONG_PTR->Name() + "_MusicListBoxEntry",
                listBoxItemTextInfo_,
                NEXT_SONG_PTR,
                CanPlaySong(NEXT_SONG_PTR)) );

            listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
        }

        listBoxSPtr_ = std::make_shared<gui::ListBox>(
            "PopupStage'sMusicListBox",
            LISTBOX_RECT,
            listBoxItemsSList,
            this,
            10.0f,
            6.0f,
            LISTBOX_BOX_INFO,
            LISTBOX_LINE_COLOR_,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        EntityAdd(listBoxSPtr_.get());
        listBoxSPtr_->SetSelectedIndex(0);
        listBoxSPtr_->SetImageColor(LISTBOX_IMAGE_COLOR_);

        //Force spell listbox to take focus so that user up/down
        //keystrokes work without having to click on the listbox.
        SetFocus(listBoxSPtr_.get());

        //Force spell listbox selection up and down to force
        //colors to correct.
        listBoxSPtr_->WillPlaySoundEffects(false);
        sf::Event::KeyEvent keyEvent;
        keyEvent.code = sf::Keyboard::Down;
        listBoxSPtr_->KeyRelease(keyEvent);
        keyEvent.code = sf::Keyboard::Up;
        listBoxSPtr_->KeyRelease(keyEvent);
        listBoxSPtr_->WillPlaySoundEffects(true);

        //setup initial values for spellbook page right text and colors
        songCurrentPtr_ = listBoxSPtr_->At(0)->SONG_CPTRC;
        SetupSpellbookPageRightForFadeIn();
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

        titleUPtr_ = std::make_unique<gui::TextRegion>(
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

        descUPtr_ = std::make_unique<gui::TextRegion>(
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
