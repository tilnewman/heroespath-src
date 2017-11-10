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
#ifndef HEROESPATH_SFMLUTIL_COLLISIONQUADTREE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLLISIONQUADTREE_HPP_INCLUDED
//
// collision-quad-tree.hpp
//
#include "sfml-util/sfml-graphics.hpp"


namespace heroespath
{
namespace sfml_util
{

    struct Quad
    {
        FloatRectVec_t coll_rects_vec;
        FloatRectVec_t quad_rects_vec;
        std::vector<Quad> child_quads_vec;
    };


    //Responsible for implementing a collision detection quad-tree.
    class QuadTree
    {
        QuadTree(const QuadTree &) = delete;
        QuadTree & operator=(const QuadTree &) = delete;

    public:
        QuadTree() = default;

        void Setup(
            const float MAP_WIDTH,
            const float MAP_HEIGHT,
            const FloatRectVec_t & COLL_RECTS_VEC);

        bool IsPointWithinCollisionRect(const float POS_LEFT, const float POS_TOP) const;

    private:
        void PopulateQuadAndRecurse(
            Quad & quad,
            const sf::FloatRect & RECT,
            const FloatRectVec_t & COLL_RECTS_VEC);

        bool IsPointWithinCollisionRect_Impl(
            const Quad & QUAD,
            const float POS_LEFT,
            const float POS_TOP) const;

    private:
        static const float MIN_QUAD_SIZE_;
        Quad headQuad_;
    };

}
}

#endif //HEROESPATH_SFMLUTIL_COLLISIONQUADTREE_HPP_INCLUDED
