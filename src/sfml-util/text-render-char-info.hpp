// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXT_RENDER_CHAR_INFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXT_RENDER_CHAR_INFO_HPP_INCLUDED
//
// text-render-char-info.hpp
//
#include "sfml-util/font-enum.hpp"
#include "sfml-util/gui/text-info.hpp"

#include <cctype> //for std::isdigit()

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        // Responsible for making render related decisions about a single character.
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
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXT_RENDER_CHAR_INFO_HPP_INCLUDED
