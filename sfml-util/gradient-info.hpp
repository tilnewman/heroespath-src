#ifndef SFMLUTIL_GRADIENTINFO_INCLUDED
#define SFMLUTIL_GRADIENTINFO_INCLUDED
//
// gradient-info.hpp
//  Gradient drawing helper struct.
//
#include "sfml-util/corner-enum.hpp"
#include "sfml-util/side-enum.hpp"

#include <SFML/Graphics/Color.hpp>


namespace sfml_util
{

    //Encapsulates everything needed to draw a gradient.
    //Note: Designed so that using the constructor (with all default, or no params)
    //      is safe and will cause nothing (no gradient) to be drawn.
    struct GradientInfo
    {
        GradientInfo(const sf::Color &  COLOR_TO   = sf::Color::Transparent,
                     const unsigned int CORNERS    = sfml_util::Corner::None,
                     const unsigned int SIDES      = sfml_util::Side::None,
                     const sf::Color &  COLOR_FROM = sf::Color::Transparent);

        unsigned int sides;
        unsigned int corners;
        sf::Color    color_from;
        sf::Color    color_to;
    };

    bool operator==(const GradientInfo & L, const GradientInfo & R);

    bool operator!=(const GradientInfo & L, const GradientInfo & R);

}
#endif //SFMLUTIL_GRADIENTINFO_INCLUDED
