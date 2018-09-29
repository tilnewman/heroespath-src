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

#include "sfml-util/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace sfml_util
{

    const sf::Color BottomSymbol::DEFAULT_COLOR_ { sf::Color(255, 255, 255, 127) };
    const std::string BottomSymbol::IMAGE_PATH_KEY_ { "media-images-gui-accents-symbol1" };

    BottomSymbol::BottomSymbol(
        const float VERT_SCALE, const bool WILL_INVERT_COLOR, const sf::Color & COLOR)
        : cachedTexture_(IMAGE_PATH_KEY_, MakeImageOpt(WILL_INVERT_COLOR))
        , sprites_()
        , region_(0.0f, 0.0f, 0.0f, 0.0f)
    {
        Setup(VERT_SCALE, WILL_INVERT_COLOR, COLOR);
    }

    void BottomSymbol::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (const auto & SPRITE : sprites_)
        {
            target.draw(SPRITE, states);
        }
    }

    void BottomSymbol::Setup(
        const float VERT_SCALE, const bool WILL_INVERT_COLOR, const sf::Color & COLOR)
    {
        sprites_.clear();

        const auto NEW_IMAGE_OPT { MakeImageOpt(WILL_INVERT_COLOR) };
        if (cachedTexture_.Options().option_enum != NEW_IMAGE_OPT)
        {
            cachedTexture_ = CachedTexture(IMAGE_PATH_KEY_, NEW_IMAGE_OPT);
        }

        const sf::FloatRect TEXTURE_REGION(
            sf::Vector2f(0.0f, 0.0f), sf::Vector2f(cachedTexture_.Get().getSize()));

        const auto FULL_SCREEN_RECT { sfml_util::Display::Instance()->FullScreenRect() };

        const sf::Vector2f IMAGE_SIZE_CONTRAINTS_V(
            0.0f, (sfutil::ScreenRatioToPixelsVert(0.137f) * VERT_SCALE));

        auto const POS_TOP { (FULL_SCREEN_RECT.height - IMAGE_SIZE_CONTRAINTS_V.y) };

        auto const HALF_SCREEN_WIDTH { FULL_SCREEN_RECT.width * 0.5f };

        float counter { 0.0f };
        float posMostLeft { HALF_SCREEN_WIDTH };
        float posMostRight { HALF_SCREEN_WIDTH };

        while ((posMostLeft > 0.0f) && (posMostRight < FULL_SCREEN_RECT.width))
        {
            sf::Sprite spriteLeft(cachedTexture_.Get());
            sf::Sprite spriteRight(cachedTexture_.Get());

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

    sfml_util::ImageOpt::Enum BottomSymbol::MakeImageOpt(const bool WILL_INVERT_COLOR) const
    {
        return ImageOpt::Default | ((WILL_INVERT_COLOR) ? ImageOpt::Invert : ImageOpt::None);
    }

} // namespace sfml_util
} // namespace heroespath
