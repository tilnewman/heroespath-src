// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render-rendering.cpp
//
#include "text-render-rendering.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        const RenderedLines Renderer::Make(const Context & CONTEXT)
        {
            const auto CAN_USE_QUICK_RENDER {
                (CONTEXT.HasWidthLimit() == false) && (CONTEXT.text_cleaner.newline_count == 0)
                && GuiFont::IsValid(CONTEXT.text_cleaner.uniform_font)
            };

            RenderedLines renderedLines;

            if (CAN_USE_QUICK_RENDER)
            {
                const Text TEXT(
                    CONTEXT.text_cleaner.text,
                    CONTEXT.text_cleaner.uniform_font,
                    CONTEXT.text_info.size);

                renderedLines.CurrentLineAppend(TEXT);
            }
            else
            {
                RenderLines(CONTEXT, Parser::Make(CONTEXT), renderedLines);
            }

            renderedLines.FinalAlignmentAndSpacing(CONTEXT);
            return renderedLines;
        }

        void Renderer::RenderSegment(
            const Context & CONTEXT,
            const ParsedTextSegment & PARSED_TEXT_SEGMENT,
            RenderedLines & renderedLines)
        {
            // color is always White because the final rendered sprite sets the color
            const Text TEXT(
                CONTEXT.text_info,
                PARSED_TEXT_SEGMENT.text,
                PARSED_TEXT_SEGMENT.font,
                sf::Color::White);

            const auto TEXT_WIDTH { TEXT.getGlobalBounds().width };

            const auto WIDTH_REMAINING { renderedLines.CurrentLineWidthRemaining(
                CONTEXT.width_limit) };

            if (CONTEXT.HasWidthLimit() && (TEXT_WIDTH > WIDTH_REMAINING))
            {
                if (renderedLines.IsCurrentLineEmpty())
                {
                    auto shrunkText { TEXT };

                    while ((shrunkText.empty() == false)
                           && (shrunkText.getGlobalBounds().width > WIDTH_REMAINING))
                    {
                        shrunkText.pop_back();
                    }

                    M_HP_LOG_WRN(
                        "text_render::engine::RenderParsedTextSegment(context_cleaned_text=\""
                        << misc::MakeLoggableString(CONTEXT.text_cleaner.text)
                        << "\", segment_text=\"" << PARSED_TEXT_SEGMENT.text
                        << "\")  but that segment was " << (TEXT_WIDTH - WIDTH_REMAINING)
                        << " wider than the width_limit (" << CONTEXT.width_limit
                        << "), on a line that was empty, and so chars were removed from the end "
                           "until it fit.  The final text is \""
                        << shrunkText.getString() << "\".");

                    if (shrunkText.empty() == false)
                    {
                        renderedLines.CurrentLineAppend(shrunkText);
                    }

                    renderedLines.AppendEmptyLine();
                }
                else
                {
                    renderedLines.AppendEmptyLine();
                    RenderSegment(CONTEXT, PARSED_TEXT_SEGMENT, renderedLines);
                }
            }
            else
            {
                renderedLines.CurrentLineAppend(TEXT);
            }
        }

        void Renderer::RenderLines(
            const Context & CONTEXT,
            const ParsedTextLines & PARSED_TEXT_LINES,
            RenderedLines & renderedLines)
        {
            for (const auto & LINE : PARSED_TEXT_LINES)
            {
                if (LINE.IsBlankLine())
                {
                    renderedLines.AppendBlankLine(CONTEXT.blank_line_height);
                }
                else
                {
                    renderedLines.AppendEmptyLine();

                    for (const auto & SEGMENT : LINE)
                    {
                        RenderSegment(CONTEXT, SEGMENT, renderedLines);
                    }
                }
            }
        }

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath
