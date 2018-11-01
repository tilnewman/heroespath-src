// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXT_RENDER_PARSING_HPP_INCLUDED
#define HEROESPATH_GUI_TEXT_RENDER_PARSING_HPP_INCLUDED
//
// text-render-parsing.hpp
//
#include "sfml-util/text-render-context.hpp"

#include <vector>

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {

        // parsed text of a single font
        struct ParsedTextSegment
        {
            explicit ParsedTextSegment(
                const std::string & TEXT = "", const GuiFont::Enum FONT = GuiFont::Count)
                : text(TEXT)
                , font(FONT)
            {}

            ParsedTextSegment(const ParsedTextSegment &) = default;
            ParsedTextSegment(ParsedTextSegment &&) = default;
            ParsedTextSegment & operator=(const ParsedTextSegment &) = default;
            ParsedTextSegment & operator=(ParsedTextSegment &&) = default;

            bool Empty() const { return text.empty(); }

            bool IsValid() const { return ((Empty() == false) && GuiFont::IsValid(font)); }

            std::string text;
            GuiFont::Enum font;
        };

        using ParsedTextSegmentVec_t = std::vector<ParsedTextSegment>;

        // a collection of ParsedTextSegments that are all on the same line
        struct ParsedTextLine
        {
            ParsedTextLine() = default;
            ParsedTextLine(const ParsedTextLine &) = default;
            ParsedTextLine(ParsedTextLine &&) = default;
            ParsedTextLine & operator=(const ParsedTextLine &) = default;
            ParsedTextLine & operator=(ParsedTextLine &&) = default;

            bool AppendAndResetIfValid(ParsedTextSegment & segment)
            {
                if (AppendIfValid(segment))
                {
                    segment = ParsedTextSegment();
                    return true;
                }
                else
                {
                    return false;
                }
            }

            bool AppendAndResetIfValid(const ParsedTextSegment & SEGMENT)
            {
                ParsedTextSegment segmentTemp { SEGMENT };
                return AppendAndResetIfValid(segmentTemp);
            }

            bool Empty() const { return segments.empty(); }
            bool IsBlankLine() const { return Empty(); }

            ParsedTextSegmentVec_t::const_iterator begin() const noexcept
            {
                return std::begin(segments);
            }

            ParsedTextSegmentVec_t::const_iterator end() const noexcept
            {
                return std::end(segments);
            }

            ParsedTextSegmentVec_t::const_iterator cbegin() const noexcept { return begin(); }
            ParsedTextSegmentVec_t::const_iterator cend() const noexcept { return end(); }

        private:
            bool AppendIfValid(const ParsedTextSegment & SEGMENT)
            {
                if (SEGMENT.IsValid())
                {
                    segments.emplace_back(SEGMENT);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            ParsedTextSegmentVec_t segments;
        };

        inline auto begin(const ParsedTextLine & PTL) noexcept { return PTL.begin(); }
        inline auto cbegin(const ParsedTextLine & PTL) noexcept { return begin(PTL); }
        inline auto end(const ParsedTextLine & PTL) noexcept { return PTL.end(); }
        inline auto cend(const ParsedTextLine & PTL) noexcept { return end(PTL); }

        using ParsedTextLineVec_t = std::vector<ParsedTextLine>;

        // a collection of ParsedTextLines that make up all the text to be rendered by a single
        // rendering job.
        struct ParsedTextLines
        {
            ParsedTextLines() = default;
            ParsedTextLines(const ParsedTextLines &) = default;
            ParsedTextLines(ParsedTextLines &&) = default;
            ParsedTextLines & operator=(const ParsedTextLines &) = default;
            ParsedTextLines & operator=(ParsedTextLines &&) = default;

            bool Empty() const { return lines.empty(); }

            void AppendBlankLine() { lines.emplace_back(ParsedTextLine()); }

            bool AppendAndResetToCurrentLineIfValid(ParsedTextSegment & segment)
            {
                bool didAppendBlankLine { false };

                if (lines.empty())
                {
                    AppendBlankLine();
                    didAppendBlankLine = true;
                }

                const auto DID_APPEND_AND_RESET { lines.back().AppendAndResetIfValid(segment) };

                if ((DID_APPEND_AND_RESET == false) && didAppendBlankLine)
                {
                    lines.pop_back();
                }

                return DID_APPEND_AND_RESET;
            }

            bool AppendAndResetToCurrentLineIfValid(const ParsedTextSegment & SEGMENT)
            {
                ParsedTextSegment segmentTemp { SEGMENT };
                return AppendAndResetToCurrentLineIfValid(segmentTemp);
            }

            ParsedTextLineVec_t::const_iterator begin() const noexcept { return std::begin(lines); }
            ParsedTextLineVec_t::const_iterator end() const noexcept { return std::end(lines); }
            ParsedTextLineVec_t::const_iterator cbegin() const noexcept { return begin(); }
            ParsedTextLineVec_t::const_iterator cend() const noexcept { return end(); }

        private:
            ParsedTextLineVec_t lines;
        };

        inline auto begin(const ParsedTextLines & PTL) noexcept { return PTL.begin(); }
        inline auto cbegin(const ParsedTextLines & PTL) noexcept { return begin(PTL); }
        inline auto end(const ParsedTextLines & PTL) noexcept { return PTL.end(); }
        inline auto cend(const ParsedTextLines & PTL) noexcept { return end(PTL); }

        // Parsing takes a std::string and makes ParsedText objects, wich are required by the
        // Rendering code.
        struct Parser
        {
            Parser() = delete;

            static const ParsedTextLines Make(const Context &);

        private:
            // splits CONTEXT.Text() by font into ParsedTextSegments
            static const ParsedTextLine SplitByFont(const Context &);

            static void
                SplitByNewlines(const Context &, const ParsedTextSegment &, ParsedTextLines &);

            static const ParsedTextLines SplitByNewlines(const Context &, const ParsedTextLine &);

            // splits by word-break-chars, after the breaking char unless that char is a dash
            // For example: "Hello Flat-Earthers"  -> "Hello", " Flat", "-", "Earthers"
            static const ParsedTextLine SplitByWord(const Context &, const ParsedTextSegment &);
            static const ParsedTextLine SplitByWord(const Context &, const ParsedTextLine &);
            static const ParsedTextLines SplitByWord(const Context &, const ParsedTextLines &);

            // at this point all the newline chars have been split into their own ParsedTextSegments
            static const ParsedTextLines SplitByLine(const ParsedTextLine &);

            // the parsing functions above can result in segments that are empty or that only
            // contain whitespace, this function collects them and prepends them to the front
            //  of the next segment
            static const ParsedTextLines CollectWhitespace(const ParsedTextLines &);
        };

    } // namespace text_rendering
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_RENDER_PARSING_HPP_INCLUDED
