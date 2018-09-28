// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render-parsing.cpp
//
#include "text-render-parsing.hpp"

#include "sfml-util/text-render-char-info.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        const ParsedTextLines Parser::Make(const Context & CONTEXT)
        {
            const auto LINE_SPLIT_BY_FONT { SplitByFont(CONTEXT) };

            const auto LINES_SPLIT_BY_FONT_AND_NEWLINES { SplitByNewlines(
                CONTEXT, LINE_SPLIT_BY_FONT) };

            const auto ALL_LINES { SplitByWord(CONTEXT, LINES_SPLIT_BY_FONT_AND_NEWLINES) };

            const auto ALL_LINES_WITH_COLLECTED_WHITESPACE { CollectWhitespace(ALL_LINES) };

            return ALL_LINES_WITH_COLLECTED_WHITESPACE;
        }

        const ParsedTextLine Parser::SplitByFont(const Context & CONTEXT)
        {
            ParsedTextLine line;
            ParsedTextSegment segment;

            for (const char CHAR : CONTEXT.text_cleaner.text)
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

        void Parser::SplitByNewlines(
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

                    SplitByNewlines(CONTEXT, SEGMENT_AFTER_NEWLINE, lines);
                }
            }
        }

        const ParsedTextLines
            Parser::SplitByNewlines(const Context & CONTEXT, const ParsedTextLine & LINE)
        {
            ParsedTextLines newLines;

            for (const auto & SEGMENT : LINE)
            {
                SplitByNewlines(CONTEXT, SEGMENT, newLines);
            }

            return newLines;
        }

        const ParsedTextLine
            Parser::SplitByWord(const Context & CONTEXT, const ParsedTextSegment & SEGMENT_ORIG)
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

        const ParsedTextLine
            Parser::SplitByWord(const Context & CONTEXT, const ParsedTextLine & LINE)
        {
            ParsedTextLine newLine;

            for (const auto & WHOLE_SEGMENT : LINE)
            {
                for (const auto & SUB_SEGMENT : SplitByWord(CONTEXT, WHOLE_SEGMENT))
                {
                    newLine.AppendAndResetIfValid(SUB_SEGMENT);
                }
            }

            return newLine;
        }

        const ParsedTextLines
            Parser::SplitByWord(const Context & CONTEXT, const ParsedTextLines & LINES)
        {
            ParsedTextLines newLines;

            for (const auto & LINE : LINES)
            {
                newLines.AppendBlankLine();

                for (const auto & SEGMENT : SplitByWord(CONTEXT, LINE))
                {
                    newLines.AppendAndResetToCurrentLineIfValid(SEGMENT);
                }
            }

            return newLines;
        }

        const ParsedTextLines Parser::CollectWhitespace(const ParsedTextLines & LINES)
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

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath
