// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-direction.cpp
//
#include "sfml-util-direction.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{

    sfml_util::Direction::Enum DirectionFromAToB(
        const sf::Sprite & A, const sf::Sprite & B, const sfml_util::Direction::Enum DEFAULT)
    {
        return DirectionFromAToB(CenterOf(A), CenterOf(B), DEFAULT);
    }

} // namespace sfml_util
} // namespace heroespath
