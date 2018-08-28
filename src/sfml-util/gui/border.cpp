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
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-primitives.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace sfml_util
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
            , regionOuter_(REGION)
            , regionInner_(REGION)
        {
            if (IsSizeZeroOrLessEither(REGION))
            {
                return;
            }

            std::vector<sf::RectangleShape> rsVec;

            rsVec.emplace_back(MakeRectangleHollow(
                REGION, LINE_COLOR, LINE_THICKNESS, WILL_GROW_BORDER_TO_CONTAIN_REGION));

            if (PAD_LINE_COLOR_ADJ != defaults::None)
            {
                const auto PAD_LINE_COLOR { LINE_COLOR - PAD_LINE_COLOR_ADJ };
                const sf::Vector2f LINE_THICKNESS_V(LINE_THICKNESS, LINE_THICKNESS);

                const auto OUTER_REGION { sf::FloatRect(
                    Position(REGION) - LINE_THICKNESS_V,
                    Size(REGION) + (LINE_THICKNESS_V * 2.0f)) };

                rsVec.emplace_back(MakeRectangleHollow(
                    OUTER_REGION,
                    PAD_LINE_COLOR,
                    LINE_THICKNESS,
                    WILL_GROW_BORDER_TO_CONTAIN_REGION));

                const auto INNER_REGION { sf::FloatRect(
                    Position(REGION) + LINE_THICKNESS_V,
                    Size(REGION) - (LINE_THICKNESS_V * 2.0f)) };

                rsVec.emplace_back(MakeRectangleHollow(
                    INNER_REGION,
                    PAD_LINE_COLOR,
                    LINE_THICKNESS,
                    WILL_GROW_BORDER_TO_CONTAIN_REGION));
            }

            rectangleShapesOpt_ = rsVec;
            SetupOuterRegion();
            SetupInnerRegion();
        }

        Border::Border(const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
            : rectangleShapesOpt_()
            , goldBarOpt_()
            , regionOuter_(REGION)
            , regionInner_(REGION)
        {
            if (IsSizeZeroOrLessEither(REGION))
            {
                return;
            }

            goldBarOpt_ = GoldBar(
                REGION,
                ((REGION.width < REGION.height) ? Orientation::Vert : Orientation::Horiz),
                WILL_GROW_BORDER_TO_CONTAIN_REGION);

            SetupOuterRegion();
            SetupInnerRegion();
        }

        void Border::draw(sf::RenderTarget & target, sf::RenderStates states) const
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
                return defaults::None;
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
            const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP)
                                - sfml_util::Position(OuterRegion()) };

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
                return defaults::None;
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
                return defaults::None;
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

        void Border::SetupOuterRegion()
        {
            if (rectangleShapesOpt_)
            {
                if (rectangleShapesOpt_->size() > 1)
                {
                    regionOuter_ = rectangleShapesOpt_.value()[1].getGlobalBounds();
                }
                else
                {
                    regionOuter_ = rectangleShapesOpt_->front().getGlobalBounds();
                }
            }
            else if (goldBarOpt_)
            {
                regionOuter_ = goldBarOpt_->OuterRegion();
            }
            else
            {
                regionOuter_ = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        void Border::SetupInnerRegion()
        {
            if (rectangleShapesOpt_)
            {
                if (rectangleShapesOpt_->size() > 2)
                {
                    regionInner_ = rectangleShapesOpt_.value()[2].getGlobalBounds();
                }
                else
                {
                    regionInner_ = rectangleShapesOpt_->front().getGlobalBounds();
                }
            }
            else if (goldBarOpt_)
            {
                regionInner_ = goldBarOpt_->InnerRegion();
            }
            else
            {
                regionInner_ = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
