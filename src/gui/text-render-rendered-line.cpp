// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render-rendered-line.cpp
//
#include "text-render-rendered-line.hpp"

#include "gui/text-render-parsing.hpp"
#include "misc/enum-util.hpp"
#include "sfutil/common.hpp"
#include "sfutil/scale.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <string>

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {

        RenderedLine::RenderedLine(const sf::Vector2f & POS_V)
            : texts()
            , region(POS_V, sf::Vector2f())
        {
            // found by experiment to be a good guess for the game
            texts.reserve(20);
        }

        void RenderedLine::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            for (const auto & TEXT : texts)
            {
                target.draw(TEXT, states);
            }
        }

        void RenderedLine::Move(const sf::Vector2f & MOVE_V)
        {
            for (auto & text : texts)
            {
                text.move(MOVE_V);
            }

            sfutil::Move(region, MOVE_V);
        }

        void RenderedLine::SetupAsBlankLine()
        {
            region.width = 1.0f;
            region.height = 1.0f;
        }

        void RenderedLine::Append(const gui::Text & TEXT)
        {
            if (TEXT.IsValid() == false)
            {
                return;
            }

            auto posLeft { sfutil::Right(region) };
            auto posTop { region.top };

            if (texts.empty() == false)
            {
                posLeft += 1.0f;
            }

            texts.emplace_back(TEXT);

            // move to top right of this line
            // then...
            // shift down to a vert pos that is appropriate for the height of this text
            const auto MOVE_V { (sf::Vector2f(posLeft, posTop) - sfutil::Position(texts.back()))
                                + sf::Vector2f(0.0f, calcDownShiftForCharHeight(TEXT)) };

            texts.back().move(MOVE_V);

            // stretch the bounding region of this line to accommodate for the shift above
            const auto TOP_BEFORE { region.top };
            region = sfutil::MinimallyEnclosing(region, texts.back().getGlobalBounds(), true);
            const auto BOTTOM_AFTER { sfutil::Bottom(region) };
            region.top = TOP_BEFORE;
            region.height = (BOTTOM_AFTER - TOP_BEFORE);
        }

        float RenderedLine::VerticalStretchToLineSpacing()
        {
            if (Empty())
            {
                return 0.0f;
            }

            const auto LARGEST_LINE_SPACING { LargestLineSpacing() };
            const auto TOP_GAP { topGap() };
            const auto TARGET_LINE_SPACING { (LARGEST_LINE_SPACING - TOP_GAP) };
            const auto DOWN_SHIFT_FULL { (TARGET_LINE_SPACING - region.height) };

            const auto FONT_SIZE_F { misc::Max(
                20.0f, static_cast<float>(texts.front().getCharacterSize())) };

            const auto FONT_SIZE_MULT { 1.0f - (FONT_SIZE_F / misc::Max(40.0f, FONT_SIZE_F)) };

            const auto DOWN_SHIFT_FINAL { (DOWN_SHIFT_FULL * 2.0f) * FONT_SIZE_MULT };

            for (auto & text : texts)
            {
                text.move(0.0f, DOWN_SHIFT_FINAL);
            }

            region.height += DOWN_SHIFT_FINAL;
            return DOWN_SHIFT_FINAL;
        }

        const std::string RenderedLine::ToString() const
        {
            std::string str;
            str.reserve(64 + (texts.size() * 128));

            str += "{[";

            if (texts.empty())
            {
                str += "EMPTY";
            }
            else
            {
                for (const auto & TEXT : texts)
                {
                    str += ((TEXT.IsValid()) ? "" : "I");
                    str += NAMEOF_ENUM(TEXT.getFont());
                    str += '(';
                    str += misc::ToString(TEXT.getGlobalBounds());
                    str += ")\"";
                    str += TEXT.getString();
                    str += "\"dsfch=" + std::to_string(calcDownShiftForCharHeight(TEXT));
                    str += "   ";
                }
            }

            str += "]gap=";
            str += std::to_string(topGap());
            str += misc::ToString(region);
            str += '}';

            return str;
        }

        // distance between the line top (region.top) and the tallest char top
        float RenderedLine::topGap() const
        {
            float highestCharPosTop { sfutil::Bottom(region) };

            for (const auto & TEXT : texts)
            {
                if (highestCharPosTop > TEXT.getGlobalBounds().top)
                {
                    highestCharPosTop = TEXT.getGlobalBounds().top;
                }
            }

            return (highestCharPosTop - region.top);
        }

        float RenderedLine::LargestLineSpacing() const
        {
            float largestLineSpacing { 0.0f };

            for (const auto & TEXT : texts)
            {
                if (largestLineSpacing < TEXT.getLineSpacing())
                {
                    largestLineSpacing = TEXT.getLineSpacing();
                }
            }

            return largestLineSpacing;
        }

        float RenderedLine::calcDownShiftForCharHeight(const Text & TEXT) const
        {
            if (TEXT.empty())
            {
                return 0.0f;
            }

            // add chars that reach high and low to stretch out vertically
            Text temp(TEXT);
            const auto TOP_BEFORE { temp.findCharacterPos(0).y };
            temp.push_back('(');
            temp.push_back('p');
            temp.push_back('q');
            temp.push_back('g');
            temp.push_back('j');
            temp.push_back('q');
            temp.push_back('y');
            const auto TOP_AFTER { temp.findCharacterPos(0).y };

            if (TEXT.getFont() == GuiFont::Number)
            {
                return (TOP_AFTER - TOP_BEFORE) + (temp.getGlobalBounds().height * 0.125f);
            }
            else
            {
                return (TOP_AFTER - TOP_BEFORE);
            }
        }

    } // namespace text_rendering
} // namespace gui
} // namespace heroespath
