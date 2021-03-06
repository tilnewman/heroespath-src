// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// font.cpp
//
#include "font.hpp"

#include <SFML/Graphics/Text.hpp>

#include <ostream>

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Font & F)
{
    os << "(\"" << F.getInfo().family << "\")";
    return os;
}
} // namespace sf

namespace heroespath
{
namespace sfutil
{

    const std::string TextStyleToString(const sf::Uint32 TEXT_STYLE)
    {
        std::string result;
        result.reserve(64);
        if (TEXT_STYLE == sf::Text::Style::Regular)
        {
            result = "Regular";
        }
        else
        {
            auto appendSeparatorAndString = [&](const std::string & STR) {
                if (result.empty() == false)
                {
                    result += '/';
                }

                result += STR;
            };

            if (TEXT_STYLE & sf::Text::Style::Bold)
            {
                appendSeparatorAndString("Bold");
            }

            if (TEXT_STYLE & sf::Text::Style::Italic)
            {
                appendSeparatorAndString("Italic");
            }

            if (TEXT_STYLE & sf::Text::Style::StrikeThrough)
            {
                appendSeparatorAndString("StrikeThrough");
            }

            if (TEXT_STYLE & sf::Text::Style::Underlined)
            {
                appendSeparatorAndString("Underlined");
            }

            if (result.empty())
            {
                result += "(error_unknown_sf_text_style=";
                result += std::to_string(TEXT_STYLE);
                result += ')';
            }
        }

        return result;
    }

} // namespace sfutil
} // namespace heroespath
