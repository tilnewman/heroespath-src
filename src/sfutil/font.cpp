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

#include "misc/strings.hpp"

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

    const std::string ToString(const sf::Font & F, const misc::ToStringPrefix::Enum OPTIONS)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix(OPTIONS, "Font") << F;
        return ss.str();
    }

    const std::string TextStyleToString(const sf::Uint32 TEXT_STYLE)
    {
        if (TEXT_STYLE == sf::Text::Style::Regular)
        {
            return "Regular";
        }

        std::ostringstream ss;

        auto appendSeparatorAndString = [&](const std::string & STR) {
            if (ss.str().empty() == false)
            {
                ss << "/";
            }

            ss << STR;
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

        if (ss.str().empty())
        {
            ss << "(error_unknown_sf_text_style=" << TEXT_STYLE << ")";
        }

        return ss.str();
    }

} // namespace sfutil
} // namespace heroespath
