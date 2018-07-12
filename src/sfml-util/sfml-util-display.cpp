// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-display.cpp
//
#include "sfml-util-display.hpp"

#include "sfml-util/display.hpp"

namespace heroespath
{
namespace sfml_util
{

    const sf::Vector2f DisplaySize()
    {
        return sf::Vector2f(
            Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight());
    }

} // namespace sfml_util
} // namespace heroespath
