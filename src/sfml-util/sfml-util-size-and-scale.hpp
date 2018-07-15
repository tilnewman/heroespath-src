// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_SIZE_AND_SCALE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_SIZE_AND_SCALE_HPP_INCLUDED
//
// sfml-util-size-and-scale.hpp
//
#include "sfml-util/sfml-util-center-of.hpp"
#include "sfml-util/sfml-util-center-to.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-position.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <cmath>
#include <tuple>
#include <type_traits>

namespace sf
{

template <typename T1, typename T2>
constexpr bool operator<(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return std::tie(L.x, L.y) < std::tie(R.x, R.y);
}

template <typename T1, typename T2>
inline constexpr bool operator>(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return (R < L);
}

template <typename T1, typename T2>
inline constexpr bool operator<=(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return !(L > R);
}

template <typename T1, typename T2>
inline constexpr bool operator>=(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return !(L < R);
}

template <typename T1, typename T2>
constexpr bool operator<(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return std::tie(L.left, L.top, L.width, L.height) < std::tie(R.left, R.top, R.width, R.height);
}

template <typename T1, typename T2>
inline constexpr bool operator>(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return (R < L);
}

template <typename T1, typename T2>
inline constexpr bool operator<=(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return !(L > R);
}

template <typename T1, typename T2>
inline constexpr bool operator>=(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return !(L < R);
}
} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    // returns a copy of V scaled by SCALE_V, same as (V.x * SCALE_V.x) and (V.y * SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T>
        ScaleCopy(const sf::Vector2<T> & V, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return sf::Vector2<T>(sf::Vector2f(
            (static_cast<float>(V.x) * static_cast<float>(SCALE_V.x)),
            (static_cast<float>(V.y) * static_cast<float>(SCALE_V.y))));
    }

    // returns a copy of V scaled by SCALE, same as (V * SCALE)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T> ScaleCopy(const sf::Vector2<T> & V, const Scale_t SCALE)
    {
        return ScaleCopy(V, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales v by SCALE_V, same as (v.x *= SCALE_V.x) and (v.y *= SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void Scale(sf::Vector2<T> & v, const sf::Vector2<Scale_t> & SCALE_V)
    {
        v = ScaleCopy(v, SCALE_V);
    }

    // scales v by SCALE, same as (v *= SCALE)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void Scale(sf::Vector2<T> & v, const Scale_t SCALE)
    {
        v = ScaleCopy(v, SCALE);
    }

    // returns a copy of R dimensionally scaled by SCALE_V, same as (R.left/width * SCALE_V.x) and
    // (R.top/height * SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ScaleRectLinearCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return { static_cast<T>(static_cast<float>(R.left) * static_cast<float>(SCALE_V.x)),
                 static_cast<T>(static_cast<float>(R.top) * static_cast<float>(SCALE_V.y)),
                 static_cast<T>(static_cast<float>(R.width) * static_cast<float>(SCALE_V.x)),
                 static_cast<T>(static_cast<float>(R.height) * static_cast<float>(SCALE_V.y)) };
    }

    // returns a copy of R with all members scaled by SCALE, same as (R.* * SCALE)
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ScaleRectLinearCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        return ScaleRectLinearCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales all r members by SCALE, same as (r.* *= SCALE)
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleRectLinear(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        r = ScaleRectLinearCopy(r, SCALE);
    }

    // dimensionally scales r by SCALE_V, same as (r.left/width *= SCALE_V.x) and (r.top/height *=
    // SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleRectLinear(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        r = ScaleRectLinearCopy(r, SCALE_V);
    }

    // returns the size of R scaled by SCALE_V, same as (R.width * SCALE_V.x) and (R.height *
    // SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T> Size(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return ScaleCopy(sf::Vector2<T>(R.width, R.height), SCALE_V);
    }

    // returns the size of R scaled by SCALE, same as (R.width * SCALE) and (R.height * SCALE)
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T> Size(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        return Size(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the size of T scaled by SCALE_V, same as (T.getSize().x * SCALE_V.x) and
    // (T.getSize().y * SCALE_V.y)
    template <typename Scale_t, typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    const sf::Vector2u Size(const sf::Texture & T, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return ScaleCopy(T.getSize(), SCALE_V);
    }

    // returns the size of T scaled by SCALE, same as (T.getSize().x * SCALE) and (T.getSize().y *
    // SCALE)
    template <
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    const sf::Vector2u Size(const sf::Texture & T, const Scale_t SCALE = 1.0f)
    {
        return Size(T, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the size of S (global) scaled by SCALE_V, same as (S.global().width * SCALE_V.x) and
    // (S.global().height * SCALE_V.y)
    template <typename Scale_t, typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    const sf::Vector2f Size(const sf::Sprite & S, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return Size(S.getGlobalBounds(), SCALE_V);
    }

    // returns the size of S (global) scaled by SCALE, same as (S.global().width/height * SCALE)
    template <
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    const sf::Vector2f Size(const sf::Sprite & S, const Scale_t SCALE = 1.0f)
    {
        return Size(S, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns a copy of V scaled by SCALE_V, same as (V.x * SCALE_V.x) and (V.y * SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T>
        ScaleSizeCopy(const sf::Vector2<T> & V, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return ScaleCopy(V, SCALE_V);
    }

    // returns a copy of V scaled by SCALE, same as (V * SCALE)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T> ScaleSizeCopy(const sf::Vector2<T> & V, const Scale_t SCALE)
    {
        return ScaleCopy(V, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns a copy of R with size scaled by SCALE_V, same as (R.width * SCALE_V.x) and (R.height
    // * SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ScaleSizeCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return { Position(R), ScaleCopy(Size(R), SCALE_V) };
    }

    // returns a copy of R with size scaled by SCALE, same as (R.width/height * SCALE)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T> ScaleSizeCopy(const sf::Rect<T> & R, const Scale_t SCALE)
    {
        return ScaleSizeCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales the size of r by SCALE_V, same as (r.width *= SCALE_V.x) and (r.height *= SCALE_V.y)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleSize(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        r = ScaleSizeCopy(r, SCALE_V);
    }

    // scales the size of r by SCALE, same as (r.width/height *= SCALE)
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleSize(sf::Rect<T> & r, const Scale_t SCALE)
    {
        ScaleSize(r, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns a copy of R rescaled to SCALE_V and recentered
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ScaleSizeAndReCenterCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return CenterToCopy(R, R, SCALE_V);
    }

    // returns a copy of R with size scaled by SCALE (same as ScaleSize(R, SCALE) and then
    // re-centered
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T> ScaleSizeAndReCenterCopy(const sf::Rect<T> & R, const Scale_t SCALE)
    {
        return ScaleSizeAndReCenterCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales r by SCALE_V and then re-centers
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleSizeAndReCenter(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        CenterTo(r, r, SCALE_V);
    }

    // scales r by SCALE and then re-centers
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleSizeAndReCenter(sf::Rect<T> & r, const Scale_t SCALE)
    {
        ScaleSizeAndReCenter(r, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales s (local) by SCALE_V and then re-centers
    template <typename Scale_t, typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void ScaleSizeAndReCenter(sf::Sprite & s, const sf::Vector2<Scale_t> & SCALE_V)
    {
        CenterTo(s, s.getGlobalBounds(), SCALE_V);
    }

    // scales s (local) by SCALE and then re-centers
    template <typename Scale_t, typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void ScaleSizeAndReCenter(sf::Sprite & s, const Scale_t SCALE)
    {
        ScaleSizeAndReCenter(s, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the smallest sf::Rect that fully includes rects A and B
    template <typename T1, typename T2>
    constexpr const sf::Rect<T1> MininallyEnclosing(const sf::Rect<T1> & A, const sf::Rect<T2> & B)
    {
        const float MIN_LEFT { std::min(static_cast<float>(A.left), static_cast<float>(B.left)) };
        const float MIN_TOP { std::min(static_cast<float>(A.top), static_cast<float>(B.top)) };
        const float MAX_RIGHT { std::max(Right(sf::FloatRect(A)), Right(sf::FloatRect(B))) };
        const float MAX_BOTTOM { std::max(Bottom(sf::FloatRect(A)), Bottom(sf::FloatRect(B))) };

        return sf::Rect<T1>(
            sf::FloatRect(MIN_LEFT, MIN_TOP, MAX_RIGHT - MIN_LEFT, MAX_BOTTOM - MIN_TOP));
    }

    // returns the smallest sf::FloatRect that fully includes rects A and B
    inline const sf::FloatRect MininallyEnclosing(const sf::Sprite & A, const sf::Sprite & B)
    {
        return MininallyEnclosing(A.getGlobalBounds(), B.getGlobalBounds());
    }

    // returns a copy of R that has the smaller dimmension set equal to the larger then scaled to
    // SCALE
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T> GrowToSquareCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        const float NEW_SIZE { static_cast<float>(std::max(R.width, R.height))
                               * static_cast<float>(SCALE) };

        return { Position(R), sf::Vector2<T>(sf::Vector2f(NEW_SIZE, NEW_SIZE)) };
    }

    // changes R so that the smaller dimmension is set equal to the larger then scaled to
    // SCALE
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void GrowToSquare(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        r = GrowToSquareCopy(r, SCALE);
    }

    // returns a copy of R that has the larger dimmension set equal to the smaller then scaled to
    // SCALE
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ShrinkToSquareCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        const float NEW_SIZE { static_cast<float>(std::min(R.width, R.height))
                               * static_cast<float>(SCALE) };

        return { Position(R), sf::Vector2<T>(sf::Vector2f(NEW_SIZE, NEW_SIZE)) };
    }

    // changes r so that the larger dimmension is set equal to the smaller then scaled to
    // SCALE
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ShrinkToSquare(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        r = ShrinkToSquareCopy(r, SCALE);
    }

    // returns a copy of R that has the smaller dimmension set equal to the larger then scaled to
    // SCALE and then recentered
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        GrowToSquareAndReCenterCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        const float NEW_SIZE { static_cast<float>(std::max(R.width, R.height))
                               * static_cast<float>(SCALE) };

        return sf::Rect<T>(sf::FloatRect(
            CenterOf(sf::FloatRect(R)) - (sf::Vector2f(NEW_SIZE, NEW_SIZE) * 0.5f),
            sf::Vector2f(NEW_SIZE, NEW_SIZE)));
    }

    // changes r so that the smaller dimmension is set equal to the larger then scaled to SCALE and
    // then recentered
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void GrowToSquareAndReCenter(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        r = GrowToSquareAndReCenterCopy(r, SCALE);
    }

    // returns a copy of R that has the larger dimmension set equal to the smaller then scaled to
    // SCALE and then recentered
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ShrinkToSquareAndReCenterCopy(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        const float NEW_SIZE { static_cast<float>(std::min(R.width, R.height))
                               * static_cast<float>(SCALE) };

        return sf::Rect<T>(sf::FloatRect(
            CenterOf(sf::FloatRect(R)) - (sf::Vector2f(NEW_SIZE, NEW_SIZE) * 0.5f),
            sf::Vector2f(NEW_SIZE, NEW_SIZE)));
    }

    // changes r so that the larger dimmension is set equal to the smaller then scaled to
    // SCALE and then recentered
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ShrinkToSquareAndReCenter(sf::Rect<T> & r, const Scale_t SCALE = 1.0f)
    {
        r = ShrinkToSquareAndReCenterCopy(r, SCALE);
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_SIZE_AND_SCALE_HPP_INCLUDED