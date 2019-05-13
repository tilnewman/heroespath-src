// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_Vectors"

#include <boost/test/unit_test.hpp>

#include "unit-test-test-stuff.hpp"

#include "misc/vectors.hpp"

#include <algorithm>

using namespace heroespath;
using namespace heroespath::misc;
using namespace heroespath::test;

using IntVec_t = std::vector<int>;

BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToSingle)
{
    const IntVec_t EMPTY;

    for (const auto VALUE : smallValuesWorthTestingInt)
    {
        IntVec_t singleValue(1, VALUE);
        const IntVec_t SINGLE_VALUE_BEFORE { singleValue };
        Vector::Append(EMPTY, singleValue, Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(
            singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << containerToString(SINGLE_VALUE_BEFORE)
                                       << ", single value vec after="
                                       << containerToString(singleValue));

        Vector::Append(EMPTY, singleValue, Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(
            singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << containerToString(SINGLE_VALUE_BEFORE)
                                       << ", single value vec after="
                                       << containerToString(singleValue));
    }
}

BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToMultiple)
{
    const IntVec_t EMPTY;

    for (const auto VALUE : smallValuesWorthTestingInt)
    {
        IntVec_t multipleValues(static_cast<std::size_t>(VALUE), VALUE);
        const IntVec_t MULTIPLE_VALUES_BEFORE { multipleValues };
        Vector::Append(EMPTY, multipleValues, Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(
            multipleValues == MULTIPLE_VALUES_BEFORE,
            "multiple values vec before=" << containerToString(MULTIPLE_VALUES_BEFORE)
                                          << ", multiple values vec after="
                                          << containerToString(multipleValues));

        Vector::Append(EMPTY, multipleValues, Vector::SortOpt::SortAndUnique);

        IntVec_t sortedAndUniqued { multipleValues };

        std::sort(sortedAndUniqued.begin(), sortedAndUniqued.end());

        sortedAndUniqued.erase(
            std::unique(sortedAndUniqued.begin(), sortedAndUniqued.end()), sortedAndUniqued.end());

        BOOST_CHECK_MESSAGE(
            multipleValues == sortedAndUniqued,
            "multiple values vec before="
                << containerToString(MULTIPLE_VALUES_BEFORE)
                << ", multiple values vec after=" << containerToString(multipleValues)
                << ", sortedAndUnique vec=" << containerToString(sortedAndUniqued));
    }
}

BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToReverseDuplicatedCounting)
{
    const IntVec_t EMPTY;

    for (const auto VALUE : smallValuesWorthTestingInt)
    {
        IntVec_t reverseDuplicatedCountingValues;
        for (int i { VALUE }; i >= 0; --i)
        {
            reverseDuplicatedCountingValues.emplace_back(i);
            reverseDuplicatedCountingValues.emplace_back(i);
        }

        const IntVec_t REV_DUP_COUNT_VALUES_BEFORE { reverseDuplicatedCountingValues };

        Vector::Append(EMPTY, reverseDuplicatedCountingValues, Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(
            reverseDuplicatedCountingValues == REV_DUP_COUNT_VALUES_BEFORE,
            "(no sorting) reverse duplicated counting values vec before="
                << containerToString(REV_DUP_COUNT_VALUES_BEFORE)
                << ", reverse duplicated counting values vec after="
                << containerToString(reverseDuplicatedCountingValues));

        IntVec_t sortedAndUniqued { reverseDuplicatedCountingValues };

        std::sort(sortedAndUniqued.begin(), sortedAndUniqued.end());

        sortedAndUniqued.erase(
            std::unique(sortedAndUniqued.begin(), sortedAndUniqued.end()), sortedAndUniqued.end());

        Vector::Append(EMPTY, reverseDuplicatedCountingValues, Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(
            reverseDuplicatedCountingValues == sortedAndUniqued,
            "(with sort and unique) reverse duplicated counting values vec before="
                << containerToString(REV_DUP_COUNT_VALUES_BEFORE)
                << ", reverse duplicated counting values vec after="
                << containerToString(reverseDuplicatedCountingValues)
                << ", sortedAndUniqued vec=" << containerToString(sortedAndUniqued));
    }
}

BOOST_AUTO_TEST_CASE(Vector_Append_CornerCases)
{
    IntVec_t a;
    IntVec_t b;

    const auto A_BEFORE_EMPTY_APPEND { a };
    const auto B_BEFORE_EMPTY_APPEND { b };

    Vector::Append(a, b, Vector::SortOpt::None);

    BOOST_CHECK_MESSAGE(
        a == A_BEFORE_EMPTY_APPEND,
        "append two empty vecs, a should remain unchanged, "
            << "a_before=" << containerToString(A_BEFORE_EMPTY_APPEND)
            << ", a_after=" << containerToString(a));

    BOOST_CHECK_MESSAGE(
        b == B_BEFORE_EMPTY_APPEND,
        "append two empty vecs, a=" << containerToString(a)
                                    << ", b=" << containerToString(B_BEFORE_EMPTY_APPEND)
                                    << ", result=" << containerToString(b));

    Vector::Append(a, b, Vector::SortOpt::SortAndUnique);

    BOOST_CHECK_MESSAGE(
        a == A_BEFORE_EMPTY_APPEND,
        "append two empty vecs with SortAndUnique, a should remain unchanged, "
            << "a_before=" << containerToString(A_BEFORE_EMPTY_APPEND)
            << ", a_after=" << containerToString(a));

    BOOST_CHECK_MESSAGE(
        b == B_BEFORE_EMPTY_APPEND,
        "append two empty vecs with SortAndUnique, a=" << containerToString(a) << ", b="
                                                       << containerToString(B_BEFORE_EMPTY_APPEND)
                                                       << ", result=" << containerToString(b));

    a.emplace_back(0);

    const auto SINGLE_ZERO { a };

    Vector::Append(a, b, Vector::SortOpt::None);

    BOOST_CHECK_MESSAGE(
        a == SINGLE_ZERO,
        "append zero to empty vec, a should remain unchanged, "
            << "a_before=" << containerToString(SINGLE_ZERO)
            << ", a_after=" << containerToString(a));

    BOOST_CHECK_MESSAGE(
        b == SINGLE_ZERO,
        "append zero to empty vec, a=" << containerToString(a)
                                       << ", b=" << containerToString(B_BEFORE_EMPTY_APPEND)
                                       << ", result=" << containerToString(b));

    Vector::Append(a, b, Vector::SortOpt::SortAndUnique);

    BOOST_CHECK_MESSAGE(
        a == SINGLE_ZERO,
        "append zero to empty vec with SortAndUnique, a should remain unchanged, "
            << "a_before=" << containerToString(SINGLE_ZERO)
            << ", a_after=" << containerToString(a));

    BOOST_CHECK_MESSAGE(
        b == SINGLE_ZERO,
        "append zero to empty vec, with SortAndUnique, a="
            << containerToString(a) << ", b=" << containerToString(B_BEFORE_EMPTY_APPEND)
            << ", result=" << containerToString(b));
}

BOOST_AUTO_TEST_CASE(Vector_Add_AB_NoSort)
{
    const IntVec_t A = { 1, 1, 2, 3 };
    const IntVec_t B = { 4, 5, 6, 6 };
    const IntVec_t AB_NOSORT_EXPECTED = { 1, 1, 2, 3, 4, 5, 6, 6 };
    const auto AB_NOSORT_RESULT { Vector::Combine(A, B) };

    BOOST_CHECK_MESSAGE(
        (AB_NOSORT_RESULT == AB_NOSORT_EXPECTED),
        "add 1,1,2,3 + 4,5,6,6 No_SortAndUnique result=" << containerToString(AB_NOSORT_RESULT)
                                                         << ", expected="
                                                         << containerToString(AB_NOSORT_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Add_AB_SortAndUnique)
{
    const IntVec_t A = { 1, 1, 2, 3 };
    const IntVec_t B = { 4, 5, 6, 6 };
    const IntVec_t AB_SORTANDUNIQUE_EXPECTED = { 1, 2, 3, 4, 5, 6 };

    const auto AB_SORTANDUNIQUE_RESULT { Vector::Combine(A, B, Vector::SortOpt::SortAndUnique) };

    BOOST_CHECK_MESSAGE(
        (AB_SORTANDUNIQUE_RESULT == AB_SORTANDUNIQUE_EXPECTED),
        "add 1,1,2,3 + 4,5,6,6 SortAndUnique result="
            << containerToString(AB_SORTANDUNIQUE_RESULT)
            << ", expected=" << containerToString(AB_SORTANDUNIQUE_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Add_BA_NoSort)
{
    const IntVec_t A = { 1, 1, 2, 3 };
    const IntVec_t B = { 4, 5, 6, 6 };
    const IntVec_t BA_NOSORT_EXPECTED = { 4, 5, 6, 6, 1, 1, 2, 3 };
    const auto BA_NOSORT_RESULT { Vector::Combine(B, A) };

    BOOST_CHECK_MESSAGE(
        (BA_NOSORT_RESULT == BA_NOSORT_EXPECTED),
        "add 4,5,6,6 + 1,1,2,3 No_SortAndUnique result=" << containerToString(BA_NOSORT_RESULT)
                                                         << ", expected="
                                                         << containerToString(BA_NOSORT_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Add_BA_SortAndUnique)
{
    const IntVec_t A = { 1, 1, 2, 3 };
    const IntVec_t B = { 4, 5, 6, 6 };
    const IntVec_t BA_SORTANDUNIQUE_EXPECTED = { 1, 2, 3, 4, 5, 6 };

    const auto BA_SORTANDUNIQUE_RESULT { Vector::Combine(B, A, Vector::SortOpt::SortAndUnique) };

    BOOST_CHECK_MESSAGE(
        (BA_SORTANDUNIQUE_RESULT == BA_SORTANDUNIQUE_EXPECTED),
        "add 4,5,6,6 + 1,1,2,3 SortAndUnique result="
            << containerToString(BA_SORTANDUNIQUE_RESULT)
            << ", expected=" << containerToString(BA_SORTANDUNIQUE_EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_EmptyEmpty)
{
    const IntVec_t EMPTY;
    const auto EMPTYEMPTY_RESULT { Vector::Exclude(EMPTY, EMPTY) };

    BOOST_CHECK_MESSAGE(
        (EMPTYEMPTY_RESULT == EMPTY),
        "two empty vecs, result=" << containerToString(EMPTYEMPTY_RESULT) << ", expected empty");
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_SomethingEmpty)
{
    const IntVec_t EMPTY;
    const IntVec_t A = { 1, 2, 3 };
    const auto AEMPTY_RESULT { Vector::Exclude(A, EMPTY) };

    BOOST_CHECK_MESSAGE(
        (AEMPTY_RESULT == A),
        "A exclude empty, result=" << containerToString(AEMPTY_RESULT)
                                   << ", expected=" << containerToString(A));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_EmptySomething)
{
    const IntVec_t EMPTY;
    const IntVec_t A = { 1, 2, 3 };

    const auto EMPTYA_RESULT { Vector::Exclude(EMPTY, A) };

    BOOST_CHECK_MESSAGE(
        (EMPTYA_RESULT == EMPTY),
        "empty exclude A, result=" << containerToString(EMPTYA_RESULT)
                                   << ", expected=" << containerToString(EMPTY));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_Duplicate)
{
    const IntVec_t EMPTY;
    const IntVec_t A = { 1, 2, 3 };

    const auto DUPLICATE_RESULT { Vector::Exclude(A, A) };

    BOOST_CHECK_MESSAGE(
        (DUPLICATE_RESULT == EMPTY),
        "duplicate exclude, result=" << containerToString(DUPLICATE_RESULT)
                                     << ", expected=" << containerToString(EMPTY));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_Single)
{
    const IntVec_t A = { 1, 2, 2, 3, 3, 3 };
    const int B { 2 };
    const IntVec_t EXPECTED = { 1, 3, 3, 3 };
    const auto SINGLE_RESULT { Vector::Exclude(A, B) };

    BOOST_CHECK_MESSAGE(
        (SINGLE_RESULT == EXPECTED),
        "1,2,2,3,3,3 exclude 2, result=" << containerToString(SINGLE_RESULT)
                                         << ", expected=" << containerToString(EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_Exclude_Multiple)
{
    const IntVec_t A = { 1, 2, 2, 3, 3, 3 };
    const IntVec_t B = { 1, 1, 3 };
    const IntVec_t EXPECTED = { 2, 2 };
    const auto MUTLIPLE_RESULT { Vector::Exclude(A, B) };

    BOOST_CHECK_MESSAGE(
        (MUTLIPLE_RESULT == EXPECTED),
        "1,2,2,3,3,3 exclude 1,1,3, result=" << containerToString(MUTLIPLE_RESULT)
                                             << ", expected=" << containerToString(EXPECTED));
}

BOOST_AUTO_TEST_CASE(Vector_SelectRandom_Single)
{
    for (std::size_t i { 0 }; i < randomRetryCount; ++i)
    {
        const IntVec_t A = { 1 };
        const auto SINGLE_RESULT { RandomSelect(A) };

        BOOST_CHECK_MESSAGE((SINGLE_RESULT == 1), "SelectRandom single, result=" << SINGLE_RESULT);
    }
}

BOOST_AUTO_TEST_CASE(Vector_SelectRandom_Duplicates)
{
    for (std::size_t i { 0 }; i < randomRetryCount; ++i)
    {
        const IntVec_t A = { 1, 1, 1 };
        const auto DUPLICATE_RESULT { RandomSelect(A) };

        BOOST_CHECK_MESSAGE(
            (DUPLICATE_RESULT == 1), "SelectRandom duplicates, result=" << DUPLICATE_RESULT);
    }
}

BOOST_AUTO_TEST_CASE(Vector_SelectRandom_Multiple)
{
    for (std::size_t i { 0 }; i < randomRetryCount; ++i)
    {
        const IntVec_t A = { 1, 2, 3 };
        const auto MULT_RESULT { RandomSelect(A) };

        BOOST_CHECK_MESSAGE(
            (MULT_RESULT == 1) || (MULT_RESULT == 2) || (MULT_RESULT == 3),
            "SelectRandom multiple, result=" << MULT_RESULT);
    }
}

BOOST_AUTO_TEST_CASE(Vector_OrderlessCompare_Empty)
{
    const IntVec_t EMPTY;
    BOOST_CHECK(Vector::OrderlessCompareEqual(EMPTY, EMPTY));

    const IntVec_t A = { 1, 2, 3 };
    BOOST_CHECK(Vector::OrderlessCompareEqual(EMPTY, A) == false);
    BOOST_CHECK(Vector::OrderlessCompareEqual(A, EMPTY) == false);
}

BOOST_AUTO_TEST_CASE(Vector_OrderlessCompare_123)
{
    const IntVec_t A = { 1, 2, 3 };
    const IntVec_t B = { 1, 3, 2 };
    const IntVec_t C = { 2, 1, 3 };
    const IntVec_t D = { 2, 3, 1 };
    const IntVec_t E = { 3, 1, 2 };
    const IntVec_t F = { 3, 2, 1 };

    BOOST_CHECK(Vector::OrderlessCompareEqual(A, A));
    BOOST_CHECK(Vector::OrderlessCompareEqual(B, B));

    BOOST_CHECK(Vector::OrderlessCompareEqual(A, B));
    BOOST_CHECK(Vector::OrderlessCompareEqual(B, A));

    BOOST_CHECK(Vector::OrderlessCompareEqual(B, C));
    BOOST_CHECK(Vector::OrderlessCompareEqual(C, D));
    BOOST_CHECK(Vector::OrderlessCompareEqual(D, E));
    BOOST_CHECK(Vector::OrderlessCompareEqual(E, F));
    BOOST_CHECK(Vector::OrderlessCompareEqual(F, A));

    auto testIncompleteFails = [&](const IntVec_t & NOT_EQUAL) {
        BOOST_CHECK(!Vector::OrderlessCompareEqual(A, NOT_EQUAL));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(B, NOT_EQUAL));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(C, NOT_EQUAL));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(D, NOT_EQUAL));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(E, NOT_EQUAL));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(F, NOT_EQUAL));

        BOOST_CHECK(!Vector::OrderlessCompareEqual(NOT_EQUAL, A));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(NOT_EQUAL, B));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(NOT_EQUAL, C));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(NOT_EQUAL, D));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(NOT_EQUAL, E));
        BOOST_CHECK(!Vector::OrderlessCompareEqual(NOT_EQUAL, F));
    };

    testIncompleteFails({ 1 });
    testIncompleteFails({ 1, 2 });
    testIncompleteFails({ 2, 1 });
    testIncompleteFails({ 1, 3 });
    testIncompleteFails({ 3, 1 });
    testIncompleteFails({ 1, 1 });
    testIncompleteFails({ 1, 1, 1 });
    testIncompleteFails({ 1, 1, 1, 1 });

    testIncompleteFails({ 2 });
    testIncompleteFails({ 2, 1 });
    testIncompleteFails({ 1, 2 });
    testIncompleteFails({ 2, 3 });
    testIncompleteFails({ 3, 2 });
    testIncompleteFails({ 2, 2 });
    testIncompleteFails({ 2, 2, 2 });
    testIncompleteFails({ 2, 2, 2, 2 });

    testIncompleteFails({ 3 });
    testIncompleteFails({ 3, 1 });
    testIncompleteFails({ 1, 3 });
    testIncompleteFails({ 3, 2 });
    testIncompleteFails({ 2, 3 });
    testIncompleteFails({ 3, 3 });
    testIncompleteFails({ 3, 3, 3 });
    testIncompleteFails({ 3, 3, 3, 3 });
}
