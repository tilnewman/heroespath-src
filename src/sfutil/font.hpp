// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_FONT_HPP_INCLUDED
#define HEROESPATH_SFUTIL_FONT_HPP_INCLUDED
//
// font.hpp
//
#include <SFML/Graphics/Font.hpp>

#include <ostream>
#include <string>

namespace sf
{

inline bool operator<(const sf::Font & L, const sf::Font & R)
{
    return (L.getInfo().family < R.getInfo().family);
}

inline bool operator==(const sf::Font & L, const sf::Font & R)
{
    return (L.getInfo().family == R.getInfo().family);
}

inline bool operator!=(const sf::Font & L, const sf::Font & R) { return !(L == R); }

inline std::ostream & operator<<(std::ostream & os, const sf::Font & F)
{
    os << "(\"" << F.getInfo().family << "\")";
    return os;
}

} // namespace sf

namespace heroespath
{
namespace sfutil
{

    const std::string TextStyleToString(const sf::Uint32 TEXT_STYLE);

} // namespace sfutil

} // namespace heroespath

#endif // HEROESPATH_SFUTIL_FONT_HPP_INCLUDED
