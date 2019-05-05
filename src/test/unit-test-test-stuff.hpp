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
#include <cstddef> //for std::size_t
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace test_stuff
{
using IntVec_t = std::vector<int>;

const IntVec_t TEST_COUNTS = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 16, 17, 19, 20, 21 };

struct Constants
{
    explicit Constants(
        const int RANDOM_RETRY_COUNT_PARAM = 1000,
        const double CLOSE_ENOUGH_EPSILON_PARAM = 0.000001,
        const float CLOSE_ENOUGH_EPSILON_F_PARAM = 0.000001f)
        : CLOSE_ENOUGH_EPSILON(CLOSE_ENOUGH_EPSILON_PARAM)
        , CLOSE_ENOUGH_EPSILON_F(CLOSE_ENOUGH_EPSILON_F_PARAM)
        , RANDOM_RETRY_COUNT(RANDOM_RETRY_COUNT_PARAM)
    {}

    const double CLOSE_ENOUGH_EPSILON;
    const float CLOSE_ENOUGH_EPSILON_F;
    const int RANDOM_RETRY_COUNT;
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

        ss << *iter;
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
    auto const VEC_SIZE { VEC.size() };

    if (VEC_SIZE < 2)
    {
        return true;
    }

    for (std::size_t i { 1 }; i < VEC_SIZE; ++i)
    {
        if (VEC[i] < VEC[i - 1])
        {
            return false;
        }
    }

    return true;
}
} // namespace test_stuff

#endif // TEST_TESTSTUFF_INCLUDED
