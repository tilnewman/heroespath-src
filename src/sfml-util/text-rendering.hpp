// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXTRENDERING_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTRENDERING_HPP_INCLUDED
//
// text-rendering.hpp
//
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/enum-util.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/rendered-text.hpp"
#include "sfml-util/sfml-util-font.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"
#include "sfml-util/text.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <algorithm>
#include <cctype> //for std::isdigit()
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace sf
{
class RenderTexture;
} // namespace sf

namespace heroespath
{

namespace sfml_util
{
    namespace text_render
    {
        namespace engine
        {

            // Parsing takes a std::string makes ParsedText objects
            // Rendering takes ParsedText and makes sfml_util::Text objects (sf::Text)

            // defines why rendering stopped at a certain location in the text
            struct StopReason : public misc::EnumBaseCounting<StopReason, misc::EnumFirstValueValid>
            {
                enum Enum : misc::EnumUnderlying_t
                {
                    EndOfText = 0,
                    EndOfWord,
                    NewlineChar,
                    WidthLimit,
                    FontChanged,
                    Count
                };

                static const std::string ToString(const Enum CAUSE)
                {
                    switch (CAUSE)
                    {
                        case EndOfText:
                        {
                            return "EndOfText";
                        }
                        case EndOfWord:
                        {
                            return "EndOfWord";
                        }
                        case NewlineChar:
                        {
                            return "NewlineChar";
                        }
                        case WidthLimit:
                        {
                            return "WidthLimit";
                        }
                        case FontChanged:
                        {
                            return "FontChanged";
                        }
                        case Count:
                        default:
                        {
                            ThrowInvalidValueForFunction(CAUSE, "ToString");
                        }
                    }
                }
            };

            // Responsible for making render related decisions about a single character that has'nt
            // yet been rendered.
            struct CharInfo
            {
                CharInfo(
                    const char CHAR,
                    const gui::TextInfo & TEXT_INFO,
                    const GuiFont::Enum PREV_FONT = GuiFont::Count)
                    : CharInfo(CHAR, TEXT_INFO.font_letters, TEXT_INFO.font_numbers, PREV_FONT)
                {}

                CharInfo(
                    const char CHAR,
                    const GuiFont::Enum LETTER_FONT = GuiFont::Count,
                    const GuiFont::Enum NUMBER_FONT = GuiFont::Count,
                    const GuiFont::Enum PREV_FONT = GuiFont::Count)
                    : which_char(CHAR)
                    , is_newline(CHAR == '\n')
                    , is_whitespace((CHAR == ' ') || (CHAR == '\t'))
                    , is_dash(CHAR == '-')
                    , is_word_break(is_newline || is_whitespace || is_dash)
                    , is_renderable(is_word_break || ((CHAR >= 32) && (CHAR <= 126)))
                    , requires_neither_font(is_word_break || (CHAR == '.') || (CHAR == '+'))
                    , requires_number_font(std::isdigit(static_cast<int>(CHAR)) != 0)
                    , requires_letter_font(!requires_number_font && !requires_neither_font)
                    , font(GuiFont::Count)
                {
                    if (requires_letter_font && GuiFont::IsValid(LETTER_FONT))
                    {
                        font = LETTER_FONT;
                    }
                    else if (requires_number_font && GuiFont::IsValid(NUMBER_FONT))
                    {
                        font = NUMBER_FONT;
                    }
                    else
                    {
                        font = PREV_FONT;
                    }
                }

                CharInfo(const CharInfo &) = default;
                CharInfo(CharInfo &&) = default;
                CharInfo & operator=(const CharInfo &) = default;
                CharInfo & operator=(CharInfo &&) = default;

                static const char whitespace_placeholder_char;

                char which_char;
                bool is_newline;
                bool is_whitespace;
                bool is_dash;
                bool is_word_break;
                bool is_renderable;
                bool requires_neither_font;
                bool requires_number_font;
                bool requires_letter_font;
                GuiFont::Enum font;
            };

            // a single line of rendered text where font/color/style/etc may vary
            class RenderedLine : public sf::Drawable
            {
            public:
                explicit RenderedLine(const sf::Vector2f & POS_V)
                    : texts()
                    , region(POS_V, sf::Vector2f())
                {
                    // found by experiment to be a good guess for the game
                    texts.reserve(20);
                }

                RenderedLine(const RenderedLine &) = default;
                RenderedLine(RenderedLine &&) = default;
                RenderedLine & operator=(const RenderedLine &) = default;
                RenderedLine & operator=(RenderedLine &&) = default;

