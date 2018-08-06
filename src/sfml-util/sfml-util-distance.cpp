// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-distance.cpp
//
#include "sfml-util-distance.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{

    float Distance(const sf::Sprite & A, const sf::Sprite & B)
    {
        return Distance(CenterOf(A), CenterOf(B));
    }

} // namespace sfml_util
} // namespace heroespath
