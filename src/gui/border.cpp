// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// border.cpp
//
#include "border.hpp"
#include "sfutil/position.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{

    Border::Border(
        const sf::FloatRect & REGION,
        const float LINE_THICKNESS,
        const sf::Color & LINE_COLOR,
        const sf::Color & PAD_LINE_COLOR_ADJ,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
        : rectangleShapesOpt_()
        , goldBarOpt_()
        , regionOuter_()
        , regionInner_()
        , willDraw_(false)
    {
        Setup(
            REGION,
            LINE_THICKNESS,
            LINE_COLOR,
            PAD_LINE_COLOR_ADJ,
            WILL_GROW_BORDER_TO_CONTAIN_REGION);
    }

    Border::Border(const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
        : rectangleShapesOpt_()
        , goldBarOpt_()
        , regionOuter_()
        , regionInner_()
        , willDraw_(false)
    {
        Setup(REGION, WILL_GROW_BORDER_TO_CONTAIN_REGION);
    }

    void Border::Reset()
    {
        rectangleShapesOpt_ = boost::none;
        goldBarOpt_ = boost::none;
        regionOuter_ = sf::FloatRect();
        regionInner_ = sf::FloatRect();
        willDraw_ = false;
    }

    void Border::Setup(
        const sf::FloatRect & REGION,
        const float LINE_THICKNESS,
        const sf::Color & LINE_COLOR,
        const sf::Color & PAD_LINE_COLOR_ADJ,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        Reset();

        if (sfutil::IsSizeZeroOrLessEither(REGION))
        {
            return;
        }

        regionOuter_ = REGION;
        regionInner_ = REGION;

        std::vector<sf::RectangleShape> rsVec;

        rsVec.emplace_back(sfutil::MakeRectangleHollow(
            REGION, LINE_COLOR, LINE_THICKNESS, WILL_GROW_BORDER_TO_CONTAIN_REGION));

        if (PAD_LINE_COLOR_ADJ != sf::Color::Transparent)
        {
            const auto PAD_LINE_COLOR { LINE_COLOR - PAD_LINE_COLOR_ADJ };
            const sf::Vector2f LINE_THICKNESS_V(LINE_THICKNESS, LINE_THICKNESS);

            regionOuter_ = sf::FloatRect(
                sfutil::Position(REGION) - LINE_THICKNESS_V,
                sfutil::Size(REGION) + (LINE_THICKNESS_V * 2.0f));

            rsVec.emplace_back(sfutil::MakeRectangleHollow(
                regionOuter_, PAD_LINE_COLOR, LINE_THICKNESS, WILL_GROW_BORDER_TO_CONTAIN_REGION));

            regionInner_ = sf::FloatRect(
                sfutil::Position(REGION) + LINE_THICKNESS_V,
                sfutil::Size(REGION) - (LINE_THICKNESS_V * 2.0f));

            rsVec.emplace_back(sfutil::MakeRectangleHollow(
                regionInner_, PAD_LINE_COLOR, LINE_THICKNESS, WILL_GROW_BORDER_TO_CONTAIN_REGION));
        }

        rectangleShapesOpt_ = rsVec;
        willDraw_ = true;
    }

    void Border::Setup(const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        Reset();

        if (sfutil::IsSizeZeroOrLessEither(REGION))
        {
            return;
        }

        goldBarOpt_ = GoldBar(
            REGION,
            ((REGION.width < REGION.height) ? Orientation::Vert : Orientation::Horiz),
            WILL_GROW_BORDER_TO_CONTAIN_REGION);

        regionOuter_ = goldBarOpt_->OuterRegion();
        regionInner_ = goldBarOpt_->InnerRegion();
        willDraw_ = true;
    }

    void Border::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (willDraw_)
        {
            if (rectangleShapesOpt_)
            {
                for (const auto & RECTANGLE_SHAPE : rectangleShapesOpt_.value())
                {
                    target.draw(RECTANGLE_SHAPE, states);
                }
            }
            else if (goldBarOpt_)
            {
                target.draw(goldBarOpt_.value(), states);
            }
        }
    }

    const sf::Color Border::Color() const
    {
        if (rectangleShapesOpt_)
        {
            return rectangleShapesOpt_->front().getOutlineColor();
        }
        else if (goldBarOpt_)
        {
            return goldBarOpt_->Color();
        }
        else
        {
            return sf::Color::Transparent;
        }
    }

    void Border::Color(const sf::Color & NEW_COLOR)
    {
        if (rectangleShapesOpt_)
        {
            sf::Color padColor { rectangleShapesOpt_->front().getOutlineColor() };

            if (rectangleShapesOpt_->size() > 1)
            {
                padColor -= rectangleShapesOpt_->back().getOutlineColor();
                padColor = NEW_COLOR - padColor;
            }

            rectangleShapesOpt_->front().setOutlineColor(NEW_COLOR);

            if (rectangleShapesOpt_->size() > 2)
            {
                rectangleShapesOpt_.value()[1].setOutlineColor(padColor);
                rectangleShapesOpt_.value()[2].setOutlineColor(padColor);
            }
        }
        else if (goldBarOpt_)
        {
            goldBarOpt_->Color(NEW_COLOR);
        }
    }

    void Border::SetPos(const float POS_LEFT, const float POS_TOP)
    {
        const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - sfutil::Position(OuterRegion()) };

        MovePos(DIFF_V.x, DIFF_V.y);
    }

    void Border::MovePos(const float HORIZ, const float VERT)
    {
        const auto DIFF_V { sf::Vector2f(HORIZ, VERT) };

        if (rectangleShapesOpt_)
        {
            for (auto & rectangleShape : rectangleShapesOpt_.value())
            {
                rectangleShape.move(DIFF_V);
            }
        }
        else if (goldBarOpt_)
        {
            goldBarOpt_->MovePos(DIFF_V.x, DIFF_V.y);
        }
    }

    bool Border::WasGrown() const
    {
        if (goldBarOpt_)
        {
            return goldBarOpt_->WasBorderGrown();
        }
        else
        {
            return false;
        }
    }

    const sf::Color Border::LineColor() const
    {
        if (rectangleShapesOpt_)
        {
            return rectangleShapesOpt_->front().getOutlineColor();
        }
        else
        {
            return sf::Color::Transparent;
        }
    }

    const sf::Color Border::PadLineColorAdj() const
    {
        if ((rectangleShapesOpt_) && (rectangleShapesOpt_->size() > 1))
        {
            sf::Color padColor { rectangleShapesOpt_->front().getOutlineColor() };
            padColor -= rectangleShapesOpt_->back().getOutlineColor();
            return padColor;
        }
        else
        {
            return sf::Color::Transparent;
        }
    }

    float Border::LineThickness() const
    {
        if (rectangleShapesOpt_)
        {
            return rectangleShapesOpt_->front().getOutlineThickness();
        }
        else
        {
            return 0.0f;
        }
    }

} // namespace gui
} // namespace heroespath
