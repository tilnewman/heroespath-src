// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_VERTEX_HPP_INCLUDED
#define HEROESPATH_SFUTIL_VERTEX_HPP_INCLUDED
//
// vertex.hpp
//
#include "gui/orientation-enum.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <vector>

namespace sf
{

bool operator<(const sf::Vertex & L, const sf::Vertex & R);
bool operator==(const sf::Vertex & L, const sf::Vertex & R);
inline bool operator!=(const sf::Vertex & L, const sf::Vertex & R) { return !(L == R); }

bool operator<(const sf::VertexArray & L, const sf::VertexArray & R);
bool operator==(const sf::VertexArray & L, const sf::VertexArray & R);
inline bool operator!=(const sf::VertexArray & L, const sf::VertexArray & R) { return !(L == R); }

} // namespace sf

namespace heroespath
{
namespace sfutil
{
    constexpr std::size_t VERTS_PER_QUAD = 4;

    // SIZE_LIMIT_V values that are negative, zero, or are greater than the corresponding
    // SIZE_FULL_V value will mean that dimension limit is ignored and the SIZE_FULL_V size will be
    // used instead. This is because having either size restricted to <= 0.0f or restricted to
    // something greater than the full amount does not make any sense.
    const std::vector<sf::Vector2f> MakeQuadVertexPositions(
        const sf::Vector2f & POS_V,
        const sf::Vector2f & SIZE_FULL_V,
        const sf::Vector2f & SIZE_LIMIT_V = sf::Vector2f(0.0f, 0.0f),
        const gui::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END = gui::Orientation::Count);

    // if either SIZE_LIMIT_V is <= 0.0f, or if either TEXTURE_RECT.width/height < 0.0f, then
    // nothing is appended
    void AppendVertexesForQuad(
        sf::VertexArray & vertexArray,
        const sf::Vector2f & POS_V,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR = sf::Color::White,
        const sf::Vector2f & SIZE_LIMIT_V = sf::Vector2f(0.0f, 0.0f),
        const gui::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END = gui::Orientation::Count);

    // if REGION.width or .height is < 0.0f then nothing will be appended
    void AppendVertexesForQuad(
        sf::VertexArray & vertexArray, const sf::FloatRect & REGION, const sf::Color & COLOR);

    // if REGION.width or .height is < 0.0f then nothing will be appended
    void AppendVertexesForQuad(
        std::vector<sf::Vertex> & vertexes, const sf::FloatRect & REGION, const sf::Color & COLOR);

    // If LENGTH is <= 0.0f or if ORIENTATION==Count then nothing is appended.  If
    // OPPOSITE_ORIENTATION_SIZE_LIMIT <= 0.0f or is greater than the corresponding
    // TEXTURE_RECT.width/height then it is ignored.  If ORIENTATION==Both then this function calls
    // itself twice, once for Horiz and then for Vert.  If OPPOSITE_ORIENTATION_SIZE_LIMIT==0.0f
    // then it is ignored.
    void AppendVertexesForQuadRepeatedOverLength(
        sf::VertexArray & vertexArray,
        const sf::Vector2f & POS_V,
        const sf::FloatRect & TEXTURE_RECT,
        const gui::Orientation::Enum ORIENTATION,
        const float LENGTH,
        const sf::Color & COLOR = sf::Color::White,
        const float OPPOSITE_ORIENTATION_SIZE_LIMIT = 0.0f,
        const gui::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END = gui::Orientation::Count);

    void AppendVertexesForQuad(
        std::vector<sf::Vertex> & vertexes,
        const sf::FloatRect & POSITION_RECT,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR = sf::Color::White);

    void SetVertexesForQuad(
        sf::Vertex vertexes[],
        const sf::FloatRect & POSITION_RECT,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR = sf::Color::White);

    void MoveVertexPositions(std::vector<sf::Vertex> & vertexes, const sf::Vector2f & MOVE_V);
    void MoveVertexPositionsForQuad(sf::Vertex vertexes[], const sf::Vector2f & MOVE_V);
    void MoveVertexTextures(std::vector<sf::Vertex> & vertexes, const sf::Vector2f & MOVE_V);
    void MoveVertexTexturesForQuad(sf::Vertex vertexes[], const sf::Vector2f & MOVE_V);

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_VERTEX_HPP_INCLUDED
