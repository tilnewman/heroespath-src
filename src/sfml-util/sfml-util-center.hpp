// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_HPP_INCLUDED
//
// sfml-util-center.hpp
//
#include "sfml-util/sfml-util-center-of.hpp"
#include "sfml-util/sfml-util-center-to.hpp"
#include "sfml-util/sfml-util-display.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{

    // returns the position at the center of the screen, same as (DisplaySize() * 0.5f)
    inline const sf::Vector2f Center() { return (DisplaySize() * 0.5f); }

    // returns the position that would center something of SIZE_V on the screen
    template <typename T>
    const sf::Vector2f CenterCopy(const sf::Vector2<T> & SIZE_V)
    {
        return CenterToCopy(sf::Vector2f(SIZE_V), DisplayRect());
    }

    // returns the position that would center something of size (WIDTH, HEIGHT) to the center of the
    // screen
    template <
        typename T,
        typename std::enable_if<
            ((std::is_integral<T> {} || std::is_floating_point<T> {})
             && (!std::is_same<T, bool> {})),
            int>::type
        = 0>
    const sf::Vector2f CenterCopy(const T WIDTH, const T HEIGHT)
    {
        return CenterCopy(sf::Vector2<T>(WIDTH, HEIGHT));
    }

    // retruns a copy of R rescaled to SCALE_V and then repositioned to the center of the screen
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    const sf::FloatRect CenterCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return CenterToCopy(sf::FloatRect(R), DisplayRect(), SCALE_V);
    }

    // retruns a copy of R rescaled to SCALE and then repositioned to the center of the screen
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    const sf::FloatRect CenterCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        return CenterCopy(sf::FloatRect(R), sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // rescales r to SCALE and then repositions to the center of the screen
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void Center(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        CenterTo(r, DisplayRect(), SCALE_V);
    }

    // rescales r to SCALE and then repositions to the center of the screen
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void Center(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        Center(r, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the position that would center something of size T.getSize() on the screen
    inline const sf::Vector2f Center(const sf::Texture & T) { return CenterCopy(T.getSize()); }

    // returns the position that would center s to the center of the screen (global)
    inline const sf::Vector2f CenterCopy(const sf::Sprite & S)
    {
        return CenterCopy(S.getGlobalBounds().width, S.getGlobalBounds().height);
    }

    // changes s's position so that it is centered on the screen (global)
    inline void Center(sf::Sprite & s) { s.setPosition(CenterCopy(s)); }

    // returns the horizontal screen position that would center something of WIDTH
    template <
        typename T,
        typename std::enable_if<
            ((std::is_integral<T> {} || std::is_floating_point<T> {}) && !std::is_same<T, bool> {}),
            int>::type
        = 0>
    float DisplayCenterHoriz(const T WIDTH)
    {
        return Center().x - (static_cast<float>(WIDTH) * 0.5f);
    }

    // returns the vertical screen position that would center something of HEIGHT
    template <
        typename T,
        typename std::enable_if<
            ((std::is_integral<T> {} || std::is_floating_point<T> {}) && !std::is_same<T, bool> {}),
            int>::type
        = 0>
    float DisplayCenterVert(const T HEIGHT)
    {
        return Center().y - (static_cast<float>(HEIGHT) * 0.5f);
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_CENTER_HPP_INCLUDED
