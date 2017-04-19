#ifndef APPBASE_SFMLUTIL_RESOLUTION_INCLUDED
#define APPBASE_SFMLUTIL_RESOLUTION_INCLUDED
//
// resolution.hpp
//  A class that contains all the info about a screen resolution.
//
#include "sfml-util/sfml-graphics.hpp"

#include <string>
#include <vector>


namespace sfml_util
{

    //Enapsulates everything about a screen resolution except for the bits-per-pixel
    struct Resolution
    {
        Resolution(const unsigned int  WIDTH          = 0,
                   const unsigned int  HEIGHT         = 0,
                   const unsigned int  BITS_PER_PIXEL = 0,
                   const std::string & NAME           = "",
                   const std::string & ASPECT_RATIO   = "");

        Resolution(const sf::VideoMode & VM);

        const std::string ToString(const bool WILL_WRAP = true) const;

        std::string  name;
        unsigned int width;
        unsigned int height;
        std::string  aspect_ratio;
        unsigned int bits_per_pixel;
    };

    using ResolutionVec_t = std::vector<Resolution>;

    bool operator==(const Resolution & L, const Resolution & R);

    bool operator!=(const Resolution & L, const Resolution & R);

    bool operator==(const Resolution & R, const sf::VideoMode & V);

    bool operator!=(const Resolution & R, const sf::VideoMode & V);


}

#endif //APPBASE_SFMLUTIL_RESOLUTION_INCLUDED
