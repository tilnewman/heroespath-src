// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXT_RENDER_RENDERING_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXT_RENDER_RENDERING_HPP_INCLUDED
//
// text-render-rendered-lines.hpp
//
#include "sfml-util/text-render-context.hpp"
#include "sfml-util/text-render-parsing.hpp"
#include "sfml-util/text-render-rendered-lines.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {
        // RenderedLinesing takes ParsedText and makes sfml_util::Text objects (sf::Text wrappers)
        struct Renderer
        {
            Renderer() = delete;

            static const RenderedLines Make(const Context &);

        private:
            static void RenderSegment(const Context &, const ParsedTextSegment &, RenderedLines &);
            static void RenderLines(const Context &, const ParsedTextLines &, RenderedLines &);
        };

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXT_RENDER_RENDERING_HPP_INCLUDED
