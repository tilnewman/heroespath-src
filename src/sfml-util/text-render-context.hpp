// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXT_RENDER_CONTEXT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXT_RENDER_CONTEXT_HPP_INCLUDED
//
// text-render-context.hpp
//
#include "sfml-util/text-info.hpp"
#include "sfml-util/text-render-cleaning.hpp"
#include "sfml-util/text.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        // Responsible for wrapping all the information required to render text that does not
        // change during the parsing or rendering.
        struct Context
        {
            Context(
                const TextInfo & TEXT_INFO,
                const TextCleaner & TEXT_CLEANER,
                const float WIDTH_LIMIT)
                : text_info(TEXT_INFO)
                , text_cleaner(TEXT_CLEANER)
                , width_limit(WIDTH_LIMIT)
                , blank_line_height(0.0f)
            {
                const Text TEMP_TEXT("ignored", TEXT_INFO.font_letters, TEXT_INFO.size);
                blank_line_height = (TEMP_TEXT.getLineSpacing() / 1.35f);
            }

            Context(const Context &) = default;
            Context(Context &&) = default;
            Context & operator=(const Context &) = default;
            Context & operator=(Context &&) = default;

            bool HasWidthLimit() const { return (width_limit > 0.0f); }

            TextInfo text_info;
            TextCleaner text_cleaner;
            float width_limit;
            float blank_line_height;
        };

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXT_RENDER_CONTEXT_HPP_INCLUDED
