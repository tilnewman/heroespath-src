// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_COLLISIONQUADTREE_HPP_INCLUDED
#define HEROESPATH_GUI_COLLISIONQUADTREE_HPP_INCLUDED
//
// collision-quad-tree.hpp
//
#include <SFML/Graphics/Rect.hpp>

#include <vector>

namespace heroespath
{
namespace gui
{
    using FloatRectVec_t = std::vector<sf::FloatRect>;
    using IntRectVec_t = std::vector<sf::IntRect>;

    struct Quad
    {
        Quad(const std::size_t RESERVE_COUNT = 32)
            : coll_rects_vec()
            , child_rect_top_left()
            , child_rect_top_right()
            , child_rect_bot_left()
            , child_rect_bot_right()
            , child_quads_vec()
        {
            coll_rects_vec.reserve(RESERVE_COUNT);
            child_quads_vec.reserve(4);
        }

        Quad(const Quad &) = default;
        Quad(Quad &&) = default;
        Quad & operator=(const Quad &) = default;
        Quad & operator=(Quad &&) = default;

        IntRectVec_t coll_rects_vec;

        sf::IntRect child_rect_top_left;
        sf::IntRect child_rect_top_right;
        sf::IntRect child_rect_bot_left;
        sf::IntRect child_rect_bot_right;

        std::vector<Quad> child_quads_vec;
    };

    // Responsible for implementing a collision detection quad-tree.
    class QuadTree
    {
    public:
        QuadTree();

        QuadTree(const QuadTree &) = delete;
        QuadTree(QuadTree &&) = delete;
        QuadTree & operator=(const QuadTree &) = delete;
        QuadTree & operator=(QuadTree &&) = delete;

        void Setup(const sf::FloatRect & TOTAL_RECT, const FloatRectVec_t & COLL_RECTS_VEC);

        bool DoesPointCollide(const sf::Vector2f &) const;
        bool DoesRectCollide(const sf::FloatRect &) const;

        std::size_t GetLastOperationCount() const { return opCount_; }

    private:
        void PopulateQuadAndRecurse(
            Quad & quad, const sf::IntRect & RECT, const IntRectVec_t & COLL_RECTS_VEC);

        bool DoesPointCollideImpl(const Quad &, const sf::Vector2i &) const;
        bool DoesRectCollideImpl(const Quad &, const sf::IntRect &) const;

    private:
        static const int MIN_QUAD_SIZE_;
        Quad headQuad_;
        mutable std::size_t opCount_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_COLLISIONQUADTREE_HPP_INCLUDED
