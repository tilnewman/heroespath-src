// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXT_RENDER_RENDERED_LINES_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXT_RENDER_RENDERED_LINES_HPP_INCLUDED
//
// text-render-rendered-lines.hpp
//
#include "sfml-util/text-render-context.hpp"
#include "sfml-util/text-render-rendered-line.hpp"
#include "sfml-util/text.hpp"

#include <SFML/Graphics/Drawable.hpp>

#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        // RenderedLinesing takes ParsedText and makes sfml_util::Text objects (sf::Text wrappers)

        // Multiple lines of rendered text
        class RenderedLines : public sf::Drawable
        {
        public:
            RenderedLines();

            RenderedLines(const RenderedLines &) = default;
            RenderedLines(RenderedLines &&) = default;
            RenderedLines & operator=(const RenderedLines &) = default;
            RenderedLines & operator=(RenderedLines &&) = default;

            void draw(sf::RenderTarget &, sf::RenderStates) const final;

            bool Empty() const { return lines.empty(); }
            bool IsValid() const;
            bool IsCurrentLineEmpty() const;
            const sf::FloatRect Region() const { return region; }
            const sf::FloatRect CalcScreenRegion(const sf::Vector2f & SCREEN_POS_V) const;

            // returns [0, WIDTH_LIMIT], returns WIDTH_LIMIT if lines.empty()
            float CurrentLineWidthRemaining(const float WIDTH_LIMIT) const;

            void AppendEmptyLine();
            void CurrentLineAppend(const sfml_util::Text & TEXT);
            void AppendBlankLine(const float BLANK_LINE_HEIGHT);
            void Move(const sf::Vector2f & MOVE_V);
            void SetPosition(const sf::Vector2f & POS_V) { Move(POS_V - Position(region)); }
            void FinalAlignmentAndSpacing(const Context & CONTEXT);
            const std::string ToString() const;

        private:
            RenderedLine & CurrentLine();

            float JustifyHorizOffset(
                const Justified::Enum JUSTIFICATION,
                const float WIDTH_LIMIT,
                const float TOTAL_WIDTH,
                const float LINE_WIDTH);

            // does nothing if JUSTIFY is invalid or Left, calling more than once results in
            // incorrect positions, sets justifyOffsetV so that calling FinalRegion() will
            // return the actual final region, the position of region is not changed to reflect
            // justifyOffsetV so that this object can be drawn to an sf::RenderedLinesTexture at
            // position (0,0)
            void Justify(const Justified::Enum JUSTIFICATION, const float WIDTH_LIMIT);

            void SetLineSpacing();

            std::vector<RenderedLine> lines;
            sf::FloatRect region;
            sf::Vector2f justifyOffsetV;
        };

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXT_RENDER_RENDERED_LINES_HPP_INCLUDED
