// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// gradient-info.cpp
//
#include "gradient-info.hpp"

namespace heroespath
{
namespace sfml_util
{

    GradientInfo::GradientInfo(
        const sf::Color & COLOR_TO,
        const unsigned int CORNERS,
        const unsigned int SIDES,
        const sf::Color & COLOR_FROM)
        : sides(SIDES)
        , corners(CORNERS)
        , color_from(COLOR_FROM)
        , color_to(COLOR_TO)
    {}

    bool operator==(const GradientInfo & L, const GradientInfo & R)
    {
        return (
            (L.sides == R.sides) && (L.corners == R.corners) && (L.color_from == R.color_from)
            && (L.color_to == R.color_to));
    }
} // namespace sfml_util
} // namespace heroespath
