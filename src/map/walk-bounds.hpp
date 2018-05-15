// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_WALK_BOUNDS_HPP_INCLUDED
#define HEROESPATH_MAP_WALK_BOUNDS_HPP_INCLUDED
//
// walk-bounds.hpp
//
#include "misc/vector-map.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <vector>

namespace heroespath
{
namespace map
{

    using WalkRectMap_t = misc::VectorMap<std::size_t, std::vector<sf::FloatRect>>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_WALK_BOUNDS_HPP_INCLUDED
