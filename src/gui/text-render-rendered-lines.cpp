// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render-rendered-lines.cpp
//
#include "text-render-rendered-lines.hpp"

#include "misc/enum-util.hpp"
#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {

        RenderedLines::RenderedLines()
            : lines()
            , region()
            , justifyOffsetV()
        {}

        void RenderedLines::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            for (const auto & LINE : lines)
            {
                target.draw(LINE, states);
            }
        }

        bool RenderedLines::IsValid() const
        {
            return ((Empty() == false) && (sfutil::IsSizeZeroOrLessEither(region) == false));
        }

        bool RenderedLines::IsCurrentLineEmpty() const
        {
            if (lines.empty())
            {
                return true;
            }
            else
            {
                return lines.back().Empty();
            }
        }

        const sf::FloatRect RenderedLines::CalcScreenRegion(const sf::Vector2f & SCREEN_POS_V) const
        {
            const auto ACTUAL_POSITION { (
                SCREEN_POS_V + sfutil::Position(region) + justifyOffsetV) };
            const auto ACTUAL_SIZE { sfutil::Size(region) };
            return sf::FloatRect(ACTUAL_POSITION, ACTUAL_SIZE);
        }

        // returns [0, WIDTH_LIMIT], returns WIDTH_LIMIT if lines.empty()
        float RenderedLines::CurrentLineWidthRemaining(const float WIDTH_LIMIT) const
        {
            if (lines.empty())
            {
                return WIDTH_LIMIT;
            }

            const auto WIDTH_REMAINING { (WIDTH_LIMIT - sfutil::Right(lines.back().Region())) };
            return std::clamp(WIDTH_REMAINING, 0.0f, WIDTH_LIMIT);
        }

        void RenderedLines::AppendEmptyLine()
        {
            auto posLeft { region.left };
            auto posTop { sfutil::Bottom(region) };

            if (lines.empty() == false)
            {
                posTop += 1.0f;
            }

            lines.emplace_back(RenderedLine(sf::Vector2f(posLeft, posTop)));
        }

        void RenderedLines::CurrentLineAppend(const gui::Text & TEXT)
        {
            CurrentLine().Append(TEXT);
            region = sfutil::MinimallyEnclosing(region, CurrentLine().Region(), true);
        }

        void RenderedLines::AppendBlankLine(const float BLANK_LINE_HEIGHT)
        {
            AppendEmptyLine();
            lines.back().SetupAsBlankLine();
            region.height += BLANK_LINE_HEIGHT;

            // need to add a little harmless width so that sfutil::MinimallyEnclosing() will not
            // ignore region
            if ((region.width > 0.0f) == false)
            {
                region.width = 0.1f;
            }
        }

        void RenderedLines::Move(const sf::Vector2f & MOVE_V)
        {
            for (auto & line : lines)
            {
                line.Move(MOVE_V);
            }

            sfutil::Move(region, MOVE_V);
        }

        void RenderedLines::FinalAlignmentAndSpacing(const Context & CONTEXT)
        {
            SetLineSpacing();
            Justify(CONTEXT.text_info.justified, CONTEXT.width_limit);
        }

        const std::string RenderedLines::ToString() const
        {
            std::ostringstream ss;
            ss << "\nRender (" << lines.size() << ")  region=" << region << "\n\t";
            for (const auto & LINE : lines)
            {
                ss << LINE.ToString() << "\n\t";
            }
            return ss.str();
        }

        RenderedLine & RenderedLines::CurrentLine()
        {
            if (Empty())
            {
                AppendEmptyLine();
            }

            return lines.back();
        }

        float RenderedLines::JustifyHorizOffset(
            const Justified::Enum JUSTIFICATION,
            const float WIDTH_LIMIT,
            const float TOTAL_WIDTH,
            const float LINE_WIDTH)
        {
            if (JUSTIFICATION == Justified::Center)
            {
                if (WIDTH_LIMIT > 0.0f)
                {
                    return ((WIDTH_LIMIT * 0.5f) - (LINE_WIDTH * 0.5f));
                }
                else
                {
                    return ((TOTAL_WIDTH * 0.5f) - (LINE_WIDTH * 0.5f));
                }
            }
            else if (JUSTIFICATION == Justified::Right)
            {
                if (WIDTH_LIMIT > 0.0f)
                {
                    return (WIDTH_LIMIT - LINE_WIDTH);
                }
                else
                {
                    return (TOTAL_WIDTH - LINE_WIDTH);
                }
            }
            else
            {
                return 0.0f;
            }
        }

        void RenderedLines::Justify(const Justified::Enum JUSTIFICATION, const float WIDTH_LIMIT)
        {
            const auto ORIG_POS_V { sfutil::Position(region) };

            if (Empty() || !EnumUtil<Justified>::IsValid(JUSTIFICATION)
                || (JUSTIFICATION == Justified::Left))
            {
                return;
            }

            const auto TOTAL_WIDTH { region.width };

            region = sf::FloatRect();

            for (auto & line : lines)
            {
                const auto HORIZ_OFFSET { JustifyHorizOffset(
                    JUSTIFICATION, WIDTH_LIMIT, TOTAL_WIDTH, line.Region().width) };

                line.Move(sf::Vector2f(HORIZ_OFFSET, 0.0f));
                region = sfutil::MinimallyEnclosing(region, line.Region(), true);
            }

            justifyOffsetV = (sfutil::Position(region) - ORIG_POS_V);
            SetPosition(ORIG_POS_V);
        }

        void RenderedLines::SetLineSpacing()
        {
            sf::Vector2f moveV(0.0f, 0.0f);

            for (auto iter(std::begin(lines)); iter != std::end(lines); ++iter)
            {
                if (std::begin(lines) != iter)
                {
                    iter->Move(moveV);
                    moveV.y += iter->VerticalStretchToLineSpacing();
                }
            }
        }

    } // namespace text_rendering
} // namespace gui
} // namespace heroespath
