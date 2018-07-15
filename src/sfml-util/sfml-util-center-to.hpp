// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_TO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_TO_HPP_INCLUDED
//
// sfml-util-center-to.hpp
//
#include "sfml-util/sfml-util-center-of.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{

    // returns a position that would center something of SIZE_V to the center of B
    template <typename T1, typename T2>
    constexpr const sf::Vector2<T1>
        CenterToCopy(const sf::Vector2<T1> & SIZE_V, const sf::Rect<T2> & B)
    {
        return sf::Vector2<T1>(CenterOf(sf::FloatRect(B)) - (sf::Vector2f(SIZE_V) * 0.5f));
    }

    // returns a copy of A rescaled to SCALE_V and then repositioned to the center of B
    template <
        typename T1,
        typename T2,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> CenterToCopy(
        const sf::Rect<T1> & A, const sf::Rect<T2> & B, const sf::Vector2<Scale_t> & SCALE_V)
    {
        const float NEW_WIDTH { static_cast<float>(A.width) * static_cast<float>(SCALE_V.x) };
        const float NEW_HEIGHT { static_cast<float>(A.height) * static_cast<float>(SCALE_V.y) };

        return sf::Rect<T1>(sf::FloatRect(
            (CenterOf(sf::FloatRect(B)) - (sf::Vector2f(NEW_WIDTH, NEW_HEIGHT) * 0.5f)),
            sf::Vector2f(NEW_WIDTH, NEW_HEIGHT)));
    }

    // returns a copy of A rescaled to SCALE and then repositioned to the center of B
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1>
        CenterToCopy(const sf::Rect<T1> & A, const sf::Rect<T2> & B, const Scale_t SCALE = 1.0f)
    {
        return CenterToCopy(A, B, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // rescales a to SCALE and then repositions it to the center of B
    template <
        typename T1,
        typename T2,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void
        CenterTo(sf::Rect<T1> & a, const sf::Rect<T2> & B, const sf::Vector2<Scale_t> & SCALE_V)
    {
        a = CenterToCopy(a, B, SCALE_V);
    }

    // rescales a to SCALE and then repositions it to the center of B
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void CenterTo(sf::Rect<T1> & a, const sf::Rect<T2> & B, const Scale_t SCALE = 1.0f)
    {
        CenterTo(a, B, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // rescales s (local) to SCALE_V and then repositions it to the center of R
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void CenterTo(sf::Sprite & s, const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        s.setScale(sf::Vector2f(SCALE_V));

        s.setPosition(
            CenterOf(sf::FloatRect(R))
            - sf::Vector2f(
                  s.getLocalBounds().width * SCALE_V.x * 0.5f,
                  s.getLocalBounds().height * SCALE_V.y * 0.5f));
    }

    // rescales s to SCALE and then repositions it to the center of B (global)
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void CenterTo(sf::Sprite & s, const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        CenterTo(s, R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_TO_HPP_INCLUDED
