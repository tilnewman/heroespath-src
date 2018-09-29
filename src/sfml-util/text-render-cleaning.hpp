// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXT_RENDER_CLEANING_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXT_RENDER_CLEANING_HPP_INCLUDED
//
// text-render-cleaning.hpp
//
#include "sfml-util/font-enum.hpp"
#include "sfml-util/gui/text-info.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        // cleans TEXT_INFO.text by removing non-displayable chars etc.
        struct TextCleaner
        {
            TextCleaner(const TextInfo &);

            TextCleaner(const TextCleaner &) = default;
            TextCleaner(TextCleaner &&) = default;
            TextCleaner & operator=(const TextCleaner &) = default;
            TextCleaner & operator=(TextCleaner &&) = default;

            std::string text;
            std::size_t newline_count;
            GuiFont::Enum uniform_font;
        };

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXT_RENDER_CLEANING_HPP_INCLUDED
