// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_VECTOR_RECT_HPP_INCLUDED
#define HEROESPATH_SFUTIL_VECTOR_RECT_HPP_INCLUDED
//
// vector-and-rect.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/real.hpp"
#include "misc/strings.hpp"
#include "sfml-util/orientation-enum.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/Rect.hpp> //also includes System/Vector2.hpp

#include <tuple>
#include <vector>

namespace sf
{

template <typename T1, typename T2>
constexpr bool operator<(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return std::tie(L.x, L.y) < std::tie(R.x, R.y);
}

template <typename T1, typename T2>
constexpr bool operator>(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return (R < L);
}

template <typename T1, typename T2>
constexpr bool operator<=(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return !(L > R);
}

template <typename T1, typename T2>
constexpr bool operator>=(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return !(L < R);
}

template <typename T1, typename T2>
constexpr bool operator<(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return std::tie(L.left, L.top, L.width, L.height) < std::tie(R.left, R.top, R.width, R.height);
}

template <typename T1, typename T2>
constexpr bool operator>(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return (R < L);
}

template <typename T1, typename T2>
constexpr bool operator<=(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return !(L > R);
}

template <typename T1, typename T2>
constexpr bool operator>=(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return !(L < R);
}

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Vector2<T> & V)
{
    os << "(" << V.x << "," << V.y << ")";
    return os;
}

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Rect<T> & R)
{
    os << "(" << R.left << "," << R.top << "/" << R.width << "x" << R.height << ")";
    return os;
}

} // namespace sf

namespace heroespath
{

using Vector2fVec_t = std::vector<sf::Vector2f>;
using Vector2iVec_t = std::vector<sf::Vector2i>;
using Vector2uVec_t = std::vector<sf::Vector2u>;

using FloatRectVec_t = std::vector<sf::FloatRect>;

using Vector2fOpt_t = boost::optional<sf::Vector2f>;
using Vector2iOpt_t = boost::optional<sf::Vector2i>;
using Vector2uOpt_t = boost::optional<sf::Vector2u>;

using FloatRectOpt_t = boost::optional<sf::FloatRect>;
using IntRectOpt_t = boost::optional<sf::IntRect>;

namespace sfutil
{

    template <typename T1, typename T2>
    constexpr const sf::Rect<T1>
        SetPositionCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & POS_V)
    {
        return sf::Rect<T1>(static_cast<T1>(POS_V.x), static_cast<T1>(POS_V.y), R.width, R.height);
    }

    template <typename T1, typename T2>
    void SetPosition(sf::Rect<T1> & r, const sf::Vector2<T2> & POS_V)
    {
        r = SetPositionCopy(r, POS_V);
    }

