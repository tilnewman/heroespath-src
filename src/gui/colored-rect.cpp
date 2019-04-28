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

#include "sfutil/scale.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{

    ColoredRect::ColoredRect(const sf::FloatRect & RECT, const sf::Color & COLOR)
        : ColoredRect(RECT, COLOR, COLOR, Side::None, Corner::None)
    {}

    ColoredRect::ColoredRect(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO,
        const Side::Enum SIDES_WITH_FROM_COLOR,
        const Corner::Enum CORNERS_WITH_FROM_COLOR)
        : vertexes_()
        , willDraw_(false)
    {
        Setup(RECT, COLOR_FROM, COLOR_TO, SIDES_WITH_FROM_COLOR, CORNERS_WITH_FROM_COLOR);
    }

    void ColoredRect::Reset()
    {
        vertexes_.clear();
        vertexes_.setPrimitiveType(sf::Quads);
        vertexes_.resize(4);
        willDraw_ = false;
    }

    void ColoredRect::Setup(const sf::FloatRect & RECT, const sf::Color & COLOR)
    {
        Setup(RECT, COLOR, COLOR, Side::None, Corner::None);
    }

    void ColoredRect::Setup(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO,
        const Side::Enum SIDES_WITH_FROM_COLOR,
        const Corner::Enum CORNERS_WITH_FROM_COLOR)
    {
        Reset();
        Rect(RECT);
        Color(COLOR_FROM);

        if (SIDES_WITH_FROM_COLOR & Side::Top)
        {
            vertexes_[2].color = COLOR_TO;
            vertexes_[3].color = COLOR_TO;
        }

        if (SIDES_WITH_FROM_COLOR & Side::Bottom)
        {
            vertexes_[0].color = COLOR_TO;
            vertexes_[1].color = COLOR_TO;
        }

        if (SIDES_WITH_FROM_COLOR & Side::Right)
        {
            vertexes_[0].color = COLOR_TO;
            vertexes_[3].color = COLOR_TO;
        }

        if (SIDES_WITH_FROM_COLOR & Side::Left)
        {
            vertexes_[1].color = COLOR_TO;
            vertexes_[2].color = COLOR_TO;
        }

        if (CORNERS_WITH_FROM_COLOR & Corner::TopLeft)
        {
            vertexes_[2].color = COLOR_TO;
        }

        if (CORNERS_WITH_FROM_COLOR & Corner::TopRight)
        {
            vertexes_[3].color = COLOR_TO;
        }

        if (CORNERS_WITH_FROM_COLOR & Corner::BottomRight)
        {
            vertexes_[0].color = COLOR_TO;
        }

        if (CORNERS_WITH_FROM_COLOR & Corner::BottomLeft)
        {
            vertexes_[1].color = COLOR_TO;
        }

        willDraw_ = true;
    }

    void ColoredRect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (willDraw_)
        {
            target.draw(vertexes_, states);
        }
    }

    bool ColoredRect::IsGradient() const
    {
        return (
            (vertexes_[0].color != vertexes_[1].color) || (vertexes_[0].color != vertexes_[2].color)
            || (vertexes_[0].color != vertexes_[3].color));
    }

    const sf::Color ColoredRect::Color() const { return vertexes_[0].color; }

    void ColoredRect::Color(const sf::Color & NEW_COLOR)
    {
        vertexes_[0].color = NEW_COLOR;
        vertexes_[1].color = NEW_COLOR;
        vertexes_[2].color = NEW_COLOR;
        vertexes_[3].color = NEW_COLOR;
    }

    const sf::FloatRect ColoredRect::Rect() const
    {
        return sf::FloatRect(
            vertexes_[0].position,
            sf::Vector2f(
                vertexes_[2].position.x - vertexes_[0].position.x,
                vertexes_[2].position.y - vertexes_[0].position.y));
    }

    void ColoredRect::Rect(const sf::FloatRect & RECT_PARAM)
    {
        const auto RECT_TO_USE = [&]() {
            if (sfutil::IsZeroOrLessAny(sfutil::Size(RECT_PARAM)))
            {
                return sf::FloatRect(sfutil::Position(RECT_PARAM), sf::Vector2f(0.0f, 0.0f));
            }
            else
            {
                return RECT_PARAM;
            }
        }();

        vertexes_[0].position = sf::Vector2f(RECT_TO_USE.left, RECT_TO_USE.top);
        vertexes_[1].position = sf::Vector2f(RECT_TO_USE.left + RECT_TO_USE.width, RECT_TO_USE.top);

        vertexes_[2].position = sf::Vector2f(
            RECT_TO_USE.left + RECT_TO_USE.width, RECT_TO_USE.top + RECT_TO_USE.height);

        vertexes_[3].position
            = sf::Vector2f(RECT_TO_USE.left, RECT_TO_USE.top + RECT_TO_USE.height);
    }

    void ColoredRect::SetPos(const float POS_LEFT, const float POS_TOP)
    {
        MovePos(POS_LEFT - vertexes_[0].position.x, POS_TOP - vertexes_[0].position.y);
    }

    void ColoredRect::MovePos(const float HORIZ, const float VERT)
    {
        const sf::Vector2f MOVE_V(HORIZ, VERT);

        vertexes_[0].position += MOVE_V;
        vertexes_[1].position += MOVE_V;
        vertexes_[2].position += MOVE_V;
        vertexes_[3].position += MOVE_V;
    }

    bool operator<(const ColoredRect & L, const ColoredRect & R)
    {
        return (L.vertexes_ < R.vertexes_);
    }

    bool operator==(const ColoredRect & L, const ColoredRect & R)
    {
        return (L.vertexes_ == R.vertexes_);
    }

} // namespace gui
} // namespace heroespath