                void draw(sf::RenderTarget & target, sf::RenderStates states) const final
                {
                    for (const auto & TEXT : texts)
                    {
                        target.draw(TEXT, states);
                    }
                }

                bool Empty() const { return texts.empty(); }

                const sf::FloatRect Region() const { return region; }

                void Move(const sf::Vector2f & MOVE_V)
                {
                    for (auto & text : texts)
                    {
                        text.move(MOVE_V);
                    }

                    sfml_util::Move(region, MOVE_V);
                }

                void SetPosition(const sf::Vector2f & POS_V) { Move(POS_V - Position(region)); }

                void SetupAsBlankLine()
                {
                    region.width = 1.0f;
                    region.height = 1.0f;
                }

                void Append(const sfml_util::Text & TEXT)
                {
                    if (TEXT.IsValid() == false)
                    {
                        return;
                    }

                    auto posLeft { Right(region) };
                    auto posTop { region.top };

                    if (texts.empty() == false)
                    {
                        posLeft += 1.0f;
                    }

                    texts.emplace_back(TEXT);

                    // move to top right of this line
                    const auto MOVE_V { sf::Vector2f(posLeft, posTop) - Position(texts.back()) };
                    texts.back().move(MOVE_V);

                    // shift down to a vert pos that is appropriate for the height of this text
                    texts.back().move(0.0f, calcDownShiftForCharHeight(TEXT));

                    // stretch the bounding region of this line to accomodate for the shift above
                    const auto TOP_BEFORE { region.top };
                    region = MinimallyEnclosing(region, texts.back().getGlobalBounds(), true);
                    const auto BOTTOM_AFTER { Bottom(region) };
                    region.top = TOP_BEFORE;
                    region.height = (BOTTOM_AFTER - TOP_BEFORE);
                }

                float VerticalStretchToLineSpacing()
                {
                    if (Empty())
                    {
                        return 0.0f;
                    }

                    const auto LARGEST_LINE_SPACING { LargestLineSpacing() };
                    const auto TOP_GAP { topGap() };
                    const auto TARGET_LINE_SPACING { (LARGEST_LINE_SPACING - TOP_GAP) };
                    const auto DOWN_SHIFT_FULL { (TARGET_LINE_SPACING - region.height) };

                    const auto FONT_SIZE_F { std::max(
                        20.0f, static_cast<float>(texts.front().getCharacterSize())) };

                    const auto FONT_SIZE_MULT { 1.0f
                                                - (FONT_SIZE_F / std::max(40.0f, FONT_SIZE_F)) };

                    const auto DOWN_SHIFT_FINAL { (DOWN_SHIFT_FULL * 2.0f) * FONT_SIZE_MULT };

                    for (auto & text : texts)
                    {
                        text.move(0.0f, DOWN_SHIFT_FINAL);
                    }

                    region.height += DOWN_SHIFT_FINAL;
                    return DOWN_SHIFT_FINAL;
                }

                // TODO TEMP REMOVE
                const std::string ToString() const
                {
                    std::ostringstream ss;
                    ss << "{[";

                    if (texts.empty())
                    {
                        ss << "EMPTY";
                    }
                    else
                    {
                        for (const auto & TEXT : texts)
                        {
                            ss << ((TEXT.IsValid()) ? "" : "I")
                               << GuiFont::ToUnderlyingType(TEXT.getFont()) << "("
                               << TEXT.getGlobalBounds() << ")\"" << TEXT.getString()
                               << "\"dsfch=" << calcDownShiftForCharHeight(TEXT) << "   ";
                        }
                    }

                    ss << "]gap=" << topGap() << region << "}";
                    return ss.str();
                }

            private:
                // distance between the line top (region.top) and the tallest char top
                float topGap() const
                {
                    float highestCharPosTop { Bottom(region) };

                    for (const auto & TEXT : texts)
                    {
                        if (highestCharPosTop > TEXT.getGlobalBounds().top)
                        {
                            highestCharPosTop = TEXT.getGlobalBounds().top;
                        }
                    }

                    return (highestCharPosTop - region.top);
                }

                float LargestLineSpacing() const
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

                float calcDownShiftForCharHeight(const Text & TEXT) const
                {
                    if (TEXT.empty())
                    {
                        return 0.0f;
                    }

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

                // rendered text
                std::vector<sfml_util::Text> texts;

                // size=zero if Empty(), otherwise defines the minimally enclosing rect
                sf::FloatRect region;
            };

