// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_CENTER_HPP_INCLUDED
#define HEROESPATH_SFUTIL_CENTER_HPP_INCLUDED
//
// center.hpp
//
#include "sfutil/center-of.hpp"
#include "sfutil/center-to.hpp"
#include "sfutil/display.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace sfutil
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
    template <typename T>
    const sf::Vector2f CenterCopy(const T WIDTH, const T HEIGHT)
    {
        return CenterCopy(sf::Vector2<T>(WIDTH, HEIGHT));
    }

    // returns a copy of R rescaled to SCALE_V and then repositioned to the center of the screen
    template <typename T, typename Scale_t>
    const sf::FloatRect CenterCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return CenterToCopy(sf::FloatRect(R), DisplayRect(), SCALE_V);
    }

    // returns a copy of R rescaled to SCALE and then repositioned to the center of the screen
    template <typename T, typename Scale_t = float>
    const sf::FloatRect CenterCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        return CenterCopy(sf::FloatRect(R), sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // rescales r to SCALE and then repositions to the center of the screen
    template <typename T, typename Scale_t>
    void Center(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        CenterTo(r, DisplayRect(), SCALE_V);
    }

    // rescales r to SCALE and then repositions to the center of the screen
    template <typename T, typename Scale_t = float>
    void Center(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        Center(r, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the position that would center s to the center of the screen (global)
    const sf::Vector2f CenterCopy(const sf::Sprite & S);

    // changes s's position so that it is centered on the screen (global)
    void Center(sf::Sprite & s);

    // returns the horizontal screen position that would center something of WIDTH
    template <typename T>
    float DisplayCenterHoriz(const T WIDTH)
    {
        return Center().x - (static_cast<float>(WIDTH) * 0.5f);
    }

    // returns the vertical screen position that would center something of HEIGHT
    template <typename T>
    float DisplayCenterVert(const T HEIGHT)
    {
        return Center().y - (static_cast<float>(HEIGHT) * 0.5f);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_CENTER_HPP_INCLUDED
