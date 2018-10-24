// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_TYPE_HELPERS_HPP_INCLUDED
#define HEROESPATH_MISC_TYPE_HELPERS_HPP_INCLUDED
//
// type-helpers.hpp
//
#include <type_traits>

namespace heroespath
{
namespace misc
{

    template <typename T>
    struct is_number
    {
        using value_type = bool;
        static constexpr bool value = std::is_arithmetic_v<T>;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template <>
    struct is_number<bool>
    {
        using value_type = bool;
        static constexpr bool value = false;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template <>
    struct is_number<const bool>
    {
        using value_type = bool;
        static constexpr bool value = false;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template <typename T>
    inline constexpr bool is_number_v = is_number<T>::value;

    template <typename T>
    struct is_number_non_floating_point
    {
        using value_type = bool;
        static constexpr bool value = (is_number_v<T> && !std::is_floating_point_v<T>);
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template <typename T>
    inline constexpr bool is_number_non_floating_point_v = is_number_non_floating_point<T>::value;

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TYPE_HELPERS_HPP_INCLUDED
