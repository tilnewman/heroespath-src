// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXT_RENDER_CHAR_INFO_HPP_INCLUDED
#define HEROESPATH_GUI_TEXT_RENDER_CHAR_INFO_HPP_INCLUDED
//
// text-render-char-info.hpp
//
#include "gui/font-enum.hpp"
#include "gui/text-info.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {

        // Responsible for making render related decisions about a single character.
        struct CharInfo
        {
            CharInfo(
                const char CHAR,
                const TextInfo & TEXT_INFO,
                const GuiFont::Enum PREV_FONT = GuiFont::Count);

            explicit CharInfo(
                const char CHAR,
                const GuiFont::Enum LETTER_FONT = GuiFont::Count,
                const GuiFont::Enum NUMBER_FONT = GuiFont::Count,
                const GuiFont::Enum PREV_FONT = GuiFont::Count);

            CharInfo(const CharInfo &) = default;
            CharInfo(CharInfo &&) = default;
            CharInfo & operator=(const CharInfo &) = default;
            CharInfo & operator=(CharInfo &&) = default;

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

    } // namespace text_rendering
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_RENDER_CHAR_INFO_HPP_INCLUDED
