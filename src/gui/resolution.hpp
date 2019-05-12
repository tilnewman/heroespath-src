// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_RESOLUTION_HPP_INCLUDED
#define HEROESPATH_GUI_RESOLUTION_HPP_INCLUDED
//
// resolution.hpp
//  A class that contains all the info about a screen resolution.
//
#include <SFML/Window/VideoMode.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Encapsulates everything about a screen resolution
    struct Resolution
    {
        explicit Resolution(
            const unsigned int WIDTH = 0,
            const unsigned int HEIGHT = 0,
            const unsigned int BITS_PER_PIXEL = 0,
            const std::string & NAME = "",
            const std::string & ASPECT_RATIO = "");

        explicit Resolution(const sf::VideoMode & VM);

        const std::string ToString(const bool WILL_WRAP = true) const;

        std::string name;
        unsigned int width;
        unsigned int height;
        std::string aspect_ratio;
        unsigned int bits_per_pixel;
    };

    using ResolutionVec_t = std::vector<Resolution>;

    bool operator==(const Resolution & L, const Resolution & R);

    inline bool operator!=(const Resolution & L, const Resolution & R) { return !(L == R); }

    bool operator<(const Resolution & L, const Resolution & R);

    bool operator==(const Resolution & R, const sf::VideoMode & V);

    inline bool operator!=(const Resolution & R, const sf::VideoMode & V) { return !(R == V); }

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_RESOLUTION_HPP_INCLUDED
