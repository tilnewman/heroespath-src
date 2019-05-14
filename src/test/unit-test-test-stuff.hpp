// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef TEST_TESTSTUFF_INCLUDED
#define TEST_TESTSTUFF_INCLUDED
//
// Test-stuff.hpp
//  Helpful testing data and functions.
//
#include <array>
#include <cstddef> //for std::size_t
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include <SFML/Graphics.hpp>

namespace heroespath
{
namespace test
{
    constexpr std::size_t randomRetryCount = 1000;

    constexpr std::array<int, 24> smallValuesWorthTestingInt
        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 16, 17, 19, 20, 21, 29, 30, 31, 32, 33 };

    constexpr std::size_t smallValuesWorthTestingCount = smallValuesWorthTestingInt.size();

    template <typename T>
    constexpr std::array<T, smallValuesWorthTestingCount> smallValuesWorthTesting()
    {
        std::array<T, smallValuesWorthTestingCount> temp;

        if constexpr (std::is_same_v<T, int>)
        {
            temp = smallValuesWorthTestingInt;
        }
        else
        {
            for (std::size_t i(0); i < temp.size(); ++i)
            {
                temp[i] = static_cast<T>(smallValuesWorthTestingInt[i]);
            }
        }

        return temp;
    }

    template <typename T>
    const std::string containerToString(const T & CONTAINER)
    {
        std::ostringstream ss;

        ss << "[";

        auto iter = std::cbegin(CONTAINER);
        while (iter != std::cend(CONTAINER))
        {
            if (iter != std::cbegin(CONTAINER))
            {
                ss << ", ";
            }

            ss << *iter;

            ++iter;
        }

        ss << "]";

        return ss.str();
    }

    template <typename T>
    bool isUnique(const T & CONTAINER)
    {
        if (CONTAINER.size() <= 1)
        {
            return true;
        }

        std::set<typename T::value_type> set;

        auto iter = std::cbegin(CONTAINER);
        while (iter != std::cend(CONTAINER))
        {
            const auto & VALUE = *iter++;

            if (set.find(VALUE) == std::end(set))
            {
                set.insert(VALUE);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    template <typename T>
    bool isSortedDescending(const T & CONTAINER)
    {
        auto const SIZE { CONTAINER.size() };

        if (SIZE <= 1)
        {
            return true;
        }

        for (std::size_t i(1); i < SIZE; ++i)
        {
            if (CONTAINER[i - 1] > CONTAINER[i])
            {
                return false;
            }
        }

        return true;
    }

} // namespace test
} // namespace heroespath

#endif // TEST_TESTSTUFF_INCLUDED
