// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_MATH_CONSTANTS_HPP_INCLUDED
#define HEROESPATH_MISC_MATH_CONSTANTS_HPP_INCLUDED
//
// type-helpers.hpp
//
#include "misc/type-helpers.hpp"

namespace heroespath
{
namespace misc
{
    namespace math
    {

        constexpr long double pi_literal
            = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651L;

        template <typename T>
        constexpr std::enable_if_t<are_floating_point_v<T>, T> pi = static_cast<T>(pi_literal);

        template <typename T>
        constexpr std::enable_if_t<are_floating_point_v<T>, T> half_pi
            = static_cast<T>(pi_literal / 2.0L);

        template <typename T>
        constexpr std::enable_if_t<are_floating_point_v<T>, T> one_over_two_pi
            = static_cast<T>(1.0L / (2.0L * pi_literal));

        template <typename T>
        constexpr std::enable_if_t<are_floating_point_v<T>, T>
            DegreesToRadians(const T DEGREES) noexcept
        {
            return ((DEGREES * pi<T>) / T(180));
        }

        template <typename T>
        constexpr std::enable_if_t<!are_floating_point_v<T>, T>
            DegreesToRadians(const T DEGREES) noexcept
        {
            return static_cast<T>(DegreesToRadians(static_cast<double>(DEGREES)));
        }

        template <typename T>
        constexpr std::enable_if_t<are_floating_point_v<T>, T>
            RadiansToDegrees(const T RADIANS) noexcept
        {
            return ((RADIANS * T(180)) / pi<T>);
        }

        template <typename T>
        constexpr std::enable_if_t<!are_floating_point_v<T>, T>
            RadiansToDegrees(const T RADIANS) noexcept
        {
            return static_cast<T>(RadiansToDegrees(static_cast<double>(RADIANS)));
        }

    } // namespace math
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_MATH_CONSTANTS_HPP_INCLUDED
