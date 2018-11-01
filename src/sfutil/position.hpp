// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_POSITION_HPP_INCLUDED
#define HEROESPATH_SFUTIL_POSITION_HPP_INCLUDED
//
// position.hpp
//
#include <SFML/Graphics/Rect.hpp>

namespace sf
{
class Sprite;
} // namespace sf

namespace heroespath
{
namespace gui
{
    class Text;
}
namespace sfutil
{

    template <typename T>
    constexpr const sf::Vector2<T> Position(const sf::Rect<T> & R)
    {
        return { R.left, R.top };
    }

    const sf::Vector2f Position(const sf::Sprite &);
    const sf::Vector2f Position(const gui::Text &);

    template <typename T>
    constexpr T Left(const sf::Rect<T> & RECT)
    {
        return RECT.left;
    }

    float Left(const sf::Sprite &);
    float Left(const gui::Text &);

    template <typename T>
    constexpr T Right(const sf::Rect<T> & RECT)
    {
        return (RECT.left + RECT.width);
    }

    float Right(const sf::Sprite &);
    float Right(const gui::Text &);

    template <typename T>
    constexpr T Top(const sf::Rect<T> & RECT)
    {
        return RECT.top;
    }

    float Top(const sf::Sprite &);
    float Top(const gui::Text &);

    template <typename T>
    constexpr T Bottom(const sf::Rect<T> & RECT)
    {
        return (RECT.top + RECT.height);
    }

    float Bottom(const sf::Sprite &);
    float Bottom(const gui::Text &);

    template <typename T>
    const sf::Vector2<T> BottomRight(const sf::Rect<T> & R)
    {
        return sf::Vector2<T>(Right(R), Bottom(R));
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_POSITION_HPP_INCLUDED
