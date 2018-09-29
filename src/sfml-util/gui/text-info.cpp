// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-info.cpp
//
#include "text-info.hpp"

#include "misc/strings.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sfml-util-color.hpp"
#include "sfml-util/sfml-util-font.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"
#include "sfml-util/text.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{

    TextInfo::TextInfo()
        : text("")
        , font_letters(GuiFont::Count)
        , font_numbers(GuiFont::Number)
        , size(FontManager::Instance()->Size_Normal())
        , color(sf::Color::White)
        , style(sf::Text::Style::Regular)
        , justified(Justified::Left)
    {}

    TextInfo::TextInfo(
        const std::string & TEXT,
        const GuiFont::Enum LETTERS_FONT,
        const unsigned int SIZE,
        const sf::Color & COLOR,
        const Justified::Enum JUSTIFIED,
        const sf::Uint32 STYLE)
        : text(TEXT)
        , font_letters(LETTERS_FONT)
        , font_numbers(GuiFont::Number)
        , size(SIZE)
        , color(COLOR)
        , style(STYLE)
        , justified(JUSTIFIED)
    {}

    TextInfo::TextInfo(
        const TextInfo & TEXT_INFO_TO_COPY,
        const std::string & TEXT,
        const sf::Color & COLOR,
        const unsigned SIZE,
        const GuiFont::Enum LETTERS_FONT,
        const GuiFont::Enum NUMBERS_FONT)
        : TextInfo(TEXT_INFO_TO_COPY)
    {
        if (TEXT.empty() == false)
        {
            text = TEXT;
        }

        if (COLOR != sf::Color::Transparent)
        {
            color = COLOR;
        }

        if (SIZE != 0)
        {
            size = SIZE;
        }

        if (LETTERS_FONT != GuiFont::Count)
        {
            font_letters = LETTERS_FONT;
        }

        if (NUMBERS_FONT != GuiFont::Count)
        {
            font_numbers = NUMBERS_FONT;
        }
    }

    // TextInfo::TextInfo(
    //    const TextInfo & TEXT_INFO_TO_COPY,
    //    const GuiFont::Enum NEW_LETTERS_FONT,
    //    const GuiFont::Enum NEW_NUMBERS_FONT)
    //    : TextInfo(
    //          TEXT_INFO_TO_COPY,
    //          "",
    //          sf::Color::Transparent,
    //          0,
    //          NEW_LETTERS_FONT,
    //          NEW_NUMBERS_FONT)
    //{}

    const std::string TextInfo::ToString(
        const bool WILL_PREFIX, const misc::Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::ostringstream ss;

        if (*this == TextInfo())
        {
            ss << "DEFAULT" << SEPARATOR << "INVALID" << SEPARATOR << "WONT_DRAW";
        }
        else
        {
            if (WillDraw() == false)
            {
                ss << "INVALID" << SEPARATOR << "WONT_DRAW";
            }
            else if (IsValid() == false)
            {
                ss << "INVALID";
            }

            ss << SEPARATOR;
            if (text.empty())
            {
                ss << "(invalid, text is empty)";
            }
            else
            {
                ss << misc::MakeLoggableString(text);
            }

            ss << SEPARATOR;
            if (GuiFont::IsValid(font_letters))
            {
                ss << GuiFont::ToString(font_letters);
            }
            else
            {
                ss << "(invalid letter_font=" << GuiFont::ToUnderlyingType(font_letters) << ")";
            }

            if (GuiFont::Number != font_numbers)
            {
                ss << SEPARATOR;
                if (GuiFont::IsValid(font_numbers))
                {
                    ss << GuiFont::ToString(font_numbers);
                }
                else
                {
                    ss << "(invalid number_font=" << GuiFont::ToUnderlyingType(font_numbers) << ")";
                }
            }

            ss << SEPARATOR << size;
            ss << SEPARATOR << sfml_util::ToString(color, misc::ToStringPrefix::SimpleName);

            ss << SEPARATOR;
            if (sfml_util::Justified::IsValid(justified))
            {
                ss << sfml_util::Justified::ToString(justified);
            }
            else
            {
                ss << "(invalid justify=" << Justified::ToUnderlyingType(justified) << ")";
            }

            if (0 != style)
            {
                auto separatorIfNotEmpty = [&](std::ostringstream & strstrm) -> std::string {
                    if (strstrm.str().empty())
                    {
                        return "";
                    }
                    else
                    {
                        return SEPARATOR;
                    }
                };

                std::ostringstream oss;

                // if (STYLE & sf::Text::Style::Regular)
                //     oss << "Regular";

                if (style & sf::Text::Style::Bold)
                {
                    oss << "Bold";
                }

                if (style & sf::Text::Style::Italic)
                {
                    oss << separatorIfNotEmpty(oss) << "Italic";
                }

                if (style & sf::Text::Style::Underlined)
                {
                    oss << separatorIfNotEmpty(oss) << "Underlined";
                }

                if (style & sf::Text::Style::StrikeThrough)
                {
                    oss << separatorIfNotEmpty(oss) << "StrikeThrough";
                }

                if (oss.str().empty())
                {
                    ss << "(invalid sf::Style=" << style << ")";
                }
                else
                {
                    ss << oss.str();
                }
            }
        }

        auto const PARTS_STR { (
            (WILL_WRAP == misc::Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("TextInfo").append((WILL_WRAP == misc::Wrap::Yes) ? "" : "=")
                + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
    }

    bool operator<(const TextInfo & L, const TextInfo & R)
    {
        return std::tie(
                   L.font_letters, L.font_numbers, L.justified, L.style, L.size, L.color, L.text)
            < std::tie(
                   R.font_letters, R.font_numbers, R.justified, R.style, R.size, R.color, R.text);
    }

    bool operator==(const TextInfo & L, const TextInfo & R)
    {
        return std::tie(
                   L.size, L.text, L.font_letters, L.color, L.justified, L.font_numbers, L.style)
            == std::tie(
                   R.size, R.text, R.font_letters, R.color, R.justified, R.font_numbers, R.style);
    }

} // namespace sfml_util
} // namespace heroespath
