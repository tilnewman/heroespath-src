// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// credit.cpp
//
#include "credit.hpp"

#include "game/game-data-file.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-position.hpp"

namespace heroespath
{
namespace stage
{

    Credit::Credit(
        sf::FloatRect & trackingRect,
        const std::string & TITLE_TEXT,
        const std::string & CONTENT_TEXT)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Text)
        , cachedTextureOpt_()
        , sprite_()
        , animUPtr_()
        , screenSizeVert_(sfml_util::Display::Instance()->GetWinHeight())
    {
        Setup(
            trackingRect,
            TITLE_TEXT,
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_);
    }

    Credit::Credit(
        sf::FloatRect & trackingRect,
        const std::string & TITLE_TEXT,
        const std::string & CONTENT_TEXT,
        const sfml_util::Animations::Enum ANIM_ENUM,
        const float ANIM_SIZE_HORIZ,
        const float ANIM_FRAME_TIME_SEC)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Anim)
        , cachedTextureOpt_()
        , sprite_()
        , animUPtr_()
        , screenSizeVert_(sfml_util::Display::Instance()->GetWinHeight())
    {
        Setup(
            trackingRect,
            TITLE_TEXT,
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_,
            ANIM_SIZE_HORIZ,
            ANIM_ENUM,
            ANIM_FRAME_TIME_SEC);
    }

    Credit::Credit(
        sf::FloatRect & trackingRect,
        const std::string & TITLE_TEXT,
        const std::string & CONTENT_TEXT,
        const std::string & IMAGE_PATH_KEY,
        const float IMAGE_SIZE_HORIZ)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Image)
        , cachedTextureOpt_(IMAGE_PATH_KEY)
        , sprite_(cachedTextureOpt_.value().Get())
        , animUPtr_()
        , screenSizeVert_(sfml_util::Display::Instance()->GetWinHeight())
    {
        Setup(
            trackingRect,
            TITLE_TEXT,
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_,
            IMAGE_SIZE_HORIZ);
    }

    Credit::Credit(
        sf::FloatRect & trackingRect,
        const std::string & TITLE_TEXT,
        const sfml_util::GuiFont::Enum FONT,
        const std::string & CONTENT_TEXT)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Image)
        , cachedTextureOpt_("media-images-logos-openfontlicense")
        , sprite_(cachedTextureOpt_.value().Get())
        , animUPtr_()
        , screenSizeVert_(sfml_util::Display::Instance()->GetWinHeight())
    {
        Setup(
            trackingRect,
            TITLE_TEXT,
            FONT,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_,
            sfml_util::ScreenRatioToPixelsHoriz(0.08f));
    }

    void Credit::Setup(
        sf::FloatRect & trackingRect,
        const std::string & TITLE_TEXT,
        const sfml_util::GuiFont::Enum TITLE_FONT,
        const unsigned int TITLE_FONT_SIZE,
        const std::string & CONTENT_TEXT,
        const MediaType::Enum MEDIA_TYPE,
        const float MEDIA_SIZE_HORIZ,
        const sfml_util::Animations::Enum ANIM_ENUM,
        const float ANIM_FRAME_TIME_SEC)
    {
        const auto BETWEEN_MEDIA_AND_TEXT_VERT_SPACER { sfml_util::ScreenRatioToPixelsVert(0.01f) };

        if (MEDIA_TYPE == MediaType::Image)
        {
            auto const SCALE { MEDIA_SIZE_HORIZ / sprite_.getLocalBounds().width };

            sprite_.setScale(SCALE, SCALE);

            auto const POS_LEFT { (trackingRect.left + (trackingRect.width * 0.5f))
                                  - (sprite_.getGlobalBounds().width * 0.5f) };

            auto const TOP_PAD { sfml_util::ScreenRatioToPixelsVert(0.0333f) };

            auto const POS_TOP { (trackingRect.top + (trackingRect.height * 0.5f))
                                 - (sprite_.getGlobalBounds().height * 0.5f) + TOP_PAD };

            sprite_.setPosition(POS_LEFT, POS_TOP);

            trackingRect.top = sfml_util::Bottom(sprite_) + BETWEEN_MEDIA_AND_TEXT_VERT_SPACER;
        }
        else if (MEDIA_TYPE == MediaType::Anim)
        {
            // inital size and pos are default (zeros)
            animUPtr_ = sfml_util::AnimationFactory::Make(
                ANIM_ENUM, sf::FloatRect(), ANIM_FRAME_TIME_SEC);

            auto const SCALE { MEDIA_SIZE_HORIZ / animUPtr_->OrigSize().x };

            // correct size and position
            auto const WIDTH { animUPtr_->OrigSize().x * SCALE };
            auto const HEIGHT { animUPtr_->OrigSize().y * SCALE };

            auto const POS_LEFT { (trackingRect.left + (trackingRect.width * 0.5f))
                                  - (WIDTH * 0.5f) };

            auto const TOP_PAD { sfml_util::ScreenRatioToPixelsVert(0.0888f) };

            auto const POS_TOP { (trackingRect.top + (trackingRect.height * 0.5f)) - (HEIGHT * 0.5f)
                                 + TOP_PAD };

            animUPtr_->SetEntityRegion(sf::FloatRect(POS_LEFT, POS_TOP, WIDTH, HEIGHT));

            trackingRect.top = sfml_util::Bottom(animUPtr_->GetEntityRegion())
                + BETWEEN_MEDIA_AND_TEXT_VERT_SPACER;
        }

        const auto TEXT_COLOR_BRIGHT { sf::Color::White };
        const auto TEXT_COLOR_NOTASBRIGHT { sf::Color(255, 255, 255, 220) };

        const auto WILL_TITLE_USE_BRIGHT_TEXT_COLOR { !(MEDIA_TYPE == MediaType::Text)
                                                      && !TITLE_TEXT.empty() };

        const auto TITLE_COLOR { (
            (WILL_TITLE_USE_BRIGHT_TEXT_COLOR) ? TEXT_COLOR_BRIGHT : TEXT_COLOR_NOTASBRIGHT) };

        const auto CONTENT_COLOR { (
            (WILL_TITLE_USE_BRIGHT_TEXT_COLOR) ? TEXT_COLOR_NOTASBRIGHT : TEXT_COLOR_BRIGHT) };

        const sfml_util::gui::TextInfo TEXT_INFO_TITLE(
            TITLE_TEXT, TITLE_FONT, TITLE_FONT_SIZE, TITLE_COLOR, sfml_util::Justified::Center);

        titleTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CreditTitle_" + TITLE_TEXT, TEXT_INFO_TITLE, trackingRect);

        if (boost::algorithm::trim_copy(TITLE_TEXT).empty() == false)
        {
            trackingRect.top += titleTextUPtr_->GetEntityRegion().height;
        }

        const auto BETWEEN_TEXT_VERTICAL_SPACER { sfml_util::ScreenRatioToPixelsVert(0.0065f) };
        trackingRect.top += BETWEEN_TEXT_VERTICAL_SPACER;

        sfml_util::gui::TextInfo textInfoContent(
            CONTENT_TEXT,
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Normal(),
            CONTENT_COLOR,
            sfml_util::Justified::Center);

        // if there is a lot of text (multi-lined), reduce the size to look better
        // auto const NUM_NEWLINES { static_cast<int>(
        //    std::count(CONTENT_TEXT.begin(), CONTENT_TEXT.end(), '\n')) };
        //
        // if (NUM_NEWLINES > 1)
        //{
        //    auto const FONT_SIZE_REDUCTION { static_cast<unsigned int>(2 * NUM_NEWLINES) };
        //
        //    if (textInfoContent.size > FONT_SIZE_REDUCTION)
        //    {
        //        textInfoContent.size -= FONT_SIZE_REDUCTION;
        //    }
        //    else
        //    {
        //        textInfoContent.size = 1;
        //    }
        //}

        contentTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CreditContent", textInfoContent, trackingRect);

        if (boost::algorithm::trim_copy(CONTENT_TEXT).empty() == false)
        {
            trackingRect.top += contentTextUPtr_->GetEntityRegion().height;
        }

        // add space between credits
        auto const VERT_SPACER { sfml_util::ScreenRatioToPixelsVert(0.111f) };
        trackingRect.top += VERT_SPACER;
    }

    void Credit::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (IsVisible() == false)
        {
            return;
        }

        if (titleTextUPtr_)
        {
            target.draw(*titleTextUPtr_, states);
        }

        if (contentTextUPtr_)
        {
            target.draw(*contentTextUPtr_, states);
        }

        if (animUPtr_)
        {
            animUPtr_->draw(target, states);
        }

        if (MediaType::Image == mediaType_)
        {
            target.draw(sprite_, states);
        }
    }

    void Credit::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (IsVisible() == false)
        {
            return;
        }

        if (animUPtr_)
        {
            animUPtr_->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }

    void Credit::Move(const float ADJ_HORIZ, const float ADJ_VERT)
    {
        if (titleTextUPtr_)
        {
            titleTextUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }

        if (contentTextUPtr_)
        {
            contentTextUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }

        sprite_.move(ADJ_HORIZ, ADJ_VERT);

        if (animUPtr_)
        {
            animUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }
    }

    bool Credit::IsVisible() const
    {
        std::vector<sf::FloatRect> rects;

        if (titleTextUPtr_)
        {
            rects.emplace_back(titleTextUPtr_->GetEntityRegion());
        }

        if (contentTextUPtr_)
        {
            rects.emplace_back(contentTextUPtr_->GetEntityRegion());
        }

        if (animUPtr_)
        {
            rects.emplace_back(animUPtr_->GetEntityRegion());
        }

        rects.emplace_back(sprite_.getGlobalBounds());

        for (auto const & RECT : rects)
        {
            auto const TOP_LIMIT { -1.0f };
            auto const BOTTOM_LIMIT { screenSizeVert_ + 1.0f };

            auto const TOP { RECT.top };
            auto const BOTTOM { RECT.top + RECT.height };

            if (((TOP > TOP_LIMIT) && (TOP < BOTTOM_LIMIT))
                || ((BOTTOM > TOP_LIMIT) && (BOTTOM < BOTTOM_LIMIT)))
            {
                return true;
            }
        }

        return false;
    }

} // namespace stage
} // namespace heroespath
