// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_COLOR_HPP_INCLUDED
#define HEROESPATH_SFUTIL_COLOR_HPP_INCLUDED
//
// color.hpp
//
#include <SFML/Graphics/Color.hpp>

#include <ostream>
#include <tuple>

namespace sf
{

inline bool operator<(const sf::Color & L, const sf::Color & R)
{
    return std::tie(L.r, L.g, L.b, L.a) < std::tie(R.r, R.g, R.b, R.a);
}

inline bool operator>(const sf::Color & L, const sf::Color & R) { return (R < L); }
inline bool operator<=(const sf::Color & L, const sf::Color & R) { return !(L > R); }
inline bool operator>=(const sf::Color & L, const sf::Color & R) { return !(L < R); }

// operator== already provided by sfml

std::ostream & operator<<(std::ostream & os, const sf::Color & C);

} // namespace sf

namespace heroespath
{
namespace sfutil
{

    namespace color
    {
        const sf::Color GrayLight { 200, 200, 200 };
        const sf::Color GrayLighter { 232, 232, 232 };
        const sf::Color GrayDark { 100, 100, 100 };
        const sf::Color GrayDarker { 64, 64, 64 };
        const sf::Color Orange { 255, 223, 181 };
        const sf::Color Light { 220, 220, 220 };
        const sf::Color GoldLight { 255, 248, 220 };
        const sf::Color FocusIn { 220, 220, 220 };
        const sf::Color FocusOut { 180, 180, 180 };
    } // namespace color

    // return a color between FROM and TO at the given RATIO
    inline const sf::Color
        Transition(const sf::Color & FROM, const sf::Color & TO, const float RATIO)
    {
        auto calcRGBAdjustment = [&](const sf::Uint8 A, const sf::Uint8 B) {
            return static_cast<int>((static_cast<float>(B) - static_cast<float>(A)) * RATIO);
        };

        auto adjustRGBValue = [&](sf::Uint8 & f, const sf::Uint8 T) {
            f = static_cast<sf::Uint8>(static_cast<int>(f) + calcRGBAdjustment(f, T));
        };

        sf::Color result = FROM;

        adjustRGBValue(result.r, TO.r);
        adjustRGBValue(result.g, TO.g);
        adjustRGBValue(result.b, TO.b);
        adjustRGBValue(result.a, TO.a);

        return result;
    }

    inline bool IsEqualWithoutAlpha(const sf::Color & L, const sf::Color & R)
    {
        return std::tie(L.r, L.g, L.b) == std::tie(R.r, R.g, R.b);
    }

    inline bool IsLessWithoutAlpha(const sf::Color & L, const sf::Color & R)
    {
        return std::tie(L.r, L.g, L.b) < std::tie(R.r, R.g, R.b);
    }

} // namespace sfutil

} // namespace heroespath

#endif // HEROESPATH_SFUTIL_COLOR_HPP_INCLUDED
