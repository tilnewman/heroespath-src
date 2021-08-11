// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_COMPILE_ASSERT_HPP_INCLUDED
#define HEROESPATH_MISC_COMPILE_ASSERT_HPP_INCLUDED
//
// compile-assert.hpp
//
#include "misc/type-helpers.hpp"

// forces a compile error in a template function when static_assert by itself won't work
#define M_HP_COMPILE_ERROR(message) static_assert(dependant_false_v<void>, message);

#define M_HP_COMPILE_ERROR_INVALID_TYPES_MSG(message, ...)                                         \
    static_assert(dependant_false_v<__VA_ARGS__>, "invalid types <" #__VA_ARGS__ "> (" message ")");

#define M_HP_COMPILE_ERROR_INVALID_TYPES(...) M_HP_COMPILE_ERROR_INVALID_TYPES_MSG("", __VA_ARGS__)

#define M_HP_COMPILE_ASSERT_TYPES_MSG(message, checker, ...)                                       \
    {                                                                                              \
        if constexpr (!(checker<__VA_ARGS__>))                                                     \
        {                                                                                          \
            M_HP_COMPILE_ERROR_INVALID_TYPES_MSG(                                                  \
                #checker "<" #__VA_ARGS__ "> (" message ")", __VA_ARGS__);                         \
        }                                                                                          \
    }

#define M_HP_COMPILE_ASSERT_TYPES(checker, ...)                                                    \
    M_HP_COMPILE_ASSERT_TYPES_MSG("", checker, __VA_ARGS__)

#endif // HEROESPATH_MISC_COMPILE_ASSERT_HPP_INCLUDED
