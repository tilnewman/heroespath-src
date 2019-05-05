// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render-char-info.cpp
//
#include "text-render-char-info.hpp"

#include "misc/enum-util.hpp"

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {

        CharInfo::CharInfo(
            const char CHAR, const TextInfo & TEXT_INFO, const GuiFont::Enum PREV_FONT)
            : CharInfo(CHAR, TEXT_INFO.font_letters, TEXT_INFO.font_numbers, PREV_FONT)
        {}

        CharInfo::CharInfo(
            const char CHAR,
            const GuiFont::Enum LETTER_FONT,
            const GuiFont::Enum NUMBER_FONT,
            const GuiFont::Enum PREV_FONT)
            : which_char(CHAR)
            , is_newline(CHAR == '\n')
            , is_whitespace((CHAR == ' ') || (CHAR == '\t'))
            , is_dash(CHAR == '-')
            , is_word_break(is_newline || is_whitespace || is_dash)
            , is_renderable(is_word_break || ((CHAR >= 32) && (CHAR <= 126)))
            , requires_neither_font(is_word_break || (CHAR == '.') || (CHAR == '+'))
            , requires_number_font(misc::IsDigit(CHAR))
            , requires_letter_font(!requires_number_font && !requires_neither_font)
            , font(GuiFont::Count)
        {
            if (requires_letter_font && EnumUtil<GuiFont>::IsValid(LETTER_FONT))
            {
                font = LETTER_FONT;
            }
            else if (requires_number_font && EnumUtil<GuiFont>::IsValid(NUMBER_FONT))
            {
                font = NUMBER_FONT;
            }
            else
            {
                font = PREV_FONT;
            }
        }

    } // namespace text_rendering
} // namespace gui
} // namespace heroespath
