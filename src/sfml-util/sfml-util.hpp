// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_HPP_INCLUDED
//
// sfml-util.hpp
//
#include "sfml-util/sfml-util-angles.hpp"
#include "sfml-util/sfml-util-center-of.hpp"
#include "sfml-util/sfml-util-center-to.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-color.hpp"
#include "sfml-util/sfml-util-direction.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-distance.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-image-manip.hpp"
#include "sfml-util/sfml-util-overlap.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-primitives.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"
#include "sfml-util/sfml-util-to-string.hpp"
#include "sfml-util/sfml-util-vector-math.hpp"

#include <tuple>

namespace sf
{

constexpr bool operator<(const sf::BlendMode & L, const sf::BlendMode & R)
{
    return std::tie(
               L.colorSrcFactor,
               L.colorDstFactor,
               L.colorEquation,
               L.alphaSrcFactor,
               L.alphaDstFactor,
               L.alphaEquation)
        < std::tie(
               R.colorSrcFactor,
               R.colorDstFactor,
               R.colorEquation,
               R.alphaSrcFactor,
               R.alphaDstFactor,
               R.alphaEquation);
}

} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    /*
    template <typename T, typename Return_t>
    constexpr Return_t FindPowerOf2GreaterThan(const T GREATER_THAN)
    {
        auto x{ static_cast<int>(GREATER_THAN) };
        auto result{ x };

        while (x &= (x ^ (~x + 1)))
        {
            result = x << 1;
        }

        return static_cast<Return_t>(result);
    }
    */

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_HPP_INCLUDED
