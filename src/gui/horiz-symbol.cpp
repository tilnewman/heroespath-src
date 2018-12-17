// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// horiz-symbol.cpp
//
#include "horiz-symbol.hpp"

#include "gui/display.hpp"
#include "gui/menu-image-enum.hpp"
#include "misc/config-file.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace gui
{

    const float BottomSymbol::DEFAULT_HEIGHT_SCREEN_RATIO_ { 0.137f };
    const float BottomSymbol::VISIBLE_VERT_POS_RATIO_ { 0.575f };
    const sf::Color BottomSymbol::DEFAULT_COLOR_ { 255, 255, 255, 16 };

    BottomSymbol::BottomSymbol(
        const bool WILL_INVERT_COLOR_TO_WHITE,
        const float VERT_RESCALE,
        const sf::Color & COLOR,
        const float HEIGHT_SCREEN_RATIO)
        : IMAGE_CONFIG_FILE_KEY_(MenuImage::ConfigFileKey(MenuImage::SymbolBottom))
        , cachedTexture_(IMAGE_CONFIG_FILE_KEY_, MakeImageOpt(WILL_INVERT_COLOR_TO_WHITE))
        , sprites_()
        , region_(0.0f, 0.0f, 0.0f, 0.0f)
    {
        Setup(WILL_INVERT_COLOR_TO_WHITE, VERT_RESCALE, COLOR, HEIGHT_SCREEN_RATIO);
    }

    void BottomSymbol::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const auto & SPRITE : sprites_)
        {
            target.draw(SPRITE, states);
        }
    }

    float BottomSymbol::VisibleVerticalCenter() const
    {
        return (region_.top + (region_.height * VISIBLE_VERT_POS_RATIO_));
    }

    void BottomSymbol::Setup(
        const bool WILL_INVERT_COLOR_TO_WHITE,
        const float VERT_RESCALE,
        const sf::Color & COLOR,
        const float HEIGHT_SCREEN_RATIO_PARAM)
    {
        sprites_.clear();

        const auto NEW_IMAGE_OPT { MakeImageOpt(WILL_INVERT_COLOR_TO_WHITE) };

        if (cachedTexture_.Options().option_enum != NEW_IMAGE_OPT)
        {
            cachedTexture_ = CachedTexture(IMAGE_CONFIG_FILE_KEY_, NEW_IMAGE_OPT);
        }

        const auto TEXTURE_RECT { misc::ConfigFile::Instance()->GetTextureRect(
            IMAGE_CONFIG_FILE_KEY_) };

        const auto FULL_SCREEN_RECT { gui::Display::Instance()->FullScreenRect() };

        const auto HEIGHT_SCREEN_RATIO_FINAL { (
            (HEIGHT_SCREEN_RATIO_PARAM > 0.0f) ? HEIGHT_SCREEN_RATIO_PARAM
                                               : DEFAULT_HEIGHT_SCREEN_RATIO_) };

        const sf::Vector2f IMAGE_SIZE_CONTRAINTS_V(
            0.0f, (sfutil::ScreenRatioToPixelsVert(HEIGHT_SCREEN_RATIO_FINAL) * VERT_RESCALE));

        const auto POS_TOP { (FULL_SCREEN_RECT.height - IMAGE_SIZE_CONTRAINTS_V.y) };

        const auto HALF_SCREEN_WIDTH { FULL_SCREEN_RECT.width * 0.5f };

        float counter { 0.0f };
        float posMostLeft { HALF_SCREEN_WIDTH };
        float posMostRight { HALF_SCREEN_WIDTH };

        while ((posMostLeft > 0.0f) && (posMostRight < FULL_SCREEN_RECT.width))
        {
            sf::Sprite spriteLeft(cachedTexture_.Get(), TEXTURE_RECT);
            sf::Sprite spriteRight(cachedTexture_.Get(), TEXTURE_RECT);

            sfutil::Fit(spriteLeft, IMAGE_SIZE_CONTRAINTS_V);
            sfutil::Fit(spriteRight, IMAGE_SIZE_CONTRAINTS_V);

            spriteLeft.setPosition(
                HALF_SCREEN_WIDTH - (spriteLeft.getGlobalBounds().width * (counter + 1.0f)),
                POS_TOP);

            spriteRight.setPosition(
                HALF_SCREEN_WIDTH + (spriteRight.getGlobalBounds().width * counter), POS_TOP);

            spriteLeft.setColor(COLOR);
            spriteRight.setColor(COLOR);

            sprites_.emplace_back(spriteLeft);
            sprites_.emplace_back(spriteRight);

            posMostLeft = spriteLeft.getGlobalBounds().left;
            posMostRight = sfutil::Right(spriteRight);

            counter += 1.0f;
        }

        region_ = sfutil::MinimallyEnclosing(sprites_);

        Color(COLOR);
    }

    const sf::Color BottomSymbol::Color() const
    {
        if (sprites_.empty())
        {
            return sf::Color::Transparent;
        }
        else
        {
            return sprites_[0].getColor();
        }
    }

    void BottomSymbol::Color(const sf::Color & NEW_COLOR)
    {
        for (auto & sprite : sprites_)
        {
            sprite.setColor(NEW_COLOR);
        }
    }

    void BottomSymbol::SetPos(const float POS_LEFT, const float POS_TOP)
    {
        MovePos(POS_LEFT - region_.left, POS_TOP - region_.top);
    }

    void BottomSymbol::MovePos(const float HORIZ, const float VERT)
    {
        for (auto & sprite : sprites_)
        {
            sprite.move(HORIZ, VERT);
        }

        region_.left += HORIZ;
        region_.top += VERT;
    }

    gui::ImageOpt::Enum BottomSymbol::MakeImageOpt(const bool WILL_INVERT_COLOR_TO_WHITE) const
    {
        return ImageOpt::Default
            | ((WILL_INVERT_COLOR_TO_WHITE) ? ImageOpt::Invert : ImageOpt::None);
    }

} // namespace gui
} // namespace heroespath
