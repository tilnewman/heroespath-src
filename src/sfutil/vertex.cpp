// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// vertex.cpp
//
#include "vertex.hpp"

#include "sfutil/color.hpp"
#include "sfutil/common.hpp"
#include "sfutil/sprite.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/VertexArray.hpp>

#include <tuple>

namespace sf
{

bool operator<(const sf::Vertex & L, const sf::Vertex & R)
{
    return std::tie(L.position, L.color, L.texCoords) < std::tie(R.position, R.color, R.texCoords);
}

bool operator==(const sf::Vertex & L, const sf::Vertex & R)
{
    return std::tie(L.position, L.color, L.texCoords) == std::tie(R.position, R.color, R.texCoords);
}

bool operator<(const sf::VertexArray & L, const sf::VertexArray & R)
{
    if (L.getPrimitiveType() != R.getPrimitiveType())
    {
        return (L.getPrimitiveType() < R.getPrimitiveType());
    }

    const auto VERTEX_COUNT_LEFT { L.getVertexCount() };
    const auto VERTEX_COUNT_RIGHT { R.getVertexCount() };

    if (VERTEX_COUNT_LEFT != VERTEX_COUNT_RIGHT)
    {
        return (VERTEX_COUNT_LEFT < VERTEX_COUNT_RIGHT);
    }
    else if (VERTEX_COUNT_LEFT == 0)
    {
        return false;
    }
    else
    {
        for (std::size_t i(0); i < VERTEX_COUNT_LEFT; ++i)
        {
            if (L[i] != R[i])
            {
                return (L[i] < R[i]);
            }
        }

        return false;
    }
}

bool operator==(const sf::VertexArray & L, const sf::VertexArray & R)
{
    if (L.getPrimitiveType() != R.getPrimitiveType())
    {
        return false;
    }

    const auto VERTEX_COUNT_LEFT { L.getVertexCount() };
    const auto VERTEX_COUNT_RIGHT { R.getVertexCount() };

    if (VERTEX_COUNT_LEFT != VERTEX_COUNT_RIGHT)
    {
        return false;
    }
    else if (VERTEX_COUNT_LEFT == 0)
    {
        return true;
    }
    else
    {
        for (std::size_t i(0); i < VERTEX_COUNT_LEFT; ++i)
        {
            if (L[i] != R[i])
            {
                return false;
            }
        }

        return true;
    }
}

} // namespace sf

namespace heroespath
{
namespace sfutil
{

    const std::vector<sf::Vector2f> MakeQuadVertexPositions(
        const sf::Vector2f & POS_V,
        const sf::Vector2f & SIZE_FULL_V,
        const sf::Vector2f & SIZE_LIMIT_V,
        const gui::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END)
    {
        std::vector<sf::Vector2f> positions;

        if ((SIZE_FULL_V.x < 0.0f) || (SIZE_FULL_V.y < 0.0f) || (SIZE_LIMIT_V.x < 0.0f)
            || (SIZE_LIMIT_V.y < 0.0f))
        {
            return positions;
        }

        positions.resize(VERTS_PER_QUAD);

        const auto SIZE_ACTUAL_V { sf::Vector2f(
            ((SIZE_LIMIT_V.x > 0.0f) ? misc::Min(SIZE_LIMIT_V.x, SIZE_FULL_V.x) : SIZE_FULL_V.x),
            ((SIZE_LIMIT_V.y > 0.0f) ? misc::Min(SIZE_LIMIT_V.y, SIZE_FULL_V.y) : SIZE_FULL_V.y)) };

        if (ORIENTATIONS_TO_DRAW_FROM_END == gui::Orientation::Count)
        {
            positions[0] = POS_V;
            positions[1] = sf::Vector2f(POS_V.x + SIZE_ACTUAL_V.x, POS_V.y);
            positions[2] = (POS_V + SIZE_ACTUAL_V);
            positions[3] = sf::Vector2f(POS_V.x, POS_V.y + SIZE_ACTUAL_V.y);
        }
        else if (ORIENTATIONS_TO_DRAW_FROM_END == gui::Orientation::Both)
        {
            const auto BOT_RIGHT_POS_V { POS_V + SIZE_FULL_V };
            const auto TOP_LEFT_POS_V { BOT_RIGHT_POS_V - SIZE_ACTUAL_V };
            const auto TOP_RIGHT_POS_V { BOT_RIGHT_POS_V - sf::Vector2f(0.0f, SIZE_ACTUAL_V.y) };
            const auto BOT_LEFT_POS_V { BOT_RIGHT_POS_V - sf::Vector2f(SIZE_ACTUAL_V.x, 0.0f) };

            positions[0] = TOP_LEFT_POS_V;
            positions[1] = TOP_RIGHT_POS_V;
            positions[2] = BOT_RIGHT_POS_V;
            positions[3] = BOT_LEFT_POS_V;
        }
        else if (ORIENTATIONS_TO_DRAW_FROM_END == gui::Orientation::Horiz)
        {
            positions[0] = sf::Vector2f(POS_V.x + (SIZE_FULL_V.x - SIZE_ACTUAL_V.x), POS_V.y);
            positions[1] = sf::Vector2f(POS_V.x + SIZE_FULL_V.x, POS_V.y);
            positions[2] = sf::Vector2f(POS_V.x + SIZE_FULL_V.x, POS_V.y + SIZE_ACTUAL_V.y);

            positions[3] = sf::Vector2f(
                POS_V.x + (SIZE_FULL_V.x - SIZE_ACTUAL_V.x), POS_V.y + SIZE_ACTUAL_V.y);
        }
        else
        {
            positions[0] = sf::Vector2f(POS_V.x, POS_V.y + (SIZE_FULL_V.y - SIZE_ACTUAL_V.y));

            positions[1] = sf::Vector2f(
                POS_V.x + SIZE_ACTUAL_V.x, POS_V.y + (SIZE_FULL_V.y - SIZE_ACTUAL_V.y));

            positions[2] = sf::Vector2f(POS_V.x + SIZE_ACTUAL_V.x, POS_V.y + SIZE_FULL_V.y);
            positions[3] = sf::Vector2f(POS_V.x, POS_V.y + SIZE_FULL_V.y);
        }

        return positions;
    }

