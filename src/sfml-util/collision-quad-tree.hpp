// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_COLLISIONQUADTREE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLLISIONQUADTREE_HPP_INCLUDED
//
// collision-quad-tree.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include <vector>

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

    // Responsible for implementing a collision detection quad-tree.
    class QuadTree
    {
    public:
        QuadTree(const QuadTree &) = delete;
        QuadTree(QuadTree &&) = delete;
        QuadTree & operator=(const QuadTree &) = delete;
        QuadTree & operator=(QuadTree &&) = delete;

    public:
        QuadTree() = default;

        void Setup(
            const float MAP_WIDTH, const float MAP_HEIGHT, const FloatRectVec_t & COLL_RECTS_VEC);

        bool IsPointWithinCollisionRect(const sf::Vector2f &) const;

        void Clear();

    private:
        void PopulateQuadAndRecurse(
            Quad & quad, const sf::FloatRect & RECT, const FloatRectVec_t & COLL_RECTS_VEC);

        bool IsPointWithinCollisionRect_Impl(const Quad &, const sf::Vector2f &) const;

    private:
        static const float MIN_QUAD_SIZE_;
        Quad headQuad_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COLLISIONQUADTREE_HPP_INCLUDED
