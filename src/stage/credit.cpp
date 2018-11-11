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

#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/text-region.hpp"
#include "misc/config-file.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    Credit::Credit(
        const float MAX_WIDTH, const std::string & TITLE_TEXT, const std::string & CONTENT_TEXT)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Text)
        , cachedTextureOpt_()
        , sprite_()
        , animUPtr_()
        , screenSizeVert_(gui::Display::Instance()->GetWinHeight())
        , region_()
    {
        Setup(
            MAX_WIDTH,
            TITLE_TEXT,
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_);
    }

    Credit::Credit(
        const float MAX_WIDTH,
        const std::string & TITLE_TEXT,
        const std::string & CONTENT_TEXT,
        const gui::Animations::Enum ANIM_ENUM,
        const float ANIM_SIZE_HORIZ,
        const float ANIM_FRAME_TIME_SEC)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Anim)
        , cachedTextureOpt_()
        , sprite_()
        , animUPtr_()
        , screenSizeVert_(gui::Display::Instance()->GetWinHeight())
        , region_()
    {
        Setup(
            MAX_WIDTH,
            TITLE_TEXT,
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_,
            ANIM_SIZE_HORIZ,
            ANIM_ENUM,
            ANIM_FRAME_TIME_SEC);
    }

    Credit::Credit(
        const float MAX_WIDTH,
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
        , screenSizeVert_(gui::Display::Instance()->GetWinHeight())
        , region_()
    {
        Setup(
            MAX_WIDTH,
            TITLE_TEXT,
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_,
            IMAGE_SIZE_HORIZ);
    }

    Credit::Credit(
        const float MAX_WIDTH,
        const std::string & TITLE_TEXT,
        const gui::GuiFont::Enum FONT,
        const std::string & CONTENT_TEXT)
        : titleTextUPtr_()
        , contentTextUPtr_()
        , mediaType_(MediaType::Image)
        , cachedTextureOpt_("media-images-logos-openfontlicense")
        , sprite_(cachedTextureOpt_.value().Get())
        , animUPtr_()
        , screenSizeVert_(gui::Display::Instance()->GetWinHeight())
        , region_()
    {
        Setup(
            MAX_WIDTH,
            TITLE_TEXT,
            FONT,
            gui::FontManager::Instance()->Size_Smallish(),
            CONTENT_TEXT,
            mediaType_,
            sfutil::ScreenRatioToPixelsHoriz(0.08f));
    }

    void Credit::Setup(
        const float MAX_WIDTH,
        const std::string & TITLE_TEXT,
        const gui::GuiFont::Enum TITLE_FONT,
        const unsigned int TITLE_FONT_SIZE,
        const std::string & CONTENT_TEXT,
        const MediaType::Enum MEDIA_TYPE,
        const float MEDIA_SIZE_HORIZ,
        const gui::Animations::Enum ANIM_ENUM,
        const float ANIM_FRAME_TIME_SEC)
    {
        if (MEDIA_TYPE == MediaType::Image)
        {
            const auto SCALE { MEDIA_SIZE_HORIZ / sprite_.getLocalBounds().width };
            sprite_.setScale(SCALE, SCALE);

            sprite_.setPosition(sfutil::DisplayCenterHoriz(sprite_.getGlobalBounds().width), 0.0f);
        }
        else if (MEDIA_TYPE == MediaType::Anim)
        {
            // initial size and pos are default (zeros)
            animUPtr_
                = gui::AnimationFactory::Make(ANIM_ENUM, sf::FloatRect(), ANIM_FRAME_TIME_SEC);

            const auto SCALE { MEDIA_SIZE_HORIZ / animUPtr_->OrigSize().x };
            const auto WIDTH { animUPtr_->OrigSize().x * SCALE };
            const auto HEIGHT { animUPtr_->OrigSize().y * SCALE };

            animUPtr_->SetEntityRegion(
                sf::FloatRect(sfutil::DisplayCenterHoriz(WIDTH), 0.0f, WIDTH, HEIGHT));
        }

        const auto TEXT_COLOR_BRIGHT { sf::Color::White };
        const auto TEXT_COLOR_NOTASBRIGHT { sf::Color(255, 255, 255, 220) };

        const auto WILL_TITLE_USE_BRIGHT_TEXT_COLOR { !(MEDIA_TYPE == MediaType::Text)
                                                      && !TITLE_TEXT.empty() };

        const auto TITLE_COLOR { (
            (WILL_TITLE_USE_BRIGHT_TEXT_COLOR) ? TEXT_COLOR_BRIGHT : TEXT_COLOR_NOTASBRIGHT) };

        const auto CONTENT_COLOR { (
            (WILL_TITLE_USE_BRIGHT_TEXT_COLOR) ? TEXT_COLOR_NOTASBRIGHT : TEXT_COLOR_BRIGHT) };

        const auto BETWEEN_MEDIA_AND_TEXT_VERT_SPACER { sfutil::ScreenRatioToPixelsVert(0.01f) };

        const auto TITLE_TEXT_POS_TOP { (
            sfutil::Bottom(CalcBounds()) + BETWEEN_MEDIA_AND_TEXT_VERT_SPACER) };

        if (TITLE_TEXT.empty() == false)
        {
            const sf::FloatRect TITLE_TEXT_REGION(
                sfutil::DisplayCenterHoriz(MAX_WIDTH), TITLE_TEXT_POS_TOP, MAX_WIDTH, 0.0f);

            const gui::TextInfo TEXT_INFO_TITLE(
                TITLE_TEXT, TITLE_FONT, TITLE_FONT_SIZE, TITLE_COLOR, gui::Justified::Center);

            titleTextUPtr_ = std::make_unique<gui::TextRegion>(
                "CreditTitle_" + TITLE_TEXT, TEXT_INFO_TITLE, TITLE_TEXT_REGION);
        }

        gui::TextInfo textInfoContent(
            CONTENT_TEXT,
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Normal(),
            CONTENT_COLOR,
            gui::Justified::Center);

        // if there is a lot of content text, reduce the font size to look better
        const auto NUM_NEWLINES { static_cast<int>(
            std::count(CONTENT_TEXT.begin(), CONTENT_TEXT.end(), '\n')) };

        if (NUM_NEWLINES > 3)
        {
            const int NEWLINE_COUNT_LIMIT { 10 };

            const auto NEWLINE_RATIO { static_cast<float>(
                                           std::min(NEWLINE_COUNT_LIMIT, NUM_NEWLINES))
                                       / static_cast<float>(NEWLINE_COUNT_LIMIT) };

            const auto FONT_SIZE_CURRENT_F { static_cast<float>(textInfoContent.size) };

            const auto FONT_SIZE_REDUCTION_F { ((FONT_SIZE_CURRENT_F * 0.333f) * NEWLINE_RATIO) };

            textInfoContent.size -= static_cast<unsigned int>(FONT_SIZE_REDUCTION_F);
        }

        const auto BETWEEN_TITLE_AND_CONTENT_TEXT_VERTICAL_SPACER { sfutil::ScreenRatioToPixelsVert(
            0.0065f) };

        const auto CONTENT_TEXT_POS_TOP { (
            sfutil::Bottom(CalcBounds()) + BETWEEN_TITLE_AND_CONTENT_TEXT_VERTICAL_SPACER) };

        if (CONTENT_TEXT.empty() == false)
        {
            const sf::FloatRect CONTENT_TEXT_REGION(
                sfutil::DisplayCenterHoriz(MAX_WIDTH), CONTENT_TEXT_POS_TOP, MAX_WIDTH, 0.0f);

            contentTextUPtr_ = std::make_unique<gui::TextRegion>(
                "CreditContent", textInfoContent, CONTENT_TEXT_REGION);
        }

        region_ = CalcBounds();
    }

    void Credit::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
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
        if (animUPtr_)
        {
            animUPtr_->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }

    void Credit::SetVerticalPosition(const float POS_TOP) { Move(0.0f, (POS_TOP - region_.top)); }

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

        sfutil::Move(region_, sf::Vector2f(ADJ_HORIZ, ADJ_VERT));
    }

    const sf::FloatRect Credit::CalcBounds() const
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

        if (sprite_.getTexture() != nullptr)
        {
            rects.emplace_back(sprite_.getGlobalBounds());
        }

        return sfutil::MinimallyEnclosing(rects, true);
    }

} // namespace stage
} // namespace heroespath
