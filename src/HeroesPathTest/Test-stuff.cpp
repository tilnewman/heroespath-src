// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// Test-stuff.cpp
//  Tests of the test_stuff helper functions functions()
//
#include "Test-stuff.hpp"

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc"
#include <boost/test/unit_test.hpp>

namespace ts = test_stuff;

BOOST_AUTO_TEST_CASE(isUnique_CornerCases)
{
    const ts::IntVec_t EMPTY = {};

    BOOST_CHECK_MESSAGE(ts::isUnique(EMPTY), "empty vec=" << ts::vectorToString(EMPTY));

    const ts::IntVec_t FIRST_DUPLICATED = { 0, 1, 0 };

    BOOST_CHECK_MESSAGE(
        ts::isUnique(FIRST_DUPLICATED) == false,
        "first duplicated vec=" << ts::vectorToString(FIRST_DUPLICATED));

    const ts::IntVec_t LAST_DUPLICATED = { 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        ts::isUnique(LAST_DUPLICATED) == false,
        "last duplicated vec=" << ts::vectorToString(LAST_DUPLICATED));

    const ts::IntVec_t PATTERN_1 = { 0, 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        ts::isUnique(PATTERN_1) == false, "pattern 1 vec=" << ts::vectorToString(PATTERN_1));

    const ts::IntVec_t PATTERN_2 = { 0, 1, 0, 1 };

    BOOST_CHECK_MESSAGE(
        ts::isUnique(PATTERN_2) == false, "pattern 2 vec=" << ts::vectorToString(PATTERN_2));
}

BOOST_AUTO_TEST_CASE(isUnique_SingleValues)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        const ts::IntVec_t SINGLE_VALUE(1, VALUE);

        BOOST_CHECK_MESSAGE(
            ts::isUnique(SINGLE_VALUE), "single value vec=" << ts::vectorToString(SINGLE_VALUE));
    }
}

BOOST_AUTO_TEST_CASE(isUnique_RepeatedValues)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        const ts::IntVec_t REPEATED_VALUES(static_cast<std::size_t>(VALUE), VALUE);

        BOOST_CHECK_MESSAGE(
            (ts::isUnique(REPEATED_VALUES) == (VALUE < 2)),
            "repeated values vec=" << ts::vectorToString(REPEATED_VALUES));
    }
}

BOOST_AUTO_TEST_CASE(isUnique_CountingValues)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t countingValues;
        for (int i{ 0 }; i <= VALUE; ++i)
        {
            countingValues.push_back(i);
        }

        BOOST_CHECK_MESSAGE(
            ts::isUnique(countingValues),
            "counting values vec=" << ts::vectorToString(countingValues));
    }
}

BOOST_AUTO_TEST_CASE(isUnique_CountingValuesWithOneDuplicate)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t countingValuesWithDuplicate;
        for (int i{ 0 }; i <= VALUE; ++i)
        {
            if (i == 0)
            {
                countingValuesWithDuplicate.push_back(i);
            }

            countingValuesWithDuplicate.push_back(i);
        }

        BOOST_CHECK_MESSAGE(
            ts::isUnique(countingValuesWithDuplicate) == (countingValuesWithDuplicate.size() < 2),
            "counting values with duplicate vec="
                << ts::vectorToString(countingValuesWithDuplicate));
    }
}

BOOST_AUTO_TEST_CASE(isUnique_CountingValuesWithMultDuplicate)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t countingValuesWithDuplicate;
        for (int i{ 0 }; i <= VALUE; ++i)
        {
            countingValuesWithDuplicate.push_back(i);
            countingValuesWithDuplicate.push_back(i);
        }

        BOOST_CHECK_MESSAGE(
            ts::isUnique(countingValuesWithDuplicate) == (countingValuesWithDuplicate.empty()),
            "counting values with duplicate vec="
                << ts::vectorToString(countingValuesWithDuplicate));
    }
}

BOOST_AUTO_TEST_CASE(isSorted_CornerCases)
{
    const ts::IntVec_t EMPTY = {};

    BOOST_CHECK_MESSAGE(ts::isSortedDescending(EMPTY), "empty vec=" << ts::vectorToString(EMPTY));

    const ts::IntVec_t FIRST_DUPLICATED = { 0, 1, 0 };

    BOOST_CHECK_MESSAGE(
        ts::isSortedDescending(FIRST_DUPLICATED) == false,
        "first duplicated vec=" << ts::vectorToString(FIRST_DUPLICATED));

    const ts::IntVec_t LAST_DUPLICATED = { 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        ts::isSortedDescending(LAST_DUPLICATED),
        "last duplicated vec=" << ts::vectorToString(LAST_DUPLICATED));

    const ts::IntVec_t PATTERN_1 = { 0, 0, 1, 1 };

    BOOST_CHECK_MESSAGE(
        ts::isSortedDescending(PATTERN_1), "pattern 1 vec=" << ts::vectorToString(PATTERN_1));

    const ts::IntVec_t PATTERN_2 = { 0, 1, 0, 1 };

    BOOST_CHECK_MESSAGE(
        ts::isSortedDescending(PATTERN_2) == false,
        "pattern 2 vec=" << ts::vectorToString(PATTERN_2));
}

BOOST_AUTO_TEST_CASE(isSorted_SingleValues)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        const ts::IntVec_t SINGLE_VALUE(1, VALUE);

        BOOST_CHECK_MESSAGE(
            ts::isSortedDescending(SINGLE_VALUE),
            "single value vec=" << ts::vectorToString(SINGLE_VALUE));
    }
}

BOOST_AUTO_TEST_CASE(isSorted_RepeatedValues)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        const ts::IntVec_t REPEATED_VALUES(static_cast<std::size_t>(VALUE), VALUE);

        BOOST_CHECK_MESSAGE(
            ts::isSortedDescending(REPEATED_VALUES),
            "repeated values vec=" << ts::vectorToString(REPEATED_VALUES));
    }
}

BOOST_AUTO_TEST_CASE(isSorted_CountingValues)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t countingValues;
        for (int i{ 0 }; i <= VALUE; ++i)
        {
            countingValues.push_back(i);
        }

        BOOST_CHECK_MESSAGE(
            ts::isSortedDescending(countingValues),
            "counting values vec=" << ts::vectorToString(countingValues));
    }
}

BOOST_AUTO_TEST_CASE(isSorted_CountingValuesWithOneDuplicate)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t countingValuesWithDuplicate;
        for (int i{ 0 }; i <= VALUE; ++i)
        {
            if (VALUE == 0)
            {
                countingValuesWithDuplicate.push_back(i);
            }

            countingValuesWithDuplicate.push_back(i);
        }

        BOOST_CHECK_MESSAGE(
            ts::isSortedDescending(countingValuesWithDuplicate),
            "counting values with duplicate vec="
                << ts::vectorToString(countingValuesWithDuplicate));
    }
}

BOOST_AUTO_TEST_CASE(isSorted_CountingValuesWithMultDuplicate)
{
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t countingValuesWithDuplicate;
        for (int i{ 0 }; i <= VALUE; ++i)
        {
            countingValuesWithDuplicate.push_back(i);
            countingValuesWithDuplicate.push_back(i);
        }

        BOOST_CHECK_MESSAGE(
            ts::isSortedDescending(countingValuesWithDuplicate),
            "counting values with duplicate vec="
                << ts::vectorToString(countingValuesWithDuplicate));
    }
}
