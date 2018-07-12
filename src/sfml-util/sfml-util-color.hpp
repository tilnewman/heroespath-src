// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_COLOR_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_COLOR_HPP_INCLUDED
//
// sfml-util-color.hpp
//
#include <SFML/Graphics/Color.hpp>

#include <tuple>

namespace sf
{

class Text;

inline constexpr bool operator<(const sf::Color & L, const sf::Color & R)
{
    return std::tie(L.r, L.g, L.b, L.a) < std::tie(R.r, R.g, R.b, R.a);
}

inline constexpr bool operator>(const sf::Color & L, const sf::Color & R) { return (R < L); }

inline constexpr bool operator<=(const sf::Color & L, const sf::Color & R) { return !(L > R); }

inline constexpr bool operator>=(const sf::Color & L, const sf::Color & R) { return !(L < R); }

} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    // linux SFML lib does not seem to support outline fonts...
    void SetColor(sf::Text & text, const sf::Color & COLOR);

    // return a color between FROM and TO at the given RATIO
    const sf::Color Transition(const sf::Color & FROM, const sf::Color & TO, const float RATIO);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_COLOR_HPP_INCLUDED