    template <typename T1, typename T2>
    constexpr const sf::Rect<T1> SetSizeCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & SIZE_V)
    {
        return sf::Rect<T1>(R.left, R.top, static_cast<T1>(SIZE_V.x), static_cast<T1>(SIZE_V.y));
    }

    template <typename T1, typename T2>
    void SetSize(sf::Rect<T1> & r, const sf::Vector2<T2> & SIZE_V)
    {
        r = SetSizeCopy(r, SIZE_V);
    }

    template <typename T1, typename T2>
    constexpr const sf::Rect<T1>
        AdjustPositionCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & ADJ_V)
    {
        const auto NEW_LEFT { static_cast<T1>(
            static_cast<float>(R.left) + static_cast<float>(ADJ_V.x)) };

        const auto NEW_TOP { static_cast<T1>(
            static_cast<float>(R.top) + static_cast<float>(ADJ_V.y)) };

        return sf::Rect<T1>(NEW_LEFT, NEW_TOP, R.width, R.height);
    }

    template <typename T1, typename T2>
    constexpr void AdjustPosition(sf::Rect<T1> & r, const sf::Vector2<T2> & ADJ_V)
    {
        r = AdjustPositionCopy(r, ADJ_V);
    }

    template <typename T1, typename T2>
    constexpr const sf::Rect<T1>
        AdjustSizeCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & ADJ_V)
    {
        const auto NEW_WIDTH { static_cast<T1>(
            static_cast<float>(R.width) + static_cast<float>(ADJ_V.x)) };

        const auto NEW_HEIGHT { static_cast<T1>(
            static_cast<float>(R.height) + static_cast<float>(ADJ_V.y)) };

        return sf::Rect<T1>(R.left, R.top, NEW_WIDTH, NEW_HEIGHT);
    }

    template <typename T1, typename T2>
    constexpr void AdjustSize(sf::Rect<T1> & r, const sf::Vector2<T2> & ADJ_V)
    {
        r = AdjustSizeCopy(r, ADJ_V);
    }

    template <typename T1, typename T2>
    constexpr void Move(sf::Rect<T1> & r, const sf::Vector2<T2> & MOVE_V)
    {
        AdjustPosition(r, MOVE_V);
    }

    template <typename T1, typename T2>
    constexpr const sf::Rect<T1> MoveCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & MOVE_V)
    {
        return AdjustPositionCopy(R, MOVE_V);
    }

    template <typename Position_t, typename Size_t>
    constexpr const sf::Rect<Position_t>
        MakeRect(const sf::Vector2<Position_t> & POS_V, const sf::Vector2<Size_t> & SIZE_V)
    {
        return sf::Rect<Position_t>(POS_V, sf::Vector2<Position_t>(SIZE_V));
    }

    template <typename T>
    constexpr const sf::Rect<T> MakeRectWithPosition(const sf::Vector2<T> & POS_V)
    {
        return sf::Rect<T>(POS_V, sf::Vector2<T>());
    }

    template <typename T>
    constexpr const sf::Rect<T> MakeRectWithSize(const sf::Vector2<T> & SIZE_V)
    {
        return sf::Rect<T>(sf::Vector2<T>(), SIZE_V);
    }

    // returns true if either x or y is <= 0
    template <typename T>
    constexpr bool IsZeroOrLessEither(const sf::Vector2<T> & V)
    {
        return (misc::IsRealZeroOrLess(V.x) || misc::IsRealZeroOrLess(V.y));
    }

    // returns true if both x and y are <= 0
    template <typename T>
    constexpr bool IsZeroOrLessBoth(const sf::Vector2<T> & V)
    {
        return (misc::IsRealZeroOrLess(V.x) && misc::IsRealZeroOrLess(V.y));
    }

    // returns true if either x or y is < 1
    template <typename T>
    constexpr bool IsLessThanOneEither(const sf::Vector2<T> & V)
    {
        return ((V.x < T(1)) || (V.y < T(1)));
    }

    // returns true if both x and y are < 1
    template <typename T>
    constexpr bool IsLessThanOneBoth(const sf::Vector2<T> & V)
    {
        return ((V.x < T(1)) && (V.y < T(1)));
    }

    template <typename T>
    const std::string ToString(
        const sf::Vector2<T> & V,
        const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix<T>(OPTIONS, "Vector2") << V;
        return ss.str();
    }

    template <typename T>
    const std::string ToString(
        const sf::Rect<T> & R,
        const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix<T>(OPTIONS, "Rect") << R;
        return ss.str();
    }

    template <typename T>
    const sf::Vector2<T> CreateVector(
        const sfml_util::Orientation::Enum ORIENTATION,
        const T VALUE_AT_ORIENTATION,
        const T OTHER_VALUE)
    {
        if (ORIENTATION == sfml_util::Orientation::Horiz)
        {
            return sf::Vector2<T>(VALUE_AT_ORIENTATION, OTHER_VALUE);
        }
        else if (ORIENTATION == sfml_util::Orientation::Vert)
        {
            return sf::Vector2<T>(OTHER_VALUE, VALUE_AT_ORIENTATION);
        }
        else if (ORIENTATION == sfml_util::Orientation::Both)
        {
            return sf::Vector2<T>(VALUE_AT_ORIENTATION, VALUE_AT_ORIENTATION);
        }
        else
        {
            return sf::Vector2<T>(OTHER_VALUE, OTHER_VALUE);
        }
    }

    template <typename T>
    constexpr void ChangeVector(
        sf::Vector2<T> & orig, const sfml_util::Orientation::Enum ORIENTATION, const T NEW_VALUE)
    {
        if (ORIENTATION == sfml_util::Orientation::Horiz)
        {
            orig.x = NEW_VALUE;
        }
        else if (ORIENTATION == sfml_util::Orientation::Vert)
        {
            orig.y = NEW_VALUE;
        }
        else if (ORIENTATION == sfml_util::Orientation::Both)
        {
            orig.x = NEW_VALUE;
            orig.y = NEW_VALUE;
        }
    }

    template <typename T>
    const sf::Vector2<T> ChangeVectorCopy(
        const sf::Vector2<T> & ORIG,
        const sfml_util::Orientation::Enum ORIENTATION,
        const T NEW_VALUE)
    {
        sf::Vector2<T> v(ORIG);
        ChangeVector(v, ORIENTATION, NEW_VALUE);
        return v;
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_VECTOR_RECT_HPP_INCLUDED
