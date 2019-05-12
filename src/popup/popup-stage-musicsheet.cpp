// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-musicsheet.cpp
//
#include "popup-stage-musicsheet.hpp"

#include "creature/creature.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/sound-manager.hpp"
#include "misc/random.hpp"
#include "popup/popup-manager.hpp"
#include "song/song.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <memory>
#include <sstream>

namespace heroespath
{
namespace popup
{

    const float PopupStageMusicSheet::BORDER_SCREEN_RATIO_ { 0.15f };
    const float PopupStageMusicSheet::COLOR_FADE_SPEED_ { 6.0f };
    const sf::Color PopupStageMusicSheet::UNABLE_TEXT_COLOR_ { sf::Color(127, 32, 32) };
    const sf::Uint8 PopupStageMusicSheet::IMAGE_ALPHA_ { 192 };
    const float PopupStageMusicSheet::WARNING_DURATION_SEC_ { 2.0f };

    PopupStageMusicSheet::PopupStageMusicSheet(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , PLAYER_IMAGE_HEIGHT_(sfutil::ScreenRatioToPixelsVert(0.1465f))
        , charDetailsTextRegionUPtr_()
        , listBoxLabelTextRegionUPtr_()
        , listBoxUPtr_()
        , playerCachedTexture_(gui::LoadAndCacheImage(
              popupInfo_.CreaturePtrOpt().value(), gui::ImageOptions::InvertedCharacterOptions()))
        , playerSprite_(playerCachedTexture_.Get())
        , pageRectLeft_()
        , pageRectRight_()
        , titleTextRegionUPtr_()
        , detailsTextUPtr_()
        , unableTextUPtr_()
        , descTextUPtr_()
        , songCachedTextureOpt_()
        , songSprite_()
        , LISTBOX_IMAGE_COLOR_(sf::Color(255, 255, 255, 190))
        , LISTBOX_LINE_COLOR_(sfutil::color::GrayDark)
        , LISTBOX_COLOR_FG_(LISTBOX_LINE_COLOR_)
        , LISTBOX_COLOR_BG_(sfutil::color::Orange - sf::Color(100, 100, 100, 220))
        , LISTBOX_COLORSET_(LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_)
        , listBoxBackgroundInfo_()
        , listElementTextInfo_(
              " ",
              gui::GuiFont::System,
              gui::FontManager::Instance()->Size_Smallish(),
              sfutil::color::GrayDarker,
              gui::Justified::Left)
        , currentSongIndex_(0)
        , imageColorSlider_(
              sf::Color::Transparent,
              sf::Color::White,
              COLOR_FADE_SPEED_,
              gui::WillOscillate::No,
              gui::WillAutoStart::No)
        , textColorSlider_(
              sf::Color::Transparent,
              sf::Color::Black,
              COLOR_FADE_SPEED_,
              gui::WillOscillate::No,
              gui::WillAutoStart::No)
        , warnTextColorSlider_(
              UNABLE_TEXT_COLOR_,
              sf::Color::Transparent,
              20.0f,
              gui::WillOscillate::Yes,
              gui::WillAutoStart::Yes)
    {}

    PopupStageMusicSheet::~PopupStageMusicSheet() = default;

    const std::string PopupStageMusicSheet::HandleCallback(
        const SongListBox_t::Callback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        if ((PACKET.gui_event == gui::GuiEvent::Click)
            || (PACKET.gui_event == gui::GuiEvent::SelectionChange)
            || (PACKET.keypress_event.code == sf::Keyboard::Up)
            || (PACKET.keypress_event.code == sf::Keyboard::Down))
        {
            if (currentSongIndex_ == PACKET.listbox_ptr->SelectionIndex())
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "song list keypress or click but the selection did not change so ignored");
            }
            else
            {
                currentSongIndex_ = PACKET.listbox_ptr->SelectionIndex();

                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::Magic1);

                imageColorSlider_.SetDirectionAway();
                imageColorSlider_.Start();
                textColorSlider_.SetDirectionAway();
                textColorSlider_.Start();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION, "song list keypress or click and the selection changed");
            }
        }
        else if (
            (PACKET.gui_event == gui::GuiEvent::DoubleClick)
            || (PACKET.keypress_event.code == sf::Keyboard::Return))
        {
            currentSongIndex_ = PACKET.listbox_ptr->SelectionIndex();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "song list double-click or keypress-enter and the selection was set...maybe not "
                "changed though...");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "song list event NOT HANDLED");
    }

    void PopupStageMusicSheet::Setup()
    {
        PopupStageBase::Setup();

        SetupRegionsExtraWork();
        SetupLeftAccentImage();
        SetupRightAccentImage();
        SetupPlayerImage();
        SetupPlayerDetailsText();
        SetupListboxLabelText();
        SetupListbox();

        // setup initial values for songbook page right text and colors
        currentSongIndex_ = listBoxUPtr_->SelectionIndex();
        SetupPageRightText(CurrentSelectedSong());

        SetFocus(listBoxUPtr_);
    }

    void PopupStageMusicSheet::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);

        target.draw(accentSprite1_, states);
        target.draw(accentSprite2_, states);
        target.draw(playerSprite_, states);
        target.draw(songSprite_, states);
        charDetailsTextRegionUPtr_->draw(target, states);
        listBoxLabelTextRegionUPtr_->draw(target, states);
        titleTextRegionUPtr_->draw(target, states);
        detailsTextUPtr_->draw(target, states);

        if (willShowXImage_)
        {
            unableTextUPtr_->draw(target, states);
        }

        descTextUPtr_->draw(target, states);

        StageBase::draw(target, states);
    }

    void PopupStageMusicSheet::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        const auto HAS_TEXT_COLOR_SLIDER_STOPPED { textColorSlider_.UpdateAndReturnIsStopped(
            ELAPSED_TIME_SECONDS) };

        const auto HAS_IMAGE_COLOR_SLIDER_STOPPED { imageColorSlider_.UpdateAndReturnIsStopped(
            ELAPSED_TIME_SECONDS) };

        SetPageRightColors(imageColorSlider_.Value(), textColorSlider_.Value());

        if (HAS_TEXT_COLOR_SLIDER_STOPPED || HAS_IMAGE_COLOR_SLIDER_STOPPED)
        {
            SetupPageRightText(CurrentSelectedSong());
            imageColorSlider_.Reset();
            textColorSlider_.Reset();
        }

        if (willShowXImage_)
        {
            unableTextUPtr_->SetEntityColorFgBoth(
                warnTextColorSlider_.UpdateAndReturnValue(ELAPSED_TIME_SECONDS));
        }
    }

    bool PopupStageMusicSheet::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (listBoxUPtr_)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape) || (KEY_EVENT.code == sf::Keyboard::Space))
            {
                PlayValidKeypressSoundEffect();
                RemovePopup(PopupButtons::Cancel, 0);
                return true;
            }
            else if (
                (KEY_EVENT.code == sf::Keyboard::Return) || (KEY_EVENT.code == sf::Keyboard::C))
            {
                return HandleSongPlay();
            }
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }

    void PopupStageMusicSheet::SetupRegions()
    {
        PopupStageBase::SetupRegionsForFullScreen(BORDER_SCREEN_RATIO_);
    }

    void PopupStageMusicSheet::SetupRegionsExtraWork()
    {
        pageRectLeft_ = MakeBackgroundImageContentRegion(popupInfo_.Image(), false, StageRegion());
        pageRectRight_ = MakeBackgroundImageContentRegion(popupInfo_.Image(), true, StageRegion());
    }

    void PopupStageMusicSheet::SetupLeftAccentImage()
    {
        const auto ACCENT_IMAGE_PATH { PopupManager::Instance()->RandomAccentImagePath() };

        if (ACCENT_IMAGE_PATH.empty() == false)
        {
            accent1CachedTextureOpt_
                = gui::CachedTexture(PathWrapper(ACCENT_IMAGE_PATH), AccentImageOptions());

            accentSprite1_.setTexture(accent1CachedTextureOpt_->Get(), true);

            sfutil::FitAndCenterTo(accentSprite1_, pageRectLeft_);

            accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }

    void PopupStageMusicSheet::SetupRightAccentImage()
    {
        const auto ACCENT_IMAGE_PATH { PopupManager::Instance()->RandomAccentImagePath() };

        if (ACCENT_IMAGE_PATH.empty() == false)
        {
            accent2CachedTextureOpt_
                = gui::CachedTexture(PathWrapper(ACCENT_IMAGE_PATH), AccentImageOptions());

            accentSprite2_.setTexture(accent2CachedTextureOpt_->Get(), true);

            sfutil::FitAndCenterTo(accentSprite2_, pageRectRight_);

            accentSprite2_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
        }
    }

    void PopupStageMusicSheet::SetupPlayerImage()
    {
        const auto SCALE { PLAYER_IMAGE_HEIGHT_ / playerSprite_.getLocalBounds().height };
        playerSprite_.setScale(SCALE, SCALE);
        playerSprite_.setColor(sf::Color(255, 255, 255, 192));
        playerSprite_.setPosition(pageRectLeft_.left, pageRectLeft_.top);
    }

    void PopupStageMusicSheet::SetupPlayerDetailsText()
    {
        const auto CREATURE_PTR { popupInfo_.CreaturePtrOpt().value() };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name() << "\n";

        if (CREATURE_PTR->IsBeast())
        {
            ss << CREATURE_PTR->RaceName();

            if (CREATURE_PTR->Race() != creature::race::Wolfen)
            {
                ss << ", " << CREATURE_PTR->RoleName();
            }

            ss << " " << CREATURE_PTR->RankClassName() << "\n";
        }
        else
        {
            ss << CREATURE_PTR->RankClassName() << " " << CREATURE_PTR->RoleName() << "\n"
               << CREATURE_PTR->RaceName() << "\n";
        }

        ss << "Rank:  " << CREATURE_PTR->Rank() << "\n"
           << "Health:  " << CREATURE_PTR->HealthCurrent() << "/" << CREATURE_PTR->HealthNormal()
           << " " << CREATURE_PTR->HealthPercentStr() << "\n"
           << "Mana:  " << CREATURE_PTR->Mana() << "/" << CREATURE_PTR->ManaNormal() << "\n"
           << "\n";

        const gui::TextInfo DETAILS_TEXTINFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            sfutil::color::GrayDarker,
            gui::Justified::Left);

        const sf::FloatRect DETAILS_TEXT_RECT { pageRectLeft_.left
                                                    + playerSprite_.getGlobalBounds().width
                                                    + sfutil::MapByRes(10.0f, 40.0f),
                                                pageRectLeft_.top + sfutil::MapByRes(20.0f, 80.0f),
                                                0.0f,
                                                0.0f };

        charDetailsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "MusicSheetPopupWindowDetails", DETAILS_TEXTINFO, DETAILS_TEXT_RECT);
    }

    void PopupStageMusicSheet::SetupListboxLabelText()
    {
        const gui::TextInfo LISTBOX_LABEL_TEXTINFO(
            "Songs",
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Largeish(),
            sfutil::color::GrayDarker,
            gui::Justified::Left);

        const sf::FloatRect LISTBOX_LABEL_TEXTRECT {
            pageRectLeft_.left + sfutil::MapByRes(10.0f, 40.0f),
            playerSprite_.getGlobalBounds().top + playerSprite_.getGlobalBounds().height
                + sfutil::MapByRes(20.0f, 80.0f),
            0.0f,
            0.0f
        };

        listBoxLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "MusicSheetPopupWindowSongListLabel", LISTBOX_LABEL_TEXTINFO, LISTBOX_LABEL_TEXTRECT);
    }

    void PopupStageMusicSheet::SetupListbox()
    {
        const auto LISTBOX_MARGIN { sfutil::MapByRes(15.0f, 45.0f) };
        const auto LISTBOX_RECT_LEFT { pageRectLeft_.left + LISTBOX_MARGIN };

        const auto LISTBOX_RECT_TOP { listBoxLabelTextRegionUPtr_->GetEntityRegion().top
                                      + listBoxLabelTextRegionUPtr_->GetEntityRegion().height
                                      + LISTBOX_MARGIN };

        const auto LISTBOX_RECT_WIDTH { pageRectLeft_.width - (LISTBOX_MARGIN * 2.0f) };

        const auto LISTBOX_RECT_HEIGHT { ((pageRectLeft_.top + pageRectLeft_.height)
                                          - LISTBOX_RECT_TOP)
                                         - (LISTBOX_MARGIN * 2.0f) };

        const sf::FloatRect LISTBOX_RECT {
            LISTBOX_RECT_LEFT, LISTBOX_RECT_TOP, LISTBOX_RECT_WIDTH, LISTBOX_RECT_HEIGHT
        };

        listBoxBackgroundInfo_.SetupColor(LISTBOX_COLOR_BG_);
        listBoxBackgroundInfo_.SetupBorder(true, 1.0f);
        listBoxBackgroundInfo_.focus_colors = LISTBOX_COLORSET_;

        listBoxUPtr_ = std::make_unique<gui::ListBox<PopupStageMusicSheet, song::SongPtr_t>>(
            GetStageName() + "'sMusicListBox",
            misc::MakeNotNull(this),
            misc::MakeNotNull(this),
            gui::ListBoxPacket(
                LISTBOX_RECT, listBoxBackgroundInfo_, LISTBOX_LINE_COLOR_, LISTBOX_IMAGE_COLOR_));

        for (const auto & SONG_PTR : popupInfo_.CreaturePtrOpt().value()->SongsPVec())
        {
            listBoxUPtr_->Append(std::make_unique<gui::ListElement<song::SongPtr_t>>(
                SONG_PTR,
                gui::TextInfo(listElementTextInfo_, SONG_PTR->Name()),
                boost::none,
                CanPlaySong(SONG_PTR)));
        }

        EntityAdd(listBoxUPtr_);
    }

    void PopupStageMusicSheet::SetupPageRightText(const song::SongPtr_t SONG_PTR)
    {
        if (!titleTextRegionUPtr_)
        {
            const sf::FloatRect SONG_TITLE_TEXTRECT {
                pageRectRight_.left, pageRectRight_.top, pageRectRight_.width, 0.0f
            };

            const gui::TextInfo SONG_TITLE_TEXTINFO(
                SONG_PTR->Name(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Large(),
                sfutil::color::GrayDarker,
                gui::Justified::Center);

            titleTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "MusicSheetPopupWindowSongTitle", SONG_TITLE_TEXTINFO, SONG_TITLE_TEXTRECT);
        }
        else
        {
            titleTextRegionUPtr_->SetText(SONG_PTR->Name());
        }

        // setup song image
        songCachedTextureOpt_ = gui::LoadAndCacheImage(SONG_PTR->Which());
        songSprite_.setTexture(songCachedTextureOpt_->Get(), true);

        const auto SONG_IMAGE_SCALE { sfutil::MapByRes(0.75f, 4.0f) };
        songSprite_.setScale(SONG_IMAGE_SCALE, SONG_IMAGE_SCALE);
        songSprite_.setColor(imageColorSlider_.Value());

        songSprite_.setPosition(
            (pageRectRight_.left + (pageRectRight_.width * 0.5f))
                - (songSprite_.getGlobalBounds().width * 0.5f),
            titleTextRegionUPtr_->GetEntityRegion().top
                + titleTextRegionUPtr_->GetEntityRegion().height + sfutil::MapByRes(5.0f, 60.0f));

        const auto SYM_SCALE { sfutil::MapByRes(0.75f, 3.75f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        const auto X_SYM_POS_LEFT { (songSprite_.getGlobalBounds().left
                                     + (songSprite_.getGlobalBounds().width * 0.5f))
                                    - (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        const auto X_SYM_POS_TOP { (songSprite_.getGlobalBounds().top
                                    + (songSprite_.getGlobalBounds().height * 0.5f))
                                   - (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        // setup song details text
        std::ostringstream ss;
        ss << "Mana Cost: " << SONG_PTR->ManaCost() << "\n"
           << "Rank: " << SONG_PTR->Rank() << "\n"
           << "Targets " << combat::TargetType::Name(SONG_PTR->Target()) << "\n"
           << "Play during " << EnumUtil<game::Phase>::ToString(SONG_PTR->ValidPhases()) << "\n";

        const auto SONGDETAILS_TEXTRECT_LEFT { pageRectRight_.left };

        const auto SONGDETAILS_TEXTRECT_TOP { songSprite_.getGlobalBounds().top
                                              + songSprite_.getGlobalBounds().height
                                              + sfutil::MapByRes(10.0f, 90.0f) };

        const auto SONGDETAILS_TEXTRECT_WIDTH { pageRectRight_.width };
        const auto SONGDETAILS_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SONG_DETAILS_TEXTRECT { SONGDETAILS_TEXTRECT_LEFT,
                                                    SONGDETAILS_TEXTRECT_TOP,
                                                    SONGDETAILS_TEXTRECT_WIDTH,
                                                    SONGDETAILS_TEXTRECT_HEIGHT };

        if (!detailsTextUPtr_)
        {
            const gui::TextInfo SONG_DETAILS_TEXTINFO(
                ss.str(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::GrayDarker,
                gui::Justified::Center);

            detailsTextUPtr_ = std::make_unique<gui::TextRegion>(
                "MusicsheetPopupWindowSongDetails", SONG_DETAILS_TEXTINFO, SONG_DETAILS_TEXTRECT);
        }
        else
        {
            detailsTextUPtr_->SetText(ss.str());
        }

        // setup song 'unable to cast' text
        willShowXImage_ = false;
        ss.str(" ");
        if (DoesCharacterHaveEnoughManaToPlaySong(SONG_PTR) == false)
        {
            willShowXImage_ = true;
            ss << "Insufficient Mana";
        }
        else if (CanPlaySongInPhase(SONG_PTR) == false)
        {
            willShowXImage_ = true;

            const auto CURRENT_PHASE { GetPhase() };

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
                ss << "Only during " << EnumUtil<game::Phase>::ToString(SONG_PTR->ValidPhases())
                   << ".";
            }
        }

        const gui::TextInfo SONG_UNABLE_TEXTINFO(
            ss.str(),
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Normal(),
            UNABLE_TEXT_COLOR_,
            gui::Justified::Center,
            sf::Text::Bold);

        const auto VERT_SPACER { sfutil::MapByRes(15.0f, 60.0f) };

        const auto SONG_UNABLE_TEXTRECT_LEFT { pageRectRight_.left };

        const auto SONG_UNABLE_TEXTRECT_TOP { sfutil::Bottom(detailsTextUPtr_->GetEntityRegion())
                                              + VERT_SPACER };

        const auto SONG_UNABLE_TEXTRECT_WIDTH { pageRectRight_.width };
        const auto SONG_UNABLE_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SONG_UNABLE_TEXTRECT { SONG_UNABLE_TEXTRECT_LEFT,
                                                   SONG_UNABLE_TEXTRECT_TOP,
                                                   SONG_UNABLE_TEXTRECT_WIDTH,
                                                   SONG_UNABLE_TEXTRECT_HEIGHT };

        unableTextUPtr_ = std::make_unique<gui::TextRegion>(
            "MusicsheetPopupWindowSongUnableToCast", SONG_UNABLE_TEXTINFO, SONG_UNABLE_TEXTRECT);

        // setup song description text
        ss.str("");
        ss << SONG_PTR->Desc() << "  " << SONG_PTR->DescExtra();

        const gui::TextInfo SONG_DESC_TEXTINFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            sfutil::color::GrayDarker,
            gui::Justified::Center);

        const auto SONG_DESC_HORIZ_MARGIN { sfutil::MapByRes(15.0f, 30.0f) };
        const auto SONG_DESC_TEXTRECT_LEFT { pageRectRight_.left + SONG_DESC_HORIZ_MARGIN };
        auto songDescTextRectTop { 0.0f };
        if (willShowXImage_)
        {
            songDescTextRectTop = unableTextUPtr_->GetEntityRegion().top
                + unableTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        else
        {
            songDescTextRectTop = detailsTextUPtr_->GetEntityRegion().top
                + detailsTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        const auto SONG_DESC_TEXTRECT_WIDTH { pageRectRight_.width
                                              - (SONG_DESC_HORIZ_MARGIN * 2.0f) };

        const auto SONG_DESC_TEXTRECT_HEIGHT {
            ((pageRectRight_.top + pageRectRight_.height) - songDescTextRectTop) - VERT_SPACER
        };

        const sf::FloatRect SONG_DESC_TEXTRECT { SONG_DESC_TEXTRECT_LEFT,
                                                 songDescTextRectTop,
                                                 SONG_DESC_TEXTRECT_WIDTH,
                                                 SONG_DESC_TEXTRECT_HEIGHT };

        if (!descTextUPtr_)
        {
            descTextUPtr_ = std::make_unique<gui::TextRegion>(
                "SongnbookPopupWindowSongDescription", SONG_DESC_TEXTINFO, SONG_DESC_TEXTRECT);
        }
        else
        {
            descTextUPtr_->Setup(
                SONG_DESC_TEXTINFO,
                SONG_DESC_TEXTRECT,
                gui::BoxEntityInfo(),
                stage::IStagePtr_t(this));
        }
    }

    void PopupStageMusicSheet::SetPageRightColors(
        const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR)
    {
        songSprite_.setColor(IMAGE_COLOR);

        const gui::FocusColors TEXT_COLOR_SET(TEXT_COLOR, TEXT_COLOR, TEXT_COLOR, TEXT_COLOR);

        titleTextRegionUPtr_->SetEntityColors(TEXT_COLOR_SET);
        detailsTextUPtr_->SetEntityColors(TEXT_COLOR_SET);
        descTextUPtr_->SetEntityColors(TEXT_COLOR_SET);

        auto unableTextColor { UNABLE_TEXT_COLOR_ };
        unableTextColor.a = TEXT_COLOR.a;

        const gui::FocusColors UNABLE_EXT_COLOR_SET(
            unableTextColor, unableTextColor, unableTextColor, unableTextColor);

        unableTextUPtr_->SetEntityColors(UNABLE_EXT_COLOR_SET);

        auto redXColor { sf::Color::Red };
        redXColor.a = IMAGE_COLOR.a;
        xSymbolSprite_.setColor(redXColor);
    }

    bool PopupStageMusicSheet::DoesCharacterHaveEnoughManaToPlaySong(
        const song::SongPtr_t SONG_PTR) const
    {
        return (popupInfo_.CreaturePtrOpt().value()->Mana() >= SONG_PTR->ManaCost());
    }

    bool PopupStageMusicSheet::CanPlaySongInPhase(const song::SongPtr_t SONG_PTR) const
    {
        return (SONG_PTR->ValidPhases() & GetPhase());
    }

    bool PopupStageMusicSheet::CanPlaySong(const song::SongPtr_t SONG_PTR) const
    {
        return (DoesCharacterHaveEnoughManaToPlaySong(SONG_PTR) && CanPlaySongInPhase(SONG_PTR));
    }

    bool PopupStageMusicSheet::HandleSongPlay()
    {
        const auto SONG_PTR { CurrentSelectedSong() };

        if (CanPlaySong(SONG_PTR))
        {
            if (SONG_PTR->Type() == song::SongType::Drum)
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::DrumBlip)
                    .PlayRandom();
            }
            else if (SONG_PTR->Type() == song::SongType::Guitar)
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::GuitarStrum)
                    .PlayRandom();
            }

            RemovePopup(PopupButtons::Select, listBoxUPtr_->SelectionIndex());
            return true;
        }
        else
        {
            gui::SoundManager::Instance()->PlaySfx_Reject();
            willShowXImage_ = true;
            return false;
        }
    }

    const song::SongPtr_t PopupStageMusicSheet::CurrentSelectedSong() const
    {
        return listBoxUPtr_->Selection()->Element();
    }

} // namespace popup
} // namespace heroespath
