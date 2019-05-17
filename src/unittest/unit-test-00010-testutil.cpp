// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-00010-testutil.cpp
//
#define BOOST_TEST_MODULE "testutil_library"

#include <boost/test/unit_test.hpp>

#include "testutil/common.hpp"

using namespace heroespath;
using namespace heroespath::test;

// this is the only unit test that does not require any of the game's subsystems, not even the
// logger or the config file, so there is no GlobalFixture put in place here.

BOOST_AUTO_TEST_CASE(is_unique_corner_cases)
{
    const std::vector<int> EMPTY;

    const std::vector<int> FIRST_DUPLICATED = { 0, 1, 0 };

    BOOST_CHECK_MESSAGE(
        test::isUnique(FIRST_DUPLICATED) == false,
        "first duplicated vec=" << test::containerToString(FIRST_DUPLICATED));

    const std::vector<int> LAST_DUPLICATED = { 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        test::isUnique(LAST_DUPLICATED) == false,
        "last duplicated vec=" << test::containerToString(LAST_DUPLICATED));

    const std::vector<int> PATTERN_1 = { 0, 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        test::isUnique(PATTERN_1) == false, "pattern 1 vec=" << test::containerToString(PATTERN_1));

    const std::vector<int> PATTERN_2 = { 0, 1, 0, 1 };

    BOOST_CHECK_MESSAGE(
        test::isUnique(PATTERN_2) == false, "pattern 2 vec=" << test::containerToString(PATTERN_2));
}

BOOST_AUTO_TEST_CASE(is_unique_single_values)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        const std::vector<int> SINGLE_VALUE(1, VALUE);

        BOOST_CHECK_MESSAGE(
            test::isUnique(SINGLE_VALUE),
            "single value vec=" << test::containerToString(SINGLE_VALUE));
    }
}

BOOST_AUTO_TEST_CASE(is_unique_repeated_values)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        const std::vector<int> REPEATED_VALUES(static_cast<std::size_t>(VALUE), VALUE);

        BOOST_CHECK_MESSAGE(
            (test::isUnique(REPEATED_VALUES) == (VALUE < 2)),
            "repeated values vec=" << test::containerToString(REPEATED_VALUES));
    }
}

BOOST_AUTO_TEST_CASE(is_unique_counting_values)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        std::vector<int> countingValues;
        for (int i { 0 }; i <= VALUE; ++i)
        {
            countingValues.emplace_back(i);
        }

        BOOST_CHECK_MESSAGE(
            test::isUnique(countingValues),
            "counting values vec=" << test::containerToString(countingValues));
    }
}

BOOST_AUTO_TEST_CASE(is_unique_counting_values_with_one_duplicate)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        std::vector<int> countingValuesWithDuplicate;
        for (int i { 0 }; i <= VALUE; ++i)
        {
            if (i == 0)
            {
                countingValuesWithDuplicate.emplace_back(i);
            }

            countingValuesWithDuplicate.emplace_back(i);
        }

        BOOST_CHECK_MESSAGE(
            test::isUnique(countingValuesWithDuplicate) == (countingValuesWithDuplicate.size() < 2),
            "counting values with duplicate vec="
                << test::containerToString(countingValuesWithDuplicate));
    }
}

BOOST_AUTO_TEST_CASE(is_unique_counting_values_with_multiple_duplicate)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        std::vector<int> countingValuesWithDuplicate;
        for (int i { 0 }; i <= VALUE; ++i)
        {
            countingValuesWithDuplicate.emplace_back(i);
            countingValuesWithDuplicate.emplace_back(i);
        }

        BOOST_CHECK_MESSAGE(
            test::isUnique(countingValuesWithDuplicate) == (countingValuesWithDuplicate.empty()),
            "counting values with duplicate vec="
                << test::containerToString(countingValuesWithDuplicate));
    }
}

BOOST_AUTO_TEST_CASE(is_sorted_corner_cases)
{
    const std::vector<int> EMPTY;

    BOOST_CHECK_MESSAGE(
        test::isSortedDescending(EMPTY), "empty vec=" << test::containerToString(EMPTY));

    const std::vector<int> FIRST_DUPLICATED = { 0, 1, 0 };

    BOOST_CHECK_MESSAGE(
        test::isSortedDescending(FIRST_DUPLICATED) == false,
        "first duplicated vec=" << test::containerToString(FIRST_DUPLICATED));

    const std::vector<int> LAST_DUPLICATED = { 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        test::isSortedDescending(LAST_DUPLICATED),
        "last duplicated vec=" << test::containerToString(LAST_DUPLICATED));

    const std::vector<int> PATTERN_1 = { 0, 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        test::isSortedDescending(PATTERN_1),
        "pattern 1 vec=" << test::containerToString(PATTERN_1));

    const std::vector<int> PATTERN_2 = { 0, 1, 0, 1 };

    BOOST_CHECK_MESSAGE(
        test::isSortedDescending(PATTERN_2) == false,
        "pattern 2 vec=" << test::containerToString(PATTERN_2));
}

BOOST_AUTO_TEST_CASE(is_sorted_single_values)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        const std::vector<int> SINGLE_VALUE(1, VALUE);

        BOOST_CHECK_MESSAGE(
            test::isSortedDescending(SINGLE_VALUE),
            "single value vec=" << test::containerToString(SINGLE_VALUE));
    }
}

BOOST_AUTO_TEST_CASE(is_sorted_repeated_values)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        const std::vector<int> REPEATED_VALUES(static_cast<std::size_t>(VALUE), VALUE);

        BOOST_CHECK_MESSAGE(
            test::isSortedDescending(REPEATED_VALUES),
            "repeated values vec=" << test::containerToString(REPEATED_VALUES));
    }
}

BOOST_AUTO_TEST_CASE(is_sorted_counting_values)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        std::vector<int> countingValues;
        for (int i { 0 }; i <= VALUE; ++i)
        {
            countingValues.emplace_back(i);
        }

        BOOST_CHECK_MESSAGE(
            test::isSortedDescending(countingValues),
            "counting values vec=" << test::containerToString(countingValues));
    }
}

BOOST_AUTO_TEST_CASE(is_sorted_counting_values_with_one_duplicate)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        std::vector<int> countingValuesWithDuplicate;
        for (int i { 0 }; i <= VALUE; ++i)
        {
            if (VALUE == 0)
            {
                countingValuesWithDuplicate.emplace_back(i);
            }

            countingValuesWithDuplicate.emplace_back(i);
        }

        BOOST_CHECK_MESSAGE(
            test::isSortedDescending(countingValuesWithDuplicate),
            "counting values with duplicate vec="
                << test::containerToString(countingValuesWithDuplicate));
    }
}

BOOST_AUTO_TEST_CASE(is_sorted_counting_values_with_multiple_duplicate)
{
    for (const auto VALUE : test::smallValuesWorthTestingInt)
    {
        std::vector<int> countingValuesWithDuplicate;
        for (int i { 0 }; i <= VALUE; ++i)
        {
            countingValuesWithDuplicate.emplace_back(i);
            countingValuesWithDuplicate.emplace_back(i);
        }

        BOOST_CHECK_MESSAGE(
            test::isSortedDescending(countingValuesWithDuplicate),
            "counting values with duplicate vec="
                << test::containerToString(countingValuesWithDuplicate));
    }
}