    void AppendVertexesForQuad(
        sf::VertexArray & vertexArray,
        const sf::Vector2f & POS_V,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR,
        const sf::Vector2f & SIZE_LIMIT_V,
        const gui::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END)
    {
        if ((SIZE_LIMIT_V.x < 0.0f) || (SIZE_LIMIT_V.y < 0.0f) || (TEXTURE_RECT.width < 0.0f)
            || (TEXTURE_RECT.height < 0.0f))
        {
            return;
        }

        const auto VERTEX_POS_V_VEC { MakeQuadVertexPositions(
            POS_V, sfutil::Size(TEXTURE_RECT), SIZE_LIMIT_V) };

        const auto TEXTURE_POS_V_VEC { MakeQuadVertexPositions(
            Position(TEXTURE_RECT),
            sfutil::Size(TEXTURE_RECT),
            SIZE_LIMIT_V,
            ORIENTATIONS_TO_DRAW_FROM_END) };

        if ((VERTEX_POS_V_VEC.size() == VERTS_PER_QUAD)
            && (TEXTURE_POS_V_VEC.size() == VERTS_PER_QUAD))
        {
            vertexArray.append(sf::Vertex(VERTEX_POS_V_VEC[0], COLOR, TEXTURE_POS_V_VEC[0]));
            vertexArray.append(sf::Vertex(VERTEX_POS_V_VEC[1], COLOR, TEXTURE_POS_V_VEC[1]));
            vertexArray.append(sf::Vertex(VERTEX_POS_V_VEC[2], COLOR, TEXTURE_POS_V_VEC[2]));
            vertexArray.append(sf::Vertex(VERTEX_POS_V_VEC[3], COLOR, TEXTURE_POS_V_VEC[3]));
        }
    }

    void AppendVertexesForQuad(
        sf::VertexArray & vertexArray, const sf::FloatRect & REGION, const sf::Color & COLOR)
    {
        for (const auto & POS_V : MakeQuadVertexPositions(Position(REGION), Size(REGION)))
        {
            vertexArray.append(sf::Vertex(POS_V, COLOR));
        }
    }

    void AppendVertexesForQuad(
        std::vector<sf::Vertex> & vertexes, const sf::FloatRect & REGION, const sf::Color & COLOR)
    {
        for (const auto & POS_V : MakeQuadVertexPositions(Position(REGION), Size(REGION)))
        {
            vertexes.emplace_back(sf::Vertex(POS_V, COLOR));
        }
    }

