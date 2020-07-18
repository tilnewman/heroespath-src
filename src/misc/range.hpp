// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_FROMTO_HPP_INCLUDED
#define HEROESPATH_MISC_FROMTO_HPP_INCLUDED
//
// trap.hpp
//
#include "misc/real.hpp"

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping two related immutable numbers.
    template <typename T>
    class Range
    {
    public:
        Range() = delete;

        constexpr Range(const T & A, const T & B) noexcept
            : a_(A)
            , b_(B)
        {}

        constexpr Range(const Range &) noexcept = default;
        constexpr Range(Range &&) noexcept = default;
        constexpr Range & operator=(const Range &) noexcept = default;
        constexpr Range & operator=(Range &&) noexcept = default;

        constexpr T A() const noexcept { return a_; }
        constexpr T B() const noexcept { return b_; }

        constexpr T From() const noexcept { return a_; }
        constexpr T To() const noexcept { return b_; }

        constexpr T Min() const noexcept { return misc::Min(a_, b_); }
        constexpr T Max() const { return misc::Max(a_, b_); }

        constexpr T Diff() const noexcept { return Max() - Min(); }

        template <typename Return_t = T>
        constexpr Return_t Mid() const noexcept
        {
            return (static_cast<Return_t>(Min()) + (static_cast<Return_t>(Diff()) / Return_t(2)));
        }

        template <typename Return_t = T>
        constexpr Return_t Avg() const noexcept
        {
            return ((static_cast<Return_t>(a_) + static_cast<Return_t>(b_)) / Return_t(2));
        }

    private:
        T a_;
        T b_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_FROMTO_HPP_INCLUDED
