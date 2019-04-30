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
#include <algorithm>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping two related immutable numbers.
    template <typename T>
    class Range
    {
    public:
        explicit Range(const T A = T(0), const T B = T(0))
            : a_(A)
            , b_(B)
        {}

        constexpr T A() const { return a_; }
        constexpr T B() const { return b_; }

        constexpr T From() const { return a_; }
        constexpr T To() const { return b_; }

        constexpr T Min() const { return misc::Min(a_, b_); }
        constexpr T Max() const { return misc::Max(a_, b_); }

        constexpr T Diff() const { return Max() - Min(); }

        constexpr T Mid() const { return Min() + (Diff() / T(2)); }

        constexpr T Avg() const { return (a_ + b_) / T(2); }

        constexpr float AvgFloat() const { return static_cast<float>(a_ + b_) * 0.5f; }

        constexpr double AvgDouble() const { return static_cast<double>(a_ + b_) * 0.5; }

    private:
        T a_;
        T b_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_FROMTO_HPP_INCLUDED
