//
// credits-stage.cpp
//
#include "credits-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "utilz/real.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/static-sounds.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/gui-elements.hpp"

#include "heroespath/game-data-file.hpp"
#include "heroespath/loop-manager.hpp"


namespace heroespath
{
namespace stage
{

    const float CreditsStage::DEFAULT_SCROLL_SPEED_(25.0f);
    const float CreditsStage::SCROLL_SPEED_MULT_(100.0f);
    const float CreditsStage::SCROLL_SPEED_MAX_(400.0f);
    const float CreditsStage::CREDIT_BOX_INNER_PAD_(20);

    CreditsStage::CreditsStage()
    :
        Stage                 ("Credits"),
        CREDIT_BOX_WIDTH_     (sfml_util::MapByRes(1000.0f, 1500.0f)),
        SCREEN_WIDTH_         (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_        (sfml_util::Display::Instance()->GetWinHeight()),
        CREDITBOX_POS_LEFT_   ((sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (CREDIT_BOX_WIDTH_ * 0.5f)),
        BG_MUSIC_SET_SPTR_    (sfml_util::SoundManager::Instance()->MusicStart({ sfml_util::music::Credits1, sfml_util::music::Credits2 })),
        creditBoxPosTop_      (0.0f),
        creditBoxHeight_      (0.0f),
        totalHeight_          (0.0f),
        heightTracker_        (0.0f),
        hpTitleTexture_       (),
        bpTitleSprite_        (),
        box_                  ("Credits"),
        backgroundImage_      (),
        creditSVec_           (),
        scrollSpeed_          (DEFAULT_SCROLL_SPEED_),
        isKeyHeldDown_        (false)
    {}


    CreditsStage::~CreditsStage()
    {
        sfml_util::SoundManager::Instance()->MusicStop(BG_MUSIC_SET_SPTR_);
    }


    void CreditsStage::Setup()
    {
        //default states are used for all drawing
        sf::RenderStates states;

        //title
        {
            sfml_util::LoadImageOrTexture<sf::Texture>(hpTitleTexture_, GameDataFile::Instance()->GetMediaPath("media-images-title-blacksymbol"));
            hpTitleTexture_.setSmooth(true);
            bpTitleSprite_.setTexture(hpTitleTexture_);
            const float SCALE(sfml_util::MapByRes(0.6f, 1.5f));
            bpTitleSprite_.setScale(SCALE, SCALE);
            bpTitleSprite_.setPosition((CREDITBOX_POS_LEFT_ + (CREDIT_BOX_WIDTH_ * 0.5f)) - (bpTitleSprite_.getGlobalBounds().width * 0.5f), 50.0f);
        }

        //size of the box in which all the credits scroll
        const float MARGIN_TOP(sfml_util::MapByRes(20.0f, 100.0f));
        const float MARGIN_BOTTOM(sfml_util::MapByRes(50.0f, 200.0f));
        creditBoxPosTop_ = bpTitleSprite_.getPosition().y + bpTitleSprite_.getGlobalBounds().height + MARGIN_TOP;
        creditBoxHeight_ = (SCREEN_HEIGHT_ - creditBoxPosTop_) - MARGIN_BOTTOM;

        //the rect that defines the credits box
        const sf::FloatRect CREDITS_BOX_RECT(CREDITBOX_POS_LEFT_, creditBoxPosTop_, CREDIT_BOX_WIDTH_, creditBoxHeight_);

        //rune background
        const sfml_util::GradientInfo RUNE_BACKGROUND_GRADIENT_INFO(sf::Color(0, 0, 0, 200), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);
        const sfml_util::gui::BackgroundInfo RUNE_BACKGROUND_INFO(GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-runes"), CREDITS_BOX_RECT, sf::Color::White, RUNE_BACKGROUND_GRADIENT_INFO);
        backgroundImage_.Setup(RUNE_BACKGROUND_INFO, sfml_util::MapByRes(1.0f, 3.0f), true);

        //box
        {
            const sf::FloatRect BOX_RECT(CREDITBOX_POS_LEFT_, creditBoxPosTop_, CREDIT_BOX_WIDTH_, creditBoxHeight_);
            sfml_util::gui::box::Info boxInfo(true, BOX_RECT, sfml_util::gui::ColorSet(sfml_util::FontManager::Instance()->Color_GrayLight(), sf::Color::Transparent, sfml_util::FontManager::Instance()->Color_GrayLight(), sf::Color::Transparent));
            box_.SetupBox(boxInfo);
        }

        //establish baseline TextInfo objects
        sfml_util::gui::TextInfo creditTextInfoSmall("", sfml_util::FontManager::Instance()->Font_Typical(), 20, sf::Color(255,255,255,200), sfml_util::Justified::Center);
        sfml_util::gui::TextInfo creditTextInfoLarge("", sfml_util::FontManager::Instance()->Font_Typical(), 24, sf::Color::White, sfml_util::Justified::Center);

        //inner/tracking rect
        //keep track of the vertical position to draw via this rect
        sf::FloatRect trackingRect(CREDITS_BOX_RECT);
        trackingRect.left   += CREDIT_BOX_INNER_PAD_;
        trackingRect.top    += CREDIT_BOX_INNER_PAD_;
        trackingRect.width  -= (CREDIT_BOX_INNER_PAD_ * 2.0f);
        trackingRect.height = 0.0f;

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect, "Game Design", "Ziesche Til Newman")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect, "Programming", "Ziesche Til Newman")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      " ",
                                                      "Special thanks to Laurent Gomila for the superb SFML\n(Simple Fast Multimedia Library)\nused under the terms and conditions of the zlib/png license.\nwww.opensource.org/licenses/zlib\n\nThis project came together quickly thanks to the completeness\n and simplicity of this library.\n\nwww.sfml-dev.org",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sfml",
                                                      sfml_util::MapByRes(0.5f, 1.5f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect, "Art", "Ziesche Til Newman")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect, "Rune Symbols and Other Designs", "Angela Diegel")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "User Interface Art",
                                                       "RPG GUI Construction Kit v1.0\nPosted on OpenGameArt.org by Lamoot\nwww.opengameart.org/users/lamoot\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nTouch-up work was done to the images provided.\nThis excellent artwork is the foundation\nof this game's GUI look and feel.")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Smoke Animation",
                                                      "Posted on OpenGameArt.org by MrBeast\nwww.opengameart.org/users/mrbeast\nUnder the Public Domain Dedication CC0 1.0 Universal License\nwww.creativecommons.org/publicdomain/zero/1.0",
                                                      credit_media_type::AnimMultiTexture,
                                                      "media-anim-images-dir-smoke",
                                                      sfml_util::MapByRes(1.0f, 3.0f),
                                                      sf::Vector2f(0.0f, 0.0f),
                                                      0.05f)));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Inferno Animation",
                                                      "Posted on OpenGameArt.org by Retimer\nWrath Games Studio\nwww.wrathgames.com/blog\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0",
                                                      credit_media_type::AnimMultiTexture,
                                                      "media-anim-images-dir-inferno",
                                                      sfml_util::MapByRes(0.7f, 2.0f),
                                                      sf::Vector2f(0.0f, 0.0f),
                                                      0.05f)));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Candle Flame Animation",
                                                      "Posted on OpenGameArt.org by Para\nwww.opengameart.org/users/para\nUnder the Public Domain Dedication CC0 1.0 Universal License\nwww.creativecommons.org/publicdomain/zero/1.0",
                                                      credit_media_type::AnimSingleTexture,
                                                      "media-anim-images-candleflame",
                                                      sfml_util::MapByRes(1.0f, 3.0f),
                                                      sf::Vector2f(0.0f, 0.0f),
                                                      0.05f,
                                                      64,
                                                      128,
                                                      128)));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Terrain Art",
                                                      "Daniel Cook\nA beautiful (and free!) (for any use) set of tiles.\nwww.lostgarden.com",
                                                      credit_media_type::Image,
                                                      "media-images-logos-terrain",
                                                      sfml_util::MapByRes(0.75f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Whispers of Avalon Grassland Tileset",
                                                      "Posted on OpenGameArt.org by Leonard Pabin\nwww.opengameart.org/users/leonard-pabin\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nTouch-ups and shading was performed on the original artwork.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-avalontrees",
                                                      sfml_util::MapByRes(0.75f, 1.75f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Manaworld Trees Tileset",
                                                      "Posted on OpenGameArt.org by Leonard Pabin\nwww.opengameart.org/users/leonard-pabin\nUnder the GPL 2.0 License\nwww.gnu.org/licenses/old-licenses/gpl-2.0.html\nPixel touch-up work was done to the original artwork.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-manaworldtrees",
                                                      sfml_util::MapByRes(0.5f, 1.5f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Bleed's Rendered Trees",
                                                      "Posted on OpenGameArt.org by Bleed as Tree Collection v2.6\nwww.opengameart.org/users/bleed\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nShadows in the original artwork were changed.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-renderedtrees",
                                                      sfml_util::MapByRes(0.5f, 1.5f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Hi-Res Paper Textures",
                                                      "Posted on OpenGameArt.org by qubodup, Created by darkwood\nwwww.opengameart.org/users/qubodup\nUnder the Attribution Unported License\nwww.creativecommons.org/licenses/by/3.0",
                                                      credit_media_type::Image,
                                                      "media-images-logos-darkwoodpaper",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Map Editing Software",
                                                      "Thorbjorn Lindeijer for Tiled\nAn incredible free mapping utility.\nwww.mapeditor.org",
                                                      credit_media_type::Image,
                                                      "media-images-logos-tiled",
                                                      sfml_util::MapByRes(0.75f, 1.5f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Inventory Sound Effects",
                                                      "Posted on OpenGameArt.org by Hansjorg Malthaner\nUnder the Attribution-ShareAlike Unported 3.0 License\nwww.creativecommons.org/licenses/by-sa/3.0",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Inventory Sound Effects",
                                                      "Posted on OpenGameArt.org as 'Little Robot Sound Factory'\nwww.littlerobotsoundfactory.com\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nwww.creativecommons.org/licenses/by/3.0",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "User Interface Sound Effects",
                                                      "Posted on OpenGameArt.org as 'UI SFX Set' by Kenney Vleugels\nwww.kenney.nl\nUnder the Public Domain Dedication CC0 1.0 Universal License\nwww.creativecommons.org/publicdomain/zero/1.0",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Wind Gust Sound Effects",
                                                      "Posted on OpenGameArt.org by IgnasD\nwww.crystalgames.space\nUnder the Attribution-ShareAlike Unported 3.0 License\nwww.creativecommons.org/licenses/by-sa/3.0",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Storm Winds Sound Effects",
                                                      "Posted on OpenGameArt.org by Luke\nwww.rustltd.com\nUnder the Public Domain Dedication CC0 1.0 Universal License\nwww.creativecommons.org/publicdomain/zero/1.0",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"Battle\"\n\"Castlecall\"\n\"Deal with the Devil\"\n\"Menu Loop\"\n\"Mini Epic Theme\"\n\"Runaway\"\n\"Steeps of Destiny\"\n\"Treasures of Ancient Dungeon\"\n\"We Are So Close\"",
                                                      "Alexandr Zhelanov\nwww.soundcloud.com/alexandr-zhelanov\n\nUnder the Attribution 3.0 Unported License\nwwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"Old Crypt\"\n\"Dramatic Event 2\"\n\"Radakan Menu\"",
                                                      "Janne Hanhisuanto for Radakan\nUnder the Attribution-ShareAlike Unported 3.0 License\nwww.creativecommons.org/licenses/by-sa/3.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"Omens\"\n\"The Plot Thickens\"",
                                                      "Eliot Corley from Chaos is Harmony\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"Almost Out\"\n\"Assassin's Assault\"\n\"Unforgiving Lands\"",
                                                      "Posted on OpenGameArt.org by HorrorPen\nwww.opengameart.org/users/horrorpen\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"DrumLoop 1\"\n\"DrumLoop 1 64BPM\"",
                                                      "Written and produced by Ove Melaa\nOmsofware@hotmail.com\nPosted on OpenGameArt.org by OveMelaa\nas 'Ove's Essential Game Audio Pack Collection'\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"Regular Battle\"",
                                                      "Posted on OpenGameArt.org by Telaron\nwww.opengameart.org/users/telaron\nUnder the Attribution Unported 3.0 License\nwww.creativecommons.org/licenses/by/3.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "\"Something is Near\"\n\"Intro of Dragons\"\n\"Small Epic\"\n\"PYC\"",
                                                      "Music by Marcelo Fernandez\nwww.marcelofernandezmusic.com\nwww.soundcloud.com/marcelofernandezmusic\nFound at www.opengameart.org/users/marcelofg55\nUnder the Attribution Unported 4.0 License\nwww.creativecommons.org/licenses/by/4.0\nThe original music was trimmed and normalized.",
                                                      credit_media_type::Image,
                                                      "media-images-logos-sound",
                                                      sfml_util::MapByRes(1.0f, 2.0f))));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Neo Euler\"",
                                                      sfml_util::FontManager::Instance()->Font_Default1(),
                                                      "Copyright (c) 2009, 2010 Khaled Hosny\nkhaledhosny@eglug.org\nUnder the SIL Open Font License v1.1\nwww.scripts.sil.org/OFL")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Goudy Bookletter 1911 \"",
                                                      sfml_util::FontManager::Instance()->Font_Typical(),
                                                      "by Barry Schwartz\nwww.crudfactory.com\nUnder the public domain (no copyright)",
                                                      credit_media_type::None)));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Gentium Plus\"",
                                                      sfml_util::FontManager::Instance()->Font_Default2(),
                                                      "Copyright (c) SIL International, 2003-2014\nwww.scripts.sil.org/Gentium\nUnder the SIL Open Font License v1.1\nwww.scripts.sil.org/OFL")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Modern Antiqua\"",
                                                      sfml_util::FontManager::Instance()->Font_BigFlavor1(),
                                                      "Copyright (c) 2011, wmk69 (wmk69@o2.pl)\nFrom www.openfontlibrary.org\nUnder the SIL Open Font License v1.1\nwww.scripts.sil.org/OFL")));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Queen & Country\"",
                                                      sfml_util::FontManager::Instance()->Font_BigFlavor3(),
                                                      "by Daniel Zadorozny\n2009 Iconian Fonts\nwww.iconian.com\nThis font is e-mailware.  If you like it,\nplease e-mail the author at iconian@aol.com.",
                                                      credit_media_type::None)));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Valley Forge\"",
                                                      sfml_util::FontManager::Instance()->Font_BigFlavor2(),
                                                      "by Daniel Zadorozny\n2008 Iconian Fonts\nwww.iconian.com\n\"free for all non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the author at iconian@aol.com.",
                                                      credit_media_type::None)));

        creditSVec_.push_back(CreditSPtr_t(new Credit(trackingRect,
                                                      "Font \"Quill Sword\"",
                                                      sfml_util::FontManager::Instance()->Font_NumbersDefault1(),
                                                      "by Daniel Zadorozny\n2015 Iconian Fonts\nwww.iconian.com\n\"free for all non-commercial uses\"\nThis font is e-mailware.  If you like it,\nplease e-mail the author at iconian@aol.com.",
                                                      credit_media_type::None)));

        totalHeight_ = trackingRect.top;

        //move down to initial positions
        const float MOVE_AMOUNT(creditBoxHeight_ + 40);
        for (auto & nextCreditSPtr : creditSVec_)
            nextCreditSPtr->Move(0.0f, MOVE_AMOUNT);
    }


    void CreditsStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        for (auto & nextCreditSPtr : creditSVec_)
            nextCreditSPtr->UpdateTime(ELAPSED_TIME_SECONDS);

        //accellerate/decellerate when pressing/releasing the down arrow key
        if (isKeyHeldDown_)
        {
            scrollSpeed_ += ELAPSED_TIME_SECONDS * SCROLL_SPEED_MULT_;
            if (scrollSpeed_ > SCROLL_SPEED_MAX_)
                scrollSpeed_ = SCROLL_SPEED_MAX_;
        }
        else
        {
            if (scrollSpeed_ > DEFAULT_SCROLL_SPEED_)
                scrollSpeed_ -= ELAPSED_TIME_SECONDS * SCROLL_SPEED_MULT_;
        }

        const float MOVE_AMOUNT(ELAPSED_TIME_SECONDS * -1.0f * scrollSpeed_);
        for (auto & nextCreditSPtr : creditSVec_)
            nextCreditSPtr->Move(0.0f, MOVE_AMOUNT);

        heightTracker_ += (MOVE_AMOUNT * -1.0f);
        const float END_ROLL_OFFSET(300.0f);
        if (heightTracker_ > (totalHeight_ + creditBoxHeight_ - END_ROLL_OFFSET))
        {
            heightTracker_ = 0.0f;

            const float ADJUSTMENT(totalHeight_ + creditBoxHeight_ - END_ROLL_OFFSET);
            for (auto & nextCreditSPtr : creditSVec_)
                nextCreditSPtr->Move(0.0f, ADJUSTMENT);
        }
    }


    void CreditsStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        target.draw(backgroundImage_, states);

        Stage::Draw(target, states);

        for (auto & nextCreditSPtr : creditSVec_)
            nextCreditSPtr->Draw(target, states);

        //draw solid black rectangles above and below the credits box to hide the scrolling credits when outside the box
        sfml_util::DrawRectangle(target, states, sf::FloatRect(0.0f, 0.0f, SCREEN_WIDTH_, creditBoxPosTop_ - 5.0f), sf::Color::Black, 1.0f, sf::Color::Black);
        sfml_util::DrawRectangle(target, states, sf::FloatRect(0.0f, creditBoxPosTop_ + creditBoxHeight_ + 5.0f, SCREEN_WIDTH_, SCREEN_HEIGHT_), sf::Color::Black, 1.0f, sf::Color::Black);

        target.draw(box_, states);

        target.draw(bpTitleSprite_, states);
    }


    bool CreditsStage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        if (KE.code == sf::Keyboard::Down)
            isKeyHeldDown_ = true;
        else
        {
            sfml_util::SoundManager::Instance()->StaticSounds_Switch()->PlayRandom();
            LoopManager::Instance()->Goto_MainMenu();
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
            return false;
    }

}
}
