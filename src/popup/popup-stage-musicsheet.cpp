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

#include "popup/popup-manager.hpp"

#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/song-image-manager.hpp"
#include "sfml-util/sound-manager.hpp"

#include "misc/random.hpp"

#include <sstream>
#include <algorithm>


namespace popup
{

    const float     PopupStageMusicSheet::BACKGROUND_WIDTH_RATIO_           { 0.9f };
    const float     PopupStageMusicSheet::COLOR_FADE_SPEED_                 { 6.0f };
    const sf::Color PopupStageMusicSheet::UNABLE_TEXT_COLOR_{ sf::Color(127, 32, 32) };
    const sf::Uint8 PopupStageMusicSheet::IMAGE_ALPHA_                      { 192 };
    const float     PopupStageMusicSheet::WARNING_DURATION_SEC_             { 2.0f };


    PopupStageMusicSheet::PopupStageMusicSheet(const PopupInfo & POPUP_INFO)
        :
        PopupStageBase(POPUP_INFO),
        currentSongPtr_(nullptr),
        charDetailsTextRegionUPtr_(),
        listBoxLabelTextRegionUPtr_(),
        listBoxUPtr_(),
        playerTexture_(),
        playerSprite_(),
        pageRectLeft_(),
        pageRectRight_(),
        titleTextRegionUPtr_(),
        detailsTextUPtr_(),
        unableTextUPtr_(),
        descTextUPtr_(),
        songTexture_(),
        songSprite_(),
        warnColorShaker_(UNABLE_TEXT_COLOR_, sf::Color::Transparent, 20.0f),
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
            sfml_util::Justified::Left),

        imageColorSlider_(sf::Color::Transparent, sf::Color::White, COLOR_FADE_SPEED_),
        textColorSlider_(sf::Color::Transparent, sf::Color::Black, COLOR_FADE_SPEED_)
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
            if ((PACKAGE.package.PTR_->GetSelected() != nullptr) &&
                (currentSongPtr_ != PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC))
            {
                if (currentSongPtr_ != PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC)
                {
                    currentSongPtr_ = PACKAGE.package.PTR_->GetSelected()->SONG_CPTRC;
                }

                if (imageColorSlider_.Direction() != sfml_util::Moving::Away)
                {
                    imageColorSlider_.ChangeDirection();
                    imageColorSlider_.Start();
                    textColorSlider_.ChangeDirection();
                    textColorSlider_.Start();
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

        SetupRegions();
        SetupLeftAccentImage();
        SetupRightAccentImage();
        SetupPlayerImage();
        SetupPlayerDetailsText();
        SetupListboxLabelText();
        SetupListbox();

        //Force song listbox to take focus so that user up/down
        //keystrokes work without having to click on the listbox.
        SetFocus(listBoxUPtr_.get());

        //Force song listbox selection up and down to force colors to correct.
        listBoxUPtr_->WillPlaySoundEffects(false);
        sf::Event::KeyEvent keyEvent;
        keyEvent.code = sf::Keyboard::Down;
        listBoxUPtr_->KeyRelease(keyEvent);
        keyEvent.code = sf::Keyboard::Up;
        listBoxUPtr_->KeyRelease(keyEvent);
        listBoxUPtr_->WillPlaySoundEffects(true);

        //setup initial values for songbook page right text and colors
        currentSongPtr_ = listBoxUPtr_->At(0)->SONG_CPTRC;
        SetupPageRightText(currentSongPtr_);
    }


    void PopupStageMusicSheet::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        target.draw(accentSprite1_, STATES);
        target.draw(accentSprite2_, STATES);
        target.draw(playerSprite_, STATES);
        target.draw(songSprite_, STATES);
        charDetailsTextRegionUPtr_->draw(target, STATES);
        listBoxLabelTextRegionUPtr_->draw(target, STATES);
        titleTextRegionUPtr_->draw(target, STATES);
        detailsTextUPtr_->draw(target, STATES);

        if (willShowXImage_)
        {
            unableTextUPtr_->draw(target, STATES);
        }

        descTextUPtr_->draw(target, STATES);

        Stage::Draw(target, STATES);

        PopupStageBase::DrawRedX(target, STATES);
    }


    void PopupStageMusicSheet::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        textColorSlider_.UpdateTime(ELAPSED_TIME_SECONDS);
        imageColorSlider_.UpdateTime(ELAPSED_TIME_SECONDS);

        SetPageRightColors(imageColorSlider_.Current(), textColorSlider_.Current());

        if ((imageColorSlider_.IsMoving() == false) &&
            (imageColorSlider_.Direction() == sfml_util::Moving::Away))
        {
            sfml_util::SoundManager::Instance()->SoundEffectPlay(
                sfml_util::sound_effect::Magic1);

            SetupPageRightText(currentSongPtr_);
            imageColorSlider_.ChangeDirection();
            imageColorSlider_.Start();
            textColorSlider_.ChangeDirection();
            textColorSlider_.Start();
        }

        if (willShowXImage_)
        {
            unableTextUPtr_->SetEntityColorFgBoth(
                warnColorShaker_.Update(ELAPSED_TIME_SECONDS));
        }
    }


    bool PopupStageMusicSheet::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::Escape) ||
            (KEY_EVENT.code == sf::Keyboard::Space))
        {
            PlayValidKeypressSoundEffect();
            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Cancel, 0);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Return) ||
                  (KEY_EVENT.code == sf::Keyboard::C))
        {
            return HandleSongPlay();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStageMusicSheet::SetupOuterAndInnerRegion()
    {
        auto const MUSICSHEET_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() *
            BACKGROUND_WIDTH_RATIO_ };

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


    void PopupStageMusicSheet::SetupRegions()
    {
        auto const LEFT_SIDE_RECT_RAW { sfml_util::ConvertRect<int, float>(
            PopupManager::Rect_MusicSheet_LeftSide()) };

        auto const SCALE(innerRegion_.width /
            static_cast<float>(backgroundTexture_.getSize().x));

        pageRectLeft_.left   = innerRegion_.left + (LEFT_SIDE_RECT_RAW.left * SCALE);
        pageRectLeft_.top    = innerRegion_.top + (LEFT_SIDE_RECT_RAW.top * SCALE);
        pageRectLeft_.width  = LEFT_SIDE_RECT_RAW.width * SCALE;
        pageRectLeft_.height = LEFT_SIDE_RECT_RAW.height * SCALE;

        auto const RIGHT_SIDE_RECT_RAW{ sfml_util::ConvertRect<int, float>(
            PopupManager::Rect_MusicSheet_RightSide()) };

        pageRectRight_.left = innerRegion_.left + (RIGHT_SIDE_RECT_RAW.left * SCALE);
        pageRectRight_.top = innerRegion_.top + (RIGHT_SIDE_RECT_RAW.top * SCALE);
        pageRectRight_.width = RIGHT_SIDE_RECT_RAW.width * SCALE;
        pageRectRight_.height = RIGHT_SIDE_RECT_RAW.height * SCALE;
    }


    void PopupStageMusicSheet::SetupLeftAccentImage()
    {
        PopupManager::Instance()->LoadRandomAccentImage(accentTexture1_);
        accentSprite1_.setTexture(accentTexture1_);

        sfml_util::CenterAndScaleSpriteToFit(
            accentSprite1_,
            pageRectLeft_,
            ACCENT_IMAGE_SCALEDOWN_RATIO_);

        accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }


    void PopupStageMusicSheet::SetupRightAccentImage()
    {
        PopupManager::Instance()->LoadRandomAccentImage(accentTexture2_);
        accentSprite2_.setTexture(accentTexture2_);

        sfml_util::CenterAndScaleSpriteToFit(
            accentSprite2_,
            pageRectRight_,
            ACCENT_IMAGE_SCALEDOWN_RATIO_);

        accentSprite2_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }


    void PopupStageMusicSheet::SetupPlayerImage()
    {
        sfml_util::gui::CreatureImageManager::Instance()->GetImage(
            playerTexture_, popupInfo_.CreaturePtr()->ImageFilename(), true);

        sfml_util::Invert(playerTexture_);
        sfml_util::Mask(playerTexture_, sf::Color::White);

        playerSprite_.setTexture(playerTexture_);
        auto const PLAYER_IMAGE_SCALE{ sfml_util::MapByRes(0.55f, 3.5f) };
        playerSprite_.setScale(PLAYER_IMAGE_SCALE, PLAYER_IMAGE_SCALE);
        playerSprite_.setColor(sf::Color(255, 255, 255, 192));
        playerSprite_.setPosition(pageRectLeft_.left, pageRectLeft_.top);
    }


    void PopupStageMusicSheet::SetupPlayerDetailsText()
    {
        auto creaturePtr{ popupInfo_.CreaturePtr() };
        std::ostringstream ss;
        ss << creaturePtr->Name() << "\n";

        if (creaturePtr->IsBeast())
        {
            ss << creaturePtr->RaceName();

            if (creaturePtr->Race() != game::creature::race::Wolfen)
            {
                ss << ", " << creaturePtr->RoleName();
            }

            ss << " " << creaturePtr->RankClassName() << "\n";
        }
        else
        {
            ss << creaturePtr->RankClassName() << " " << creaturePtr->RoleName() << "\n"
                << creaturePtr->RaceName() << "\n";
        }

        ss << "Rank:  " << creaturePtr->Rank() << "\n"
            << "Health:  " << creaturePtr->HealthCurrent() << "/" << creaturePtr->HealthNormal() << " "
            << creaturePtr->HealthPercentStr() << "\n"
            << "Mana:  " << creaturePtr->TraitWorking(game::stats::Traits::Mana) << "/"
            << creaturePtr->TraitNormal(game::stats::Traits::Mana) << "\n"
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

        charDetailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "MusicSheetPopupWindowDetails",
            DETAILS_TEXTINFO,
            DETAILS_TEXT_RECT);
    }


    void PopupStageMusicSheet::SetupListboxLabelText()
    {
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

        listBoxLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "MusicSheetPopupWindowSongListLabel",
            LISTBOX_LABEL_TEXTINFO,
            LISTBOX_LABEL_TEXTRECT);
    }


    void PopupStageMusicSheet::SetupListbox()
    {
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
        auto const SONG_PVEC{ popupInfo_.CreaturePtr()->SongsPVec() };
        for (auto const NEXT_SONG_PTR : SONG_PVEC)
        {
            listBoxItemTextInfo_.text = NEXT_SONG_PTR->Name();

            auto const LISTBOXITEM_SPTR( std::make_shared<sfml_util::gui::ListBoxItem>(
                NEXT_SONG_PTR->Name() + "_MusicListBoxEntry",
                listBoxItemTextInfo_,
                NEXT_SONG_PTR,
                CanPlaySong(NEXT_SONG_PTR)) );

            listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
        }

        listBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
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

        EntityAdd(listBoxUPtr_.get());
        listBoxUPtr_->SetSelectedIndex(0);
        listBoxUPtr_->SetImageColor(LISTBOX_IMAGE_COLOR_);
    }


    void PopupStageMusicSheet::SetupPageRightText(const game::song::SongPtrC_t SONG_CPTRC)
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

        if (titleTextRegionUPtr_.get() == nullptr)
        {
            titleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "MusicSheetPopupWindowSongTitle",
                SONG_TITLE_TEXTINFO,
                SONG_TITLE_TEXTRECT);
        }
        else
        {
            titleTextRegionUPtr_->SetText(SONG_CPTRC->Name());
        }

        //setup song image
        sfml_util::gui::SongImageManager::Instance()->Get(songTexture_,
                                                          SONG_CPTRC->Which());

        songSprite_.setTexture(songTexture_);
        auto const SONG_IMAGE_SCALE{ sfml_util::MapByRes(0.75f, 4.0f) };
        songSprite_.setScale(SONG_IMAGE_SCALE, SONG_IMAGE_SCALE);
        songSprite_.setColor(imageColorSlider_.Current());

        songSprite_.setPosition((pageRectRight_.left + (pageRectRight_.width * 0.5f)) -
            (songSprite_.getGlobalBounds().width * 0.5f),
                titleTextRegionUPtr_->GetEntityRegion().top + titleTextRegionUPtr_->
                    GetEntityRegion().height + sfml_util::MapByRes(5.0f, 60.0f));

        auto const SYM_SCALE{ sfml_util::MapByRes(0.75f, 3.75f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        auto const X_SYM_POS_LEFT{ (songSprite_.getGlobalBounds().left +
            (songSprite_.getGlobalBounds().width * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        auto const X_SYM_POS_TOP{ (songSprite_.getGlobalBounds().top +
            (songSprite_.getGlobalBounds().height * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        //setup song details text
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

        auto const SONGDETAILS_TEXTRECT_TOP    { songSprite_.getGlobalBounds().top +
            songSprite_.getGlobalBounds().height + sfml_util::MapByRes(10.0f, 90.0f) };

        auto const SONGDETAILS_TEXTRECT_WIDTH  { pageRectRight_.width };
        auto const SONGDETAILS_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SONG_DETAILS_TEXTRECT{ SONGDETAILS_TEXTRECT_LEFT,
                                                   SONGDETAILS_TEXTRECT_TOP,
                                                   SONGDETAILS_TEXTRECT_WIDTH,
                                                   SONGDETAILS_TEXTRECT_HEIGHT };

        if (detailsTextUPtr_.get() == nullptr)
        {
            detailsTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "MusicsheetPopupWindowSongDetails",
                SONG_DETAILS_TEXTINFO,
                SONG_DETAILS_TEXTRECT);
        }
        else
        {
            detailsTextUPtr_->SetText(ss.str());
        }

        //setup song 'unable to cast' text
        willShowXImage_ = false;
        ss.str(" ");
        if (DoesCharacterHaveEnoughManaToPlaySong(SONG_CPTRC) == false)
        {
            willShowXImage_ = true;
            ss << "Insufficient Mana";
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
        }

        const sfml_util::gui::TextInfo SONG_UNABLE_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            UNABLE_TEXT_COLOR_,
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        auto const VERT_SPACER{ sfml_util::MapByRes(15.0f, 60.0f) };

        auto const SONG_UNABLE_TEXTRECT_LEFT   { pageRectRight_.left };

        auto const SONG_UNABLE_TEXTRECT_TOP    { detailsTextUPtr_->GetEntityRegion().top +
            detailsTextUPtr_->GetEntityRegion().height + VERT_SPACER };

        auto const SONG_UNABLE_TEXTRECT_WIDTH  { pageRectRight_.width };
        auto const SONG_UNABLE_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SONG_UNABLE_TEXTRECT{
            SONG_UNABLE_TEXTRECT_LEFT,
            SONG_UNABLE_TEXTRECT_TOP,
            SONG_UNABLE_TEXTRECT_WIDTH,
            SONG_UNABLE_TEXTRECT_HEIGHT };

        unableTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "MusicsheetPopupWindowSongUnableToCast",
            SONG_UNABLE_TEXTINFO,
            SONG_UNABLE_TEXTRECT);

        //setup song description text
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
        auto songDescTextRectTop{ 0.0f };
        if (willShowXImage_)
        {
            songDescTextRectTop = unableTextUPtr_->GetEntityRegion().top +
                unableTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        else
        {
            songDescTextRectTop = detailsTextUPtr_->GetEntityRegion().top +
                detailsTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        auto const SONG_DESC_TEXTRECT_WIDTH{ pageRectRight_.width -
            (SONG_DESC_HORIZ_MARGIN * 2.0f) };

        auto const SONG_DESC_TEXTRECT_HEIGHT{ ((pageRectRight_.top + pageRectRight_.height) -
            songDescTextRectTop) - VERT_SPACER };

        const sf::FloatRect SONG_DESC_TEXTRECT{ SONG_DESC_TEXTRECT_LEFT,
                                                songDescTextRectTop,
                                                SONG_DESC_TEXTRECT_WIDTH,
                                                SONG_DESC_TEXTRECT_HEIGHT };

        if (descTextUPtr_.get() == nullptr)
        {
            descTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SongnbookPopupWindowSongDescription",
                SONG_DESC_TEXTINFO,
                SONG_DESC_TEXTRECT);
        }
        else
        {
            descTextUPtr_->Setup(SONG_DESC_TEXTINFO, SONG_DESC_TEXTRECT, this);
        }
    }


    void PopupStageMusicSheet::SetPageRightColors(
        const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR)
    {
        songSprite_.setColor(IMAGE_COLOR);

        const sfml_util::gui::ColorSet TEXT_COLOR_SET(
            TEXT_COLOR,
            TEXT_COLOR,
            TEXT_COLOR,
            TEXT_COLOR);

        titleTextRegionUPtr_->SetEntityColors(TEXT_COLOR_SET);
        detailsTextUPtr_->SetEntityColors(TEXT_COLOR_SET);
        descTextUPtr_->SetEntityColors(TEXT_COLOR_SET);

        auto unableTextColor{ UNABLE_TEXT_COLOR_ };
        unableTextColor.a = TEXT_COLOR.a;

        const sfml_util::gui::ColorSet UNABLE_EXT_COLOR_SET(
            unableTextColor,
            unableTextColor,
            unableTextColor,
            unableTextColor);

        unableTextUPtr_->SetEntityColors(UNABLE_EXT_COLOR_SET);

        auto redXColor{ sf::Color::Red };
        redXColor.a = IMAGE_COLOR.a;
        xSymbolSprite_.setColor(redXColor);
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
        if (CanPlaySong(listBoxUPtr_->GetSelected()->SONG_CPTRC))
        {
            if (currentSongPtr_ != nullptr)
            {
                if (currentSongPtr_->Type() == game::song::SongType::Drum)
                {
                    sfml_util::SoundManager::Instance()->Getsound_effect_set(
                        sfml_util::sound_effect_set::DrumBlip).PlayRandom();
                }
                else if (currentSongPtr_->Type() == game::song::SongType::Guitar)
                {
                    sfml_util::SoundManager::Instance()->Getsound_effect_set(
                        sfml_util::sound_effect_set::GuitarStrum).PlayRandom();
                }
            }

            game::LoopManager::Instance()->PopupWaitEnd(
                ResponseTypes::Select, listBoxUPtr_->GetSelectedIndex());

            return true;
        }
        else
        {
            sfml_util::SoundManager::Instance()->Getsound_effect_set(
                sfml_util::sound_effect_set::Prompt).Play(sfml_util::sound_effect::PromptWarn);

            willShowXImage_ = true;

            return false;
        }
    }

}
