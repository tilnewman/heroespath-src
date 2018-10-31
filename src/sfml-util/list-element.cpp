// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// list-element.cpp
//
#include "list-element.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace sfml_util
{
    namespace ListElementHelpers
    {

        void DrawHelper(
            sf::RenderTarget & target, const sf::RenderStates states, const sf::Drawable & DRAWABLE)
        {
            target.draw(DRAWABLE, states);
        }

    } // namespace ListElementHelpers
} // namespace sfml_util
} // namespace heroespath
