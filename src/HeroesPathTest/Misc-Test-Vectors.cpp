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
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc"

#include <boost/test/unit_test.hpp>

#include "Test-stuff.hpp"

#include "misc/vectors.hpp"

#include <algorithm>


namespace ts = test_stuff;


BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToSingle)
{
    const ts::IntVec_t EMPTY;

    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t singleValue(1, VALUE);
        const ts::IntVec_t SINGLE_VALUE_BEFORE{ singleValue };
        misc::Vector::Append(EMPTY, singleValue, misc::Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << ts::vectorToString(SINGLE_VALUE_BEFORE)
            << ", single value vec after=" << ts::vectorToString(singleValue));

        misc::Vector::Append(EMPTY, singleValue, misc::Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << ts::vectorToString(SINGLE_VALUE_BEFORE)
            << ", single value vec after=" << ts::vectorToString(singleValue));
    }
}


BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToMultiple)
{
    const ts::IntVec_t EMPTY;

    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t multipleValues(static_cast<std::size_t>(VALUE), VALUE);
        const ts::IntVec_t MULTIPLE_VALUES_BEFORE{ multipleValues };
        misc::Vector::Append(EMPTY, multipleValues, misc::Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(multipleValues == MULTIPLE_VALUES_BEFORE,
            "mutliple values vec before=" << ts::vectorToString(MULTIPLE_VALUES_BEFORE)
            << ", multiple values vec after=" << ts::vectorToString(multipleValues));

        misc::Vector::Append(EMPTY, multipleValues, misc::Vector::SortOpt::SortAndUnique);

        ts::IntVec_t sortedAndUniqued{ multipleValues };

        std::sort(sortedAndUniqued.begin(), sortedAndUniqued.end());

        sortedAndUniqued.erase(std::unique(
            sortedAndUniqued.begin(),
            sortedAndUniqued.end()), sortedAndUniqued.end());

        BOOST_CHECK_MESSAGE(multipleValues == sortedAndUniqued,
            "multiple values vec before=" << ts::vectorToString(MULTIPLE_VALUES_BEFORE)
            << ", multiple values vec after=" << ts::vectorToString(multipleValues)
            << ", sortedAndUnique vec=" << ts::vectorToString(sortedAndUniqued));
    }
}


BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToReverseDuplicatedCounting)
{
    const ts::IntVec_t EMPTY;

    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t reverseDuplicatedCountingValues;
        for (int i{ VALUE }; i >= 0; --i)
        {
            reverseDuplicatedCountingValues.push_back(i);
            reverseDuplicatedCountingValues.push_back(i);
        }

        const ts::IntVec_t REV_DUP_COUNT_VALUES_BEFORE{ reverseDuplicatedCountingValues };

        misc::Vector::Append(EMPTY, reverseDuplicatedCountingValues, misc::Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(reverseDuplicatedCountingValues == REV_DUP_COUNT_VALUES_BEFORE,
            "(no sorting) reverse duplicated counting values vec before="
            << ts::vectorToString(REV_DUP_COUNT_VALUES_BEFORE)
            << ", reverse duplicated coutning values vec after="
            << ts::vectorToString(reverseDuplicatedCountingValues));

        ts::IntVec_t sortedAndUniqued{ reverseDuplicatedCountingValues };

        std::sort(sortedAndUniqued.begin(), sortedAndUniqued.end());
        
        sortedAndUniqued.erase( std::unique(
            sortedAndUniqued.begin(),
            sortedAndUniqued.end()), sortedAndUniqued.end());

        misc::Vector::Append(
            EMPTY,
            reverseDuplicatedCountingValues,
            misc::Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(reverseDuplicatedCountingValues == sortedAndUniqued,
            "(with sort and unique) reverse duplicated counting values vec before="
            << ts::vectorToString(REV_DUP_COUNT_VALUES_BEFORE)
            << ", reverse duplicated coutning values vec after="
            << ts::vectorToString(reverseDuplicatedCountingValues)
            << ", sortedAndUniqued vec=" << ts::vectorToString(sortedAndUniqued));
    }
}


BOOST_AUTO_TEST_CASE(Vector_Append_CornerCases)
{
    ts::IntVec_t a;
    ts::IntVec_t b;

    auto const A_BEFORE_EMPTY_APPEND{ a };
    auto const B_BEFORE_EMPTY_APPEND{ b };

    misc::Vector::Append(a, b, misc::Vector::SortOpt::None);

    BOOST_CHECK_MESSAGE(a == A_BEFORE_EMPTY_APPEND,
        "append two empty vecs, a should remain unchanged, "
        << "a_before=" << ts::vectorToString(A_BEFORE_EMPTY_APPEND)
        << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(b == B_BEFORE_EMPTY_APPEND,
        "append two empty vecs, a=" << ts::vectorToString(a)
        << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
        << ", result=" << ts::vectorToString(b));

    misc::Vector::Append(a, b, misc::Vector::SortOpt::SortAndUnique);

    BOOST_CHECK_MESSAGE(a == A_BEFORE_EMPTY_APPEND,
        "append two empty vecs with SortAndUnique, a should remain unchanged, "
        << "a_before=" << ts::vectorToString(A_BEFORE_EMPTY_APPEND)
        << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(b == B_BEFORE_EMPTY_APPEND,
        "append two empty vecs with SortAndUnique, a=" << ts::vectorToString(a)
        << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
        << ", result=" << ts::vectorToString(b));

    a.push_back(0);

    auto const SINGLE_ZERO{ a };

    misc::Vector::Append(a, b, misc::Vector::SortOpt::None);

    BOOST_CHECK_MESSAGE(a == SINGLE_ZERO,
        "append zero to empty vec, a should remain unchanged, "
        << "a_before=" << ts::vectorToString(SINGLE_ZERO)
        << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(b == SINGLE_ZERO,
        "append zero to empty vec, a=" << ts::vectorToString(a)
        << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
        << ", result=" << ts::vectorToString(b));

    misc::Vector::Append(a, b, misc::Vector::SortOpt::SortAndUnique);

    BOOST_CHECK_MESSAGE(a == SINGLE_ZERO,
        "append zero to empty vec with SortAndUnique, a should remain unchanged, "
        << "a_before=" << ts::vectorToString(SINGLE_ZERO)
        << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(b == SINGLE_ZERO,
        "append zero to empty vec, with SortAndUnique, a=" << ts::vectorToString(a)
        << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
        << ", result=" << ts::vectorToString(b));
}


BOOST_AUTO_TEST_CASE(Vector_Add_AB_NoSort)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t AB_NOSORT_EXPECTED = { 1, 1, 2, 3, 4, 5, 6, 6 };
    auto const AB_NOSORT_RESULT{ misc::Vector::AndCopy(A, B) };

    BOOST_CHECK_MESSAGE((AB_NOSORT_RESULT == AB_NOSORT_EXPECTED),
        "add 1,1,2,3 + 4,5,6,6 No_SortAndUnique result=" << ts::vectorToString(AB_NOSORT_RESULT)
        << ", expected=" << ts::vectorToString(AB_NOSORT_EXPECTED));
}


BOOST_AUTO_TEST_CASE(Vector_Add_AB_SortAndUnique)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t AB_SORTANDUNIQUE_EXPECTED = { 1, 2, 3, 4, 5, 6 };

    auto const AB_SORTANDUNIQUE_RESULT{ misc::Vector::AndCopy(
        A,
        B,
        misc::Vector::SortOpt::SortAndUnique) };

    BOOST_CHECK_MESSAGE((AB_SORTANDUNIQUE_RESULT == AB_SORTANDUNIQUE_EXPECTED),
        "add 1,1,2,3 + 4,5,6,6 SortAndUnique result="
        << ts::vectorToString(AB_SORTANDUNIQUE_RESULT)
        << ", expected=" << ts::vectorToString(AB_SORTANDUNIQUE_EXPECTED));
}


BOOST_AUTO_TEST_CASE(Vector_Add_BA_NoSort)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t BA_NOSORT_EXPECTED = { 4, 5, 6, 6, 1, 1, 2, 3 };
    auto const BA_NOSORT_RESULT{ misc::Vector::AndCopy(B, A) };

    BOOST_CHECK_MESSAGE((BA_NOSORT_RESULT == BA_NOSORT_EXPECTED),
        "add 4,5,6,6 + 1,1,2,3 No_SortAndUnique result=" << ts::vectorToString(BA_NOSORT_RESULT)
        << ", expected=" << ts::vectorToString(BA_NOSORT_EXPECTED));
}


BOOST_AUTO_TEST_CASE(Vector_Add_BA_SortAndUnique)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t BA_SORTANDUNIQUE_EXPECTED = { 1, 2, 3, 4, 5, 6 };

    auto const BA_SORTANDUNIQUE_RESULT{ misc::Vector::AndCopy(
        B,
        A,
        misc::Vector::SortOpt::SortAndUnique) };

    BOOST_CHECK_MESSAGE((BA_SORTANDUNIQUE_RESULT == BA_SORTANDUNIQUE_EXPECTED),
        "add 4,5,6,6 + 1,1,2,3 SortAndUnique result="
        << ts::vectorToString(BA_SORTANDUNIQUE_RESULT)
        << ", expected=" << ts::vectorToString(BA_SORTANDUNIQUE_EXPECTED));
}
