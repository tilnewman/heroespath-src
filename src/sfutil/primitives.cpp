// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// primitives.cpp
//
#include "primitives.hpp"

#include "sfutil/common.hpp"
#include "sfutil/scale.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace sfutil
{

    const sf::RectangleShape MakeRectangle(
        const sf::FloatRect & REGION,
        const sf::Color & LINE_COLOR,
        const float LINE_THICKNESS,
        const sf::Color & FILL_COLOR,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        sf::RectangleShape rs;

        // RectangleShape's thickness starts at the given region and grows outward
        if (WILL_GROW_BORDER_TO_CONTAIN_REGION)
        {
            rs.setPosition(Position(REGION));
            rs.setSize(Size(REGION));
        }
        else
        {
            const sf::Vector2f THICK_V(LINE_THICKNESS, LINE_THICKNESS);
            rs.setPosition(Position(REGION) + THICK_V);
            rs.setSize(Size(REGION) - (THICK_V * 2.0f));
        }

        rs.setFillColor(FILL_COLOR);
        rs.setOutlineColor(LINE_COLOR);
        rs.setOutlineThickness(LINE_THICKNESS);

        return rs;
    }

    const sf::RectangleShape MakeRectangleSolid(
        const sf::FloatRect & REGION,
        const sf::Color & COLOR,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        return MakeRectangle(REGION, COLOR, 0.0f, COLOR, WILL_GROW_BORDER_TO_CONTAIN_REGION);
    }

    const sf::RectangleShape MakeRectangleHollow(
        const sf::FloatRect & REGION,
        const sf::Color & COLOR,
        const float LINE_THICKNESS,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        return MakeRectangle(
            REGION,
            COLOR,
            LINE_THICKNESS,
            sf::Color::Transparent,
            WILL_GROW_BORDER_TO_CONTAIN_REGION);
    }

} // namespace sfutil
} // namespace heroespath
