// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-render-cleaning.cpp
//
#include "text-render-cleaning.hpp"

#include "sfml-util/text-render-char-info.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace text_rendering
    {

        TextCleaner::TextCleaner(const gui::TextInfo & TEXT_INFO)
            : text("")
            , newline_count(0)
            , uniform_font(GuiFont::Count)
        {
            auto isLetterFontRequired { false };
            auto isNumberFontRequired { false };

            // consider and then append characters one at a time
            for (const char CHAR_ORIG : TEXT_INFO.text)
            {
                const CharInfo CHAR_INFO(CHAR_ORIG, TEXT_INFO);

                if (CHAR_INFO.is_renderable == false)
                {
                    continue;
                }

                text += CHAR_INFO.which_char;

                if (CHAR_INFO.is_newline)
                {
                    ++newline_count;
                }
                else if (CHAR_INFO.requires_letter_font)
                {
                    isLetterFontRequired = true;
                }
                else if (CHAR_INFO.requires_number_font)
                {
                    isNumberFontRequired = true;
                }
            }

            // trim leading whitespace
            while ((text.empty() == false) && CharInfo(text.front()).is_whitespace)
            {
                text.erase(std::begin(text));
            }

            // trim trailing whitespace
            while ((text.empty() == false) && CharInfo(text.back()).is_whitespace)
            {
                text.pop_back();
            }

            if (isLetterFontRequired)
            {
                if (isNumberFontRequired)
                {
                    uniform_font = GuiFont::Count;
                }
                else
                {
                    uniform_font = TEXT_INFO.font_letters;
                }
            }
            else
            {
                if (isNumberFontRequired)
                {
                    uniform_font = TEXT_INFO.font_numbers;
                }
                else
                {
                    // default to using letter font if neither is required
                    uniform_font = TEXT_INFO.font_letters;
                }
            }
        }

    } // namespace text_rendering
} // namespace sfml_util
} // namespace heroespath
