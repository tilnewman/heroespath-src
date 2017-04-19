//
// resolution.cpp
//
#include "resolution.hpp"
#include <sstream>
#include <iomanip>

namespace sfml_util
{

    Resolution::Resolution(const unsigned int  WIDTH,
                           const unsigned int  HEIGHT,
                           const unsigned int  BITS_PER_PIXEL,
                           const std::string & NAME,
                           const std::string & ASPECT_RATIO)
    :
        name          (NAME),
        width         (WIDTH),
        height        (HEIGHT),
        aspect_ratio  (ASPECT_RATIO),
        bits_per_pixel(BITS_PER_PIXEL)
    {}


    Resolution::Resolution(const sf::VideoMode & VM)
    :
        name          ("unknown"),
        width         (VM.width),
        height        (VM.height),
        aspect_ratio  ("unknown"),
        bits_per_pixel(VM.bitsPerPixel)
    {}


    const std::string Resolution::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;
        ss  << ((WILL_WRAP) ? "[" : "")
            << std::setw(4) << width
            << "x"
            << std::setw(4) << height
            << " : "
            << std::setw(2) << bits_per_pixel
            << " "
            << std::setw(5) << aspect_ratio
            << " "
            << std::setw(7) << name
            << ((WILL_WRAP) ? "]" : "");
        return ss.str();
    }


    bool operator==(const Resolution & L, const Resolution & R)
    {
        return ((L.name == R.name)     &&
                (L.width == R.width)   &&
                (L.height == R.height) &&
                (L.bits_per_pixel == R.bits_per_pixel) &&
                (L.aspect_ratio == R.aspect_ratio));
    }


    bool operator!=(const Resolution & L, const Resolution & R)
    {
        return ! (L == R);
    }


    bool operator==(const Resolution & R, const sf::VideoMode & V)
    {
        return ((R.width == V.width) && (R.height == V.height) && (R.bits_per_pixel == V.bitsPerPixel));
    }


    bool operator!=(const Resolution & R, const sf::VideoMode & V)
    {
        return ! (R == V);
    }


    bool operator==(const sf::VideoMode & V, const Resolution & R)
    {
        return (R == V);
    }


    bool operator!=(const sf::VideoMode & V, const Resolution & R)
    {
        return (R != V);
    }

}
