///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
        explicit Range(const T A = 0, const T B = 0)
            : a_(A)
            , b_(B)
        {}

        inline T A() const { return a_; }
        inline T B() const { return b_; }

        inline T From() const { return a_; }
        inline T To() const { return b_; }

        inline T Min() const { return std::min(a_, b_); }
        inline T Max() const { return std::max(a_, b_); }

        inline T Diff() const { return Max() - Min(); }

        inline T Mid() const { return Min() + (Diff() / T(2)); }

        inline T Avg() const { return (a_ + b_) / T(2); }

        inline float AvgFloat() const { return static_cast<float>(a_ + b_) * 0.5f; }

        inline double AvgDouble() const { return static_cast<double>(a_ + b_) * 0.5; }

    private:
        T a_;
        T b_;
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_FROMTO_HPP_INCLUDED
