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
        }

        Border::Border(const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
            : rectangleShapesOpt_()
            , goldBarOpt_()
        {
            if (IsSizeZeroOrLessEither(REGION))
            {
                return;
            }

            goldBarOpt_ = GoldBar(
                REGION,
                ((REGION.width < REGION.height) ? Orientation::Vert : Orientation::Horiz),
                WILL_GROW_BORDER_TO_CONTAIN_REGION);
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

        const sf::FloatRect Border::OuterRegion() const
        {
            if (rectangleShapesOpt_)
            {
                if (rectangleShapesOpt_->size() > 1)
                {
                    return rectangleShapesOpt_.value()[1].getGlobalBounds();
                }
                else
                {
                    return rectangleShapesOpt_->front().getGlobalBounds();
                }
            }
            else if (goldBarOpt_)
            {
                return goldBarOpt_->OuterRegion();
            }
            else
            {
                return sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        const sf::FloatRect Border::InnerRegion() const
        {
            // For now lets use the outer region because the inside of the gold bars probably has a
            // soft line of shadows instead of a hard line, so if the inner content actually started
            // at the inner region then there would be an ugly black gap.
            return OuterRegion();
            /*
            if (rectangleShapesOpt_)
            {
                if (rectangleShapesOpt_->size() > 2)
                {
                    rectangleShapesOpt_.value()[2].getGlobalBounds();
                }
                else
                {
                    rectangleShapesOpt_->front().getGlobalBounds();
                }
            }
            else if (goldBarOpt_)
            {
                goldBarOpt_->InnerRegion();
            }
            else
            {
                return sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
            }
            */
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

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
