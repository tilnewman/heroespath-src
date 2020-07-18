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
#include "gui/entity.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"
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
        m_entityPtrs.clear();
    }

    void SingleImageDisplayer::appendImageToSeries(gui::CachedTexture cachedTexture)
    {
        m_cachedTextureUPtrs.emplace_back(
            std::make_unique<gui::CachedTexture>(std::move(cachedTexture)));

        sf::Sprite sprite(m_cachedTextureUPtrs.back()->Get());
        const auto REGION = tileImagesAndReturnNextImageRegion(sfutil::Size(sprite));
        sfutil::FitAndCenterTo(sprite, REGION);
        m_sprites.emplace_back(sprite);

        incrememntProgress();
    }

    void SingleImageDisplayer::appendEntity(const gui::IEntityPtr_t ENTITY_PTR)
    {
        const auto REGION
            = tileImagesAndReturnNextImageRegion(sfutil::Size(ENTITY_PTR->GetEntityRegion()));

        ENTITY_PTR->SetEntityRegion(REGION);
        DisplayerBase::appendEntity(ENTITY_PTR);
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

    const std::tuple<bool, sf::Vector2f> SingleImageDisplayer::prevSizeIfAny() const
    {
        if (!m_sprites.empty())
        {
            return { true, sfutil::Size(m_sprites.back().getGlobalBounds()) };
        }
        else if (!m_entityPtrs.empty())
        {
            return { true, sfutil::Size(m_entityPtrs.back()->GetEntityRegion()) };
        }
        else
        {
            return { false, { 0.0f, 0.0f } };
        }
    }

    const sf::Vector2f
        SingleImageDisplayer::verifyCurrentSizeCalcNewSize(const sf::Vector2f & CURR_SIZE) const
    {
        if (sfutil::IsZeroOrLessAny(CURR_SIZE))
        {
            std::ostringstream ss;

            ss << makeErrorMessagePrefix()
               << "verifyCurrentSizeAndGetNewSize(CURR_SIZE=" << CURR_SIZE
               << ") given invalid size";

            throw std::runtime_error(ss.str());
        }

        if ((misc::CountBits(static_cast<std::uint32_t>(CURR_SIZE.x)) != 1)
            || (misc::CountBits(static_cast<std::uint32_t>(CURR_SIZE.y)) != 1))
        {
            M_HP_LOG_WRN(
                makeErrorMessagePrefix() << "verifyCurrentSizeAndGetNewSize(CURR_SIZE=" << CURR_SIZE
                                         << ") given invalid size that was not a power of two.");
        }

        const auto [HAS_PREV, PREV_SIZE] = prevSizeIfAny();

        if (willEnsureAllImagesSameSize() && HAS_PREV)
        {
            const auto SIZE_DIFF { sfutil::AbsCopy(CURR_SIZE - PREV_SIZE) };

            if (sfutil::IsOneOrGreaterAny(SIZE_DIFF))
            {
                std::ostringstream ss;

                ss << makeErrorMessagePrefix()
                   << "verifyCurrentSizeAndGetNewSize() created sprite with size=" << CURR_SIZE
                   << ", but that was different from all previous that had size=" << PREV_SIZE
                   << ", and that is not allowed in this image set.";

                throw std::runtime_error(ss.str());
            }
        }

        const sf::Vector2f MAX_SIZE { sfutil::Size(contentRegion(), 0.49f) };

        if ((CURR_SIZE.x > MAX_SIZE.x) || (CURR_SIZE.y > MAX_SIZE.y))
        {
            return sfutil::FitCopy(CURR_SIZE, MAX_SIZE);
        }
        else
        {
            return CURR_SIZE;
        }
    }

    void SingleImageDisplayer::moveAllAndRemoveOffscreen(const sf::Vector2f & MOVE)
    {
        const auto CONTENT_REGION { contentRegion() };

        for (auto & sprite : m_sprites)
        {
            sprite.move(MOVE);
        }

        m_sprites.erase(
            std::remove_if(
                std::begin(m_sprites),
                std::end(m_sprites),
                [&CONTENT_REGION](const auto & SPRITE) {
                    return !sfutil::Intersects(SPRITE, CONTENT_REGION);
                }),
            std::end(m_sprites));

        for (auto & entityPtr : m_entityPtrs)
        {
            entityPtr->MoveEntityPos(MOVE);
        }

        m_entityPtrs.erase(
            std::remove_if(
                std::begin(m_entityPtrs),
                std::end(m_entityPtrs),
                [&CONTENT_REGION](const auto ENTITY_PTR) {
                    return !sfutil::Intersects(ENTITY_PTR->GetEntityRegion(), CONTENT_REGION);
                }),
            std::end(m_entityPtrs));
    }

    const sf::FloatRect SingleImageDisplayer::prevImageBounds() const
    {
        sf::FloatRect result;

        if (!m_sprites.empty())
        {
            result = m_sprites.back().getGlobalBounds();
        }

        if (!m_entityPtrs.empty())
        {
            const auto PREV_ENTITY_BOUNDS = m_entityPtrs.back()->GetEntityRegion();

            if (m_sprites.empty() || (PREV_ENTITY_BOUNDS.left > result.left)
                || ((PREV_ENTITY_BOUNDS.left + PREV_ENTITY_BOUNDS.width)
                    > (result.left + result.width)))
            {
                result = PREV_ENTITY_BOUNDS;
            }
        }

        return result;
    }

    const sf::FloatRect SingleImageDisplayer::boundsOfImageFurthestRight() const
    {
        sf::FloatRect result;

        auto setResultIfRightMost = [&](const sf::FloatRect & BOUNDS) {
            if (BOUNDS.left < result.left)
            {
                return false;
            }

            const float BOUNDS_RIGHT { BOUNDS.left + BOUNDS.width };

            if (BOUNDS_RIGHT > (result.left + result.width))
            {
                result = BOUNDS;
            }

            return true;
        };

        for (auto iter = std::rbegin(m_sprites); iter != std::rend(m_sprites); ++iter)
        {
            if (!setResultIfRightMost(iter->getGlobalBounds()))
            {
                break;
            }
        }

        for (auto iter = std::rbegin(m_entityPtrs); iter != std::rend(m_entityPtrs); ++iter)
        {
            if (!setResultIfRightMost((*iter)->GetEntityRegion()))
            {
                break;
            }
        }

        return result;
    }

    const sf::FloatRect
        SingleImageDisplayer::tileImagesAndReturnNextImageRegion(const sf::Vector2f & ORIG_SIZE)
    {
        const auto CONTENT_REGION { contentRegion() };
        const auto CONTENT_REGION_RIGHT { CONTENT_REGION.left + CONTENT_REGION.width };
        const auto CONTENT_REGION_BOTTOM { CONTENT_REGION.top + CONTENT_REGION.height };

        sf::FloatRect newRegion { sfutil::Position(CONTENT_REGION),
                                  verifyCurrentSizeCalcNewSize(ORIG_SIZE) };

        if (sfutil::IsZeroOrLessAll(prevImageBounds()))
        {
            sfutil::CenterTo(newRegion, CONTENT_REGION);
            return newRegion;
        }

        if (m_sprites.empty() && (m_entityPtrs.size() == 1))
        {
            m_entityPtrs.front()->SetEntityPos(sfutil::Position(CONTENT_REGION));
        }

        if ((m_sprites.size() == 1) && m_entityPtrs.empty())
        {
            m_sprites.front().setPosition(sfutil::Position(CONTENT_REGION));
        }

        const auto PREV_IMAGE_BOUNDS = prevImageBounds();
        const auto PREV_IMAGE_BOTTOM { PREV_IMAGE_BOUNDS.top + PREV_IMAGE_BOUNDS.height };

        const bool CAN_FIT_VERTICALLY_UNDER_PREV_IMAGE(
            (PREV_IMAGE_BOTTOM + newRegion.height) < CONTENT_REGION_BOTTOM);

        if (CAN_FIT_VERTICALLY_UNDER_PREV_IMAGE)
        {
            newRegion.left = PREV_IMAGE_BOUNDS.left;
            newRegion.top = PREV_IMAGE_BOTTOM;
        }
        else
        {
            // If it doesn't fit under the previous image then it will be placed at the top.
            // So now PREV_IMAGE_BOUNDS might not be the image to our left anymore.
            // So we have to search for the image that is now to our left in the tile.
            const auto RIGHT_MOST_BOUNDS = boundsOfImageFurthestRight();
            newRegion.left = (RIGHT_MOST_BOUNDS.left + RIGHT_MOST_BOUNDS.width);

            newRegion.top = CONTENT_REGION.top;
        }

        const auto NEW_REGION_RIGHT { newRegion.left + newRegion.width };
        const bool CAN_FIT_HORIZONTALLY(NEW_REGION_RIGHT < CONTENT_REGION_RIGHT);

        if (!CAN_FIT_HORIZONTALLY)
        {
            const sf::Vector2f SHIFT_LEFT_POS_ADJ((CONTENT_REGION_RIGHT - NEW_REGION_RIGHT), 0.0f);
            moveAllAndRemoveOffscreen(SHIFT_LEFT_POS_ADJ);
            newRegion.left = (CONTENT_REGION_RIGHT - newRegion.width);
        }

        return newRegion;
    }

} // namespace test
} // namespace heroespath
