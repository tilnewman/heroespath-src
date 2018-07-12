// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_ANGLES_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_ANGLES_HPP_INCLUDED
//
// sfml-util-angles.hpp
//
#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{

    // assumes 'facing right'
    float GetAngleInDegrees(const sf::Vector2f & BEGIN_POS_V, const sf::Vector2f & END_POS_V);

    const sf::Vector2f ProjectToScreenEdge(
        const sf::Vector2f & FIRST_POS_V,
        const sf::Vector2f & SECOND_POS_V,
        const sf::Vector2f & PROJECTED_IMAGE_SIZE);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_ANGLES_HPP_INCLUDED
