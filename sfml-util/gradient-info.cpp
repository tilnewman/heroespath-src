//
// gradient-info.cpp
//
#include "gradient-info.hpp"


namespace sfml_util
{

    GradientInfo::GradientInfo(const sf::Color &  COLOR_TO,
                               const unsigned int CORNERS,
                               const unsigned int SIDES,
                               const sf::Color &  COLOR_FROM)
    :
        sides     (SIDES),
        corners   (CORNERS),
        color_from(COLOR_FROM),
        color_to  (COLOR_TO)
    {}


    bool operator==(const GradientInfo & L, const GradientInfo & R)
    {
        return ((L.sides == R.sides) &&
                (L.corners == R.corners) &&
                (L.color_from == R.color_from) &&
                (L.color_to == L.color_to));
    }


    bool operator!=(const GradientInfo & L, const GradientInfo & R)
    {
        return ! (L == R);
    }

}
