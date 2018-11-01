// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXT_RENDER_RENDERED_LINE_HPP_INCLUDED
#define HEROESPATH_GUI_TEXT_RENDER_RENDERED_LINE_HPP_INCLUDED
//
// text-render-rendered-line.hpp
//
#include "sfml-util/text-render-context.hpp"
#include "sfml-util/text.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/Drawable.hpp>

#include <vector>

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {

        // Rendering takes ParsedText and makes gui::Text objects (sf::Text wrappers)

        // a single line of rendered text where font/color/style/etc may vary
        class RenderedLine : public sf::Drawable
        {
        public:
            explicit RenderedLine(const sf::Vector2f & POS_V);

            RenderedLine(const RenderedLine &) = default;
            RenderedLine(RenderedLine &&) = default;
            RenderedLine & operator=(const RenderedLine &) = default;
            RenderedLine & operator=(RenderedLine &&) = default;

            void draw(sf::RenderTarget &, sf::RenderStates) const final;

            bool Empty() const { return texts.empty(); }
            const sf::FloatRect Region() const { return region; }
            void Move(const sf::Vector2f &);
            void SetPosition(const sf::Vector2f & POS_V) { Move(POS_V - sfutil::Position(region)); }
            void SetupAsBlankLine();
            void Append(const gui::Text &);
            float VerticalStretchToLineSpacing();
            const std::string ToString() const;

        private:
            // distance between the line top (region.top) and the tallest char top
            float topGap() const;

            float LargestLineSpacing() const;
            float calcDownShiftForCharHeight(const Text &) const;

            // rendered text
            std::vector<gui::Text> texts;

            // size=zero if Empty(), otherwise defines the minimally enclosing rect
            sf::FloatRect region;
        };

    } // namespace text_rendering
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_RENDER_RENDERED_LINE_HPP_INCLUDED
