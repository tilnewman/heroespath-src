// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_POSITION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_POSITION_HPP_INCLUDED
//
// sfml-util-position.hpp
//
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{

    template <typename T>
    constexpr const sf::Vector2<T> Position(const sf::Rect<T> & R)
    {
        return { R.left, R.top };
    }

    inline const sf::Vector2f Position(const sf::Sprite & S) { return S.getPosition(); }

    template <typename T>
    constexpr T Right(const sf::Rect<T> & RECT)
    {
        return (RECT.left + RECT.width);
    }

    inline float Right(const sf::Sprite & SPRITE) { return Right(SPRITE.getGlobalBounds()); }

    template <typename T>
    constexpr T Bottom(const sf::Rect<T> & RECT)
    {
        return (RECT.top + RECT.height);
    }

    inline float Bottom(const sf::Sprite & SPRITE) { return Bottom(SPRITE.getGlobalBounds()); }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_POSITION_HPP_INCLUDED
