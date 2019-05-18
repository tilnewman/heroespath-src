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
        , m_cachedTextureUPtrs()
        , m_sprites()
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

        const auto WINDOW_REGION = windowRegion();

        const auto CONTENT_REGION { contentRegion() };
        const auto CONTENT_REGION_RIGHT { CONTENT_REGION.left + CONTENT_REGION.width };
        const auto CONTENT_REGION_BOTTOM { CONTENT_REGION.top + CONTENT_REGION.height };

        sf::Sprite sprite(m_cachedTextureUPtrs.back()->Get());

        {
            const auto SPRITE_INITIAL_GLOBAL_BOUNDS = sprite.getGlobalBounds();

            if (!(SPRITE_INITIAL_GLOBAL_BOUNDS.width > 0.0f)
                || !(SPRITE_INITIAL_GLOBAL_BOUNDS.height > 0.0f))
            {
                std::ostringstream ss;

                ss << makeErrorMessagePrefix()
                   << "appendSprite() created sprite with invalid bounds="
                   << SPRITE_INITIAL_GLOBAL_BOUNDS;

                throw std::runtime_error(ss.str());
            }

            if (willEnsureAllImagesSameSize() && !m_sprites.empty())
            {
                const auto CURR_SPRITE_SIZE = sfutil::Size(SPRITE_INITIAL_GLOBAL_BOUNDS);
                const auto PREV_SPRITE_SIZE = sfutil::Size(m_sprites.back().getGlobalBounds());
                const auto SPRITE_SIZE_DIFF { CURR_SPRITE_SIZE - PREV_SPRITE_SIZE };

                if (!(misc::Abs(SPRITE_SIZE_DIFF.x) < 1.0f)
                    || !(misc::Abs(SPRITE_SIZE_DIFF.y) < 1.0f))
                {
                    std::ostringstream ss;

                    ss << makeErrorMessagePrefix()
                       << "appendSprite() created sprite with size=" << CURR_SPRITE_SIZE
                       << ", but that was different from all previous that had size="
                       << PREV_SPRITE_SIZE << ", and that is not allowed in this image set.";

                    throw std::runtime_error(ss.str());
                }
            }

            const sf::Vector2f SPRITE_MAX_SIZE(
                (WINDOW_REGION.width * 0.49f), (WINDOW_REGION.height * 0.49f));

            if ((SPRITE_INITIAL_GLOBAL_BOUNDS.width > SPRITE_MAX_SIZE.x)
                || (SPRITE_INITIAL_GLOBAL_BOUNDS.height > SPRITE_MAX_SIZE.y))
            {
                sfutil::Fit(sprite, SPRITE_MAX_SIZE);
            }
        }

        if (m_sprites.empty())
        {
            sprite.setPosition(sfutil::Position(CONTENT_REGION));
        }
        else
        {
            auto shiftSpritesAndRemoveAnyOffscreen = [&](const float SHIFT) {
                for (auto & otherSprite : m_sprites)
                {
                    otherSprite.move(SHIFT, 0.0f);
                }

                m_sprites.erase(
                    std::remove_if(
                        std::begin(m_sprites),
                        std::end(m_sprites),
                        [&CONTENT_REGION](const auto & SPRITE) {
                            return !sfutil::Intersects(SPRITE, CONTENT_REGION);
                        }),
                    std::end(m_sprites));
            };

            const auto SPRITE_BOUNDS = sprite.getGlobalBounds();

            const auto PREV_SPRITE_BOUNDS = m_sprites.back().getGlobalBounds();
            const auto PREV_SPRITE_BOTTOM { PREV_SPRITE_BOUNDS.top + PREV_SPRITE_BOUNDS.height };

            const bool CAN_FIT_VERTICALLY_UNDER_PREV_SPRITE(
                (PREV_SPRITE_BOTTOM + SPRITE_BOUNDS.height) < CONTENT_REGION_BOTTOM);

            float left = 0.0f;
            float top = 0.0f;

            if (CAN_FIT_VERTICALLY_UNDER_PREV_SPRITE)
            {
                left = PREV_SPRITE_BOUNDS.left;
                top = PREV_SPRITE_BOTTOM;
            }
            else
            {
                left = (PREV_SPRITE_BOUNDS.left + PREV_SPRITE_BOUNDS.width);
                top = CONTENT_REGION.top;

                // if we don't fit under the previous then we are being placed on top
                // and so now the previous sprite is NOT the sprite to our left
                // so this loop finds the right-most end of all the sprites to our left
                for (auto iter = std::rbegin(m_sprites); iter != std::rend(m_sprites); ++iter)
                {
                    const auto REV_BOUNDS = iter->getGlobalBounds();
                    if (misc::IsRealClose(PREV_SPRITE_BOUNDS.left, REV_BOUNDS.left))
                    {
                        const float REV_BOUNDS_RIGHT { REV_BOUNDS.left + REV_BOUNDS.width };
                        if (REV_BOUNDS_RIGHT > left)
                        {
                            left = REV_BOUNDS_RIGHT;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            const bool CAN_FIT_HORIZONTALLY((left + SPRITE_BOUNDS.width) < CONTENT_REGION_RIGHT);

            if (!CAN_FIT_HORIZONTALLY)
            {
                const float HORIZ_SHIFT { CONTENT_REGION_RIGHT - (left + SPRITE_BOUNDS.width) };
                shiftSpritesAndRemoveAnyOffscreen(HORIZ_SHIFT);
                left = (CONTENT_REGION_RIGHT - SPRITE_BOUNDS.width);
            }

            sprite.setPosition(left, top);
        }

        m_sprites.emplace_back(sprite);
    }

} // namespace test
} // namespace heroespath
