// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_VECTOR_RECT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_VECTOR_RECT_HPP_INCLUDED
//
// sfml-util-vector-rect.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/strings.hpp"
#include "sfml-util/orientation-enum.hpp"

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

namespace sfml_util
{

    namespace defaults
    {
        const sf::Vector2f VECTOR2F {};
        const sf::Vector2i VECTOR2I {};
        const sf::Vector2u VECTOR2U {};
        const sf::FloatRect FLOATRECT {};
        const sf::IntRect INTRECT {};

    } // namespace defaults

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
        const Orientation::Enum ORIENTATION, const T VALUE_AT_ORIENTATION, const T OTHER_VALUE)
    {
        if (ORIENTATION == Orientation::Horiz)
        {
            return sf::Vector2<T>(VALUE_AT_ORIENTATION, OTHER_VALUE);
        }
        else if (ORIENTATION == Orientation::Vert)
        {
            return sf::Vector2<T>(OTHER_VALUE, VALUE_AT_ORIENTATION);
        }
        else if (ORIENTATION == Orientation::Both)
        {
            return sf::Vector2<T>(VALUE_AT_ORIENTATION, VALUE_AT_ORIENTATION);
        }
        else
        {
            return sf::Vector2<T>(OTHER_VALUE, OTHER_VALUE);
        }
    }

    template <typename T>
    constexpr void
        ChangeVector(sf::Vector2<T> & orig, const Orientation::Enum ORIENTATION, const T NEW_VALUE)
    {
        if (ORIENTATION == Orientation::Horiz)
        {
            orig.x = NEW_VALUE;
        }
        else if (ORIENTATION == Orientation::Vert)
        {
            orig.y = NEW_VALUE;
        }
        else if (ORIENTATION == Orientation::Both)
        {
            orig.x = NEW_VALUE;
            orig.y = NEW_VALUE;
        }
    }

    template <typename T>
    const sf::Vector2<T> ChangeVectorCopy(
        const sf::Vector2<T> & ORIG, const Orientation::Enum ORIENTATION, const T NEW_VALUE)
    {
        sf::Vector2<T> v(ORIG);
        ChangeVector(v, ORIENTATION, NEW_VALUE);
        return v;
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_VECTOR_RECT_HPP_INCLUDED
