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
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/to-string-prefix-enum.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/Font.hpp>

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

std::ostream & operator<<(std::ostream & os, const sf::Font & F);

} // namespace sf

namespace heroespath
{
using FontPtr_t = misc::NotNull<sf::Font *>;
using FontPtrOpt_t = boost::optional<FontPtr_t>;
using FontUPtr_t = std::unique_ptr<sf::Font>;
using FontUVec_t = std::vector<FontUPtr_t>;

namespace sfutil
{

    const std::string ToString(
        const sf::Font & F,
        const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default);

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_FONT_HPP_INCLUDED
