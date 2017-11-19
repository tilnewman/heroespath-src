// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// collision-quad-tree.cpp
//
#include "collision-quad-tree.hpp"


namespace heroespath
{
namespace sfml_util
{

    //This size was found by experiment to work well the the typical map found in the game.
    const float QuadTree::MIN_QUAD_SIZE_{ 800.0f };


    void QuadTree::Setup(
        const float MAP_WIDTH,
        const float MAP_HEIGHT,
        const FloatRectVec_t & COLL_RECTS_VEC)
    {
        PopulateQuadAndRecurse(
            headQuad_,
            sf::FloatRect(0.0f, 0.0f, MAP_WIDTH, MAP_HEIGHT),
            COLL_RECTS_VEC);
    }


    void QuadTree::PopulateQuadAndRecurse(
        Quad & quad,
        const sf::FloatRect &  RECT,
        const FloatRectVec_t & COLL_RECTS_VEC)
    {
        for (auto const & NEXT_COLL_RECT : COLL_RECTS_VEC)
        {
            if ((RECT.left < (NEXT_COLL_RECT.left + NEXT_COLL_RECT.width)) &&
                ((RECT.left + RECT.width) > NEXT_COLL_RECT.left) &&
                (RECT.top < (NEXT_COLL_RECT.top + NEXT_COLL_RECT.height)) &&
                ((RECT.height + RECT.top) > NEXT_COLL_RECT.top))
            {
                quad.coll_rects_vec.push_back(NEXT_COLL_RECT);
            }
        }

        if ((quad.coll_rects_vec.empty() == false) &&
            (RECT.width > MIN_QUAD_SIZE_) &&
            (RECT.height > MIN_QUAD_SIZE_))
        {
            auto const HALF_WIDTH{ RECT.width * 0.5f };
            auto const HALF_HEIGHT{ RECT.height * 0.5f };

            quad.quad_rects_vec.push_back(sf::FloatRect(
                RECT.left,
                RECT.top,
                HALF_WIDTH,
                HALF_HEIGHT));

            quad.quad_rects_vec.push_back(sf::FloatRect(
                RECT.left + HALF_WIDTH,
                RECT.top,
                HALF_WIDTH,
                HALF_HEIGHT));

            quad.quad_rects_vec.push_back(sf::FloatRect(
                RECT.left,
                RECT.top + HALF_HEIGHT,
                HALF_WIDTH,
                HALF_HEIGHT));

            quad.quad_rects_vec.push_back(sf::FloatRect(
                RECT.left + HALF_WIDTH,
                RECT.top + HALF_HEIGHT,
                HALF_WIDTH,
                HALF_HEIGHT));

            for (std::size_t i(0); i < 4; ++i)
            {
                quad.child_quads_vec.push_back( Quad() );
                PopulateQuadAndRecurse(
                    quad.child_quads_vec[i],
                    quad.quad_rects_vec[i],
                    quad.coll_rects_vec);
            }

            quad.coll_rects_vec.clear();
        }
    }


    bool QuadTree::IsPointWithinCollisionRect(const sf::Vector2f & POINT) const
    {
        return IsPointWithinCollisionRect_Impl(headQuad_, POINT);
    }


    bool QuadTree::IsPointWithinCollisionRect_Impl(
        const Quad & QUAD,
        const sf::Vector2f & POINT) const
    {
        if (QUAD.quad_rects_vec.empty())
        {
            for (auto const & NEXT_COLL_RECT : QUAD.coll_rects_vec)
            {
                if (NEXT_COLL_RECT.contains(POINT))
                {
                    return true;
                }
            }

            return false;
        }
        else
        {
            if (POINT.x < QUAD.quad_rects_vec[1].left)
            {
                if (POINT.y < QUAD.quad_rects_vec[2].top)
                {
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[0], POINT);
                }
                else
                {
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[2], POINT);
                }
            }
            else
            {
                if (POINT.y < QUAD.quad_rects_vec[2].top)
                {
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[1], POINT);
                }
                else
                {
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[3], POINT);
                }
            }
        }
    }

}
}
