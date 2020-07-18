// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_BIT_UTIL_HPP_INCLUDED
#define HEROESPATH_MISC_BIT_UTIL_HPP_INCLUDED
//
// bit-util.hpp
//
#include "misc/type-helpers.hpp"

namespace heroespath
{
namespace misc
{

    template <typename T>
    constexpr std::enable_if_t<(are_arithmetic_unsigned_nobool_v<T> || are_enum_v<T>), std::size_t>
        CountBits(const T NUMBER_ORIG) noexcept
    {
        if constexpr (are_enum_v<T>)
        {
            static_assert(
                are_arithmetic_unsigned_nobool_v<std::underlying_type_t<T>>,
                "You can only give this function an enum that has an unsigned underlying type.");
        }

        auto numberU64 = static_cast<std::uint64_t>(NUMBER_ORIG);

        std::size_t count = 0;
        for (; numberU64; count++)
        {
            numberU64 &= (numberU64 - 1);
        }

        return count;
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_BIT_UTIL_HPP_INCLUDED
