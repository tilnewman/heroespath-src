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
#include <iterator>
#include <type_traits>

namespace heroespath
{
namespace misc
{

    // Explanation of dependant_false_v<>
    //
    // The Problem:
    // Technically, any static_assert(false) can fail to compile even if this template is never
    // created/used/called.  For example, a template class that you want to prevent compiling if the
    // wrong type is used.  This won't always work in msvc or g++, but we often want it to...
    //
    //      template <typename T1, typename T2>
    //      struct T2CannotBeInt {};
    //
    //      template <typename T1>
    //      struct SecondTypeCannotBeInt<T1, int> { static_assert(false); };
    //
    // The Solution:
    // Create an 'always false' template.  That way the compiler won't know it is false until trying
    // to create it.  (see dependant_false below)  Now you can do this, which will always work:
    //
    //      template <typename T1>
    //      struct SecondTypeCannotBeInt<T1, int> { static_assert(dependant_false_v<T>); };
    //
    template <typename...>
    struct dependant_false
    {
        static constexpr bool value = false;
    };

    template <typename... T>
    constexpr bool dependant_false_v = dependant_false<T...>::value;

    template <typename T, typename... Ts>
    constexpr bool are_all_v
        = std::conjunction_v<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<Ts>>...>;

    template <typename T, typename... Ts>
    constexpr bool are_same_v = are_all_v<T, Ts...>;

    template <typename T, typename... Ts>
    constexpr bool are_any_v
        = std::disjunction_v<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<Ts>>...>;

    template <typename T, typename... Ts>
    constexpr bool are_none_v = !are_any_v<T, Ts...>;

    template <typename... T>
    constexpr bool are_none_bool_v = are_none_v<bool, T...>;

    // returns false for bool
    template <typename... T>
    constexpr bool are_signed_v = std::conjunction_v<std::is_signed<T>...>;

    template <typename... T>
    constexpr bool are_floating_point_v = std::conjunction_v<std::is_floating_point<T>...>;

    // returns true for bool
    template <typename... T>
    constexpr bool are_integral_v = std::conjunction_v<std::is_integral<T>...>;

    template <typename... T>
    constexpr bool are_integral_nobool_v = (are_integral_v<T...> && are_none_bool_v<T...>);

    template <typename... T>
    constexpr bool are_integral_signed_nobool_v
        = (are_integral_v<T...> && are_signed_v<T...> && are_none_bool_v<T...>);

    template <typename... T>
    constexpr bool are_integral_unsigned_nobool_v
        = (are_integral_v<T...> && !are_signed_v<T...> && are_none_bool_v<T...>);

    // returns true for bool
    template <typename... T>
    constexpr bool are_arithmetic_v = std::conjunction_v<std::is_arithmetic<T>...>;

    template <typename... T>
    constexpr bool are_arithmetic_nobool_v = (are_arithmetic_v<T...> && are_none_bool_v<T...>);

    template <typename... T>
    constexpr bool are_arithmetic_signed_nobool_v
        = (are_arithmetic_v<T...> && are_signed_v<T...> && are_none_bool_v<T...>);

    template <typename... T>
    constexpr bool are_arithmetic_unsigned_nobool_v
        = (are_arithmetic_v<T...> && !are_signed_v<T...> && are_none_bool_v<T...>);

    template <typename... T>
    constexpr bool are_enum_v = std::conjunction_v<std::is_enum<T>...>;

    template <typename... T>
    constexpr bool are_pointer_v = std::conjunction_v<std::is_pointer<T>...>;

    template <typename T, typename = void>
    struct iterator_value_type
    {};

    template <typename T>
    struct iterator_value_type<T, typename std::enable_if_t<are_pointer_v<T>>>
    {
        using type = std::remove_pointer_t<T>;
    };

    template <typename T>
    struct iterator_value_type<T, typename std::enable_if_t<!are_pointer_v<T>>>
    {
        using type = typename T::value_type;
    };

    template <typename T>
    using iterator_value_type_t = typename iterator_value_type<T>::type;

    namespace helpers
    {

        // not as clean as everything above, but see comment below as to why.
        template <typename T, typename Enable1_t = void, typename Enable2_t = void>
        struct is_iterator : std::false_type
        {};

        // this looks weird because MSVC still can't handle the easy way -DO NOT TOUCH
        template <typename T>
        struct is_iterator<
            T,
            typename std::enable_if_t<
                !are_same_v<typename std::iterator_traits<T>::value_type, void>>,
            typename std::enable_if_t<
                !are_same_v<typename std::iterator_traits<T>::iterator_category, void>>>
            : std::true_type
        {};

        template <typename T>
        constexpr bool is_iterator_v = is_iterator<T>::value;

        // this looks weird because MSVC still can't handle the easy way -DO NOT TOUCH
        template <typename, typename, typename = void>
        struct is_iterator_with_tag
        {
            static constexpr bool value = false;
        };

        // this looks weird because MSVC still can't handle the easy way -DO NOT TOUCH
        template <typename Tag_t, typename Iterator_t>
        struct is_iterator_with_tag<
            Tag_t,
            Iterator_t,
            typename std::enable_if_t<is_iterator_v<Iterator_t>>>
        {
            static constexpr bool value
                = (is_iterator_v<
                       Iterator_t> && std::is_convertible_v<typename std::iterator_traits<Iterator_t>::iterator_category, Tag_t>);
        };

        template <typename Tag_t, typename Iterator_t>
        constexpr bool is_iterator_with_tag_v = is_iterator_with_tag<Tag_t, Iterator_t>::value;

        template <typename Tag_t, typename... Iterators_t>
        constexpr bool are_iterator_with_tag_v
            = std::conjunction_v<is_iterator_with_tag<Tag_t, Iterators_t>...>;

    } // namespace helpers

    template <typename... T>
    constexpr bool are_iterator_v = std::conjunction_v<helpers::is_iterator<T>...>;

    template <typename... T>
    constexpr bool are_input_iterator_v
        = helpers::are_iterator_with_tag_v<std::input_iterator_tag, T...>;

    template <typename... T>
    constexpr bool are_output_iterator_v
        = helpers::are_iterator_with_tag_v<std::output_iterator_tag, T...>;

    template <typename... T>
    constexpr bool are_forward_iterator_v
        = helpers::are_iterator_with_tag_v<std::forward_iterator_tag, T...>;

    template <typename... T>
    constexpr bool are_bidirectional_iterator_v
        = helpers::are_iterator_with_tag_v<std::bidirectional_iterator_tag, T...>;

    template <typename... T>
    constexpr bool are_random_access_iterator_v
        = helpers::are_iterator_with_tag_v<std::random_access_iterator_tag, T...>;

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TYPE_HELPERS_HPP_INCLUDED
