// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-color.cpp
//
#include "sfml-util-color.hpp"

#include <SFML/Graphics/Text.hpp>

namespace heroespath
{
namespace sfml_util
{

    void SetColor(sf::Text & text, const sf::Color & COLOR)
    {
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_LINUX
        text.setColor(COLOR);
#else
        text.setFillColor(COLOR);
#endif
    }

    const sf::Color Transition(const sf::Color & FROM, const sf::Color & TO, const float RATIO)
    {
        auto const RED{ static_cast<sf::Uint8>(
            FROM.r
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.r) - static_cast<float>(FROM.r)) * RATIO)) };

        auto const GREEN{ static_cast<sf::Uint8>(
            FROM.g
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.g) - static_cast<float>(FROM.g)) * RATIO)) };

        auto const BLUE{ static_cast<sf::Uint8>(
            FROM.b
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.b) - static_cast<float>(FROM.b)) * RATIO)) };

        auto const ALPHA{ static_cast<sf::Uint8>(
            FROM.a
            + static_cast<sf::Uint8>(
                  (static_cast<float>(TO.a) - static_cast<float>(FROM.a)) * RATIO)) };

        return sf::Color(RED, GREEN, BLUE, ALPHA);
    }

} // namespace sfml_util
} // namespace heroespath