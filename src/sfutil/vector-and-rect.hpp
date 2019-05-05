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
#include "misc/strings.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <ostream>
#include <tuple>

namespace heroespath
{
namespace sfutil
{

    template <typename T>
    const std::string VectorToString(const sf::Vector2<T> & V)
    {
        return "(" + misc::ToString(V.x) + "," + misc::ToString(V.y) + ")";
    }

    template <typename T>
    const std::string RectToString(const sf::Rect<T> & R)
    {
        return "(" + misc::ToString(R.left) + "," + misc::ToString(R.top) + "/"
            + misc::ToString(R.width) + "x" + misc::ToString(R.height) + ")";
    }

} // namespace sfutil
} // namespace heroespath

namespace sf
{

template <typename T1, typename T2>
bool operator<(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    if (L.x < R.x)
    {
        return true;
    }
    else if (L.x > R.x)
    {
        return false;
    }
    else
    {
        return (L.y < R.y);
    }
}

template <typename T1, typename T2>
bool operator>(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return (R < L);
}

template <typename T1, typename T2>
bool operator<=(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return !(L > R);
}

template <typename T1, typename T2>
bool operator>=(const sf::Vector2<T1> & L, const sf::Vector2<T2> & R)
{
    return !(L < R);
}

template <typename T1, typename T2>
bool operator<(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return std::tie(L.left, L.top, L.width, L.height) < std::tie(R.left, R.top, R.width, R.height);
}

template <typename T1, typename T2>
bool operator>(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return (R < L);
}

template <typename T1, typename T2>
bool operator<=(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return !(L > R);
}

template <typename T1, typename T2>
bool operator>=(const sf::Rect<T1> & L, const sf::Rect<T2> & R)
{
    return !(L < R);
}

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Vector2<T> & V)
{
    os << heroespath::sfutil::VectorToString(V);
    return os;
}

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Rect<T> & R)
{
    os << heroespath::sfutil::RectToString(R);
    return os;
}

} // namespace sf

#endif // HEROESPATH_SFUTIL_VECTOR_RECT_HPP_INCLUDED
