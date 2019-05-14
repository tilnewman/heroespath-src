// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_HPP_INCLUDED
//
// single-image-displayer.hpp
//
#include "testutil/displayer-base.hpp"

namespace heroespath
{
namespace test
{

    struct SingleImageDisplayer : public DisplayerBase
    {
        SingleImageDisplayer()
            : DisplayerBase("SingleImageDisplayer")
        {}

        virtual ~SingleImageDisplayer() = default;

        void releaseAndFreeAll() override
        {
            m_cachedTextureUPtrs.clear();
            m_sprites.clear();
        }

        void appendImageToSeries(heroespath::gui::CachedTexture && cachedTexture) override
        {
            appendTexture(std::move(cachedTexture));
            appendSprite();
            incrememntProgress();
        }

        void draw(sf::RenderTarget & target, sf::RenderStates = sf::RenderStates()) const override
        {
            for (const auto & SPRITE : m_sprites)
            {
                // if (heroespath::sfutil::Intersects(SPRITE, m_imageRegion))
                {
                    target.draw(SPRITE);
                }
            }

            drawCommon(target);
        }

    private:
        void appendTexture(heroespath::gui::CachedTexture && cachedTexture)
        {
            m_cachedTextureUPtrs.emplace_back(
                std::make_unique<heroespath::gui::CachedTexture>(cachedTexture));

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

        void appendSprite()
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

                ss << makeErrorMessagePrefix()
                   << "setupSprite() created sprite with invalid bounds="
                   << SPRITE_INITIAL_GLOBAL_BOUNDS;

                throw std::runtime_error(ss.str());
            }

            if ((SPRITE_INITIAL_GLOBAL_BOUNDS.width > windowRegion().width)
                || (SPRITE_INITIAL_GLOBAL_BOUNDS.height > windowRegion().height))
            {
                heroespath::sfutil::Fit(sprite, heroespath::sfutil::Size(contentRegion()));
            }

            heroespath::sfutil::CenterTo(sprite, contentRegion());

            const auto WIDTH = sprite.getGlobalBounds().width;
            sprite.setPosition(contentRegion().width, sprite.getPosition().y);

            for (auto & otherSprite : m_sprites)
            {
                otherSprite.move(-WIDTH, 0.0f);
            }
        }

    private:
        std::vector<std::unique_ptr<heroespath::gui::CachedTexture>> m_cachedTextureUPtrs;
        std::vector<sf::Sprite> m_sprites;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_HPP_INCLUDED
