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
#include <SFML/Graphics/Rect.hpp>

#include <ostream>
#include <tuple>

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

#endif // HEROESPATH_SFUTIL_VECTOR_RECT_HPP_INCLUDED
