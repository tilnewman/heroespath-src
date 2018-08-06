// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_OF_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_OF_HPP_INCLUDED
//
// sfml-util-center-of.hpp
//
#include "sfml-util/sfml-util-position.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace sfml_util
{

    // returns the center of R, same as (R.left + (R.width / 2)) and (R.top + (R.height / 2))
    template <typename T>
    constexpr const sf::Vector2<T> CenterOf(const sf::Rect<T> & R)
    {
        return sf::Vector2<T>(sf::Vector2f(Position(R)) + (sf::Vector2f(R.width, R.height) * 0.5f));
    }

    // returns the horizontal center of R, same as (R.left + (R.width / 2))
    template <typename T>
    constexpr T CenterOfHoriz(const sf::Rect<T> & R)
    {
        return static_cast<T>(static_cast<float>(R.left) + (static_cast<float>(R.width) * 0.5f));
    }

    // returns the vertical center of R, same as (R.top + (R.height / 2))
    template <typename T>
    constexpr T CenterOfVert(const sf::Rect<T> & R)
    {
        return static_cast<T>(static_cast<float>(R.top) + (static_cast<float>(R.height) * 0.5f));
    }

    // returns the center of S (global), same as Center(S.getGlobalBounds())
    const sf::Vector2f CenterOf(const sf::Sprite & S);

    // returns the horizontal center of S (global)
    float CenterOfHoriz(const sf::Sprite & S);

    // returns the vertical center of S (global)
    float CenterOfVert(const sf::Sprite & S);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_OF_HPP_INCLUDED
