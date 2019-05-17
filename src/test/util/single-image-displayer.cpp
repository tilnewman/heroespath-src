// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// single-image-displayer.cpp
//
#include "single-image-displayer.hpp"

#include "gui/cached-texture.hpp"
#include "sfutil/center.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics.hpp>

namespace heroespath
{
namespace test
{

    SingleImageDisplayer::SingleImageDisplayer()
        : DisplayerBase("SingleImageDisplayer")
    {}

    SingleImageDisplayer::~SingleImageDisplayer() {}

    void SingleImageDisplayer::releaseAndFreeAll()
    {
        m_cachedTextureUPtrs.clear();
        m_sprites.clear();
    }

    void SingleImageDisplayer::appendImageToSeries(gui::CachedTexture cachedTexture)
    {
        appendTexture(std::move(cachedTexture));
        appendSprite();
        incrememntProgress();
    }

    void SingleImageDisplayer::draw(sf::RenderTarget & target, sf::RenderStates) const
    {
        for (const auto & SPRITE : m_sprites)
        {
            target.draw(SPRITE);
        }

        drawCommon(target);
    }

    void SingleImageDisplayer::appendTexture(gui::CachedTexture cachedTexture)
    {
        m_cachedTextureUPtrs.emplace_back(
            std::make_unique<gui::CachedTexture>(std::move(cachedTexture)));

        const auto SIZE = m_cachedTextureUPtrs.back()->Get().getSize();

        if ((SIZE.x == 0) || (SIZE.y == 0))
        {
            std::ostringstream ss;
            ss << makeErrorMessagePrefix()
               << "setupTexture(cachedTexture.Path=\"" + cachedTexture.Path()
                    + "\") Created a CachedTexture with an invalid size="
               << SIZE;

            throw std::runtime_error(ss.str());
        }
    }

    void SingleImageDisplayer::appendSprite()
    {
        if (m_cachedTextureUPtrs.empty())
        {
            std::ostringstream ss;

            ss << makeErrorMessagePrefix()
               << "setupSprite() called when m_cachedTextureUPtrs was empty.";

            throw std::runtime_error(ss.str());
        }

        m_sprites.emplace_back(sf::Sprite(m_cachedTextureUPtrs.back()->Get()));

        auto & sprite = m_sprites.back();
        const auto SPRITE_INITIAL_GLOBAL_BOUNDS = sprite.getGlobalBounds();

        if (!(SPRITE_INITIAL_GLOBAL_BOUNDS.width > 0.0f)
            || !(SPRITE_INITIAL_GLOBAL_BOUNDS.height > 0.0f))
        {
            std::ostringstream ss;

            ss << makeErrorMessagePrefix() << "setupSprite() created sprite with invalid bounds="
               << SPRITE_INITIAL_GLOBAL_BOUNDS;

            throw std::runtime_error(ss.str());
        }

        if ((SPRITE_INITIAL_GLOBAL_BOUNDS.width > windowRegion().width)
            || (SPRITE_INITIAL_GLOBAL_BOUNDS.height > windowRegion().height))
        {
            sfutil::Fit(sprite, sfutil::Size(contentRegion()));
        }

        sfutil::CenterTo(sprite, contentRegion());

        const auto WIDTH = sprite.getGlobalBounds().width;
        sprite.setPosition(contentRegion().width, sprite.getPosition().y);

        const auto CONTENT_REGION = contentRegion();

        for (auto & otherSprite : m_sprites)
        {
            otherSprite.move(-WIDTH, 0.0f);
        }

        m_sprites.erase(
            std::remove_if(
                std::begin(m_sprites),
                std::end(m_sprites),
                [&CONTENT_REGION](const auto & SPRITE) {
                    return !sfutil::Intersects(SPRITE, CONTENT_REGION);
                }),
            std::end(m_sprites));
    }

} // namespace test
} // namespace heroespath
