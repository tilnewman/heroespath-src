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
#ifndef TEST_TESTSTUFF_INCLUDED
#define TEST_TESTSTUFF_INCLUDED
//
// Test-stuff.hpp
//  Helpful testing data and functions. 
//
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef> //for std::size_t


namespace test_stuff
{
    using IntVec_t = std::vector<int>;

    const IntVec_t TEST_COUNTS =
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 16, 17, 19, 20, 21 };

    struct Constants
    {
        static const int RANDOM_RETRY_COUNT = 1000;
    };

    template <typename T>
    const std::string vectorToString(const std::vector<T> & VEC)
    {
        std::ostringstream ss;

        ss << "[";

        for (typename std::vector<T>::const_iterator iter(VEC.begin()); iter != VEC.end(); ++iter)
        {
            if (iter != VEC.begin())
            {
                ss << ", ";
            }

            ss << * iter;
        }

        ss << "]";

        return ss.str();
    }

    template <typename T>
    bool isUnique(const std::vector<T> & VEC)
    {
        std::set<T> set;
        for (auto const & VALUE : VEC)
        {
            if (set.find(VALUE) != set.end())
            {
                return false;
            }

            set.insert(VALUE);
        }

        return true;
    }

    template <typename T>
    bool isSortedDescending(const std::vector<T> & VEC)
    {
        auto const VEC_SIZE{ VEC.size() };

        if (VEC_SIZE < 2)
        {
            return true;
        }

        for (std::size_t i{ 1 }; i < VEC_SIZE; ++i)
        {
            if (VEC[i] < VEC[i - 1])
            {
                return false;
            }
        }

        return true;
    }
}

#endif //TEST_TESTSTUFF_INCLUDED