            // parsed text of a single font
            struct ParsedTextSegment
            {
                ParsedTextSegment(
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

            // a collection of ParsedTextLines that make up a complete parse
            struct ParsedTextLines
            {
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

                ParsedTextLineVec_t::const_iterator begin() const noexcept
                {
                    return std::begin(lines);
                }
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

            // A simple wrapper of the result(s) of calling CleanText().
            struct CleanedText
            {
                std::string text;
                std::size_t newline_count;
                GuiFont::Enum uniform_font;
            };

            // Responsible for wrapping all the information required by Render() that does not
            // change.
            struct Context
            {
                Context(
                    const gui::TextInfo & TEXT_INFO,
                    const CleanedText & CLEANED_TEXT,
                    const float WIDTH_LIMIT)
                    : text_info(TEXT_INFO)
                    , cleaned_text(CLEANED_TEXT)
                    , width_limit(WIDTH_LIMIT)
                    , tallest_char_height(0.0f)
                {
                    const Text TALLEST_CHAR_TEXT(
                        std::string(1, CharInfo::whitespace_placeholder_char),
                        TEXT_INFO.font_letters,
                        TEXT_INFO.size);

                    tallest_char_height = (TALLEST_CHAR_TEXT.getGlobalBounds().height / 1.2f);
                }

                Context(const Context &) = default;
                Context(Context &&) = default;
                Context & operator=(const Context &) = default;
                Context & operator=(Context &&) = default;

                bool HasWidthLimit() const { return (width_limit > 0.0f); }

                float LineSpacing() const;

                gui::TextInfo text_info;
                CleanedText cleaned_text;
                float width_limit;
                float tallest_char_height;
            };

            // Responsible for implementing multiple lines of rendered text.
            struct Render : public sf::Drawable
            {
                Render()
                    : lines()
                    , region()
                    , justifyOffsetV()
                {}

                Render(const Render &) = default;
                Render(Render &&) = default;
                Render & operator=(const Render &) = default;
                Render & operator=(Render &&) = default;

                void draw(sf::RenderTarget & target, sf::RenderStates states) const final
                {
                    for (const auto & LINE : lines)
                    {
                        target.draw(LINE, states);
                    }
                }

                bool Empty() const { return lines.empty(); }

                bool IsValid() const
                {
                    return ((Empty() == false) && (IsSizeZeroOrLessEither(region) == false));
                }

                bool IsCurrentLineEmpty() const
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

                const sf::FloatRect Region() const { return region; }

                const sf::FloatRect CalcScreenRegion(const sf::Vector2f & SCREEN_POS_V) const
                {
                    const auto ACTUAL_POSITION { (
                        SCREEN_POS_V + Position(region) + justifyOffsetV) };

                    const auto ACTUAL_SIZE { Size(region) };

                    return sf::FloatRect(ACTUAL_POSITION, ACTUAL_SIZE);
                }

                // returns [0, WIDTH_LIMIT], returns WIDTH_LIMIT if lines.empty()
                float CurrentLineWidthRemaining(const float WIDTH_LIMIT) const
                {
                    if (lines.empty())
                    {
                        return WIDTH_LIMIT;
                    }

                    const auto WIDTH_REMAINING { (WIDTH_LIMIT - Right(lines.back().Region())) };
                    return std::clamp(WIDTH_REMAINING, 0.0f, WIDTH_LIMIT);
                }

                void AppendEmptyLine()
                {
                    auto posLeft { region.left };
                    auto posTop { Bottom(region) };

                    if (lines.empty() == false)
                    {
                        posTop += 1.0f;
                    }

                    lines.emplace_back(RenderedLine(sf::Vector2f(posLeft, posTop)));
                }

                void CurrentLineAppend(const sfml_util::Text & TEXT)
                {
                    CurrentLine().Append(TEXT);
                    region = MinimallyEnclosing(region, CurrentLine().Region(), true);
                }

                void AppendBlankLine(const float BLANK_LINE_HEIGHT)
                {
                    AppendEmptyLine();
                    lines.back().SetupAsBlankLine();
                    region.height += BLANK_LINE_HEIGHT;

                    // need to add a little harmless width so that MinimallyEnclosing() will not
                    // ignore region
                    if ((region.width > 0.0f) == false)
                    {
                        region.width = 0.1f;
                    }
                }

                void Move(const sf::Vector2f & MOVE_V)
                {
                    for (auto & line : lines)
                    {
                        line.Move(MOVE_V);
                    }

                    sfml_util::Move(region, MOVE_V);
                }

                void SetPosition(const sf::Vector2f & POS_V) { Move(POS_V - Position(region)); }

                void FinalAlignmentAndSpacing(const Context & CONTEXT)
                {
                    SetLineSpacing();
                    Justify(CONTEXT.text_info.justified, CONTEXT.width_limit);
                }

                const std::string ToString() const
                {
                    std::ostringstream ss;
                    ss << "\nRender (" << lines.size() << ")  region=" << region << "\n\t";
                    for (const auto & LINE : lines)
                    {
                        ss << LINE.ToString() << "\n\t";
                    }
                    return ss.str();
                }

            private:
                RenderedLine & CurrentLine()
                {
                    if (Empty())
                    {
                        AppendEmptyLine();
                    }

                    return lines.back();
                }

                float JustifyHorizOffset(
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

                // does nothing if JUSTIFY is invalid or Left, calling more than once results in
                // incorrect positions, sets justifyOffsetV so that calling FinalRegion() will
                // return the actual final region, the position of region is not changed to reflect
                // justifyOffsetV so that this object can be drawn to an sf::RenderTexture at
                // position (0,0)
                void Justify(const Justified::Enum JUSTIFICATION, const float WIDTH_LIMIT)
                {
                    const auto ORIG_POS_V { Position(region) };

                    if (Empty() || !Justified::IsValid(JUSTIFICATION)
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
                        region = MinimallyEnclosing(region, line.Region(), true);
                    }

                    justifyOffsetV = (Position(region) - ORIG_POS_V);
                    SetPosition(ORIG_POS_V);
                }

                void SetLineSpacing()
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

                std::vector<RenderedLine> lines;
                sf::FloatRect region;
                sf::Vector2f justifyOffsetV;
            };

            // cleans TEXT_INFO.text by removing non-displayable chars etc.
            const CleanedText CleanText(const gui::TextInfo &);

            // does not destroy/re-create the RenderTexture if it is already big enough
            void RenderToTexture(const Render &, sf::RenderTexture &);

            // splits CONTEXT.Text() by font into ParsedTextSegments
            const ParsedTextLine ParseAllAndSplitByFont(const Context &);

            void ParseAllAndSplitByNewlines(
                const Context &, const ParsedTextSegment &, ParsedTextLines &);

            const ParsedTextLines
                ParseAllAndSplitByNewlines(const Context &, const ParsedTextLine &);

            // splits by word-break-chars, after the breaking char unless that char is a dash
            // For example: "Hello Flat-Earthers"  -> "Hello", " Flat", "-", "Earthers"
            const ParsedTextLine ParseAndSplitByWord(const Context &, const ParsedTextSegment &);
            const ParsedTextLine ParseAndSplitByWord(const Context &, const ParsedTextLine &);
            const ParsedTextLines ParseAndSplitByWord(const Context &, const ParsedTextLines &);

            // at this point all the newline chars have been split into their own ParsedTextSegments
            const ParsedTextLines ParseAndSplitByLine(const ParsedTextLine &);

            // the parsing functions above can result in segments that are empty or that only
            // contain whitespace, this function collects them and prepends them to the front
            //  of the next segment
            const ParsedTextLines ParseAndCollectWhitespace(const ParsedTextLines &);

            const ParsedTextLines ParseAll(const Context &);

            void RenderParsedTextSegment(const Context &, const ParsedTextSegment &, Render &);
            void RenderParsedTextLines(const Context &, const ParsedTextLines &, Render &);
            void RenderAll(const Context &, const ParsedTextLines &, Render &);

            const Render ParseAndRender(const Context &);

            void DrawToTexture(const Render &, sf::RenderTexture &);

        } // namespace engine

        using RenderTextureUPtr_t = std::unique_ptr<sf::RenderTexture>;

        // if returns true then then renderTextureUPtr and *renderTextureUPtr were not changed,
        // REGION.height is ignored/not-enforced, REGION.width is ignored/not-enforced if <=
        // 0.0f, the caller is responsible for checking if Result.region != REGION because
        // Justification can change position, the RenderTexture might have been re-used so its
        // size might not match Size(REGION) or Size(Result.region)
        bool Render(
            const gui::TextInfo &,
            const sf::FloatRect &,
            RenderTextureUPtr_t &,
            sf::FloatRect & actualRegion);

        // same as Render() above only this version will setup the sprite on success
        bool Render(
            const gui::TextInfo &, const sf::FloatRect &, RenderTextureUPtr_t &, sf::Sprite &);

    } // namespace text_render
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTRENDERING_HPP_INCLUDED
