// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// display.cpp
//
#include "display.hpp"

#include "sfml-util/display.hpp"

namespace heroespath
{
namespace sfutil
{

    const sf::Vector2f DisplaySize()
    {
        return sf::Vector2f(
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight());
    }

} // namespace sfutil
} // namespace heroespath
