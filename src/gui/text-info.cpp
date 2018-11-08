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

#include "gui/font-manager.hpp"
#include "gui/text.hpp"
#include "misc/enum-util.hpp"
#include "misc/strings.hpp"
#include "sfutil/color.hpp"
#include "sfutil/font.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace gui
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

    bool TextInfo::IsValid() const
    {
        return (
            (text.empty() == false) && EnumUtil<GuiFont>::IsValid(font_letters)
            && EnumUtil<GuiFont>::IsValid(font_numbers) && (size > 0));
    }

    const std::string TextInfo::ToString(
        const bool WILL_PREFIX, const Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        auto appendWithSeparator = [&](std::ostringstream & strstrm, const auto & THING) {
            if (strstrm.str().empty() == false)
            {
                strstrm << SEPARATOR;
            }

            strstrm << THING;
        };

        std::ostringstream ssInfo;
        std::ostringstream ssInvalid;

        if (*this == TextInfo())
        {
            appendWithSeparator(ssInvalid, "default_constructed");
        }

        if (WillDraw() == false)
        {
            appendWithSeparator(ssInvalid, "will_draw_false");
        }
        else if (IsValid() == false)
        {
            appendWithSeparator(ssInvalid, "is_valid_false");
        }

        if (text.empty())
        {
            appendWithSeparator(ssInvalid, "text_empty");
        }
        else
        {
            appendWithSeparator(ssInfo, misc::Quoted(misc::MakeLoggableString(text)));
        }

        if (EnumUtil<GuiFont>::IsValid(font_letters))
        {
            appendWithSeparator(ssInfo, GuiFont::ToString(font_letters));
        }
        else
        {
            appendWithSeparator(
                ssInvalid,
                "letter_font=" + misc::ToString(EnumUtil<GuiFont>::ToUnderlyingType(font_letters)));
        }

        if (GuiFont::Number != font_numbers)
        {
            if (EnumUtil<GuiFont>::IsValid(font_numbers))
            {
                appendWithSeparator(ssInfo, GuiFont::ToString(font_numbers));
            }
            else
            {
                appendWithSeparator(
                    ssInvalid,
                    "letter_font="
                        + misc::ToString(EnumUtil<GuiFont>::ToUnderlyingType(font_numbers)));
            }
        }

        appendWithSeparator(ssInfo, misc::ToString(size));
        appendWithSeparator(ssInfo, sfutil::ToString(color, misc::ToStringPrefix::SimpleName));

        if (EnumUtil<gui::Justified>::IsValid(justified))
        {
            appendWithSeparator(ssInfo, gui::Justified::ToString(justified));
        }
        else
        {
            appendWithSeparator(
                ssInvalid,
                "justify=" + misc::ToString(EnumUtil<Justified>::ToUnderlyingType(justified)));
        }

        if (0 != style)
        {
            std::ostringstream ssStyle;

            if (style & sf::Text::Style::Bold)
            {
                appendWithSeparator(ssStyle, "Bold");
            }

            if (style & sf::Text::Style::Italic)
            {
                appendWithSeparator(ssStyle, "Italic");
            }

            if (style & sf::Text::Style::Underlined)
            {
                appendWithSeparator(ssStyle, "Underlined");
            }

            if (style & sf::Text::Style::StrikeThrough)
            {
                appendWithSeparator(ssStyle, "StrikeThrough");
            }

            if (ssStyle.str().empty())
            {
                appendWithSeparator(ssInvalid, "unknown_style=" + misc::ToString(style));
            }
            else
            {
                appendWithSeparator(ssInfo, ssStyle.str());
            }
        }

        std::ostringstream ssFinal;

        if (WILL_WRAP == Wrap::Yes)
        {
            ssFinal << "(";
        }

        if (ssInfo.str().empty() == false)
        {
            ssFinal << ssInfo.str();
        }

        if (ssInvalid.str().empty() == false)
        {
            appendWithSeparator(ssFinal, ssInvalid.str());
        }

        if (WILL_WRAP == Wrap::Yes)
        {
            ssFinal << ")";
        }

        if (WILL_PREFIX)
        {
            return std::string("TextInfo").append((WILL_WRAP == Wrap::Yes) ? "" : "=")
                + ssFinal.str();
        }
        else
        {
            return ssFinal.str();
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

} // namespace gui
} // namespace heroespath
