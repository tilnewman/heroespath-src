// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-rendering.cpp
//
#include "text-rendering.hpp"

#include "log/log-macros.hpp"
#include "misc/boost-string-includes.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath::sfml_util::text_render
{
namespace engine
{

    // this is used as the vertical whitespace placeholder for blank lines
    const char CharInfo::whitespace_placeholder_char { '(' };

    const CleanedText CleanText(const gui::TextInfo & TEXT_INFO)
    {
        CleanedText cleanedText;

        auto isLetterFontRequired { false };
        auto isNumberFontRequired { false };

        // consider and then append characters one at a time
        for (const char CHAR_ORIG : TEXT_INFO.text)
        {
            const CharInfo CHAR_INFO(CHAR_ORIG, TEXT_INFO);

            if (CHAR_INFO.is_renderable == false)
            {
                continue;
            }

            cleanedText.text += CHAR_INFO.which_char;

            if (CHAR_INFO.is_newline)
            {
                ++cleanedText.newline_count;
            }
            else if (CHAR_INFO.requires_letter_font)
            {
                isLetterFontRequired = true;
            }
            else if (CHAR_INFO.requires_number_font)
            {
                isNumberFontRequired = true;
            }
        }

        // trim leading whitespace
        while ((cleanedText.text.empty() == false)
               && CharInfo(cleanedText.text.front()).is_whitespace)
        {
            cleanedText.text.erase(std::begin(cleanedText.text));
        }

        // trim trailing whitespace
        while ((cleanedText.text.empty() == false)
               && CharInfo(cleanedText.text.back()).is_whitespace)
        {
            cleanedText.text.pop_back();
        }

        if (isLetterFontRequired)
        {
            if (isNumberFontRequired)
            {
                cleanedText.uniform_font = GuiFont::Count;
            }
            else
            {
                cleanedText.uniform_font = TEXT_INFO.font_letters;
            }
        }
        else
        {
            if (isNumberFontRequired)
            {
                cleanedText.uniform_font = TEXT_INFO.font_numbers;
            }
            else
            {
                // default to using letter font if neither is required
                cleanedText.uniform_font = TEXT_INFO.font_letters;
            }
        }

        return cleanedText;
    }

    const ParsedTextLine ParseAllAndSplitByFont(const Context & CONTEXT)
    {
        ParsedTextLine line;
        ParsedTextSegment segment;

        for (const char CHAR : CONTEXT.cleaned_text.text)
        {
            const CharInfo CHAR_INFO(CHAR, CONTEXT.text_info, segment.font);

            if (segment.font != CHAR_INFO.font)
            {
                line.AppendAndResetIfValid(segment);
                segment.font = CHAR_INFO.font;
            }

            segment.text += CHAR;
        }

        // default to using the letters font if neither was required before reaching the end
        if (GuiFont::IsValid(segment.font) == false)
        {
            segment.font = CONTEXT.text_info.font_letters;
        }

        line.AppendAndResetIfValid(segment);

        return line;
    }

    void ParseAllAndSplitByNewlines(
        const Context & CONTEXT, const ParsedTextSegment & SEGMENT, ParsedTextLines & lines)
    {
        const auto NEWLINE_CHAR_POS { SEGMENT.text.find("\n") };

        if (NEWLINE_CHAR_POS == std::string::npos)
        {
            lines.AppendAndResetToCurrentLineIfValid(SEGMENT);
        }
        else
        {
            const ParsedTextSegment SEGMENT_BEFORE_NEWLINE(
                SEGMENT.text.substr(0, NEWLINE_CHAR_POS), SEGMENT.font);

            lines.AppendAndResetToCurrentLineIfValid(SEGMENT_BEFORE_NEWLINE);

            if (lines.Empty())
            {
                lines.AppendBlankLine();
            }

            lines.AppendBlankLine();

            if (NEWLINE_CHAR_POS < (SEGMENT.text.size() - 1))
            {
                const ParsedTextSegment SEGMENT_AFTER_NEWLINE(
                    SEGMENT.text.substr(NEWLINE_CHAR_POS + 1), SEGMENT.font);

                ParseAllAndSplitByNewlines(CONTEXT, SEGMENT_AFTER_NEWLINE, lines);
            }
        }
    }

    const ParsedTextLines
        ParseAllAndSplitByNewlines(const Context & CONTEXT, const ParsedTextLine & LINE)
    {
        ParsedTextLines newLines;

        for (const auto & SEGMENT : LINE)
        {
            ParseAllAndSplitByNewlines(CONTEXT, SEGMENT, newLines);
        }

        return newLines;
    }

    const ParsedTextLine
        ParseAndSplitByWord(const Context & CONTEXT, const ParsedTextSegment & SEGMENT_ORIG)
    {
        ParsedTextLine line;
        ParsedTextSegment segmentNew;

        for (const char CHAR : SEGMENT_ORIG.text)
        {
            segmentNew.font = SEGMENT_ORIG.font;

            const auto CHAR_INFO { CharInfo(CHAR, CONTEXT.text_info) };

            if (CHAR_INFO.is_word_break)
            {
                if (segmentNew.text != " ")
                {
                    line.AppendAndResetIfValid(segmentNew);
                }

                // dashes are a special case because we don't want "-word" segments
                if (CHAR_INFO.is_dash)
                {
                    segmentNew = ParsedTextSegment("-", SEGMENT_ORIG.font);
                    line.AppendAndResetIfValid(segmentNew);
                    continue;
                }
            }

            segmentNew.font = SEGMENT_ORIG.font;
            segmentNew.text += CHAR;
        }

        line.AppendAndResetIfValid(segmentNew);

        return line;
    }

    const ParsedTextLine ParseAndSplitByWord(const Context & CONTEXT, const ParsedTextLine & LINE)
    {
        ParsedTextLine newLine;

        for (const auto & WHOLE_SEGMENT : LINE)
        {
            for (const auto & SUB_SEGMENT : ParseAndSplitByWord(CONTEXT, WHOLE_SEGMENT))
            {
                newLine.AppendAndResetIfValid(SUB_SEGMENT);
            }
        }

        return newLine;
    }

    const ParsedTextLines
        ParseAndSplitByWord(const Context & CONTEXT, const ParsedTextLines & LINES)
    {
        ParsedTextLines newLines;

        for (const auto & LINE : LINES)
        {
            newLines.AppendBlankLine();

            for (const auto & SEGMENT : ParseAndSplitByWord(CONTEXT, LINE))
            {
                newLines.AppendAndResetToCurrentLineIfValid(SEGMENT);
            }
        }

        return newLines;
    }

    const ParsedTextLines ParseAndCollectWhitespace(const ParsedTextLines & LINES)
    {
        ParsedTextLines newLines;

        for (const auto & LINE : LINES)
        {
            newLines.AppendBlankLine();

            ParsedTextSegment newSegment;

            for (const auto & SEGMENT : LINE)
            {
                if (SEGMENT.text.empty())
                {
                    continue;
                }

                auto isSegmentAllWhiteSpace = [&]() {
                    for (const char CHAR : SEGMENT.text)
                    {
                        if (CharInfo(CHAR).is_whitespace == false)
                        {
                            return false;
                        }
                    }

                    return true;
                };

                if (isSegmentAllWhiteSpace())
                {
                    newSegment.text += SEGMENT.text;
                }
                else
                {
                    newSegment.text += SEGMENT.text;
                    newSegment.font = SEGMENT.font;
                    newLines.AppendAndResetToCurrentLineIfValid(newSegment);
                }
            }
        }

        return newLines;
    }

    const ParsedTextLines ParseAll(const Context & CONTEXT)
    {
        const auto LINE_SPLIT_BY_FONT { ParseAllAndSplitByFont(CONTEXT) };

        const auto LINES_SPLIT_BY_FONT_AND_NEWLINES { ParseAllAndSplitByNewlines(
            CONTEXT, LINE_SPLIT_BY_FONT) };

        const auto ALL_PARSED_TEXT_LINES { ParseAndSplitByWord(
            CONTEXT, LINES_SPLIT_BY_FONT_AND_NEWLINES) };

        const auto ALL_PARSED_TEXT_LINES_WITH_COLLECTED_WHITESPACE { ParseAndCollectWhitespace(
            ALL_PARSED_TEXT_LINES) };

        return ALL_PARSED_TEXT_LINES_WITH_COLLECTED_WHITESPACE;
    }

    void RenderParsedTextSegment(
        const Context & CONTEXT, const ParsedTextSegment & PARSED_TEXT_SEGMENT, Render & render)
    {
        // color is always White because the final rendered sprite sets the color
        const Text TEXT(
            CONTEXT.text_info,
            PARSED_TEXT_SEGMENT.text,
            PARSED_TEXT_SEGMENT.font,
            sf::Color::White);

        const auto TEXT_WIDTH { TEXT.getGlobalBounds().width };
        const auto WIDTH_REMAINING { render.CurrentLineWidthRemaining(CONTEXT.width_limit) };

        if (CONTEXT.HasWidthLimit() && (TEXT_WIDTH > WIDTH_REMAINING))
        {
            if (render.IsCurrentLineEmpty())
            {
                auto shrunkText { TEXT };

                while ((shrunkText.empty() == false)
                       && (shrunkText.getGlobalBounds().width > WIDTH_REMAINING))
                {
                    shrunkText.pop_back();
                }

                M_HP_LOG_WRN(
                    "text_render::engine::RenderParsedTextSegment(context_cleaned_text=\""
                    << misc::MakeLoggableString(CONTEXT.cleaned_text.text) << "\", segment_text=\""
                    << PARSED_TEXT_SEGMENT.text << "\")  but that segment was "
                    << (TEXT_WIDTH - WIDTH_REMAINING) << " wider than the width_limit ("
                    << CONTEXT.width_limit
                    << "), on a line that was empty, and so chars were removed from the end until "
                       "it fit.  The final text is \""
                    << shrunkText.getString() << "\".");

                if (shrunkText.empty() == false)
                {
                    render.CurrentLineAppend(shrunkText);
                }

                render.AppendEmptyLine();
            }
            else
            {
                render.AppendEmptyLine();
                RenderParsedTextSegment(CONTEXT, PARSED_TEXT_SEGMENT, render);
            }
        }
        else
        {
            render.CurrentLineAppend(TEXT);
        }
    }

    void RenderAll(
        const Context & CONTEXT, const ParsedTextLines & PARSED_TEXT_LINES, Render & render)
    {
        for (const auto & LINE : PARSED_TEXT_LINES)
        {
            if (LINE.IsBlankLine())
            {
                render.AppendBlankLine(CONTEXT.tallest_char_height);
            }
            else
            {
                render.AppendEmptyLine();

                for (const auto & SEGMENT : LINE)
                {
                    RenderParsedTextSegment(CONTEXT, SEGMENT, render);
                }
            }
        }
    }

    const Render ParseAndRender(const Context & CONTEXT)
    {
        const auto CAN_USE_QUICK_RENDER { (CONTEXT.HasWidthLimit() == false)
                                          && (CONTEXT.cleaned_text.newline_count == 0)
                                          && GuiFont::IsValid(CONTEXT.cleaned_text.uniform_font) };

        Render render;

        if (CAN_USE_QUICK_RENDER)
        {
            const Text TEXT(
                CONTEXT.cleaned_text.text,
                CONTEXT.cleaned_text.uniform_font,
                CONTEXT.text_info.size);

            render.CurrentLineAppend(TEXT);
        }
        else
        {
            RenderAll(CONTEXT, ParseAll(CONTEXT), render);
        }

        render.FinalAlignmentAndSpacing(CONTEXT);
        return render;
    }

    void DrawToTexture(const Render & RENDERED_TEXT, sf::RenderTexture & renderTexture)
    {
        const sf::Vector2u TEXT_SIZE { Size(RENDERED_TEXT.Region()) };

        if ((renderTexture.getSize().x < TEXT_SIZE.x) && (renderTexture.getSize().y < TEXT_SIZE.y))
        {
            const auto ORIG_SIZE { renderTexture.getSize() };
            const auto CREATE_TEXTURE_RESULT { renderTexture.create(TEXT_SIZE.x, TEXT_SIZE.y) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (CREATE_TEXTURE_RESULT),
                "sfml_util::text_render::engine::DrawToTexture(rendered_text="
                    << RENDERED_TEXT.ToString() << ", render_texture_size_orig=" << ORIG_SIZE
                    << ")  The sf::RenderTexture give was not big enough, but attempting to make "
                       "one big enough by calling sf::RenderTexture::Create("
                    << TEXT_SIZE << ") failed and returned false.");
        }

        renderTexture.setSmooth(true);
        renderTexture.clear(sf::Color::Transparent);

        // if you draw text to a transparent texture using BlendAlpha the result will be fuzy
        // so use BlendNone and it will look good
        sf::RenderStates states;
        states.blendMode = sf::BlendNone;

        renderTexture.draw(RENDERED_TEXT, states);
        renderTexture.display();
    }

} // namespace engine

bool Render(
    const gui::TextInfo & TEXT_INFO,
    const sf::FloatRect & REGION,
    RenderTextureUPtr_t & renderTextureUPtr,
    sf::FloatRect & actualRegion)
{
    auto logErrorMsgAndReturnErrorResult = [&](const std::string & ERROR_MSG) {
        std::ostringstream ss;

        ss << "sfml_util::text_render::Render(" << TEXT_INFO << ", region=" << REGION << ")  "
           << ERROR_MSG;

        M_HP_LOG_ERR(ss.str());

        return false;
    };

    if (!TEXT_INFO.IsValid())
    {
        return logErrorMsgAndReturnErrorResult(
            "No text was rendered because the TEXT_INFO object was invalid.");
    }

    const auto CLEANED_TEXT { engine::CleanText(TEXT_INFO) };

    if (CLEANED_TEXT.text.empty())
    {
        return logErrorMsgAndReturnErrorResult("No text was rendered because the text was "
                                               "empty after removing invalid characters.");
    }

    if (TEXT_INFO.text != CLEANED_TEXT.text)
    {
        M_HP_LOG_WRN(
            "sfml_util::text_render::Render("
            << TEXT_INFO << ", region=" << REGION
            << ")  Original text was changed during engine::CleanText():\nold=\"" << TEXT_INFO.text
            << "\"\nnew=\"" << CLEANED_TEXT.text << "\"");
    }

    const engine::Context CONTEXT(TEXT_INFO, CLEANED_TEXT, REGION.width);
    const auto RENDER { ParseAndRender(CONTEXT) };

    if (RENDER.IsValid())
    {
        if (!renderTextureUPtr)
        {
            renderTextureUPtr = std::make_unique<sf::RenderTexture>();
        }

        engine::DrawToTexture(RENDER, *renderTextureUPtr);
        actualRegion = RENDER.CalcScreenRegion(Position(REGION));
        return true;
    }
    else
    {
        std::ostringstream ss;
        ss << "No text was rendered by RenderLines().  render=" << RENDER.ToString();
        return logErrorMsgAndReturnErrorResult(ss.str());
    }
}

bool Render(
    const gui::TextInfo & TEXT_INFO,
    const sf::FloatRect & REGION,
    RenderTextureUPtr_t & renderTextureUPtr_,
    sf::Sprite & sprite)
{
    sf::FloatRect finalRegion;
    const auto RENDER_RESULT { Render(TEXT_INFO, REGION, renderTextureUPtr_, finalRegion) };

    if (RENDER_RESULT && renderTextureUPtr_)
    {
        sprite.setTexture(renderTextureUPtr_->getTexture());
        sprite.setTextureRect(sf::IntRect(sf::Vector2i(), sf::Vector2i(Size(finalRegion))));
        sprite.setPosition(Position(finalRegion));
        sprite.setColor(TEXT_INFO.color);
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace heroespath::sfml_util::text_render
