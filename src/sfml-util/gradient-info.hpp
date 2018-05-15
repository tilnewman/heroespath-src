// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GRADIENTINFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GRADIENTINFO_HPP_INCLUDED
//
// gradient-info.hpp
//  Gradient drawing helper struct.
//
#include "sfml-util/corner-enum.hpp"
#include "sfml-util/side-enum.hpp"

#include <SFML/Graphics/Color.hpp>

namespace heroespath
{
namespace sfml_util
{

    // Encapsulates everything needed to draw a gradient.
    // Note: Designed so that using the constructor (with all default, or no params)
    //      is safe and will cause nothing (no gradient) to be drawn.
    struct GradientInfo
    {
        explicit GradientInfo(
            const sf::Color & COLOR_TO = sf::Color::Transparent,
            const unsigned int CORNERS = sfml_util::Corner::None,
            const unsigned int SIDES = sfml_util::Side::None,
            const sf::Color & COLOR_FROM = sf::Color::Transparent);

        unsigned int sides;
        unsigned int corners;
        sf::Color color_from;
        sf::Color color_to;
    };

    bool operator==(const GradientInfo & L, const GradientInfo & R);

    inline bool operator!=(const GradientInfo & L, const GradientInfo & R) { return !(L == R); }
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GRADIENTINFO_HPP_INCLUDED
