// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// colored-rect.cpp
//
#include "colored-rect.hpp"

namespace heroespath
{
namespace sfml_util
{

    ColoredRect::ColoredRect(const sf::FloatRect & RECT, const sf::Color & COLOR)
        : quads_(sf::Quads, 4)
    {
        Rect(RECT);
        Color(COLOR);
    }

    const sf::FloatRect ColoredRect::Rect() const
    {
        return sf::FloatRect(
            quads_[0].position,
            sf::Vector2f(
                quads_[2].position.x - quads_[0].position.x,
                quads_[2].position.y - quads_[0].position.y));
    }

    void ColoredRect::Rect(const sf::FloatRect & NEW_RECT)
    {
        quads_[0].position = sf::Vector2f(NEW_RECT.left, NEW_RECT.top);
        quads_[1].position = sf::Vector2f(NEW_RECT.left + NEW_RECT.width, NEW_RECT.top);

        quads_[2].position
            = sf::Vector2f(NEW_RECT.left + NEW_RECT.width, NEW_RECT.top + NEW_RECT.height);

        quads_[3].position = sf::Vector2f(NEW_RECT.left, NEW_RECT.top + NEW_RECT.height);
    }

    const sf::Color ColoredRect::Color() const { return quads_[0].color; }

    void ColoredRect::Color(const sf::Color & NEW_COLOR)
    {
        quads_[0].color = NEW_COLOR;
        quads_[1].color = NEW_COLOR;
        quads_[2].color = NEW_COLOR;
        quads_[3].color = NEW_COLOR;
    }

    void ColoredRect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(quads_, states);
    }

} // namespace sfml_util
} // namespace heroespath
