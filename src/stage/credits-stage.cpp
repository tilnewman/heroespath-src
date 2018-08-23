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
#include "log/log-macros.hpp"
#include "misc/real.hpp"
#include "sfml-util/gui/gui-images.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-primitives.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace stage
{

    const float CreditsStage::DEFAULT_SCROLL_SPEED_(25.0f);
    const float CreditsStage::SCROLL_SPEED_MULT_(100.0f);
    const float CreditsStage::SCROLL_SPEED_MAX_(400.0f);

    CreditsStage::CreditsStage()
        : Stage(
              "Credits",
              {
                  sfml_util::GuiFont::Default,
                  sfml_util::GuiFont::DefaultBoldFlavor,
                  sfml_util::GuiFont::Number,
                  sfml_util::GuiFont::System,
                  sfml_util::GuiFont::SystemCondensed,
                  sfml_util::GuiFont::SignThinTallNarrow,
                  sfml_util::GuiFont::SignBoldShortWide,
                  sfml_util::GuiFont::Handwriting,
                  sfml_util::GuiFont::DialogModern,
                  sfml_util::GuiFont::DialogMedieval,
              },
              true)
        , CREDIT_BOX_WIDTH_(sfml_util::ScreenRatioToPixelsHoriz(0.4f))
        , CREDITBOX_POS_LEFT_((StageRegionWidth() * 0.5f) - (CREDIT_BOX_WIDTH_ * 0.5f))
        , CREDIT_BOX_INNER_PAD_(sfml_util::ScreenRatioToPixelsHoriz(0.0333f))
        , creditBoxPosTop_(0.0f)
        , creditBoxHeight_(0.0f)
        , totalHeight_(0.0f)
        , heightTracker_(0.0f)
        , titleCachedTexture_("media-images-title-blacksymbol")
        , bpTitleSprite_(titleCachedTexture_.Get())
        , boxUPtr_()
        , boxBorderUPtr_()
        , creditUVec_()
        , scrollSpeed_(DEFAULT_SCROLL_SPEED_)
        , isKeyHeldDown_(false)
        , blackRectUpper_()
        , blackRectLower_()
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
        auto const VERT_SPACER { sfml_util::ScreenRatioToPixelsVert(0.022f) };

        sfml_util::Fit(bpTitleSprite_, sfml_util::ScreenRatiosToPixels(0.5f, 0.0f));

        bpTitleSprite_.setPosition(
            sfml_util::DisplayCenterHoriz(bpTitleSprite_.getGlobalBounds().width), VERT_SPACER);

        creditBoxPosTop_ = sfml_util::Bottom(bpTitleSprite_) + VERT_SPACER;
        creditBoxHeight_ = (StageRegionHeight() - creditBoxPosTop_) - (VERT_SPACER * 2.0f);

        const sf::FloatRect CREDITS_BOX_RECT(
            CREDITBOX_POS_LEFT_, creditBoxPosTop_, CREDIT_BOX_WIDTH_, creditBoxHeight_);

        // rune background
        sfml_util::gui::BoxEntityInfo boxInfo;

        boxInfo.SetupImage(
            sfml_util::CachedTexture(
                "media-images-backgrounds-tile-runes",
                sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated),
            sfml_util::ScreenRatioToPixelsHoriz(0.15f));

        boxInfo.SetupColor(
            sf::Color::Transparent,
            sf::Color(0, 0, 0, 200),
            sfml_util::Side::None,
            sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        boxInfo.focus_colors = sfml_util::gui::FocusColors(
            sfml_util::defaults::GrayLight,
            sf::Color::Transparent,
            sfml_util::defaults::GrayLight,
            sf::Color::Transparent);

        boxUPtr_
            = std::make_unique<sfml_util::gui::BoxEntity>("Credits", CREDITS_BOX_RECT, boxInfo);

        sfml_util::gui::BoxEntityInfo boxBorderInfo;
        boxBorderInfo.SetupBorder(true);

        boxBorderUPtr_ = std::make_unique<sfml_util::gui::BoxEntity>(
            "CreditsBorder", CREDITS_BOX_RECT, boxBorderInfo);

        // draw solid black rectangles above and below the credits box to hide the
        // scrolling credits when outside the box
        blackRectUpper_.Setup(
            sf::FloatRect(0.0f, 0.0f, StageRegionWidth(), creditBoxPosTop_ - 5.0f),
            sf::Color::Black);

        blackRectLower_.Setup(
            sf::FloatRect(
                0.0f,
                creditBoxPosTop_ + creditBoxHeight_ + 5.0f,
                StageRegionWidth(),
                StageRegionHeight()),
            sf::Color::Black);

        // establish baseline TextInfo objects
        sfml_util::gui::TextInfo creditTextInfoSmall(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::SystemCondensed),
            20,
            sf::Color(255, 255, 255, 200),
            sfml_util::Justified::Center);

        sfml_util::gui::TextInfo creditTextInfoLarge(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::SystemCondensed),
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
            "",
            "Special thanks to Laurent Gomila for the Simple Fast Multimedia "
            "Library used under the terms and conditions of the zlib/png "
            "license.\nwww.opensource.org/licenses/zlib\n\nThis project came together quickly "
            "thanks to the power and simplicity of this library.\n\nwww.sfml-dev.org",
            "media-images-logos-sfml",
            sfml_util::ScreenRatioToPixelsHoriz(0.146f)));

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
            sfml_util::ScreenRatioToPixelsHoriz(0.117f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Map Editing Software",
            "Thorbjorn Lindeijer for Tiled\nAn incredible free mapping utility.\nwww.mapeditor.org",
            "media-images-logos-tiled",
            sfml_util::ScreenRatioToPixelsHoriz(0.15f)));

        const auto SOUND_IMAGE_WIDTH { sfml_util::ScreenRatioToPixelsHoriz(0.05f) };

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Battle\"\n\"Castlecall\"\n\"Deal with the Devil\"\n\"Menu Loop\"\n\"Mini Epic "
            "Theme\"\n\"Runaway\"\n\"Steeps of Destiny\"\n\"Treasures of Ancient Dungeon\"\n\"We "
            "Are So Close\"",
            "Alexandr Zhelanov\nwww.soundcloud.com/alexandr-zhelanov\n\nUnder the Attribution 3.0 "
            "Unported License\nwwww.creativecommons.org/licenses/by/3.0\nThe original music was "
            "trimmed and normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Old Crypt\"\n\"Dramatic Event 2\"\n\"Radakan Menu\"",
            "Janne Hanhisuanto for Radakan\nUnder the Attribution-ShareAlike Unported 3.0 "
            "License\nwww.creativecommons.org/licenses/by-sa/3.0\nThe original music was trimmed "
            "and normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Omens\"\n\"The Plot Thickens\"",
            "Eliot Corley from Chaos is Harmony\nUnder the Attribution Unported 3.0 "
            "License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and "
            "normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "\"Something is Near\"\n\"Intro of Dragons\"\n\"Small Epic\"\n\"PYC\"",
            "Music by Marcelo "
            "Fernandez\nwww.marcelofernandezmusic.com\nwww.soundcloud.com/"
            "marcelofernandezmusic\nFound at www.opengameart.org/users/marcelofg55\nUnder the "
            "Attribution Unported 4.0 License\nwww.creativecommons.org/licenses/by/4.0\nThe "
            "original music was trimmed and normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Neo Euler\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::Default),
            "Hermann Zapf\nCopyright (c) 2009, 2010 Khaled Hosny\nkhaledhosny@eglug.org\nUnder the "
            "SIL Open Font "
            "License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Modern Antiqua\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::DefaultBoldFlavor),
            "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Gentium Plus\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            "J.Victor Gaultney\nAnnie Olsen\nIska Routamaa\nBecca "
            "Hirsbrunner\nCopyright (c) SIL International, "
            "2003-2014\nwww.scripts.sil.org/Gentium\nUnder the "
            "SIL Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Goudy Bookletter 1911\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::SystemCondensed),
            "by Barry Schwartz\nwww.crudfactory.com\nUnder the public domain (no copyright)"));

        /*
        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Quill Sword\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::Number),
            "by Daniel Zadorozny\n2015 Iconian Fonts\nwww.iconian.com\n\"free for all "
            "non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the "
            "author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Queen & Country\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::SignBoldShortWide),
            "by Daniel Zadorozny\n2009 Iconian Fonts\nwww.iconian.com\nThis font is e-mailware.  "
            "If you like it,\nplease e-mail the author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Valley Forge\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            "by Daniel Zadorozny\n2008 Iconian Fonts\nwww.iconian.com\n\"free for all "
            "non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the "
            "author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Square Antiqua\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::DialogModern),
            "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            trackingRect,
            "Font \"Mops Antiqua\"",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::DialogModern),
            "Created by Uwe Borchert\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));
        */

        totalHeight_ = trackingRect.top;

        // move down to initial positions
        auto const MOVE_AMOUNT { creditBoxHeight_ + 40.0f };
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
        target.draw(*boxUPtr_, STATES);

        Stage::Draw(target, STATES);

        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->Draw(target, STATES);
        }

        // draw solid black rectangles above and below the credits box to hide the
        // scrolling credits when outside the box
        target.draw(blackRectUpper_, STATES);
        target.draw(blackRectLower_, STATES);

        target.draw(bpTitleSprite_, STATES);

        target.draw(*boxBorderUPtr_, STATES);
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
