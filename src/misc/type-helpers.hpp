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

namespace heroespath
{
namespace misc
{
    // static_assert won't work in template functions, so this is a handy work-around.
    // usage:  static_assert(dependant_false_v<T>, message);
    template <typename... T>
    struct dependant_false
    {
        static constexpr bool value = false;
    };

    template <typename... T>
    inline constexpr bool dependant_false_v = dependant_false<T...>::value;

    template <typename T, typename... Ts>
    struct are_any : std::disjunction<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<Ts>>...>
    {};

    template <typename... T>
    inline constexpr bool are_any_v = are_any<T...>::value;

    template <typename T, typename... Ts>
    struct are_none : std::integral_constant<bool, (are_any_v<T, Ts...> == false)>
    {};

    template <typename... T>
    inline constexpr bool are_none_v = are_none<T...>::value;

    template <typename T, typename... Ts>
    struct are_all : std::conjunction<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<Ts>>...>
    {};

    template <typename... T>
    inline constexpr bool are_all_v = are_all<T...>::value;

    template <typename... T>
    using are_same = are_all<T...>;

    template <typename... T>
    inline constexpr bool are_same_v = are_same<T...>::value;

    template <typename... T>
    struct are_floating_point : std::conjunction<std::is_floating_point<std::remove_cv_t<T>>...>
    {};

    template <typename... T>
    inline constexpr bool are_floating_point_v = are_floating_point<T...>::value;

    template <typename... T>
    struct are_all_enum : std::conjunction<std::is_enum<std::remove_cv_t<T>>...>
    {};

    template <typename... T>
    inline constexpr bool are_all_enum_v = are_all_enum<T...>::value;

    template <typename... T>
    struct are_any_enum : std::disjunction<std::is_enum<std::remove_cv_t<T>>...>
    {};

    template <typename... T>
    inline constexpr bool are_any_enum_v = are_any_enum<T...>::value;

    template <typename... T>
    struct are_integral : std::conjunction<std::is_integral<std::remove_cv_t<T>>...>
    {};

    template <typename... T>
    inline constexpr bool are_integral_v = are_integral<T...>::value;

    template <typename... T>
    struct are_integral_nobool
        : std::integral_constant<bool, (are_integral_v<T...> && are_none_v<bool, T...>)>
    {};

    template <typename... T>
    inline constexpr bool are_integral_nobool_v = are_integral_nobool<T...>::value;

    template <typename... T>
    struct are_arithmetic : std::conjunction<std::is_arithmetic<std::remove_cv_t<T>>...>
    {};

    template <typename... T>
    inline constexpr bool are_arithmetic_v = are_arithmetic<T...>::value;

    template <typename... T>
    struct are_arithmetic_nobool
        : std::integral_constant<bool, (are_arithmetic_v<T...> && are_none_v<bool, T...>)>
    {};

    template <typename... T>
    inline constexpr bool are_arithmetic_nobool_v = are_arithmetic_nobool<T...>::value;

    template <typename... T>
    using are_number = are_arithmetic_nobool<T...>;

    template <typename... T>
    inline constexpr bool are_number_v = are_number<T...>::value;

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TYPE_HELPERS_HPP_INCLUDED
