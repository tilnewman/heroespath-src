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

#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"
#include "sfutil/sprite-texture.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/VertexArray.hpp>

#include <iostream>
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

bool operator!=(const sf::Vertex & L, const sf::Vertex & R) { return !(L == R); }

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

bool operator!=(const sf::VertexArray & L, const sf::VertexArray & R) { return !(L == R); }
} // namespace sf

namespace heroespath
{
namespace sfutil
{

    const std::vector<sf::Vector2f> MakeQuadVertexPositions(
        const sf::Vector2f & POS_V,
        const sf::Vector2f & SIZE_FULL_V,
        const sf::Vector2f & SIZE_LIMIT_V,
        const sfml_util::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END)
    {
        std::vector<sf::Vector2f> v;

        if ((SIZE_FULL_V.x < 0.0f) || (SIZE_FULL_V.y < 0.0f) || (SIZE_LIMIT_V.x < 0.0f)
            || (SIZE_LIMIT_V.y < 0.0f))
        {
            return v;
        }

        v.reserve(4);

        const auto SIZE_ACTUAL_V { sf::Vector2f(
            ((SIZE_LIMIT_V.x > 0.0f) ? std::min(SIZE_LIMIT_V.x, SIZE_FULL_V.x) : SIZE_FULL_V.x),
            ((SIZE_LIMIT_V.y > 0.0f) ? std::min(SIZE_LIMIT_V.y, SIZE_FULL_V.y) : SIZE_FULL_V.y)) };

        if (ORIENTATIONS_TO_DRAW_FROM_END == sfml_util::Orientation::Count)
        {
            v.emplace_back(POS_V);
            v.emplace_back(POS_V.x + SIZE_ACTUAL_V.x, POS_V.y);
            v.emplace_back(POS_V + SIZE_ACTUAL_V);
            v.emplace_back(POS_V.x, POS_V.y + SIZE_ACTUAL_V.y);
        }
        else if (ORIENTATIONS_TO_DRAW_FROM_END == sfml_util::Orientation::Both)
        {
            const auto BOT_RIGHT_POS_V { POS_V + SIZE_FULL_V };
            const auto TOP_LEFT_POS_V { BOT_RIGHT_POS_V - SIZE_ACTUAL_V };
            const auto TOP_RIGHT_POS_V { BOT_RIGHT_POS_V - sf::Vector2f(0.0f, SIZE_ACTUAL_V.y) };
            const auto BOT_LEFT_POS_V { BOT_RIGHT_POS_V - sf::Vector2f(SIZE_ACTUAL_V.x, 0.0f) };

            v.emplace_back(TOP_LEFT_POS_V);
            v.emplace_back(TOP_RIGHT_POS_V);
            v.emplace_back(BOT_RIGHT_POS_V);
            v.emplace_back(BOT_LEFT_POS_V);
        }
        else if (ORIENTATIONS_TO_DRAW_FROM_END == sfml_util::Orientation::Horiz)
        {
            v.emplace_back(POS_V.x + (SIZE_FULL_V.x - SIZE_ACTUAL_V.x), POS_V.y);
            v.emplace_back(POS_V.x + SIZE_FULL_V.x, POS_V.y);
            v.emplace_back(POS_V.x + SIZE_FULL_V.x, POS_V.y + SIZE_ACTUAL_V.y);
            v.emplace_back(POS_V.x + (SIZE_FULL_V.x - SIZE_ACTUAL_V.x), POS_V.y + SIZE_ACTUAL_V.y);
        }
        else
        {
            v.emplace_back(POS_V.x, POS_V.y + (SIZE_FULL_V.y - SIZE_ACTUAL_V.y));
            v.emplace_back(POS_V.x + SIZE_ACTUAL_V.x, POS_V.y + (SIZE_FULL_V.y - SIZE_ACTUAL_V.y));
            v.emplace_back(POS_V.x + SIZE_ACTUAL_V.x, POS_V.y + SIZE_FULL_V.y);
            v.emplace_back(POS_V.x, POS_V.y + SIZE_FULL_V.y);
        }

        return v;
    }

    void AppendVertexesForQuad(
        sf::VertexArray & vertexArray,
        const sf::Vector2f & POS_V,
        const sf::FloatRect & TEXTURE_RECT,
        const sf::Color & COLOR,
        const sf::Vector2f & SIZE_LIMIT_V,
        const sfml_util::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END)
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

        const std::size_t COUNT_EXPECTED(4);

        if ((VERTEX_POS_V_VEC.size() == COUNT_EXPECTED)
            && (TEXTURE_POS_V_VEC.size() == COUNT_EXPECTED))
        {
            for (std::size_t i(0); i < COUNT_EXPECTED; ++i)
            {
                vertexArray.append(sf::Vertex(VERTEX_POS_V_VEC[i], COLOR, TEXTURE_POS_V_VEC[i]));
            }
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
        const sfml_util::Orientation::Enum ORIENTATION,
        const float LENGTH,
        const sf::Color & COLOR,
        const float OPPOSITE_ORIENTATION_SIZE_LIMIT,
        const sfml_util::Orientation::Enum ORIENTATIONS_TO_DRAW_FROM_END)
    {
        if (((LENGTH > 0.0f) == false) || (ORIENTATION == sfml_util::Orientation::Count))
        {
            return;
        }
        else if (ORIENTATION == sfml_util::Orientation::Both)
        {
            AppendVertexesForQuadRepeatedOverLength(
                vertexArray,
                POS_V,
                TEXTURE_RECT,
                sfml_util::Orientation::Horiz,
                LENGTH,
                COLOR,
                OPPOSITE_ORIENTATION_SIZE_LIMIT,
                ORIENTATIONS_TO_DRAW_FROM_END);

            AppendVertexesForQuadRepeatedOverLength(
                vertexArray,
                POS_V,
                TEXTURE_RECT,
                sfml_util::Orientation::Vert,
                LENGTH,
                COLOR,
                OPPOSITE_ORIENTATION_SIZE_LIMIT,
                ORIENTATIONS_TO_DRAW_FROM_END);
        }
        else
        {
            float pos { ((ORIENTATION == sfml_util::Orientation::Horiz) ? POS_V.x : POS_V.y) };

            const auto POS_LIMIT { pos + LENGTH };

            const auto TEXTURE_LENGTH_OF_ORIENTATION { (
                (ORIENTATION == sfml_util::Orientation::Horiz) ? TEXTURE_RECT.width
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

} // namespace sfutil
} // namespace heroespath
