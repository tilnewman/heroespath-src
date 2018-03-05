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
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_Vectors"

#include <boost/test/unit_test.hpp>

#include "Test-stuff.hpp"

#include "misc/vectors.hpp"

#include <algorithm>

namespace ts = test_stuff;

using namespace heroespath;

BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToSingle)
{
    const ts::IntVec_t EMPTY;

    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t singleValue(1, VALUE);
        const ts::IntVec_t SINGLE_VALUE_BEFORE{ singleValue };
        misc::Vector::Append(EMPTY, singleValue, misc::Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(
            singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << ts::vectorToString(SINGLE_VALUE_BEFORE)
                                       << ", single value vec after="
                                       << ts::vectorToString(singleValue));

        misc::Vector::Append(EMPTY, singleValue, misc::Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(
            singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << ts::vectorToString(SINGLE_VALUE_BEFORE)
                                       << ", single value vec after="
                                       << ts::vectorToString(singleValue));
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

        BOOST_CHECK_MESSAGE(
            multipleValues == MULTIPLE_VALUES_BEFORE,
            "mutliple values vec before=" << ts::vectorToString(MULTIPLE_VALUES_BEFORE)
                                          << ", multiple values vec after="
                                          << ts::vectorToString(multipleValues));

        misc::Vector::Append(EMPTY, multipleValues, misc::Vector::SortOpt::SortAndUnique);

        ts::IntVec_t sortedAndUniqued{ multipleValues };

        std::sort(sortedAndUniqued.begin(), sortedAndUniqued.end());

        sortedAndUniqued.erase(
            std::unique(sortedAndUniqued.begin(), sortedAndUniqued.end()), sortedAndUniqued.end());

        BOOST_CHECK_MESSAGE(
            multipleValues == sortedAndUniqued,
            "multiple values vec before="
                << ts::vectorToString(MULTIPLE_VALUES_BEFORE)
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

        BOOST_CHECK_MESSAGE(
            reverseDuplicatedCountingValues == REV_DUP_COUNT_VALUES_BEFORE,
            "(no sorting) reverse duplicated counting values vec before="
                << ts::vectorToString(REV_DUP_COUNT_VALUES_BEFORE)
                << ", reverse duplicated coutning values vec after="
                << ts::vectorToString(reverseDuplicatedCountingValues));

        ts::IntVec_t sortedAndUniqued{ reverseDuplicatedCountingValues };

        std::sort(sortedAndUniqued.begin(), sortedAndUniqued.end());

        sortedAndUniqued.erase(
            std::unique(sortedAndUniqued.begin(), sortedAndUniqued.end()), sortedAndUniqued.end());

        misc::Vector::Append(
            EMPTY, reverseDuplicatedCountingValues, misc::Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(
            reverseDuplicatedCountingValues == sortedAndUniqued,
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

    BOOST_CHECK_MESSAGE(
        a == A_BEFORE_EMPTY_APPEND,
        "append two empty vecs, a should remain unchanged, "
            << "a_before=" << ts::vectorToString(A_BEFORE_EMPTY_APPEND)
            << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(
        b == B_BEFORE_EMPTY_APPEND,
        "append two empty vecs, a=" << ts::vectorToString(a)
                                    << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
                                    << ", result=" << ts::vectorToString(b));

    misc::Vector::Append(a, b, misc::Vector::SortOpt::SortAndUnique);

    BOOST_CHECK_MESSAGE(
        a == A_BEFORE_EMPTY_APPEND,
        "append two empty vecs with SortAndUnique, a should remain unchanged, "
            << "a_before=" << ts::vectorToString(A_BEFORE_EMPTY_APPEND)
            << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(
        b == B_BEFORE_EMPTY_APPEND,
        "append two empty vecs with SortAndUnique, a=" << ts::vectorToString(a) << ", b="
                                                       << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
                                                       << ", result=" << ts::vectorToString(b));

    a.push_back(0);

    auto const SINGLE_ZERO{ a };

    misc::Vector::Append(a, b, misc::Vector::SortOpt::None);

    BOOST_CHECK_MESSAGE(
        a == SINGLE_ZERO,
        "append zero to empty vec, a should remain unchanged, "
            << "a_before=" << ts::vectorToString(SINGLE_ZERO)
            << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(
        b == SINGLE_ZERO,
        "append zero to empty vec, a=" << ts::vectorToString(a)
                                       << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
                                       << ", result=" << ts::vectorToString(b));

    misc::Vector::Append(a, b, misc::Vector::SortOpt::SortAndUnique);

    BOOST_CHECK_MESSAGE(
        a == SINGLE_ZERO,
        "append zero to empty vec with SortAndUnique, a should remain unchanged, "
            << "a_before=" << ts::vectorToString(SINGLE_ZERO)
            << ", a_after=" << ts::vectorToString(a));

    BOOST_CHECK_MESSAGE(
        b == SINGLE_ZERO,
        "append zero to empty vec, with SortAndUnique, a="
            << ts::vectorToString(a) << ", b=" << ts::vectorToString(B_BEFORE_EMPTY_APPEND)
            << ", result=" << ts::vectorToString(b));
}

BOOST_AUTO_TEST_CASE(Vector_Add_AB_NoSort)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t AB_NOSORT_EXPECTED = { 1, 1, 2, 3, 4, 5, 6, 6 };
    auto const AB_NOSORT_RESULT{ misc::Vector::AndCopy(A, B) };

    BOOST_CHECK_MESSAGE(
        (AB_NOSORT_RESULT == AB_NOSORT_EXPECTED),
        "add 1,1,2,3 + 4,5,6,6 No_SortAndUnique result=" << ts::vectorToString(AB_NOSORT_RESULT)
                                                         << ", expected="
                                                         << ts::vectorToString(AB_NOSORT_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Add_AB_SortAndUnique)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t AB_SORTANDUNIQUE_EXPECTED = { 1, 2, 3, 4, 5, 6 };

    auto const AB_SORTANDUNIQUE_RESULT{ misc::Vector::AndCopy(
        A, B, misc::Vector::SortOpt::SortAndUnique) };

    BOOST_CHECK_MESSAGE(
        (AB_SORTANDUNIQUE_RESULT == AB_SORTANDUNIQUE_EXPECTED),
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

    BOOST_CHECK_MESSAGE(
        (BA_NOSORT_RESULT == BA_NOSORT_EXPECTED),
        "add 4,5,6,6 + 1,1,2,3 No_SortAndUnique result=" << ts::vectorToString(BA_NOSORT_RESULT)
                                                         << ", expected="
                                                         << ts::vectorToString(BA_NOSORT_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Add_BA_SortAndUnique)
{
    const ts::IntVec_t A = { 1, 1, 2, 3 };
    const ts::IntVec_t B = { 4, 5, 6, 6 };
    const ts::IntVec_t BA_SORTANDUNIQUE_EXPECTED = { 1, 2, 3, 4, 5, 6 };

    auto const BA_SORTANDUNIQUE_RESULT{ misc::Vector::AndCopy(
        B, A, misc::Vector::SortOpt::SortAndUnique) };

    BOOST_CHECK_MESSAGE(
        (BA_SORTANDUNIQUE_RESULT == BA_SORTANDUNIQUE_EXPECTED),
        "add 4,5,6,6 + 1,1,2,3 SortAndUnique result="
            << ts::vectorToString(BA_SORTANDUNIQUE_RESULT)
            << ", expected=" << ts::vectorToString(BA_SORTANDUNIQUE_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_EmptyEmpty)
{
    const ts::IntVec_t EMPTY;
    auto const EMPTYEMPTY_RESULT{ misc::Vector::Exclude(EMPTY, EMPTY) };

    BOOST_CHECK_MESSAGE(
        (EMPTYEMPTY_RESULT == EMPTY),
        "two empty vecs, result=" << ts::vectorToString(EMPTYEMPTY_RESULT) << ", expected empty");
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_SomethingEmpty)
{
    const ts::IntVec_t EMPTY;
    const ts::IntVec_t A = { 1, 2, 3 };
    auto const AEMPTY_RESULT{ misc::Vector::Exclude(A, EMPTY) };

    BOOST_CHECK_MESSAGE(
        (AEMPTY_RESULT == A),
        "A exclude empty, result=" << ts::vectorToString(AEMPTY_RESULT)
                                   << ", expected=" << ts::vectorToString(A));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_EmptySomething)
{
    const ts::IntVec_t EMPTY;
    const ts::IntVec_t A = { 1, 2, 3 };

    auto const EMPTYA_RESULT{ misc::Vector::Exclude(EMPTY, A) };

    BOOST_CHECK_MESSAGE(
        (EMPTYA_RESULT == EMPTY),
        "empty exclude A, result=" << ts::vectorToString(EMPTYA_RESULT)
                                   << ", expected=" << ts::vectorToString(EMPTY));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_Duplicate)
{
    const ts::IntVec_t EMPTY;
    const ts::IntVec_t A = { 1, 2, 3 };

    auto const DUPLICATE_RESULT{ misc::Vector::Exclude(A, A) };

    BOOST_CHECK_MESSAGE(
        (DUPLICATE_RESULT == EMPTY),
        "duplicate exclude, result=" << ts::vectorToString(DUPLICATE_RESULT)
                                     << ", expected=" << ts::vectorToString(EMPTY));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_Single)
{
    const ts::IntVec_t A = { 1, 2, 2, 3, 3, 3 };
    const int B{ 2 };
    const ts::IntVec_t EXPECTED = { 1, 3, 3, 3 };
    auto const SINGLE_RESULT{ misc::Vector::Exclude(A, B) };

    BOOST_CHECK_MESSAGE(
        (SINGLE_RESULT == EXPECTED),
        "1,2,2,3,3,3 exclude 2, result=" << ts::vectorToString(SINGLE_RESULT)
                                         << ", expected=" << ts::vectorToString(EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_Multiple)
{
    const ts::IntVec_t A = { 1, 2, 2, 3, 3, 3 };
    const ts::IntVec_t B = { 1, 1, 3 };
    const ts::IntVec_t EXPECTED = { 2, 2 };
    auto const MUTLIPLE_RESULT{ misc::Vector::Exclude(A, B) };

    BOOST_CHECK_MESSAGE(
        (MUTLIPLE_RESULT == EXPECTED),
        "1,2,2,3,3,3 exclude 1,1,3, result=" << ts::vectorToString(MUTLIPLE_RESULT)
                                             << ", expected=" << ts::vectorToString(EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_ShuffleVec_Empty)
{
    ts::IntVec_t a;
    auto const EMPTY_RESULT{ misc::Vector::ShuffleVec(a) };

    BOOST_CHECK_MESSAGE(
        (EMPTY_RESULT == false), "shuffle empty, result=" << std::boolalpha << EMPTY_RESULT);
}

BOOST_AUTO_TEST_CASE(Vector_ShuffleVec_Single)
{
    ts::IntVec_t a = { 1 };
    auto const SINGLE_RESULT{ misc::Vector::ShuffleVec(a) };

    BOOST_CHECK_MESSAGE(
        (SINGLE_RESULT == false), "shuffle single, result=" << std::boolalpha << SINGLE_RESULT);
}

BOOST_AUTO_TEST_CASE(Vector_SelectRandom_Single)
{
    ts::Constants constants;

    for (int i{ 0 }; i < constants.RANDOM_RETRY_COUNT; ++i)
    {
        const ts::IntVec_t A = { 1 };
        auto const SINGLE_RESULT{ misc::Vector::SelectRandom(A) };

        BOOST_CHECK_MESSAGE((SINGLE_RESULT == 1), "SelectRandom single, result=" << SINGLE_RESULT);
    }
}

BOOST_AUTO_TEST_CASE(Vector_SelectRandom_Duplicates)
{
    ts::Constants constants;

    for (int i{ 0 }; i < constants.RANDOM_RETRY_COUNT; ++i)
    {
        const ts::IntVec_t A = { 1, 1, 1 };
        auto const DUPLICATE_RESULT{ misc::Vector::SelectRandom(A) };

        BOOST_CHECK_MESSAGE(
            (DUPLICATE_RESULT == 1), "SelectRandom duplicates, result=" << DUPLICATE_RESULT);
    }
}

BOOST_AUTO_TEST_CASE(Vector_SelectRandom_Multiple)
{
    ts::Constants constants;

    for (int i{ 0 }; i < constants.RANDOM_RETRY_COUNT; ++i)
    {
        const ts::IntVec_t A = { 1, 2, 3 };
        auto const MULT_RESULT{ misc::Vector::SelectRandom(A) };

        BOOST_CHECK_MESSAGE(
            (MULT_RESULT == 1) || (MULT_RESULT == 2) || (MULT_RESULT == 3),
            "SelectRandom mutliple, result=" << MULT_RESULT);
    }
}

BOOST_AUTO_TEST_CASE(Vector_Join_Empty)
{
    const ts::IntVec_t EMPTY;
    for (auto const VALUE : ts::TEST_COUNTS)
    {
        auto const SIZET_VALUE{ static_cast<std::size_t>(VALUE) };

        auto const EMPTY_RESULT_1{ misc::Vector::Join(EMPTY, SIZET_VALUE) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_1 == ""),
            "Join(empty, " << VALUE << ") result=\"" << EMPTY_RESULT_1 << "\"");

        auto const EMPTY_RESULT_2{ misc::Vector::Join(EMPTY, SIZET_VALUE, misc::Vector::Wrap) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_2 == ""),
            "Join(empty, " << VALUE << ", Wrap) result=\"" << EMPTY_RESULT_2 << "\"");

        auto const EMPTY_RESULT_3{ misc::Vector::Join(EMPTY, SIZET_VALUE, misc::Vector::And) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_3 == ""),
            "Join(empty, " << VALUE << ", And) result=\"" << EMPTY_RESULT_3 << "\"");

        auto const EMPTY_RESULT_4{ misc::Vector::Join(EMPTY, SIZET_VALUE, misc::Vector::Ellipsis) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_4 == ""),
            "Join(empty, " << VALUE << ", Ellipsis) result=\"" << EMPTY_RESULT_4 << "\"");

        auto const EMPTY_RESULT_5{ misc::Vector::Join(
            EMPTY,
            SIZET_VALUE,
            static_cast<misc::Vector::JoinOpt>(
                misc::Vector::Wrap | misc::Vector::And | misc::Vector::Ellipsis)) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_5 == ""),
            "Join(empty, " << VALUE << ", Wrap/And/Ellipsis) result=\"" << EMPTY_RESULT_5 << "\"");
    }
}

BOOST_AUTO_TEST_CASE(Vector_Join_123)
{
    const ts::IntVec_t A = { 1, 2, 3 };

    BOOST_CHECK(misc::Vector::Join(A, 0) == "1, 2, 3");
    BOOST_CHECK(misc::Vector::Join(A, 9) == "1, 2, 3");
    BOOST_CHECK(misc::Vector::Join(A, 3) == "1, 2, 3");
    BOOST_CHECK(misc::Vector::Join(A, 2) == "1, 2");
    BOOST_CHECK(misc::Vector::Join(A, 1) == "1");

    BOOST_CHECK(misc::Vector::Join(A, 0, misc::Vector::Wrap) == "(1, 2, 3)");
    BOOST_CHECK(misc::Vector::Join(A, 2, misc::Vector::Wrap) == "(1, 2)");
    BOOST_CHECK(misc::Vector::Join(A, 1, misc::Vector::Wrap) == "(1)");

    BOOST_CHECK(misc::Vector::Join(A, 0, misc::Vector::And) == "1, 2, and 3");
    BOOST_CHECK(misc::Vector::Join(A, 2, misc::Vector::And) == "1, 2");
    BOOST_CHECK(misc::Vector::Join(A, 1, misc::Vector::And) == "1");

    BOOST_CHECK(misc::Vector::Join(A, 0, misc::Vector::Ellipsis) == "1, 2, 3");
    BOOST_CHECK(misc::Vector::Join(A, 2, misc::Vector::Ellipsis) == "1, 2...");
    BOOST_CHECK(misc::Vector::Join(A, 1, misc::Vector::Ellipsis) == "1...");
}

BOOST_AUTO_TEST_CASE(Vector_OrderlessCompare_Empty)
{
    const ts::IntVec_t EMPTY;
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(EMPTY, EMPTY));

    const ts::IntVec_t A = { 1, 2, 3 };
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(EMPTY, A) == false);
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(A, EMPTY) == false);
}

BOOST_AUTO_TEST_CASE(Vector_OrderlessCompare_123)
{
    const ts::IntVec_t A = { 1, 2, 3 };
    const ts::IntVec_t B = { 1, 3, 2 };
    const ts::IntVec_t C = { 2, 1, 3 };
    const ts::IntVec_t D = { 2, 3, 1 };
    const ts::IntVec_t E = { 3, 1, 2 };
    const ts::IntVec_t F = { 3, 2, 1 };

    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(A, A));
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(B, B));

    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(A, B));
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(B, A));

    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(B, C));
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(C, D));
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(D, E));
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(E, F));
    BOOST_CHECK(misc::Vector::OrderlessCompareEqual(F, A));
}
