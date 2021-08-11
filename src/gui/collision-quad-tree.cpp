// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// collision-quad-tree.cpp
//
#include "collision-quad-tree.hpp"

#include "sfutil/vector-and-rect.hpp"

namespace heroespath
{
namespace gui
{

    // This size was found by experiment to work well the typical map found in the game.
    const int QuadTree::MIN_QUAD_SIZE_ { 1600 };

    QuadTree::QuadTree()
        : headQuad_()
        , opCount_(0)
    {}

    void
        QuadTree::Setup(const sf::FloatRect & TOTAL_RECT_F, const FloatRectVec_t & COLL_RECTS_F_VEC)
    {
        headQuad_ = Quad();

        sf::IntRect finalTotalRect { TOTAL_RECT_F };

        // add one to width and height to account for rounding errors
        ++finalTotalRect.width;
        ++finalTotalRect.height;

        // make sure the total rect is has even width and height so it divides by 2 correctly
        if ((finalTotalRect.width % 2) != 0)
        {
            ++finalTotalRect.width;
        }

        if ((finalTotalRect.height % 2) != 0)
        {
            ++finalTotalRect.height;
        }

        IntRectVec_t intRects;
        intRects.reserve(COLL_RECTS_F_VEC.size());
        for (const auto & COLL_RECT_F : COLL_RECTS_F_VEC)
        {
            intRects.emplace_back(sf::IntRect { COLL_RECT_F });
        }

        opCount_ = 0;
        PopulateQuadAndRecurse(headQuad_, finalTotalRect, intRects);

        // TEMP TODO REMOVE WHEN DONE TESTING
        M_HP_LOG_DBG(
            "QuadTree orig=" << TOTAL_RECT_F << ", final=" << finalTotalRect << ", with rect_count="
                             << COLL_RECTS_F_VEC.size() << ", recurse_count=" << opCount_);
    }

    void QuadTree::PopulateQuadAndRecurse(
        Quad & quad, const sf::IntRect & TOTAL_RECT, const IntRectVec_t & COLL_RECTS_VEC)
    {
        ++opCount_;

        for (const auto & COLL_RECT : COLL_RECTS_VEC)
        {
            if (COLL_RECT.intersects(TOTAL_RECT))
            {
                quad.coll_rects_vec.emplace_back(COLL_RECT);
            }
        }

        if ((quad.coll_rects_vec.empty() == false) && (TOTAL_RECT.width >= MIN_QUAD_SIZE_)
            && (TOTAL_RECT.height >= MIN_QUAD_SIZE_))
        {
            const auto HALF_WIDTH { TOTAL_RECT.width / 2 };
            const auto HALF_HEIGHT { TOTAL_RECT.height / 2 };

            quad.child_rect_top_left
                = sf::IntRect(TOTAL_RECT.left, TOTAL_RECT.top, HALF_WIDTH, HALF_HEIGHT);

            quad.child_rect_top_right = sf::IntRect(
                TOTAL_RECT.left + HALF_WIDTH, TOTAL_RECT.top, HALF_WIDTH, HALF_HEIGHT);

            quad.child_rect_bot_left = sf::IntRect(
                TOTAL_RECT.left, TOTAL_RECT.top + HALF_HEIGHT, HALF_WIDTH, HALF_HEIGHT);

            quad.child_rect_bot_right = sf::IntRect(
                TOTAL_RECT.left + HALF_WIDTH,
                TOTAL_RECT.top + HALF_HEIGHT,
                HALF_WIDTH,
                HALF_HEIGHT);

            auto populateAndRecurse = [&](const sf::IntRect & CHILD_RECT) {
                quad.child_quads_vec.emplace_back(Quad(quad.coll_rects_vec.size()));

                PopulateQuadAndRecurse(
                    quad.child_quads_vec.back(), CHILD_RECT, quad.coll_rects_vec);
            };

            populateAndRecurse(quad.child_rect_top_left);
            populateAndRecurse(quad.child_rect_top_right);
            populateAndRecurse(quad.child_rect_bot_left);
            populateAndRecurse(quad.child_rect_bot_right);

            quad.coll_rects_vec.clear();
        }
    }

    bool QuadTree::DoesPointCollide(const sf::Vector2f & POINT_F) const
    {
        opCount_ = 0;
        return DoesPointCollideImpl(headQuad_, sf::Vector2i { POINT_F });
    }

    bool QuadTree::DoesRectCollide(const sf::FloatRect & RECT_F) const
    {
        opCount_ = 0;
        return DoesRectCollideImpl(headQuad_, sf::IntRect { RECT_F });
    }

    bool QuadTree::DoesPointCollideImpl(const Quad & QUAD, const sf::Vector2i & POINT) const
    {
        ++opCount_;
        if (QUAD.coll_rects_vec.empty() == false)
        {
            for (const auto & COLL_RECT : QUAD.coll_rects_vec)
            {
                ++opCount_;
                if (COLL_RECT.contains(POINT))
                {
                    return true;
                }
            }

            return false;
        }

        ++opCount_;
        if (POINT.x < QUAD.child_rect_top_right.left)
        {
            ++opCount_;
            if (POINT.y < QUAD.child_rect_bot_left.top)
            {
                return DoesPointCollideImpl(QUAD.child_quads_vec[0], POINT);
            }
            else
            {
                return DoesPointCollideImpl(QUAD.child_quads_vec[2], POINT);
            }
        }
        else
        {
            if (POINT.y < QUAD.child_rect_bot_left.top)
            {
                return DoesPointCollideImpl(QUAD.child_quads_vec[1], POINT);
            }
            else
            {
                return DoesPointCollideImpl(QUAD.child_quads_vec[3], POINT);
            }
        }
    }

    bool QuadTree::DoesRectCollideImpl(const Quad & QUAD, const sf::IntRect & RECT) const
    {
        ++opCount_;
        if (QUAD.coll_rects_vec.empty() == false)
        {
            for (const auto & COLL_RECT : QUAD.coll_rects_vec)
            {
                ++opCount_;
                if (COLL_RECT.intersects(RECT))
                {
                    return true;
                }
            }

            return false;
        }

        if (QUAD.child_quads_vec.size() == 4)
        {
            auto recurseIfNeeded = [&](const sf::IntRect & CHILD_RECT, const Quad & CHILD_QUAD) {
                ++opCount_;

                if (CHILD_RECT.intersects(RECT))
                {
                    return DoesRectCollideImpl(CHILD_QUAD, RECT);
                }

                return false;
            };

            if (recurseIfNeeded(QUAD.child_rect_top_left, QUAD.child_quads_vec[0]))
            {
                return true;
            }
            else if (recurseIfNeeded(QUAD.child_rect_top_right, QUAD.child_quads_vec[1]))
            {
                return true;
            }
            else if (recurseIfNeeded(QUAD.child_rect_bot_left, QUAD.child_quads_vec[2]))
            {
                return true;
            }
            else
            {
                return recurseIfNeeded(QUAD.child_rect_bot_right, QUAD.child_quads_vec[3]);
            }
        }

        return false;
    }

} // namespace gui
} // namespace heroespath
