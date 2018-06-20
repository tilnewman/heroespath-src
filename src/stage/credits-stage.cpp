// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// credits-stage.cpp
//
#include "credits-stage.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "misc/real.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace stage
{

    const float CreditsStage::DEFAULT_SCROLL_SPEED_(25.0f);
    const float CreditsStage::SCROLL_SPEED_MULT_(100.0f);
    const float CreditsStage::SCROLL_SPEED_MAX_(400.0f);
    const float CreditsStage::CREDIT_BOX_INNER_PAD_(20);

    CreditsStage::CreditsStage()
        : Stage(
              "Credits",
              {
                  sfml_util::Font::Default,
                  sfml_util::Font::DefaultBoldFlavor,
                  sfml_util::Font::System,
                  sfml_util::Font::SystemCondensed,
                  sfml_util::Font::SignThinTallNarrow,
                  sfml_util::Font::SignBoldShortWide,
                  sfml_util::Font::Handwriting,
                  sfml_util::Font::DialogModern,
                  sfml_util::Font::DialogMedieval,
              },
              true)
        , CREDIT_BOX_WIDTH_(sfml_util::MapByRes(1000.0f, 1500.0f))
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , CREDITBOX_POS_LEFT_(
              (sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (CREDIT_BOX_WIDTH_ * 0.5f))
        , creditBoxPosTop_(0.0f)
        , creditBoxHeight_(0.0f)
        , totalHeight_(0.0f)
        , heightTracker_(0.0f)
        , titleCachedTexture_("media-images-title-blacksymbol")
        , bpTitleSprite_(titleCachedTexture_.Get())
        , box_("Credits")
        , backgroundImage_()
        , creditUVec_()
        , scrollSpeed_(DEFAULT_SCROLL_SPEED_)
        , isKeyHeldDown_(false)
    {
        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::Credits);
    }

    CreditsStage::~CreditsStage()
    {
        sfml_util::SoundManager::Instance()->MusicStop(sfml_util::music::Credits);
        Stage::ClearAllEntities();
    }

    void CreditsStage::Setup()
    {
        // default states are used for all drawing
        sf::RenderStates states;

        // title
        {
            auto const TITLE_WIDTH{ sfml_util::ScreenRatioToPixelsHoriz(0.5f) };
            auto const SCALE{ TITLE_WIDTH / bpTitleSprite_.getLocalBounds().width };
            bpTitleSprite_.setScale(SCALE, SCALE);

            bpTitleSprite_.setPosition(
                (CREDITBOX_POS_LEFT_ + (CREDIT_BOX_WIDTH_ * 0.5f))
                    - (bpTitleSprite_.getGlobalBounds().width * 0.5f),
                50.0f);
        }

        // size of the box in which all the credits scroll
        auto const MARGIN_TOP{ sfml_util::ScreenRatioToPixelsVert(0.022f) };
        auto const MARGIN_BOTTOM{ sfml_util::ScreenRatioToPixelsVert(0.0555f) };

        creditBoxPosTop_
            = bpTitleSprite_.getPosition().y + bpTitleSprite_.getGlobalBounds().height + MARGIN_TOP;

        creditBoxHeight_ = (SCREEN_HEIGHT_ - creditBoxPosTop_) - MARGIN_BOTTOM;

        // the rect that defines the credits box
        const sf::FloatRect CREDITS_BOX_RECT(
            CREDITBOX_POS_LEFT_, creditBoxPosTop_, CREDIT_BOX_WIDTH_, creditBoxHeight_);

        // rune background
        const sfml_util::GradientInfo RUNE_BACKGROUND_GRADIENT_INFO(
            sf::Color(0, 0, 0, 200), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        const sfml_util::gui::BackgroundInfo RUNE_BACKGROUND_INFO(
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-runes"),
            CREDITS_BOX_RECT,
            sf::Color::White,
            RUNE_BACKGROUND_GRADIENT_INFO);

        backgroundImage_.Setup(RUNE_BACKGROUND_INFO, 1.5f, true);

        // box
        {
            const sf::FloatRect BOX_RECT(
                CREDITBOX_POS_LEFT_, creditBoxPosTop_, CREDIT_BOX_WIDTH_, creditBoxHeight_);

            sfml_util::gui::box::Info boxInfo(
                true,
                BOX_RECT,
                sfml_util::gui::ColorSet(
                    sfml_util::FontManager::Color_GrayLight(),
                    sf::Color::Transparent,
                    sfml_util::FontManager::Color_GrayLight(),
                    sf::Color::Transparent));

            box_.SetupBox(boxInfo);
        }

        // establish baseline TextInfo objects
        sfml_util::gui::TextInfo creditTextInfoSmall(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SystemCondensed),
            20,
            sf::Color(255, 255, 255, 200),
            sfml_util::Justified::Center);

        sfml_util::gui::TextInfo creditTextInfoLarge(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SystemCondensed),
            24,
            sf::Color::White,
            sfml_util::Justified::Center);

        // inner/tracking rect
        // keep track of the vertical position to draw via this rect
        sf::FloatRect trackingRect(CREDITS_BOX_RECT);
        trackingRect.left += CREDIT_BOX_INNER_PAD_;
        trackingRect.top += CREDIT_BOX_INNER_PAD_;
        trackingRect.width -= (CREDIT_BOX_INNER_PAD_ * 2.0f);
        trackingRect.height = 0.0f;

        creditUVec_.emplace_back(
            std::make_unique<Credit>(trackingRect, "Game Design", "Ziesche Til Newman"));

        creditUVec_.emplace_back(
            std::make_unique<Credit>(trackingRect, "Programming", "Ziesche Til Newman"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            " ",
            "Special thanks to Laurent Gomila for the superb SFML\n(Simple Fast Multimedia "
            "Library)\nused under the terms and conditions of the zlib/png "
            "license.\nwww.opensource.org/licenses/zlib\n\nThis project came together quickly "
            "thanks to the completeness\n and simplicity of this library.\n\nwww.sfml-dev.org",
            "media-images-logos-sfml",
            sfml_util::MapByRes(0.5f, 1.5f)));

        creditUVec_.emplace_back(
            std::make_unique<Credit>(trackingRect, "Art", "Ziesche Til Newman"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect, "Rune Symbols and Other Designs", "Angela Diegel"));

        creditUVec_.emplace_back(std::make_unique<Credit>(trackingRect, "Art", "Nel Carlson"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Terrain Art",
            "Daniel Cook\nA beautiful (and free!) (for any use) set of tiles.\nwww.lostgarden.com",
            "media-images-logos-terrain",
            sfml_util::MapByRes(0.75f, 2.0f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Map Editing Software",
            "Thorbjorn Lindeijer for Tiled\nAn incredible free mapping utility.\nwww.mapeditor.org",
            "media-images-logos-tiled",
            sfml_util::MapByRes(0.75f, 1.5f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Battle\"\n\"Castlecall\"\n\"Deal with the Devil\"\n\"Menu Loop\"\n\"Mini Epic "
            "Theme\"\n\"Runaway\"\n\"Steeps of Destiny\"\n\"Treasures of Ancient Dungeon\"\n\"We "
            "Are So Close\"",
            "Alexandr Zhelanov\nwww.soundcloud.com/alexandr-zhelanov\n\nUnder the Attribution 3.0 "
            "Unported License\nwwww.creativecommons.org/licenses/by/3.0\nThe original music was "
            "trimmed and normalized.",
            "media-images-logos-sound",
            sfml_util::MapByRes(1.0f, 2.0f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Old Crypt\"\n\"Dramatic Event 2\"\n\"Radakan Menu\"",
            "Janne Hanhisuanto for Radakan\nUnder the Attribution-ShareAlike Unported 3.0 "
            "License\nwww.creativecommons.org/licenses/by-sa/3.0\nThe original music was trimmed "
            "and normalized.",
            "media-images-logos-sound",
            sfml_util::MapByRes(1.0f, 2.0f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Omens\"\n\"The Plot Thickens\"",
            "Eliot Corley from Chaos is Harmony\nUnder the Attribution Unported 3.0 "
            "License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and "
            "normalized.",
            "media-images-logos-sound",
            sfml_util::MapByRes(1.0f, 2.0f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Something is Near\"\n\"Intro of Dragons\"\n\"Small Epic\"\n\"PYC\"",
            "Music by Marcelo "
            "Fernandez\nwww.marcelofernandezmusic.com\nwww.soundcloud.com/"
            "marcelofernandezmusic\nFound at www.opengameart.org/users/marcelofg55\nUnder the "
            "Attribution Unported 4.0 License\nwww.creativecommons.org/licenses/by/4.0\nThe "
            "original music was trimmed and normalized.",
            "media-images-logos-sound",
            sfml_util::MapByRes(1.0f, 2.0f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Neo Euler\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
            "Hermann Zapf\nCopyright (c) 2009, 2010 Khaled Hosny\nkhaledhosny@eglug.org\nUnder the "
            "SIL Open Font "
            "License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Modern Antiqua\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::DefaultBoldFlavor),
            "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Gentium Plus\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            "J.Victor Gaultney\nAnnie Olsen\nIska Routamaa\nBecca "
            "Hirsbrunner\nCopyright (c) SIL International, "
            "2003-2014\nwww.scripts.sil.org/Gentium\nUnder the "
            "SIL Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Goudy Bookletter 1911\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SystemCondensed),
            "by Barry Schwartz\nwww.crudfactory.com\nUnder the public domain (no copyright)"));

        /*
        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Quill Sword\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Number),
            "by Daniel Zadorozny\n2015 Iconian Fonts\nwww.iconian.com\n\"free for all "
            "non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the "
            "author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Queen & Country\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SignBoldShortWide),
            "by Daniel Zadorozny\n2009 Iconian Fonts\nwww.iconian.com\nThis font is e-mailware.  "
            "If you like it,\nplease e-mail the author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Valley Forge\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            "by Daniel Zadorozny\n2008 Iconian Fonts\nwww.iconian.com\n\"free for all "
            "non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the "
            "author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Square Antiqua\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::DialogModern),
            "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Mops Antiqua\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::DialogModern),
            "Created by Uwe Borchert\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));
        */

        totalHeight_ = trackingRect.top;

        // move down to initial positions
        auto const MOVE_AMOUNT{ creditBoxHeight_ + 40.0f };
        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->Move(0.0f, MOVE_AMOUNT);
        }
    }

    void CreditsStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->UpdateTime(ELAPSED_TIME_SECONDS);
        }

        // accellerate/decellerate when pressing/releasing the down arrow key
        if (isKeyHeldDown_)
        {
            scrollSpeed_ += ELAPSED_TIME_SECONDS * SCROLL_SPEED_MULT_;
            if (scrollSpeed_ > SCROLL_SPEED_MAX_)
            {
                scrollSpeed_ = SCROLL_SPEED_MAX_;
            }
        }
        else
        {
            if (scrollSpeed_ > DEFAULT_SCROLL_SPEED_)
            {
                scrollSpeed_ -= ELAPSED_TIME_SECONDS * SCROLL_SPEED_MULT_;
            }
        }

        const float MOVE_AMOUNT(ELAPSED_TIME_SECONDS * -1.0f * scrollSpeed_);
        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->Move(0.0f, MOVE_AMOUNT);
        }

        heightTracker_ += (MOVE_AMOUNT * -1.0f);
        const float END_ROLL_OFFSET(300.0f);
        if (heightTracker_ > (totalHeight_ + creditBoxHeight_ - END_ROLL_OFFSET))
        {
            heightTracker_ = 0.0f;

            const float ADJUSTMENT(totalHeight_ + creditBoxHeight_ - END_ROLL_OFFSET);
            for (auto & creditUPtr : creditUVec_)
            {
                creditUPtr->Move(0.0f, ADJUSTMENT);
            }
        }
    }

    void CreditsStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);

        Stage::Draw(target, STATES);

        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->Draw(target, STATES);
        }

        // draw solid black rectangles above and below the credits box to hide the
        // scrolling credits when outside the box
        sfml_util::DrawRectangle(
            target,
            STATES,
            sf::FloatRect(0.0f, 0.0f, SCREEN_WIDTH_, creditBoxPosTop_ - 5.0f),
            sf::Color::Black,
            1.0f,
            sf::Color::Black);

        sfml_util::DrawRectangle(
            target,
            STATES,
            sf::FloatRect(
                0.0f, creditBoxPosTop_ + creditBoxHeight_ + 5.0f, SCREEN_WIDTH_, SCREEN_HEIGHT_),
            sf::Color::Black,
            1.0f,
            sf::Color::Black);

        target.draw(box_, STATES);
        target.draw(bpTitleSprite_, STATES);
    }

    bool CreditsStage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        if (KE.code == sf::Keyboard::Down)
        {
            isKeyHeldDown_ = true;
        }
        else
        {
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_MainMenu();
        }

        return true;
    }

    bool CreditsStage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        if (KE.code == sf::Keyboard::Down)
        {
            isKeyHeldDown_ = false;
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace stage
} // namespace heroespath
