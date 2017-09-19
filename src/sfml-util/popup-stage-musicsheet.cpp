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
// popup-stage-musicsheet.cpp
//
#include "popup-stage-musicsheet.hpp"

#include "game/song/song.hpp"
#include "game/creature/creature.hpp"
#include "game/loop-manager.hpp"

#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/song-image-manager.hpp"
#include "sfml-util/sound-manager.hpp"

#include "misc/random.hpp"

#include <sstream>
#include <random>
#include <algorithm>


namespace sfml_util
{

    const float     PopupStageMusicSheet::MUSICSHEET_POPUP_BACKGROUND_WIDTH_RATIO_  { 0.9f };
    const float     PopupStageMusicSheet::SPELLBOOK_COLOR_FADE_SPEED_               { 6.0f };
    const sf::Color PopupStageMusicSheet::SPELL_UNABLE_TEXT_COLOR_{ sf::Color(127, 32, 32) };
    const sf::Uint8 PopupStageMusicSheet::SPELLBOOK_IMAGE_ALPHA_                    { 192 };
    const float     PopupStageMusicSheet::SPELL_WARNING_DURATION_SEC_               { 2.0f };


    PopupStageMusicSheet::PopupStageMusicSheet(const game::PopupInfo & POPUP_INFO)
    :
        PopupStageBase(POPUP_INFO),
        songCurrentPtr_(nullptr),
        charDetailsTextRegionUPtr_(),
        listBoxLabelTextRegionUPtr_(),
        listBoxSPtr_(),
        fadeState_(FadeState::Initial),
        playerTexture_(),
        playerSprite_(),
        pageRectLeft_(),
        pageRectRight_(),
        spellColorImageCurrent_(sf::Color::Transparent),
        spellColorImageStart_(sf::Color::Transparent),
        spellColorImageEnd_(sf::Color::Transparent),
        spellColorTextCurrent_(sf::Color::Transparent),
        spellColorTextStart_(sf::Color::Transparent),
        spellColorTextEnd_(sf::Color::Transparent),
        spellTitleTextRegionUPtr_(),
        spellDetailsTextUPtr_(),
        spellUnableTextUPtr_(),
        spellDescTextUPtr_(),
        spellTexture_(),
        spellSprite_(),
        spellUnableTextWillShow_(false),
        spellWarningTimerSec_(0.0f),
        spellWarnColorShaker_(SPELL_UNABLE_TEXT_COLOR_, sf::Color::Transparent, 60.0f),
        LISTBOX_IMAGE_COLOR_(sf::Color(255, 255, 255, 190)),
        LISTBOX_LINE_COLOR_(sfml_util::FontManager::Color_GrayDark()),
        LISTBOX_COLOR_FG_(LISTBOX_LINE_COLOR_),

        LISTBOX_COLOR_BG_(sfml_util::FontManager::Color_Orange() -
            sf::Color(100, 100, 100, 220)),

        LISTBOX_COLORSET_(LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_),
        LISTBOX_BG_INFO_(LISTBOX_COLOR_BG_),

        listBoxItemTextInfo_(" ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left)
    {}


    PopupStageMusicSheet::~PopupStageMusicSheet()
    {}


    bool PopupStageMusicSheet::HandleCallback(
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


    void PopupStageMusicSheet::Setup()
    {
        PopupStageBase::Setup();

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


    void PopupStageMusicSheet::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

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

        Stage::Draw(target, STATES);
    }


    void PopupStageMusicSheet::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

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
            spellUnableTextUPtr_->SetEntityColorFgBoth(
                spellWarnColorShaker_.Update(ELAPSED_TIME_SECONDS));

            spellWarningTimerSec_ += ELAPSED_TIME_SECONDS;
            if (spellWarningTimerSec_ > SPELL_WARNING_DURATION_SEC_)
            {
                spellUnableTextUPtr_->SetEntityColorFgBoth(SPELL_UNABLE_TEXT_COLOR_);
                spellWarningTimerSec_ = 0.0f;
                fadeState_ = FadeState::Waiting;
            }
        }
    }


    bool PopupStageMusicSheet::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
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

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStageMusicSheet::SetupOuterAndInnerRegion()
    {
        auto const MUSICSHEET_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() *
            MUSICSHEET_POPUP_BACKGROUND_WIDTH_RATIO_ };

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



    void PopupStageMusicSheet::SetupMusicSheetPageRightText(const game::song::SongPtrC_t SONG_CPTRC)
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


    void PopupStageMusicSheet::MoveSpellbookPageRightColors(const float ELAPSED_TIME_SECONDS)
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


    void PopupStageMusicSheet::SetupSpellbookPageRightForFadeIn()
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


    void PopupStageMusicSheet::SetSpellbookPageRightColors()
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


    bool PopupStageMusicSheet::DoesCharacterHaveEnoughManaToPlaySong(
        const game::song::SongPtrC_t SONG_CPTRC) const
    {
        return (popupInfo_.CreaturePtr()->TraitWorking(game::stats::Traits::Mana) >=
            SONG_CPTRC->ManaCost());
    }


    bool PopupStageMusicSheet::CanPlaySongInPhase(const game::song::SongPtrC_t SONG_CPTRC) const
    {
        return (SONG_CPTRC->ValidPhases() & game::LoopManager::Instance()->GetPhase());
    }


    bool PopupStageMusicSheet::CanPlaySong(const game::song::SongPtrC_t SONG_CPTRC) const
    {
        return (DoesCharacterHaveEnoughManaToPlaySong(SONG_CPTRC) &&
            CanPlaySongInPhase(SONG_CPTRC));
    }


    bool PopupStageMusicSheet::HandleSongPlay()
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

}
