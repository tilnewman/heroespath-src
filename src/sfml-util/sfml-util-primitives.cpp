// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-primitives.cpp
//
#include "sfml-util-primitives.hpp"

namespace heroespath
{
namespace sfml_util
{

    void DrawRectangleWithLineVerts(
        const sf::FloatRect & RECT, const sf::Color & COLOR, std::vector<sf::Vertex> & verts)
    {
        auto const LEFT{ RECT.left };
        auto const RIGHT{ RECT.left + RECT.width };
        auto const TOP{ RECT.top };
        auto const BOTTOM{ RECT.top + RECT.height };

        verts.emplace_back(sf::Vertex(sf::Vector2f(LEFT, TOP), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(RIGHT, TOP), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(RIGHT, TOP), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(RIGHT, BOTTOM), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(RIGHT, BOTTOM), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(LEFT, BOTTOM), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(LEFT, BOTTOM), COLOR));
        verts.emplace_back(sf::Vertex(sf::Vector2f(LEFT, TOP), COLOR));
    }

    void DrawQuad(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_LEFT,
        const sf::Color & COLOR_RIGHT,
        std::vector<sf::Vertex> & verts)
    {
        auto const LEFT{ RECT.left };
        auto const RIGHT{ RECT.left + RECT.width };
        auto const TOP{ RECT.top };
        auto const BOTTOM{ RECT.top + RECT.height };

        verts.emplace_back(sf::Vertex(sf::Vector2f(LEFT, TOP), COLOR_LEFT));
        verts.emplace_back(sf::Vertex(sf::Vector2f(RIGHT, TOP), COLOR_RIGHT));
        verts.emplace_back(sf::Vertex(sf::Vector2f(RIGHT, BOTTOM), COLOR_RIGHT));
        verts.emplace_back(sf::Vertex(sf::Vector2f(LEFT, BOTTOM), COLOR_LEFT));
    }

} // namespace sfml_util
} // namespace heroespath
