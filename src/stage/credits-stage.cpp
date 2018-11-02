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

#include "gui/gui-images.hpp"
#include "gui/music-enum.hpp"
#include "gui/sound-manager.hpp"
#include "gui/text-info.hpp"
#include "gui/text-region.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/real.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/position.hpp"
#include "sfutil/primitives.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    const float CreditsStage::DEFAULT_SCROLL_SPEED_(45.0f);
    const float CreditsStage::SCROLL_SPEED_MULT_(100.0f);
    const float CreditsStage::SCROLL_SPEED_MAX_(600.0f);

    CreditsStage::CreditsStage()
        : StageBase(
            "Credits",
            {
                gui::GuiFont::Default,
                gui::GuiFont::DefaultBoldFlavor,
                gui::GuiFont::Number,
                gui::GuiFont::System,
                gui::GuiFont::SystemCondensed,
                gui::GuiFont::SignThinTallNarrow,
                gui::GuiFont::SignBoldShortWide,
                gui::GuiFont::Handwriting,
                gui::GuiFont::DialogModern,
                gui::GuiFont::DialogMedieval,
            },
            true)
        , titleCachedTexture_("media-images-title-blacksymbol")
        , bpTitleSprite_(titleCachedTexture_.Get())
        , boxUPtr_()
        , boxBorderUPtr_()
        , creditUVec_()
        , scrollSpeed_(DEFAULT_SCROLL_SPEED_)
        , isKeyHeldArrowUp_(false)
        , isKeyHeldArrowDown_(false)
        , keyPressClock_()
        , blackRectUpper_()
        , blackRectLower_()
    {
        gui::SoundManager::Instance()->MusicStart(gui::music::Credits);
    }

    CreditsStage::~CreditsStage()
    {
        gui::SoundManager::Instance()->MusicStop(gui::music::Credits);
        StageBase::ClearAllEntities();
    }

    void CreditsStage::Setup()
    {
        const auto TITLE_VERT_PAD { sfutil::ScreenRatioToPixelsVert(0.022f) };

        sfutil::Fit(bpTitleSprite_, sfutil::ScreenRatiosToPixels(0.5f, 0.0f));

        bpTitleSprite_.setPosition(
            sfutil::DisplayCenterHoriz(bpTitleSprite_.getGlobalBounds().width), TITLE_VERT_PAD);

        const auto CREDITS_BOX_REGION = [&]() {
            sf::FloatRect rect;
            rect.width = sfutil::ScreenRatioToPixelsHoriz(0.4f);
            rect.left = (StageRegion().width * 0.5f) - (rect.width * 0.5f);
            rect.top = sfutil::Bottom(bpTitleSprite_) + TITLE_VERT_PAD;
            rect.height = (StageRegion().height - rect.top) - (TITLE_VERT_PAD * 2.0f);
            return rect;
        }();

        // rune background
        gui::BoxEntityInfo boxInfo;

        boxInfo.SetupImage(
            gui::CachedTexture(
                "media-images-backgrounds-tile-runes",
                gui::ImageOpt::Default | gui::ImageOpt::Repeated),
            sfutil::ScreenRatioToPixelsHoriz(0.15f));

        boxInfo.SetupColor(
            sf::Color::Transparent,
            sf::Color(0, 0, 0, 200),
            gui::Side::None,
            gui::Corner::TopLeft | gui::Corner::BottomRight);

        boxInfo.focus_colors = gui::FocusColors(
            sfutil::color::GrayLight,
            sf::Color::Transparent,
            sfutil::color::GrayLight,
            sf::Color::Transparent);

        boxUPtr_ = std::make_unique<gui::BoxEntity>("Credits", CREDITS_BOX_REGION, boxInfo);

        gui::BoxEntityInfo boxBorderInfo;
        boxBorderInfo.SetupBorder(true);

        boxBorderUPtr_
            = std::make_unique<gui::BoxEntity>("CreditsBorder", CREDITS_BOX_REGION, boxBorderInfo);

        // draw solid black rectangles above and below the credits box to hide the
        // scrolling credits when they move outside the box
        blackRectUpper_.Setup(
            sf::FloatRect(0.0f, 0.0f, StageRegion().width, boxUPtr_->OuterRegion().top),
            sf::Color::Black);

        blackRectLower_.Setup(
            sf::FloatRect(
                0.0f,
                sfutil::Bottom(boxUPtr_->OuterRegion()),
                StageRegion().width,
                StageRegion().height),
            sf::Color::Black);

        const auto CREDIT_BOX_INNER_PAD { sfutil::ScreenRatioToPixelsHoriz(0.0333f) };
        const auto CREDIT_MAX_WIDTH { CREDITS_BOX_REGION.width - (CREDIT_BOX_INNER_PAD * 2.0f) };

        creditUVec_.emplace_back(
            std::make_unique<Credit>(CREDIT_MAX_WIDTH, "Game Design", "Ziesche Til Newman"));

        creditUVec_.emplace_back(
            std::make_unique<Credit>(CREDIT_MAX_WIDTH, "Programming", "Ziesche Til Newman"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "",
            "Special thanks to Laurent Gomila for the Simple Fast Multimedia Library.  This "
            "project came together quickly thanks to the power and simplicity of this library."
            "\n\nwww.sfml-dev.org\nwww.opensource.org/licenses/zlib\n\n",
            "media-images-logos-sfml",
            sfutil::ScreenRatioToPixelsHoriz(0.146f)));

        creditUVec_.emplace_back(
            std::make_unique<Credit>(CREDIT_MAX_WIDTH, "Art", "Ziesche Til Newman"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH, "Rune Symbols and Other Designs", "Angela Diegel"));

        creditUVec_.emplace_back(std::make_unique<Credit>(CREDIT_MAX_WIDTH, "Art", "Nel Carlson"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Terrain Art",
            "Daniel Cook\nA beautiful (and free!) (for any use) set of "
            "tiles.\nwww.lostgarden.com",
            "media-images-logos-terrain",
            sfutil::ScreenRatioToPixelsHoriz(0.117f)));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Map Editing Software",
            "Thorbjorn Lindeijer for Tiled\nAn incredible free mapping "
            "utility.\nwww.mapeditor.org",
            "media-images-logos-tiled",
            sfutil::ScreenRatioToPixelsHoriz(0.15f)));

        const auto SOUND_IMAGE_WIDTH { sfutil::ScreenRatioToPixelsHoriz(0.05f) };

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "\"Battle\"\n\"Castlecall\"\n\"Deal with the Devil\"\n\"Menu Loop\"\n\"Mini Epic "
            "Theme\"\n\"Runaway\"\n\"Steeps of Destiny\"\n\"Treasures of Ancient "
            "Dungeon\"\n\"We "
            "Are So Close\"",
            "Alexandr Zhelanov\nwww.soundcloud.com/alexandr-zhelanov\n\nUnder the Attribution "
            "3.0 "
            "Unported License\nwwww.creativecommons.org/licenses/by/3.0\nThe original music "
            "was "
            "trimmed and normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "\"Old Crypt\"\n\"Dramatic Event 2\"\n\"Radakan Menu\"",
            "Janne Hanhisuanto for Radakan\nUnder the Attribution-ShareAlike Unported 3.0 "
            "License\nwww.creativecommons.org/licenses/by-sa/3.0\nThe original music was "
            "trimmed "
            "and normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "\"Omens\"\n\"The Plot Thickens\"",
            "Eliot Corley from Chaos is Harmony\nUnder the Attribution Unported 3.0 "
            "License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed "
            "and "
            "normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "\"Something is Near\"\n\"Intro of Dragons\"\n\"Small Epic\"\n\"PYC\"",
            "Music by Marcelo "
            "Fernandez\nwww.marcelofernandezmusic.com\nwww.soundcloud.com/"
            "marcelofernandezmusic\nFound at www.opengameart.org/users/marcelofg55\nUnder the "
            "Attribution Unported 4.0 License\nwww.creativecommons.org/licenses/by/4.0\nThe "
            "original music was trimmed and normalized.",
            "media-images-logos-sound",
            SOUND_IMAGE_WIDTH));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Neo Euler\"",
            gui::GuiFont::Default,
            "Hermann Zapf\nCopyright (c) 2009, 2010 Khaled Hosny\nkhaledhosny@eglug.org\nUnder "
            "the "
            "SIL Open Font "
            "License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Modern Antiqua\"",
            gui::GuiFont::DefaultBoldFlavor,
            "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the "
            "SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Gentium Plus\"",
            gui::GuiFont::System,
            "J.Victor Gaultney\nAnnie Olsen\nIska Routamaa\nBecca "
            "Hirsbrunner\nCopyright (c) SIL International, "
            "2003-2014\nwww.scripts.sil.org/Gentium\nUnder the "
            "SIL Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at "
            "www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Goudy Bookletter 1911\"",
            gui::GuiFont::SystemCondensed,
            "by Barry Schwartz\nwww.crudfactory.com\nUnder the public domain (no copyright)"));

        /*
        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Quill Sword\"",
            gui::GuiFont::Number,
            "by Daniel Zadorozny\n2015 Iconian Fonts\nwww.iconian.com\n\"free for all "
            "non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the
        " "author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Queen & Country\"",
            gui::GuiFont::SignBoldShortWide,
            "by Daniel Zadorozny\n2009 Iconian Fonts\nwww.iconian.com\nThis font is e-mailware.
        " "If you like it,\nplease e-mail the author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Valley Forge\"",
            gui::GuiFont::System,
            "by Daniel Zadorozny\n2008 Iconian Fonts\nwww.iconian.com\n\"free for all "
            "non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the
        " "author at iconian@aol.com."));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Square Antiqua\"",
            gui::GuiFont::DialogModern,
            "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the
        SIL " "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));

        creditUVec_.emplace_back(std::make_unique<Credit>(
            CREDIT_MAX_WIDTH,
            "Font \"Mops Antiqua\"",
            gui::GuiFont::DialogModern,
            "Created by Uwe Borchert\nUnder the SIL "
            "Open Font License v1.1\nwww.scripts.sil.org/OFL\nFound at www.fontlibrary.org"));
        */

        MoveCreditsToStartPos();
    }

    void CreditsStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);
        UpdateCreditAnimations(ELAPSED_TIME_SECONDS);
        UpdateCreditPositions(ELAPSED_TIME_SECONDS);
        UpdateScrollSpeed(ELAPSED_TIME_SECONDS);
    }

    void CreditsStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(*boxUPtr_, states);

        StageBase::draw(target, states);

        for (const auto & CREDIT_UPTR : creditUVec_)
        {
            if (IsCreditVisible(*CREDIT_UPTR))
            {
                target.draw(*CREDIT_UPTR, states);
            }
        }

        // draw solid black rectangles above and below the credits box to hide the
        // scrolling credits when outside the box
        target.draw(blackRectUpper_, states);
        target.draw(blackRectLower_, states);
        target.draw(bpTitleSprite_, states);
        target.draw(*boxBorderUPtr_, states);
    }

    bool CreditsStage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        keyPressClock_.restart();

        if (KE.code == sf::Keyboard::Up)
        {
            isKeyHeldArrowUp_ = true;
        }
        else if (KE.code == sf::Keyboard::Down)
        {
            isKeyHeldArrowDown_ = true;
        }
        else
        {
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Menu);
        }

        return true;
    }

    bool CreditsStage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        keyPressClock_.restart();

        if (KE.code == sf::Keyboard::Up)
        {
            isKeyHeldArrowUp_ = false;
            return true;
        }
        else if (KE.code == sf::Keyboard::Down)
        {
            isKeyHeldArrowDown_ = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    void CreditsStage::UpdateMouseDown(const sf::Vector2f &) { TransitionTo(stage::Stage::Menu); }

    void CreditsStage::UpdateScrollSpeed(const float ELAPSED_TIME_SECONDS)
    {
        const auto SPEED_ADJ { (
            (ELAPSED_TIME_SECONDS * SCROLL_SPEED_MULT_)
            + std::pow((1.0f + keyPressClock_.getElapsedTime().asSeconds()), 2.35f)) };

        // accellerate/decellerate when pressing/releasing the down arrow key
        if ((isKeyHeldArrowUp_ == false) && (isKeyHeldArrowDown_ == false)
            && (std::abs(scrollSpeed_ - DEFAULT_SCROLL_SPEED_) > 5.0f))
        {
            if (scrollSpeed_ < DEFAULT_SCROLL_SPEED_)
            {
                scrollSpeed_ += SPEED_ADJ;
            }
            else
            {
                scrollSpeed_ -= SPEED_ADJ;
            }
        }
        else if (isKeyHeldArrowUp_)
        {
            scrollSpeed_ += SPEED_ADJ;
        }
        else if (isKeyHeldArrowDown_)
        {
            scrollSpeed_ -= SPEED_ADJ;
        }

        if (scrollSpeed_ > SCROLL_SPEED_MAX_)
        {
            scrollSpeed_ = SCROLL_SPEED_MAX_;
        }
        else if (scrollSpeed_ < -SCROLL_SPEED_MAX_)
        {
            scrollSpeed_ = -SCROLL_SPEED_MAX_;
        }
    }

    void CreditsStage::UpdateCreditAnimations(const float ELAPSED_TIME_SECONDS)
    {
        for (auto & creditUPtr : creditUVec_)
        {
            if (IsCreditVisible(*creditUPtr))
            {
                creditUPtr->UpdateTime(ELAPSED_TIME_SECONDS);
            }
        }
    }

    void CreditsStage::UpdateCreditPositions(const float ELAPSED_TIME_SECONDS)
    {
        const float MOVE_AMOUNT(ELAPSED_TIME_SECONDS * scrollSpeed_);

        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->Move(0.0f, -MOVE_AMOUNT);
        }

        const auto TOP_OF_FIRST_CREDIT { creditUVec_.front()->Region().top };

        if (TOP_OF_FIRST_CREDIT > sfutil::DisplaySize().y)
        {
            MoveCreditsToReverseStartPos();
        }
        else
        {
            const auto BOTTOM_OF_LAST_CREDIT { sfutil::Bottom(creditUVec_.back()->Region()) };

            if (BOTTOM_OF_LAST_CREDIT < 0.0f)
            {
                MoveCreditsToStartPos();
            }
        }
    }

    bool CreditsStage::IsCreditVisible(const Credit & CREDIT) const
    {
        return CREDIT.Region().intersects(boxUPtr_->OuterRegion());
    }

    void CreditsStage::MoveCreditsToStartPos()
    {
        auto iter { std::begin(creditUVec_) };

        const auto FIRST_CREDIT_START_POS_TOP { (
            sfutil::Bottom(boxUPtr_->OuterRegion()) + sfutil::ScreenRatioToPixelsVert(0.022f)) };

        (*iter)->SetVerticalPosition(FIRST_CREDIT_START_POS_TOP);

        const auto BETWEEN_CREDIT_SPACER { sfutil::ScreenRatioToPixelsVert(0.08f) };

        while (iter != std::end(creditUVec_))
        {
            const auto BOTTOM_OF_PREV_CREDIT { sfutil::Bottom((*iter)->Region()) };
            ++iter;

            if (iter == std::end(creditUVec_))
            {
                break;
            }
            else
            {
                (*iter)->SetVerticalPosition(BOTTOM_OF_PREV_CREDIT + BETWEEN_CREDIT_SPACER);
            }
        }
    }

    void CreditsStage::MoveCreditsToReverseStartPos()
    {
        const auto VERT_MOVE_OF_LAST_CREDIT_TO_SET_REVERSE_START { (
            0.0f - sfutil::Bottom(creditUVec_.back()->Region())) };

        for (auto & creditUPtr : creditUVec_)
        {
            creditUPtr->Move(0.0f, VERT_MOVE_OF_LAST_CREDIT_TO_SET_REVERSE_START);
        }
    }

} // namespace stage
} // namespace heroespath
