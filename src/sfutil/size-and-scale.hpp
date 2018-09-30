// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_SIZE_AND_SCALE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_SIZE_AND_SCALE_HPP_INCLUDED
//
// size-and-scale.hpp
//
#include "misc/real.hpp"
#include "sfutil/center.hpp"
#include "sfutil/position.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <algorithm>
#include <vector>

namespace sf
{
class Sprite;
} // namespace sf

namespace heroespath
{
namespace sfml_util
{
    class Text;
}
namespace sfutil
{

    template <typename T>
    constexpr T Width(const sf::Rect<T> & RECT)
    {
        return RECT.width;
    }

    float Width(const sf::Sprite &);
    float Width(const sfml_util::Text &);

    template <typename T>
    constexpr T Height(const sf::Rect<T> & RECT)
    {
        return RECT.height;
    }

    float Height(const sf::Sprite &);
    float Height(const sfml_util::Text &);

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

    // returns the size of R scaled by SCALE
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T> Size(const sf::Rect<T> & R, const Scale_t SCALE = 1.0f)
    {
        return Size(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // returns the size of S (global) scaled by SCALE_V
    const sf::Vector2f Size(const sf::Sprite & S, const sf::Vector2f & SCALE_V);

    // returns the size of S (global) scaled by SCALE
    const sf::Vector2f Size(const sf::Sprite & S, const float SCALE = 1.0f);

    // returns the size of T (global) scaled by SCALE_V
    const sf::Vector2f Size(const sfml_util::Text & T, const sf::Vector2f & SCALE_V);

    // returns the size of T (global) scaled by SCALE
    const sf::Vector2f Size(const sfml_util::Text & T, const float SCALE = 1.0f);

    // returns a copy of R with size scaled by SCALE and then re-centered to the orig center of R
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T>
        ScaleAndReCenterCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V)
    {
        return sf::Rect<T>(sf::FloatRect(
            sf::Vector2f(CenterOf(R)) - (sf::Vector2f(ScaleCopy(Size(R), SCALE_V)) * 0.5f),
            sf::Vector2f(ScaleCopy(Size(R), SCALE_V))));
    }

    // returns a copy of R with size scaled by SCALE and then re-centered to the orig center of R
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T> ScaleAndReCenterCopy(const sf::Rect<T> & R, const Scale_t & SCALE)
    {
        return ScaleAndReCenterCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales the size of r by SCALE and then re-centers to the orig center of R
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleAndReCenter(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V)
    {
        r = ScaleAndReCenterCopy(r, SCALE_V);
    }

    // scales the size of r by SCALE and then re-centers to the orig center of R
    template <
        typename T,
        typename Scale_t,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void ScaleAndReCenter(sf::Rect<T> & r, const Scale_t & SCALE)
    {
        r = ScaleAndReCenterCopy(r, SCALE);
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

    // sets s's scale (local) to SCALE_V and then re-centers
    void SetScaleAndReCenter(sf::Sprite & s, const sf::Vector2f & SCALE_V);

    // sets s's scale (local) to SCALE and then re-centers
    void SetScaleAndReCenter(sf::Sprite & s, const float SCALE);

    // multiplies s's scale (global) by SCALE_V and then re-centers
    void ScaleAndReCenter(sf::Sprite & s, const sf::Vector2f & SCALE_V);

    // multiplies s's scale (global) by SCALE and then re-centers
    void ScaleAndReCenter(sf::Sprite & s, const float SCALE);

    // sets the size (global) of s to match V, if either V is <= zero then that dimmension is not
    // changed
    void SetSize(sf::Sprite & s, const sf::Vector2f & V_ORIG);

    // sets the size (global) of s to match R, then positions at R, if either size of R is <= zero
    // then that dimmension is not changed
    void SetSizeAndPos(sf::Sprite & s, const sf::FloatRect & R);

    // returns true if either width or height is <= 0
    template <typename T>
    constexpr bool IsSizeZeroOrLessEither(const sf::Rect<T> & R)
    {
        return IsZeroOrLessEither(Size(R));
    }

    // returns true if either width or height is < 1
    template <typename T>
    constexpr bool IsSizeLessThanOneEither(const sf::Rect<T> & R)
    {
        return IsLessThanOneEither(Size(R));
    }

    // returns true if both width and height are <= 0
    template <typename T>
    constexpr bool IsSizeZeroOrLessBoth(const sf::Rect<T> & R)
    {
        return IsZeroOrLessBoth(Size(R));
    }

    // returns true if both width and height are < 1
    template <typename T>
    constexpr bool IsSizeLessThanOneBoth(const sf::Rect<T> & R)
    {
        return IsLessThanOneBoth(Size(R));
    }

    // returns true if either the global bounds width or height is <= 0
    bool IsSizeZeroOrLessEither(const sf::Sprite & S);

    // returns true if both the global bounds width and height are <= 0
    bool IsSizeZeroOrLessBoth(const sf::Sprite & S);

    // returns true if either the global bounds width or height is < 1
    bool IsSizeLessThanOneEither(const sf::Sprite & S);

    // returns true if both the global bounds width and height are < 1
    bool IsSizeLessThanOneBoth(const sf::Sprite & S);

    // returns true if either the global bounds width or height are <= 0
    bool IsSizeZeroOrLessEither(const sfml_util::Text & T);

    // returns true if both the global bounds width and height are <= 0
    bool IsSizeZeroOrLessBoth(const sfml_util::Text & T);

    // returns true if either the global bounds width or height are < 1
    bool IsSizeLessThanOneEither(const sfml_util::Text & T);

    // returns true if both the global bounds width and height are < 1
    bool IsSizeLessThanOneBoth(const sfml_util::Text & T);

    // returns the smallest sf::Rect that fully includes rects A and B
    template <typename T1, typename T2>
    constexpr const sf::Rect<T1> MinimallyEnclosing(
        const sf::Rect<T1> & A,
        const sf::Rect<T2> & B,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS = false)
    {
        if (WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS)
        {
            const bool IS_EITHER_SIZE_ZERO_OR_LESS_A { IsSizeZeroOrLessEither(A) };
            const bool IS_EITHER_SIZE_ZERO_OR_LESS_B { IsSizeZeroOrLessEither(B) };

            if (IS_EITHER_SIZE_ZERO_OR_LESS_A && IS_EITHER_SIZE_ZERO_OR_LESS_B)
            {
                return { 0, 0, 0, 0 };
            }
            else if (IS_EITHER_SIZE_ZERO_OR_LESS_A)
            {
                return B;
            }
            else if (IS_EITHER_SIZE_ZERO_OR_LESS_B)
            {
                return A;
            }
        }

        const float MIN_LEFT { std::min(static_cast<float>(A.left), static_cast<float>(B.left)) };
        const float MIN_TOP { std::min(static_cast<float>(A.top), static_cast<float>(B.top)) };
        const float MAX_RIGHT { std::max(Right(sf::FloatRect(A)), Right(sf::FloatRect(B))) };
        const float MAX_BOTTOM { std::max(Bottom(sf::FloatRect(A)), Bottom(sf::FloatRect(B))) };

        return sf::Rect<T1>(
            sf::FloatRect(MIN_LEFT, MIN_TOP, MAX_RIGHT - MIN_LEFT, MAX_BOTTOM - MIN_TOP));
    }

    // returns the smallest sf::Rect that fully includes all the rects in V
    template <typename T>
    constexpr const sf::Rect<T> MinimallyEnclosing(
        const std::vector<sf::Rect<T>> & V,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS = false)
    {
        sf::Rect<T> r(0, 0, 0, 0);

        if (V.size() >= 1)
        {
            if (!WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS || !IsSizeZeroOrLessEither(V.front()))
            {
                r = V.front();
            }
        }

        if (V.size() > 1)
        {
            for (std::size_t i(1); i < V.size(); ++i)
            {
                if (!WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS || !IsSizeZeroOrLessEither(V[i]))
                {
                    r = MinimallyEnclosing(r, V[i]);
                }
            }
        }

        return r;
    }

    // returns the smallest sf::FloatRect that includes the global bounds of both
    const sf::FloatRect MinimallyEnclosing(
        const sf::Sprite & A,
        const sf::Sprite & B,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS = false);

    // returns the smallest sf::FloatRect that includes the global bounds of both
    const sf::FloatRect MinimallyEnclosing(
        const sfml_util::Text & A,
        const sfml_util::Text & B,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS = false);

    // returns the smallest sf::Rect that fully includes all the sprites (global) in V
    const sf::FloatRect MinimallyEnclosing(
        const std::vector<sf::Sprite> & VEC,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS = false);

    // returns the smallest sf::Rect that fully includes all the sprites (global) in V
    const sf::FloatRect MinimallyEnclosing(
        const std::vector<sfml_util::Text> & VEC,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS = false);

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

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_SIZE_AND_SCALE_HPP_INCLUDED
