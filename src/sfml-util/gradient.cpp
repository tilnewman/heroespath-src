// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// gradient.cpp
//
#include "gradient.hpp"

namespace heroespath
{
namespace sfml_util
{

    GradientRect::GradientRect()
        : quads_(sf::Quads, 4)
    {}

    GradientRect::GradientRect(const sf::FloatRect & REGION, const GradientInfo & INFO)
        : quads_(sf::Quads, 4)
    {
        Setup(REGION, INFO);
    }

    GradientRect::GradientRect(
        const float POS_LEFT,
        const float POS_TOP,
        const float WIDTH,
        const float HEIGHT,
        const GradientInfo & INFO)
        : quads_(sf::Quads, 4)
    {
        Setup(sf::FloatRect(POS_LEFT, POS_TOP, WIDTH, HEIGHT), INFO);
    }

    GradientRect::~GradientRect() = default;

    void GradientRect::Setup(const sf::FloatRect & REGION, const GradientInfo & INFO)
    {
        quads_[0].position = sf::Vector2f(REGION.left, REGION.top);
        quads_[1].position = sf::Vector2f(REGION.left + REGION.width, REGION.top);
        quads_[2].position = sf::Vector2f(REGION.left + REGION.width, REGION.top + REGION.height);
        quads_[3].position = sf::Vector2f(REGION.left, REGION.top + REGION.height);

        quads_[0].color = INFO.color_from;
        quads_[1].color = INFO.color_from;
        quads_[2].color = INFO.color_from;
        quads_[3].color = INFO.color_from;

        if (INFO.sides & Side::Top)
        {
            quads_[2].color = INFO.color_to;
            quads_[3].color = INFO.color_to;
        }

        if (INFO.sides & Side::Bottom)
        {
            quads_[0].color = INFO.color_to;
            quads_[1].color = INFO.color_to;
        }

        if (INFO.sides & Side::Right)
        {
            quads_[0].color = INFO.color_to;
            quads_[3].color = INFO.color_to;
        }

        if (INFO.sides & Side::Left)
        {
            quads_[1].color = INFO.color_to;
            quads_[2].color = INFO.color_to;
        }

        if (INFO.corners & Corner::TopLeft)
            quads_[2].color = INFO.color_to;

        if (INFO.corners & Corner::TopRight)
            quads_[3].color = INFO.color_to;

        if (INFO.corners & Corner::BottomRight)
            quads_[0].color = INFO.color_to;

        if (INFO.corners & Corner::BottomLeft)
            quads_[1].color = INFO.color_to;
    }

    void GradientRect::SetPosition(const float POS_LEFT, const float POS_TOP)
    {
        const float WIDTH(quads_[1].position.x - quads_[0].position.x);
        const float HEIGHT(quads_[2].position.y - quads_[1].position.y);

        quads_[0].position = sf::Vector2f(POS_LEFT, POS_TOP);
        quads_[1].position = sf::Vector2f(POS_LEFT + WIDTH, POS_TOP);
        quads_[2].position = sf::Vector2f(POS_LEFT + WIDTH, POS_TOP + HEIGHT);
        quads_[3].position = sf::Vector2f(POS_LEFT, POS_TOP + HEIGHT);
    }

    void GradientRect::MovePosition(const float HORIZ, const float VERT)
    {
        quads_[0].position.x += HORIZ;
        quads_[1].position.x += HORIZ;
        quads_[2].position.x += HORIZ;
        quads_[3].position.x += HORIZ;

        quads_[0].position.y += VERT;
        quads_[1].position.y += VERT;
        quads_[2].position.y += VERT;
        quads_[3].position.y += VERT;
    }

    void GradientRect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(quads_, states);
    }
} // namespace sfml_util
} // namespace heroespath
