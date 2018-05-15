// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// resolution.cpp
//
#include "resolution.hpp"

#include <iomanip>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{

    Resolution::Resolution(
        const unsigned int WIDTH,
        const unsigned int HEIGHT,
        const unsigned int BITS_PER_PIXEL,
        const std::string & NAME,
        const std::string & ASPECT_RATIO)
        : name(NAME)
        , width(WIDTH)
        , height(HEIGHT)
        , aspect_ratio(ASPECT_RATIO)
        , bits_per_pixel(BITS_PER_PIXEL)
    {}

    Resolution::Resolution(const sf::VideoMode & VM)
        : name("unknown")
        , width(VM.width)
        , height(VM.height)
        , aspect_ratio("unknown")
        , bits_per_pixel(VM.bitsPerPixel)
    {}

    const std::string Resolution::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "[" : "") << std::setw(4) << width << "x" << std::setw(4) << height
           << " : " << std::setw(2) << bits_per_pixel << " " << std::setw(5) << aspect_ratio << " "
           << std::setw(7) << name << ((WILL_WRAP) ? "]" : "");
        return ss.str();
    }

    bool operator==(const Resolution & L, const Resolution & R)
    {
        return std::tie(L.name, L.width, L.height, L.aspect_ratio, L.bits_per_pixel)
            == std::tie(R.name, R.width, R.height, R.aspect_ratio, R.bits_per_pixel);
    }

    bool operator==(const Resolution & R, const sf::VideoMode & V)
    {
        // ignore aspect ratio and name
        return std::tie(R.width, R.height, R.bits_per_pixel)
            == std::tie(V.width, V.height, V.bitsPerPixel);
    }

} // namespace sfml_util
} // namespace heroespath
