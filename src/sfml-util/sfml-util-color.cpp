// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-color.cpp
//
#include "sfml-util-color.hpp"

#include "misc/strings.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"

#include <SFML/Graphics/Text.hpp>

#include <tuple>

namespace sf
{

bool operator<(const sf::Color & L, const sf::Color & R)
{
    return std::tie(L.r, L.g, L.b, L.a) < std::tie(R.r, R.g, R.b, R.a);
}

bool operator>(const sf::Color & L, const sf::Color & R) { return (R < L); }

bool operator<=(const sf::Color & L, const sf::Color & R) { return !(L > R); }

bool operator>=(const sf::Color & L, const sf::Color & R) { return !(L < R); }

std::ostream & operator<<(std::ostream & os, const sf::Color & C)
{
    os << "(";

    if (sf::Color::Black == C)
    {
        os << "Black";
    }
    else if (sf::Color::White == C)
    {
        os << "White";
    }
    else if (sf::Color::Red == C)
    {
        os << "Red";
    }
    else if (sf::Color::Green == C)
    {
        os << "Green";
    }
    else if (sf::Color::Blue == C)
    {
        os << "Blue";
    }
    else if (sf::Color::Yellow == C)
    {
        os << "Yellow";
    }
    else if (sf::Color::Magenta == C)
    {
        os << "Magenta";
    }
    else if (sf::Color::Cyan == C)
    {
        os << "Cyan";
    }
    else
    {
        os << static_cast<unsigned>(C.r) << "," << static_cast<unsigned>(C.g) << ","
           << static_cast<unsigned>(C.b);

        if (C.a != 255)
        {
            os << "," << static_cast<unsigned>(C.a);
        }
    }

    os << ")";

    return os;
}
} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    const sf::Color MakeHighlight(const sf::Uint8 COLOR_VALUE)
    {
        return sf::Color(COLOR_VALUE, COLOR_VALUE, COLOR_VALUE, 0);
    }

    const std::string ToString(const sf::Color & C, const misc::ToStringPrefix::Enum OPTIONS)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix(OPTIONS, "Color") << C;
        return ss.str();
    }

    const sf::Color Transition(const sf::Color & FROM, const sf::Color & TO, const float RATIO)
    {
        auto const RED { static_cast<sf::Uint8>(
            FROM.r
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.r) - static_cast<float>(FROM.r)) * RATIO)) };

        auto const GREEN { static_cast<sf::Uint8>(
            FROM.g
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.g) - static_cast<float>(FROM.g)) * RATIO)) };

        auto const BLUE { static_cast<sf::Uint8>(
            FROM.b
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.b) - static_cast<float>(FROM.b)) * RATIO)) };

        auto const ALPHA { static_cast<sf::Uint8>(
            FROM.a
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.a) - static_cast<float>(FROM.a)) * RATIO)) };

        return sf::Color(RED, GREEN, BLUE, ALPHA);
    }

    bool IsEqualWithoutAlpha(const sf::Color & L, const sf::Color & R)
    {
        return std::tie(L.r, L.g, L.b) == std::tie(R.r, R.g, R.b);
    }

    bool IsLessWithoutAlpha(const sf::Color & L, const sf::Color & R)
    {
        return std::tie(L.r, L.g, L.b) < std::tie(R.r, R.g, R.b);
    }

} // namespace sfml_util
} // namespace heroespath
