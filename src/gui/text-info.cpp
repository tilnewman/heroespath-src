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
        , font_letters(GuiFont::Default)
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

    bool TextInfo::IsValid() const
    {
        return (
            EnumUtil<GuiFont>::IsValid(font_letters) && EnumUtil<GuiFont>::IsValid(font_numbers));
    }

    const std::string TextInfo::ToString(
        const bool WILL_PREFIX, const Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::ostringstream ss;

        auto appendWithSeparator = [&](const auto & THING) {
            if (ss.str().empty() == false)
            {
                ss << SEPARATOR;
            }

            ss << THING;
        };

        if (*this == TextInfo())
        {
            ss << "default";
        }
        else
        {
            if (IsValid() == false)
            {
                appendWithSeparator("invalid");
            }
            else if (WillDraw() == false)
            {
                appendWithSeparator("not_visible");
            }

            appendWithSeparator(misc::Quoted(misc::MakeLoggableString(text)));
            appendWithSeparator("font=" + GuiFont::ToString(font_letters));
            appendWithSeparator("num_font=" + GuiFont::ToString(font_numbers));
            appendWithSeparator(sfutil::ToString(color, misc::ToStringPrefix::SimpleName));
            appendWithSeparator(gui::Justified::ToString(justified));
            appendWithSeparator(sfutil::TextStyleToString(style));
        }

        if (WILL_WRAP == Wrap::Yes)
        {
            ss.str("(" + ss.str() + ")");
        }

        if (WILL_PREFIX)
        {
            return std::string("TextInfo").append((WILL_WRAP == Wrap::Yes) ? "" : "=") + ss.str();
        }
        else
        {
            return ss.str();
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
