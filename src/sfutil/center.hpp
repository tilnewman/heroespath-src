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

#include "sfutil/common.hpp"
#include "sfutil/display.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace heroespath
{
namespace sfutil
{

    // returns the horizontal center of R, same as (R.left + (R.width / 2))
    template <typename T>
    const T CenterOfHoriz(const sf::Rect<T> & R) noexcept
    {
        return (R.left + (R.width / T(2)));
    }

    // returns the horizontal center of S (global)
    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> CenterOfHoriz(const T & THING) noexcept
    {
        return CenterOfHoriz(THING.getGlobalBounds());
    }

    // returns the vertical center of R, same as (R.top + (R.height / 2))
    template <typename T>
    const T CenterOfVert(const sf::Rect<T> & R) noexcept
    {
        return (R.top + (R.height / T(2)));
    }

    // returns the vertical center of S (global)
    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> CenterOfVert(const T & THING) noexcept
    {
        return CenterOfVert(THING.getGlobalBounds());
    }

    // returns the center of R, same as (R.left_top + (R.width_hieght / 2))
    template <typename T>
    const sf::Vector2<T> CenterOf(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>(CenterOfHoriz(R), CenterOfVert(R));
    }

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f> CenterOf(const T & THING) noexcept
    {
        return CenterOf(THING.getGlobalBounds());
    }

    // returns a position that would center something of SIZE_V to the center of R
    template <typename T1, typename T2>
    const sf::Vector2<T1>
        CenterToCopy(const sf::Vector2<T1> & SIZE_V, const sf::Rect<T2> & R) noexcept
    {
        return sf::Vector2<T1>(CenterOf(sf::Rect<float>(R)) - Half(sf::Vector2f(SIZE_V)));
    }

    // returns a copy of A repositioned to the center of B
    template <typename T1, typename T2>
    const sf::Rect<T1> CenterToCopy(const sf::Rect<T1> & A, const sf::Rect<T2> & B) noexcept
    {
        return SetPositionCopy(A, CenterToCopy(Size(A), B));
    }

    // returns a copy of A rescaled to SCALE_V and then repositioned to the center of B
    template <typename T1, typename T2, typename Scale_t>
    const sf::Rect<T1> CenterToCopy(
        const sf::Rect<T1> & A,
        const sf::Rect<T2> & B,
        const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        const auto A_FLOAT = sf::FloatRect(A);
        const auto SCALE_FLOAT = sf::Vector2f(SCALE_V);

        return sf::Rect<T1>(sf::FloatRect(
            CenterToCopy(Size(A_FLOAT, SCALE_FLOAT), sf::FloatRect(B)),
            Size(A_FLOAT, SCALE_FLOAT)));
    }

    // returns a copy of A rescaled to SCALE and then repositioned to the center of B
    template <typename T1, typename T2, typename Scale_t>
    const sf::Rect<T1>
        CenterToCopy(const sf::Rect<T1> & A, const sf::Rect<T2> & B, const Scale_t SCALE) noexcept
    {
        return CenterToCopy(A, B, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // a repositioned to the center of B
    template <typename T1, typename T2>
    void CenterTo(sf::Rect<T1> & a, const sf::Rect<T2> & B) noexcept
    {
        a = CenterToCopy(a, B);
    }

    // rescales a to SCALE and then repositions it to the center of B
    template <typename T1, typename T2, typename Scale_t>
    void CenterTo(
        sf::Rect<T1> & a, const sf::Rect<T2> & B, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        a = CenterToCopy(a, B, SCALE_V);
    }

    // rescales a to SCALE and then repositions it to the center of B
    template <typename T1, typename T2, typename Scale_t>
    void CenterTo(sf::Rect<T1> & a, const sf::Rect<T2> & B, const Scale_t SCALE) noexcept
    {
        CenterTo(a, B, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // repositions s to the center of R
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void CenterTo(T & thing, const sf::FloatRect & R) noexcept
    {
        thing.setPosition(CenterToCopy(Size(thing.getGlobalBounds()), R) + thing.getOrigin());
    }

    // returns the position at the center of the screen, same as (DisplaySize() * 0.5f)
    template <typename T = float>
    const sf::Vector2<T> Center()
    {
        return sf::Vector2<T>(Half(DisplaySize()));
    }

    // returns the position that would center something of SIZE_V on the screen
    template <typename T>
    const sf::Vector2<T> CenterCopy(const sf::Vector2<T> & SIZE_V)
    {
        return CenterToCopy(SIZE_V, DisplayRect());
    }

    // returns the position that would center something of size (WIDTH, HEIGHT) to the center of the
    // screen
    template <typename T>
    const sf::Vector2<T> CenterCopy(const T WIDTH, const T HEIGHT)
    {
        return CenterCopy(sf::Vector2<T>(WIDTH, HEIGHT));
    }

    // returns a copy of R repositioned to the center of the screen
    template <typename T>
    const sf::Rect<T> CenterCopy(const sf::Rect<T> & R)
    {
        return CenterToCopy(R, DisplayRect());
    }

    // returns a copy of R rescaled to SCALE_V and then repositioned to the center of the screen
    template <typename T, typename Scale_t>
    const sf::Rect<T> CenterCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return CenterToCopy(R, DisplayRect(), SCALE_V);
    }

    // returns a copy of R rescaled to SCALE and then repositioned to the center of the screen
    template <typename T, typename Scale_t>
    const sf::Rect<T> CenterCopy(const sf::Rect<T> & R, const Scale_t SCALE)
    {
        return CenterCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // repositions r to the center of the screen
    template <typename T>
    void Center(sf::Rect<T> & r)
    {
        CenterTo(r, DisplayRect());
    }

    // rescales r to SCALE and then repositions to the center of the screen
    template <typename T, typename Scale_t>
    void Center(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        CenterTo(r, DisplayRect(), SCALE_V);
    }

    // rescales r to SCALE and then repositions to the center of the screen
    template <typename T, typename Scale_t = float>
    void Center(sf::Rect<T> & r, const Scale_t SCALE = Scale_t(1))
    {
        Center(r, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the position that would center s to the center of the screen (global)
    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f> CenterCopy(const T & THING)
    {
        return CenterCopy(Size(THING.getGlobalBounds())) + THING.getOrigin();
    }

    // changes s's position so that it is centered on the screen (global)
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void Center(T & thing)
    {
        thing.setPosition(CenterCopy(thing));
    }

    // returns the horizontal screen position that would center something of WIDTH
    template <typename T>
    T DisplayCenterHoriz(const T WIDTH)
    {
        return static_cast<T>(Center().x - (static_cast<float>(WIDTH) * 0.5f));
    }

    // returns the vertical screen position that would center something of HEIGHT
    template <typename T>
    T DisplayCenterVert(const T HEIGHT)
    {
        return static_cast<T>(Center().y - (static_cast<float>(HEIGHT) * 0.5f));
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_CENTER_HPP_INCLUDED