    void AppendVertexesForQuadRepeatedOverLength(
        sf::VertexArray & vertexArray,
        const sf::Vector2f & POS_V,
        const sf::FloatRect & TEXTURE_RECT,
        const gui::Orientation::Enum ORIENTATION,
        const float LENGTH,
        const sf::Color & COLOR,
        const float OPPOSITE_ORIENTATION_SIZE_LIMIT,
        const gui::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END)
    {
        if (((LENGTH > 0.0f) == false) || (ORIENTATION == gui::Orientation::Count))
        {
            return;
        }
        else if (ORIENTATION == gui::Orientation::Both)
        {
            AppendVertexesForQuadRepeatedOverLength(
                vertexArray,
                POS_V,
                TEXTURE_RECT,
                gui::Orientation::Horiz,
                LENGTH,
                COLOR,
                OPPOSITE_ORIENTATION_SIZE_LIMIT,
                ORIENTATIONS_TO_DRAW_FROM_END);

            AppendVertexesForQuadRepeatedOverLength(
                vertexArray,
                POS_V,
                TEXTURE_RECT,
                gui::Orientation::Vert,
                LENGTH,
                COLOR,
                OPPOSITE_ORIENTATION_SIZE_LIMIT,
                ORIENTATIONS_TO_DRAW_FROM_END);
        }
        else
        {
            float pos { ((ORIENTATION == gui::Orientation::Horiz) ? POS_V.x : POS_V.y) };

            const auto POS_LIMIT { pos + LENGTH };

            const auto TEXTURE_LENGTH_OF_ORIENTATION { (
                (ORIENTATION == gui::Orientation::Horiz) ? TEXTURE_RECT.width
                                                         : TEXTURE_RECT.height) };

            if (TEXTURE_LENGTH_OF_ORIENTATION < LENGTH)
            {
                while ((pos + TEXTURE_LENGTH_OF_ORIENTATION) < POS_LIMIT)
                {
                    AppendVertexesForQuad(
                        vertexArray,
                        ChangeVectorCopy(POS_V, ORIENTATION, pos),
                        TEXTURE_RECT,
                        COLOR,
                        CreateVector(ORIENTATION, 0.0f, OPPOSITE_ORIENTATION_SIZE_LIMIT),
                        ORIENTATIONS_TO_DRAW_FROM_END);

                    pos += TEXTURE_LENGTH_OF_ORIENTATION;
                }
            }

            const auto REMAINING_LENGTH { (POS_LIMIT - pos) };

            if (REMAINING_LENGTH > 0.0f)
            {
                AppendVertexesForQuad(
                    vertexArray,
                    ChangeVectorCopy(POS_V, ORIENTATION, pos),
                    TEXTURE_RECT,
                    COLOR,
                    CreateVector(ORIENTATION, REMAINING_LENGTH, OPPOSITE_ORIENTATION_SIZE_LIMIT),
                    ORIENTATIONS_TO_DRAW_FROM_END);
            }
        }
    }

    void AppendVertexesForQuad(
        std::vector<sf::Vertex> & vertexes,
        const sf::FloatRect & POSITION_RECT,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR)
    {
        const auto FIRST_VERT_INDEX { vertexes.size() };
        vertexes.resize(FIRST_VERT_INDEX + VERTS_PER_QUAD);
        SetVertexesForQuad(&vertexes[FIRST_VERT_INDEX], POSITION_RECT, TEXTURE_RECT, COLOR);
    }

    void SetVertexesForQuad(
        sf::Vertex vertexes[],
        const sf::FloatRect & POSITION_RECT,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR)
    {
        vertexes[0]
            = sf::Vertex(sfutil::TopLeft(POSITION_RECT), COLOR, sfutil::TopLeft(TEXTURE_RECT));

        vertexes[1]
            = sf::Vertex(sfutil::TopRight(POSITION_RECT), COLOR, sfutil::TopRight(TEXTURE_RECT));

        vertexes[2] = sf::Vertex(
            sfutil::BottomRight(POSITION_RECT), COLOR, sfutil::BottomRight(TEXTURE_RECT));

        vertexes[3] = sf::Vertex(
            sfutil::BottomLeft(POSITION_RECT), COLOR, sfutil::BottomLeft(TEXTURE_RECT));
    }

    void MoveVertexPositions(std::vector<sf::Vertex> & vertexes, const sf::Vector2f & MOVE_V)
    {
        const auto VERT_COUNT { vertexes.size() };
        for (std::size_t index(0); index < VERT_COUNT; ++index)
        {
            vertexes[index].position += MOVE_V;
        }
    }

    void MoveVertexPositionsForQuad(sf::Vertex vertexes[], const sf::Vector2f & MOVE_V)
    {
        vertexes[0].position += MOVE_V;
        vertexes[1].position += MOVE_V;
        vertexes[2].position += MOVE_V;
        vertexes[3].position += MOVE_V;
    }

    void MoveVertexTextures(std::vector<sf::Vertex> & vertexes, const sf::Vector2f & MOVE_V)
    {
        const auto VERT_COUNT { vertexes.size() };
        for (std::size_t index(0); index < VERT_COUNT; ++index)
        {
            vertexes[index].texCoords += MOVE_V;
        }
    }

    void MoveVertexTexturesForQuad(sf::Vertex vertexes[], const sf::Vector2f & MOVE_V)
    {
        vertexes[0].texCoords += MOVE_V;
        vertexes[1].texCoords += MOVE_V;
        vertexes[2].texCoords += MOVE_V;
        vertexes[3].texCoords += MOVE_V;
    }

} // namespace sfutil
} // namespace heroespath
