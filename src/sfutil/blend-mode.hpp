// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_BLENDMODE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_BLENDMODE_HPP_INCLUDED
//
// blend-mode.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/to-string-prefix-enum.hpp"

#include <SFML/Graphics/BlendMode.hpp>

#include <string>

namespace sf
{

bool operator<(const sf::BlendMode & L, const sf::BlendMode & R);

std::ostream & operator<<(std::ostream & os, const sf::BlendMode & BM);

} // namespace sf

namespace heroespath
{

using BlendModeOpt_t = boost::optional<sf::BlendMode>;

namespace sfutil
{

    const std::string ToString(
        const sf::BlendMode & BM,
        const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default);

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_BLENDMODE_HPP_INCLUDED
