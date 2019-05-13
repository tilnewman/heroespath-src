// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc"

#include <boost/test/unit_test.hpp>

#include "game/strong-types.hpp"
#include "gui/list-no-element.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/real.hpp"

#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <type_traits>
#include <vector>

#include <SFML/Graphics.hpp>

using namespace heroespath;
using namespace heroespath::gui;
using namespace heroespath::misc;

// a simple class used to test working with non-POD
class Thing
{
public:
    explicit Thing(const int NUMBER = 0)
        : m_number(NUMBER)
    {}

    int get() const { return m_number; }
    void set(const int NUM) { m_number = NUM; }

private:
    int m_number;
};

inline bool operator==(const Thing & L, const Thing & R) { return (L.get() == R.get()); }
inline bool operator!=(const Thing & L, const Thing & R) { return !(L == R); }

template <typename T>
constexpr bool isExact(const T A, const T B) noexcept
{
    return (!(A < B) && !(A > B));
};

#define M_FUNCTION_CALL_EXPANDER_TYPES_NUMERIC(function_name)                                      \
    {                                                                                              \
        function_name<char>();                                                                     \
        function_name<short>();                                                                    \
        function_name<int>();                                                                      \
        function_name<long>();                                                                     \
        function_name<long long>();                                                                \
                                                                                                   \
        function_name<unsigned char>();                                                            \
        function_name<unsigned short>();                                                           \
        function_name<unsigned int>();                                                             \
        function_name<unsigned long>();                                                            \
        function_name<unsigned long long>();                                                       \
                                                                                                   \
        function_name<float>();                                                                    \
        function_name<double>();                                                                   \
    }

template <typename T>
void isRealTestIdentity(const std::vector<T> & VALUES)
{
    for (const auto VALUE : VALUES)
    {
        BOOST_CHECK(IsRealClose(VALUE, VALUE));
        BOOST_CHECK(IsRealLessOrClose(VALUE, VALUE));
        BOOST_CHECK(IsRealGreaterOrClose(VALUE, VALUE));

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

        BOOST_CHECK(
            IsRealZero(VALUE)
            == !((VALUE < -std::numeric_limits<T>::epsilon())
                 || (VALUE > std::numeric_limits<T>::epsilon())));

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
        BOOST_CHECK(
            IsRealOne(VALUE)
            == !((VALUE < (T(1) - std::numeric_limits<T>::epsilon()))
                 || (VALUE > (T(1) + std::numeric_limits<T>::epsilon()))));

        if constexpr (!std::is_floating_point_v<T>)
        {
            BOOST_CHECK(IsRealZero(VALUE) == (VALUE == T(0)));
            BOOST_CHECK(IsRealOne(VALUE) == (VALUE == T(1)));

            BOOST_CHECK(isExact(NextIfReal(VALUE), VALUE));
            BOOST_CHECK(isExact(BeforeIfReal(VALUE), VALUE));
        }

        BOOST_CHECK(IsRealZeroOrLess(VALUE) == (isExact(VALUE, T(0)) || (VALUE < T(0))));

        BOOST_CHECK(IsRealZeroOrMore(VALUE) == (isExact(VALUE, T(0)) || (VALUE > T(0))));
    }
}

template <typename T>
void isRealTestDifferentValuesTransitive(const T A, const T B)
{
    BOOST_CHECK(!IsRealClose(A, B));
    BOOST_CHECK(!IsRealClose(B, A));

    BOOST_CHECK(IsRealLessOrClose(A, B) == (A < B));
    BOOST_CHECK(!IsRealLessOrClose(A, B) == !(A < B));

    BOOST_CHECK(IsRealLessOrClose(B, A) == (B < A));
    BOOST_CHECK(!IsRealLessOrClose(B, A) == !(B < A));

    BOOST_CHECK(IsRealGreaterOrClose(A, B) == (A > B));
    BOOST_CHECK(!IsRealGreaterOrClose(A, B) == !(A > B));

    BOOST_CHECK(IsRealGreaterOrClose(B, A) == (B > A));
    BOOST_CHECK(!IsRealGreaterOrClose(B, A) == !(B > A));
}

template <typename T>
void isRealTestDifferentValues(const std::vector<T> & VALUES)
{
    const auto VALUES_SIZE = VALUES.size();

    BOOST_CHECK(VALUES_SIZE >= 2);

    for (std::size_t i(0); i < VALUES_SIZE; ++i)
    {
        const auto & A = VALUES[i];
        for (std::size_t k(i + 1); k < VALUES_SIZE; ++k)
        {
            const auto & B = VALUES[k];
            isRealTestDifferentValuesTransitive(A, B);
        }
    }
}

template <typename T>
void isRealTestsForType()
{
    if constexpr (std::is_floating_point_v<T>)
    {

        auto myNextBefore = [](const T X) {
            if (X > std::numeric_limits<T>::lowest())
            {
                return std::nexttoward(X, std::numeric_limits<long double>::lowest());
            }
            else
            {
                return std::numeric_limits<T>::lowest();
            }
        };

        auto myNextAfter
            = [&](const T X) { return std::nextafter(X, std::numeric_limits<T>::max()); };

        // clang-format off
        const std::vector<T> STARTER_SET = {
                                        T(-9999),
                                        T(-101),
                                        T(-100),
                                        T(-99),
                                        T(-11),
                                        T(-10),
                                        T(-9),
                                        T(-3),
                                        T(-2),
                                        T(-1),
                                        T(0),
                                        T(1),
                                        T(2),
                                        T(3),
                                        T(9),
                                        T(10),
                                        T(11),
                                        T(99),
                                        T(100),
                                        T(101),
                                        T(9999)};
        // clang-format on

        std::vector<T> finalSet;
        finalSet.reserve(STARTER_SET.size() * 12);

        for (const auto X : STARTER_SET)
        {
            finalSet.emplace_back(X - T(0.499));
            finalSet.emplace_back(X - T(0.49));
            finalSet.emplace_back(X - T(0.4));
            finalSet.emplace_back(X - T(0.1));
            finalSet.emplace_back(X - T(0.01));
            finalSet.emplace_back(X);
            finalSet.emplace_back(X + T(0.01));
            finalSet.emplace_back(X + T(0.1));
            finalSet.emplace_back(X + T(0.4));
            finalSet.emplace_back(X + T(0.49));
            finalSet.emplace_back(X + T(0.499));
        }

        finalSet.emplace_back(std::numeric_limits<T>::lowest());
        finalSet.emplace_back(std::numeric_limits<T>::max());

        isRealTestIdentity(finalSet);

        for (const auto X : finalSet)
        {
            const auto BEFORE = myNextBefore(X);
            const auto AFTER = myNextAfter(X);

            BOOST_CHECK(isExact(BeforeIfReal(X), BEFORE));
            BOOST_CHECK(isExact(NextIfReal(X), AFTER));

            BOOST_CHECK(IsRealClose(X, AFTER));
            BOOST_CHECK(IsRealClose(AFTER, X));

            BOOST_CHECK(IsRealClose(X, BEFORE));
            BOOST_CHECK(IsRealClose(BEFORE, X));
        }

        isRealTestDifferentValues(finalSet);

        {
            const std::vector<T> EXTREME_SET = { std::numeric_limits<T>::lowest(),
                                                 (std::numeric_limits<T>::lowest() * T(0.5)),
                                                 T(-1),
                                                 T(-0.9),
                                                 T(-0.5),
                                                 T(-0.1),
                                                 T(0),
                                                 T(0.1),
                                                 T(0.5),
                                                 T(0.9),
                                                 T(1),
                                                 (std::numeric_limits<T>::max() * T(0.5)),
                                                 std::numeric_limits<T>::max() };

            for (const auto X : EXTREME_SET)
            {
                const auto BEFORE = myNextBefore(X);
                const auto AFTER = myNextAfter(X);

                BOOST_CHECK(isExact(BeforeIfReal(X), BEFORE));
                BOOST_CHECK(isExact(NextIfReal(X), AFTER));

                BOOST_CHECK(IsRealClose(X, AFTER));
                BOOST_CHECK(IsRealClose(AFTER, X));

                BOOST_CHECK(IsRealClose(X, BEFORE));
                BOOST_CHECK(IsRealClose(BEFORE, X));
            }

            isRealTestDifferentValues(EXTREME_SET);
        }
    }
    else if (std::is_signed_v<T>)
    {
        // clang-format off
        const std::vector<T> VALUES = { std::numeric_limits<T>::lowest(),
                                        (std::numeric_limits<T>::lowest() / T(2)),
                                        T(-101),
                                        T(-100),
                                        T(-99),
                                        T(-11),
                                        T(-10),
                                        T(-9),
                                        T(-3),
                                        T(-2),
                                        T(-1),
                                        T(0),
                                        T(1),
                                        T(2),
                                        T(3),
                                        T(9),
                                        T(10),
                                        T(11),
                                        T(99),
                                        T(100),
                                        T(101),
                                        (std::numeric_limits<T>::max() / T(2)),
                                        std::numeric_limits<T>::max() };
        // clang-format on

        isRealTestIdentity(VALUES);
        isRealTestDifferentValues(VALUES);
    }
    else
    {
        // clang-format off
        const std::vector<T> VALUES = { T(0),
                                        T(1),
                                        T(2),
                                        T(3),
                                        T(9),
                                        T(10),
                                        T(11),
                                        T(99),
                                        T(100),
                                        T(101),
                                        (std::numeric_limits<T>::max() / T(2)),
                                        std::numeric_limits<T>::max() };
        // clang-format on

        isRealTestIdentity(VALUES);
        isRealTestDifferentValues(VALUES);
    }
}

#define M_ITER_TYPE_TEST(expected, which, type)                                                    \
    {                                                                                              \
        BOOST_CHECK_MESSAGE(                                                                       \
            (which<type>) == (expected),                                                           \
            "M ITER TYPE TEST, " #which "<" #type ">, expected=" << expected                       \
                                                                 << ", actual=" << (which<type>)); \
    }

#define M_ITER_TYPE_TESTS_FOR_CONT_AND_ITER_OF(expected, which, container, iterator_type)          \
    {                                                                                              \
        M_ITER_TYPE_TEST(expected, which, container<int>::iterator_type);                          \
        M_ITER_TYPE_TEST(expected, which, container<int *>::iterator_type);                        \
        M_ITER_TYPE_TEST(expected, which, container<std::unique_ptr<int>>::iterator_type);         \
                                                                                                   \
        M_ITER_TYPE_TEST(expected, which, container<std::string>::iterator_type);                  \
        M_ITER_TYPE_TEST(expected, which, container<std::string *>::iterator_type);                \
        M_ITER_TYPE_TEST(expected, which, container<std::unique_ptr<std::string>>::iterator_type); \
    }

#define M_ITER_TYPE_TESTS_FOR_CONT(expected, which, container)                                     \
    {                                                                                              \
        M_ITER_TYPE_TESTS_FOR_CONT_AND_ITER_OF(expected, which, container, iterator);              \
        M_ITER_TYPE_TESTS_FOR_CONT_AND_ITER_OF(expected, which, container, const_iterator);        \
        M_ITER_TYPE_TESTS_FOR_CONT_AND_ITER_OF(expected, which, container, reverse_iterator);      \
                                                                                                   \
        M_ITER_TYPE_TESTS_FOR_CONT_AND_ITER_OF(                                                    \
            expected, which, container, const_reverse_iterator);                                   \
    }

#define M_ITER_TYPE_TEST_ALL(which, is_input, has_random_access)                                   \
    {                                                                                              \
        M_ITER_TYPE_TEST(false, which, int);                                                       \
        M_ITER_TYPE_TEST(false, which, std::string);                                               \
        M_ITER_TYPE_TEST(false, which, sf::FloatRect);                                             \
        M_ITER_TYPE_TEST(false, which, std::vector<sf::FloatRect>);                                \
                                                                                                   \
        const bool IS_INPUT { (is_input) };                                                        \
                                                                                                   \
        M_ITER_TYPE_TEST(IS_INPUT, which, int *);                                                  \
        M_ITER_TYPE_TEST(IS_INPUT, which, const int *);                                            \
        M_ITER_TYPE_TEST(false, which, int * const);                                               \
        M_ITER_TYPE_TEST(false, which, const int * const);                                         \
                                                                                                   \
        M_ITER_TYPE_TEST(IS_INPUT, which, std::string::iterator);                                  \
        M_ITER_TYPE_TEST(IS_INPUT, which, std::string::const_iterator);                            \
        M_ITER_TYPE_TEST(IS_INPUT, which, std::string::reverse_iterator);                          \
        M_ITER_TYPE_TEST(IS_INPUT, which, std::string::const_reverse_iterator);                    \
                                                                                                   \
        using Array1_t = std::array<int, 10>;                                                      \
        using Array2_t = std::array<int *, 10>;                                                    \
        using Array3_t = std::array<std::unique_ptr<int>, 10>;                                     \
                                                                                                   \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array1_t::iterator);                                     \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array2_t::iterator);                                     \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array3_t::iterator);                                     \
                                                                                                   \
        using Array4_t = std::array<std::string, 10>;                                              \
        using Array5_t = std::array<std::string *, 10>;                                            \
        using Array6_t = std::array<std::unique_ptr<std::string>, 10>;                             \
                                                                                                   \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array4_t::iterator);                                     \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array5_t::iterator);                                     \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array6_t::iterator);                                     \
                                                                                                   \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array1_t::const_iterator);                               \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array1_t::reverse_iterator);                             \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array1_t::const_iterator);                               \
        M_ITER_TYPE_TEST(IS_INPUT, which, Array1_t::const_reverse_iterator);                       \
                                                                                                   \
        const bool IS_INPUT_AND_HAS_RANDOM_ACCESS { IS_INPUT && (has_random_access) };             \
                                                                                                   \
        M_ITER_TYPE_TESTS_FOR_CONT(IS_INPUT, which, std::vector);                                  \
        M_ITER_TYPE_TESTS_FOR_CONT(IS_INPUT, which, std::deque);                                   \
        M_ITER_TYPE_TESTS_FOR_CONT(IS_INPUT_AND_HAS_RANDOM_ACCESS, which, std::list);              \
        M_ITER_TYPE_TESTS_FOR_CONT(IS_INPUT_AND_HAS_RANDOM_ACCESS, which, std::set);               \
        M_ITER_TYPE_TESTS_FOR_CONT(IS_INPUT_AND_HAS_RANDOM_ACCESS, which, std::multiset);          \
    }

BOOST_AUTO_TEST_CASE(Type_Helpers_Tests_CompileTime)
{
    static_assert((are_same_v<int, int>), "are_same_v<int, int> should be true");
    static_assert((!are_same_v<int, char>), "are_same_v<int, char> should be false");

    static_assert(
        (are_same_v<int>),
        "are_same_v<int> should be true because I want it that way and that is also how "
        "std::conditional works");

    // should not compile, std::conditional would, but I don't want it to, so there
    // { auto a = are_same_v<>; }

    static_assert(
        (are_same_v<int, const int>),
        "are_same_v<int, const int> should be true because it removes_cv");

    static_assert(
        (are_same_v<int, const volatile int>),
        "are_same_v<int, const int> should be true because it removes_cv");

    static_assert(
        (!are_same_v<int *, const int *>),
        "are_same_v<int, const int> should be true because it removes_cv");

    static_assert(
        (are_same_v<int, const int>),
        "are_same_v<int, const int> should be true because it removes_cv");

    static_assert((are_same_v<int, int, int>), "are_same_v<int, int, int> should be true");
    //
    static_assert((!are_same_v<char, int, int>), "are_same_v<char, int, int> should be false");

    static_assert((!are_same_v<int, char, int>), "are_same_v<int, char, int> should be false");

    static_assert((!are_same_v<int, int, char>), "are_same_v<int, int, char> should be false");

    static_assert((!are_same_v<char, char, int>), "are_same_v<int, int, int> should be false");

    static_assert((!are_same_v<int, char, char>), "are_same_v<int, int, int> should be false");

    //

    static_assert((are_any_v<int, int>), "are_any_v<int, int> should be true");
    static_assert((!are_any_v<int, char>), "are_any_v<int, char> should be false");

    // Should not compile, std::conditional would, but I don't want it to, so there.
    // { auto a = are_any_v<>; }

    static_assert(
        (!are_any_v<int>), "are_any_v<int> should be false because that is technically !are_same");

    static_assert((are_any_v<int, int, int>), "are_any_v<int, int, int> should be true");
    //
    static_assert((!are_any_v<char, int, int>), "are_any_v<char, int, int> should be false");
    static_assert((are_any_v<int, char, int>), "are_any_v<int, char, int> should be true");
    static_assert((are_any_v<int, int, char>), "are_any_v<int, int, char> should be true");
    static_assert((are_any_v<char, char, int>), "are_any_v<int, int, int> should be true");
    static_assert((!are_any_v<int, char, char>), "are_any_v<int, int, int> should be false");

    //

    static_assert((!are_none_v<int, int>), "are_none_v<int, int> should be false");
    static_assert((are_none_v<int, char>), "are_none_v<int, char> should be true");

    // should not compile, std::conditional would, but I don't want it to, so there
    //{ auto a = are_none_v<>; }

    static_assert(
        (are_none_v<int>), "are_none_v<int> should be true because that is technically !are_any");

    static_assert((!are_none_v<int, int, int>), "are_none_v<int, int, int> should be false");
    //
    static_assert((are_none_v<char, int, int>), "are_none_v<char, int, int> should be true");

    static_assert((!are_none_v<int, char, int>), "are_none_v<int, char, int> should be false");

    static_assert((!are_none_v<int, int, char>), "are_none_v<int, int, char> should be false");

    static_assert((!are_none_v<char, char, int>), "are_none_v<int, int, int> should be false");

    static_assert((are_none_v<int, char, char>), "are_none_v<int, int, int> should be true");
}

BOOST_AUTO_TEST_CASE(Type_Helpers_Tests_Runtime)
{
    BOOST_CHECK_MESSAGE((are_same_v<int, int>), "are_same_v<int, int> should be true");
    BOOST_CHECK_MESSAGE((!are_same_v<int, char>), "are_same_v<int, char> should be false");

    BOOST_CHECK_MESSAGE(
        (are_same_v<int>),
        "are_same_v<int> should be true because I want it that way and that is also how "
        "std::conditional works");

    // should not compile, std::conditional would, but I don't want it to, so there
    // { auto a = are_same_v<>; }

    BOOST_CHECK_MESSAGE(
        (are_same_v<int, const int>),
        "are_same_v<int, const int> should be true because it removes_cv");

    BOOST_CHECK_MESSAGE(
        (are_same_v<int, const volatile int>),
        "are_same_v<int, const int> should be true because it removes_cv");

    BOOST_CHECK_MESSAGE(
        (!are_same_v<int *, const int *>),
        "are_same_v<int, const int> should be true because it removes_cv");

    BOOST_CHECK_MESSAGE(
        (are_same_v<int, const int>),
        "are_same_v<int, const int> should be true because it removes_cv");

    BOOST_CHECK_MESSAGE((are_same_v<int, int, int>), "are_same_v<int, int, int> should be true");
    //
    BOOST_CHECK_MESSAGE(
        (!are_same_v<char, int, int>), "are_same_v<char, int, int> should be false");

    BOOST_CHECK_MESSAGE(
        (!are_same_v<int, char, int>), "are_same_v<int, char, int> should be false");

    BOOST_CHECK_MESSAGE(
        (!are_same_v<int, int, char>), "are_same_v<int, int, char> should be false");

    BOOST_CHECK_MESSAGE(
        (!are_same_v<char, char, int>), "are_same_v<int, int, int> should be false");

    BOOST_CHECK_MESSAGE(
        (!are_same_v<int, char, char>), "are_same_v<int, int, int> should be false");

    //

    BOOST_CHECK_MESSAGE((are_any_v<int, int>), "are_any_v<int, int> should be true");
    BOOST_CHECK_MESSAGE((!are_any_v<int, char>), "are_any_v<int, char> should be false");

    // Should not compile, std::conditional would, but I don't want it to, so there.
    // { auto a = are_any_v<>; }

    BOOST_CHECK_MESSAGE(
        (!are_any_v<int>), "are_any_v<int> should be false because that is technically !are_same");

    BOOST_CHECK_MESSAGE((are_any_v<int, int, int>), "are_any_v<int, int, int> should be true");
    //
    BOOST_CHECK_MESSAGE((!are_any_v<char, int, int>), "are_any_v<char, int, int> should be false");
    BOOST_CHECK_MESSAGE((are_any_v<int, char, int>), "are_any_v<int, char, int> should be true");
    BOOST_CHECK_MESSAGE((are_any_v<int, int, char>), "are_any_v<int, int, char> should be true");
    BOOST_CHECK_MESSAGE((are_any_v<char, char, int>), "are_any_v<int, int, int> should be true");
    BOOST_CHECK_MESSAGE((!are_any_v<int, char, char>), "are_any_v<int, int, int> should be false");

    //

    BOOST_CHECK_MESSAGE((!are_none_v<int, int>), "are_none_v<int, int> should be false");
    BOOST_CHECK_MESSAGE((are_none_v<int, char>), "are_none_v<int, char> should be true");

    // should not compile, std::conditional would, but I don't want it to, so there
    //{ auto a = are_none_v<>; }

    BOOST_CHECK_MESSAGE(
        (are_none_v<int>), "are_none_v<int> should be true because that is technically !are_any");

    BOOST_CHECK_MESSAGE((!are_none_v<int, int, int>), "are_none_v<int, int, int> should be false");
    //
    BOOST_CHECK_MESSAGE((are_none_v<char, int, int>), "are_none_v<char, int, int> should be true");

    BOOST_CHECK_MESSAGE(
        (!are_none_v<int, char, int>), "are_none_v<int, char, int> should be false");

    BOOST_CHECK_MESSAGE(
        (!are_none_v<int, int, char>), "are_none_v<int, int, char> should be false");

    BOOST_CHECK_MESSAGE(
        (!are_none_v<char, char, int>), "are_none_v<int, int, int> should be false");

    BOOST_CHECK_MESSAGE((are_none_v<int, char, char>), "are_none_v<int, int, int> should be true");
}

BOOST_AUTO_TEST_CASE(iterator_type_tests)
{
    M_ITER_TYPE_TEST_ALL(are_iterator_v, true, true);
    M_ITER_TYPE_TEST_ALL(are_input_iterator_v, true, true);
    M_ITER_TYPE_TEST_ALL(are_output_iterator_v, false, true);
    M_ITER_TYPE_TEST_ALL(are_forward_iterator_v, true, true);
    M_ITER_TYPE_TEST_ALL(are_bidirectional_iterator_v, true, true);
    M_ITER_TYPE_TEST_ALL(are_random_access_iterator_v, true, false);
}

BOOST_AUTO_TEST_CASE(Real_Abs)
{
    BOOST_CHECK(Abs(0) == 0);
    BOOST_CHECK(Abs(1) == 1);
    BOOST_CHECK(Abs(123) == 123);

    BOOST_CHECK(Abs(-0) == 0);
    BOOST_CHECK(Abs(-1) == 1);
    BOOST_CHECK(Abs(-123) == 123);

    BOOST_CHECK(isExact(Abs(0.0), 0.0));
    BOOST_CHECK(isExact(Abs(1.0), 1.0));
    BOOST_CHECK(isExact(Abs(123.456), 123.456));

    BOOST_CHECK(isExact(Abs(-0.0), 0.0));
    BOOST_CHECK(isExact(Abs(-1.0), 1.0));
    BOOST_CHECK(isExact(Abs(-123.456), 123.456));
}

BOOST_AUTO_TEST_CASE(Real_MinMin)
{
    BOOST_CHECK(Min(0, 0) == 0);
    BOOST_CHECK(Min(1, 1) == 1);
    BOOST_CHECK(Min(123, 123) == 123);

    BOOST_CHECK(Min(-0, 0) == -0);
    BOOST_CHECK(Min(-1, 1) == -1);
    BOOST_CHECK(Min(-123, 123) == -123);

    BOOST_CHECK(Min(0, -0) == -0);
    BOOST_CHECK(Min(1, -1) == -1);
    BOOST_CHECK(Min(123, -123) == -123);

    BOOST_CHECK(isExact(Min(0.0, 0.0), 0.0));
    BOOST_CHECK(isExact(Min(1.0, 1.0), 1.0));
    BOOST_CHECK(isExact(Min(123.456, 123.456), 123.456));

    BOOST_CHECK(isExact(Min(-0.0, 0.0), -0.0));
    BOOST_CHECK(isExact(Min(-1.0, 1.0), -1.0));
    BOOST_CHECK(isExact(Min(-123.456, 123.456), -123.456));

    BOOST_CHECK(Max(0, 0) == 0);
    BOOST_CHECK(Max(1, 1) == 1);
    BOOST_CHECK(Max(123, 123) == 123);

    BOOST_CHECK(Max(-0, 0) == 0);
    BOOST_CHECK(Max(-1, 1) == 1);
    BOOST_CHECK(Max(-123, 123) == 123);

    BOOST_CHECK(Max(0, -0) == 0);
    BOOST_CHECK(Max(1, -1) == 1);
    BOOST_CHECK(Max(123, -123) == 123);

    BOOST_CHECK(isExact(Max(0.0, 0.0), 0.0));
    BOOST_CHECK(isExact(Max(1.0, 1.0), 1.0));
    BOOST_CHECK(isExact(Max(123.456, 123.456), 123.456));

    BOOST_CHECK(isExact(Max(-0.0, 0.0), 0.0));
    BOOST_CHECK(isExact(Max(-1.0, 1.0), 1.0));
    BOOST_CHECK(isExact(Max(-123.456, 123.456), 123.456));

    const auto INT_BE_LOW = std::numeric_limits<int>::lowest();
    const auto INT_BE_MIN = std::numeric_limits<int>::min();
    const auto INT_BE_MAX = std::numeric_limits<int>::max();

    BOOST_CHECK(Min(INT_BE_LOW, INT_BE_MAX) == INT_BE_LOW);
    BOOST_CHECK(Min(INT_BE_MAX, INT_BE_LOW) == INT_BE_LOW);
    BOOST_CHECK(Min(INT_BE_LOW, INT_BE_MIN, INT_BE_MAX) == INT_BE_LOW);
    BOOST_CHECK(Min(INT_BE_MAX, INT_BE_MIN, INT_BE_LOW) == INT_BE_LOW);

    BOOST_CHECK(
        Min(INT_BE_MAX,
            INT_BE_MIN,
            INT_BE_LOW,
            INT_BE_MAX,
            INT_BE_MIN,
            INT_BE_LOW,
            0,
            -0,
            0,
            21534,
            15,
            125,
            -16,
            2,
            -2,
            11)
        == INT_BE_LOW);

    BOOST_CHECK(Max(INT_BE_LOW, INT_BE_MAX) == INT_BE_MAX);
    BOOST_CHECK(Max(INT_BE_MAX, INT_BE_LOW) == INT_BE_MAX);
    BOOST_CHECK(Max(INT_BE_LOW, INT_BE_MIN, INT_BE_MAX) == INT_BE_MAX);
    BOOST_CHECK(Max(INT_BE_MAX, INT_BE_MIN, INT_BE_LOW) == INT_BE_MAX);

    BOOST_CHECK(
        Max(INT_BE_MAX,
            INT_BE_MIN,
            INT_BE_LOW,
            INT_BE_MAX,
            INT_BE_MIN,
            INT_BE_LOW,
            0,
            -0,
            0,
            21534,
            15,
            125,
            -16,
            2,
            -2,
            11)
        == INT_BE_MAX);

    BOOST_CHECK(Min(1, 2, 0) == 0);
    BOOST_CHECK(Min(1, 2, -1) == -1);

    BOOST_CHECK(Max(1, 2, 0) == 2);
    BOOST_CHECK(Max(0, 1, 2) == 2);
}

BOOST_AUTO_TEST_CASE(Real_IsRealZero)
{
    BOOST_CHECK(IsRealClose(0, 0));
    BOOST_CHECK(IsRealClose(1, 1));
    BOOST_CHECK(IsRealClose(-1, -1));

    BOOST_CHECK(IsRealClose(0.0, 0.0));
    BOOST_CHECK(IsRealClose(0.0001, 0.0001));
    BOOST_CHECK(IsRealClose(9999.0, 9999.0));

    BOOST_CHECK(!IsRealClose(0.0001, 0.00001));
    BOOST_CHECK(!IsRealClose(9999.0, 9999.1));

    BOOST_CHECK(!IsRealClose(0.0001, -0.0001));
    BOOST_CHECK(!IsRealClose(9999.0, -9999.0));

    BOOST_CHECK(IsRealClose(-0.0, 0.0));
    BOOST_CHECK(IsRealClose(-0.0, -0.0));
    BOOST_CHECK(IsRealClose(0.0, -0.0));

    M_FUNCTION_CALL_EXPANDER_TYPES_NUMERIC(isRealTestsForType);
}

BOOST_AUTO_TEST_CASE(NotNull_Tests)
{
    // heroespath::misc::NotNull<int *> notNull; // should not compile
    // heroespath::misc::NotNull<int *> notNull { nullptr }; // should not compile
    // heroespath::misc::NotNull<int *> notNull { 0 }; // should not compile

    // std::cout << "(the next line output to the console will be an exception about a NotNull "
    //             "constructor given nullptr that is expected and can be ignored)"
    //          << std::endl;
    //
    // int * p0 { nullptr };
    // BOOST_CHECK_THROW(heroespath::misc::NotNull<int *> notNull0 { p0 }, std::exception);

    int one { 1 };
    heroespath::misc::NotNull<int *> notNull1A { &one };
    BOOST_CHECK(*notNull1A == one);
    BOOST_CHECK(*notNull1A == 1);
    BOOST_CHECK(notNull1A == &one);
    BOOST_CHECK(&one == notNull1A);
    BOOST_CHECK(notNull1A == &one);
    BOOST_CHECK(&one == notNull1A);

    const int TEMP_ONE { *notNull1A };
    BOOST_CHECK(TEMP_ONE == 1);

    BOOST_CHECK((notNull1A != &one) == false);
    BOOST_CHECK((&one != notNull1A) == false);
    BOOST_CHECK((notNull1A != &one) == false);
    BOOST_CHECK((&one != notNull1A) == false);

    int * p1 { &one };
    heroespath::misc::NotNull<int *> notNull1B { p1 };
    BOOST_CHECK(*notNull1B == one);
    BOOST_CHECK(*notNull1B == 1);

    BOOST_CHECK(notNull1A == notNull1B);
    BOOST_CHECK(notNull1A == notNull1B);
    BOOST_CHECK(*notNull1A == *notNull1B);

    heroespath::misc::NotNull<int *> notNull1C { notNull1A };

    BOOST_CHECK(notNull1A == notNull1C);
    BOOST_CHECK(notNull1A == notNull1C);
    BOOST_CHECK(*notNull1A == *notNull1C);

    char c { 'x' };
    heroespath::misc::NotNull<char *> notNullC { &c };

    // BOOST_CHECK(notNull1A != notNullC); //should not compile
    // BOOST_CHECK(notNull1A != notNullC); // should not compile

    int * p2 { new int(2) };
    heroespath::misc::NotNull<int *> notNull2 { p2 };

    BOOST_CHECK(notNull2 == p2);
    BOOST_CHECK(p2 == notNull2);
    BOOST_CHECK(notNull2 == p2);
    BOOST_CHECK(p2 == notNull2);

    // heroespath::misc::NotNull<int *> notNullX{ *p2 }; // should not compile

    BOOST_CHECK(notNull1A != notNull2);
    BOOST_CHECK(notNull1A != notNull2);
    BOOST_CHECK(*notNull1A != *notNull2);

    notNull1A = MakeNotNull(p2);

    BOOST_CHECK(notNull1A == notNull2);
    BOOST_CHECK(notNull1A == notNull2);
    BOOST_CHECK(*notNull1A == *notNull2);

    notNull1B = notNull2;

    BOOST_CHECK(notNull1B == notNull2);
    BOOST_CHECK(notNull1B == notNull2);
    BOOST_CHECK(*notNull1B == *notNull2);

    heroespath::misc::NotNull<int *> notNull3 { new int(3) };

    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(*notNull2 != *notNull3);

    *notNull3 = 2;

    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(*notNull2 == *notNull3);

    *notNull3 = 1;
    BOOST_CHECK(*notNull3 == 1);

    const heroespath::misc::NotNull<const int *> NOTNULL4 { new int(4) };
    BOOST_CHECK(NOTNULL4 != notNull3);
    BOOST_CHECK(NOTNULL4 != notNull3);
    BOOST_CHECK(*NOTNULL4 != *notNull3);

    // NOTNULL4 = 69; // should not compile
    //*NOTNULL4 = 0; // should NOT compile since the NotNull<T> type is a const
    // NOTNULL4 = p2; //should NOT compile because the pointer type is const

    // delete notNull2; should not compile
    delete p2;
    delete notNull3.get();
    delete NOTNULL4.get();

    boost::optional<heroespath::misc::NotNull<Thing *>> notNullThingPtrOpt { new Thing(69) };
    BOOST_CHECK(notNullThingPtrOpt.value()->get() == 69);
    BOOST_CHECK((*notNullThingPtrOpt)->get() == 69);
    delete notNullThingPtrOpt.value();
}

BOOST_AUTO_TEST_CASE(BoostOptionalThrowOnUninitTests)
{
    // In this codebase <boost/optional.hpp> is not included directly, instead a wrapper include
    // is used "misc/boost-optional-that-throws.hpp". This forces any attempted access of an
    // uninitialized boost::optional to throw a std::runtime_error. So these tests make sure
    // that boost::optional behaves in the way required by the codebase.

    boost::optional<Thing> optional { boost::none };

    BOOST_CHECK(optional.get_ptr() == nullptr);
    BOOST_CHECK(optional.get_value_or(Thing(69)).get() == 69);
    BOOST_CHECK(optional.is_initialized() == false);

    BOOST_CHECK_THROW(optional->get(), std::exception);
    BOOST_CHECK_THROW((*optional).get(), std::exception);
    BOOST_CHECK_THROW(optional.value().get(), std::exception);
    BOOST_CHECK_NO_THROW(optional.value_or(Thing(0)));
    BOOST_CHECK(optional.value_or(Thing(0)) == Thing(0));
}

BOOST_AUTO_TEST_CASE(BoostOptionalNoElementTests)
{
    // first test that NoElement works as expected
    BOOST_CHECK(NoElement_t() == NoElement_t());

    NoElement_t noElement1;
    NoElement_t noElement2;

    BOOST_CHECK(noElement1 == noElement1);
    BOOST_CHECK(noElement2 == noElement2);
    BOOST_CHECK(noElement1 == noElement2);
    BOOST_CHECK(noElement1 == NoElement_t());
    BOOST_CHECK(noElement2 == NoElement_t());

    // test that an optional of type gui::NoElement_t works in every way
    boost::optional<NoElement_t> noOpt;
    BOOST_CHECK(!noOpt);
    BOOST_CHECK(!noOpt.is_initialized());
    BOOST_CHECK(noOpt.get_value_or(noElement1) == noElement1);
    BOOST_CHECK(noOpt.get_value_or(noElement1) == noElement2);
    BOOST_CHECK_THROW(noOpt.get(), std::exception);
    BOOST_CHECK(noOpt.get_ptr() == nullptr);
    BOOST_CHECK_THROW(noOpt.value(), std::exception);
    BOOST_CHECK_NO_THROW(noOpt.reset());

    boost::optional<NoElement_t> noOptInit(noElement1);
    BOOST_CHECK(noOptInit);
    BOOST_CHECK(!!noOptInit);
    BOOST_CHECK(noOptInit.is_initialized());
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement1);
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement2);
    BOOST_CHECK(noOptInit.get() == noElement1);
    BOOST_CHECK(noOptInit.get() == noElement2);
    BOOST_CHECK(noOptInit.get_ptr() != nullptr);
    BOOST_CHECK(noOptInit.value() == noElement1);

    BOOST_CHECK_NO_THROW(noOptInit.reset());

    BOOST_CHECK(!noOptInit);

    noOptInit = noElement1;

    BOOST_CHECK(noOptInit);
    BOOST_CHECK(!!noOptInit);
    BOOST_CHECK(noOptInit.is_initialized());
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement1);
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement2);
    BOOST_CHECK(noOptInit.get() == noElement1);
    BOOST_CHECK(noOptInit.get() == noElement2);
    BOOST_CHECK(noOptInit.get_ptr() != nullptr);
    BOOST_CHECK(noOptInit.value() == noElement1);

    BOOST_CHECK(noOptInit == noElement1);
    BOOST_CHECK(noOptInit == noElement2);

    boost::optional<NoElement_t> noOptInitB(noElement1);
    BOOST_CHECK(noOptInit == noOptInitB);
    BOOST_CHECK(noOptInit.get() == noOptInitB.get());
    BOOST_CHECK(noOptInit.value() == noOptInitB.value());

    boost::optional<NoElement_t> noOptInit2(noElement2);
    BOOST_CHECK(noOptInit == noOptInit2);
    BOOST_CHECK(noOptInit.get() == noOptInit2.get());
    BOOST_CHECK(noOptInit.value() == noOptInit2.value());
}

BOOST_AUTO_TEST_CASE(BoostOptionalComparisonTests)
{
    using ThingPtr_t = heroespath::misc::NotNull<Thing *>;
    using ThingPtrOpt_t = boost::optional<ThingPtr_t>;

    Thing myThing1(1);
    ThingPtr_t thingPtr1 { &myThing1 };

    Thing myThing2(2);
    ThingPtr_t thingPtr2 { &myThing2 };

    ThingPtrOpt_t optInvalid { boost::none };
    ThingPtrOpt_t optValid1 { thingPtr1 };
    ThingPtrOpt_t optValid2 { thingPtr2 };

    BOOST_CHECK(optValid1 != optInvalid);
    BOOST_CHECK(optValid2 != optInvalid);
    BOOST_CHECK(optValid1 != optValid2);
    BOOST_CHECK(optValid1.value() != optValid2.value());
    BOOST_CHECK(optInvalid == ThingPtrOpt_t());

    // these test are the real meat of this test set
    BOOST_CHECK(optInvalid == boost::none);
    BOOST_CHECK(optValid1 != boost::none);
    BOOST_CHECK(optValid1 == thingPtr1);
    BOOST_CHECK(optValid1 != thingPtr2);

    ThingPtrOpt_t optExtra { thingPtr1 };
    BOOST_CHECK(optExtra == thingPtr1);
}

BOOST_AUTO_TEST_CASE(strong_numeric_type_tests)
{
    // using Health_t as a test because it's value_type is int...less typing

    Health_t a; // should zero initialize
    BOOST_CHECK_EQUAL(a.Get(), 0);
    BOOST_CHECK(a.IsZero());
    BOOST_CHECK(a.IsPositive());
    BOOST_CHECK(!a.IsNegative());
    BOOST_CHECK_EQUAL(a.MakePositiveCopy(), 0_health);
    BOOST_CHECK_EQUAL(a.MakeNegativeCopy(), 0_health);
    a.MakePositive();
    BOOST_CHECK_EQUAL(a.Get(), 0);
    a.MakeNegative();
    BOOST_CHECK_EQUAL(a.Get(), 0);
    BOOST_CHECK_EQUAL(a, Health_t::Make(0.0f));

    Health_t b(a);
    BOOST_CHECK_EQUAL(b.Get(), 0);

    Health_t c = a;
    BOOST_CHECK_EQUAL(c.Get(), 0);

    Health_t d = Health_t::Make(123);
    BOOST_CHECK_EQUAL(d.Get(), 123);
    BOOST_CHECK(!d.IsZero());
    BOOST_CHECK(d.IsPositive());
    BOOST_CHECK(!d.IsNegative());
    BOOST_CHECK_EQUAL(d.MakePositiveCopy(), 123_health);
    BOOST_CHECK_EQUAL(d.MakeNegativeCopy(), -123_health);
    d.MakePositive();
    BOOST_CHECK_EQUAL(d, 123_health);
    d.MakeNegative();
    BOOST_CHECK_EQUAL(d, -123_health);
    d = -d;
    BOOST_CHECK_EQUAL(d, 123_health);
    BOOST_CHECK_EQUAL(d.GetAs<float>(), float(123));
    BOOST_CHECK_EQUAL(d.GetAs<unsigned>(), unsigned(123));
    BOOST_CHECK_EQUAL(d, Health_t::Make(123.0f));

    Health_t e = Health_t::Make(-123);
    BOOST_CHECK_EQUAL(e.Get(), -123);
    BOOST_CHECK(!e.IsZero());
    BOOST_CHECK(!e.IsPositive());
    BOOST_CHECK(e.IsNegative());
    BOOST_CHECK_EQUAL(e.MakePositiveCopy(), 123_health);
    BOOST_CHECK_EQUAL(e.MakeNegativeCopy(), -123_health);
    e.MakePositive();
    BOOST_CHECK_EQUAL(e, 123_health);
    e.MakeNegative();
    BOOST_CHECK_EQUAL(e, -123_health);
    e = -e;
    BOOST_CHECK_EQUAL(e, 123_health);
    e = -e;
    BOOST_CHECK_EQUAL(e.Get(), -123);
    BOOST_CHECK_EQUAL(e.GetAs<float>(), float(-123));
    BOOST_CHECK_EQUAL(e.GetAs<unsigned>(), unsigned(-123));
    BOOST_CHECK_EQUAL(e, Health_t::Make(-123.0f));

    Health_t f = Health_t::Make(Health_t::value_type(-123));
    BOOST_CHECK_EQUAL(f.Get(), -123);

    std::ostringstream ss;
    ss << a;
    ss.str("");
    ss << f;
    BOOST_CHECK_EQUAL(f.Get(), -123);

    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != b));
    BOOST_CHECK(!(a < b));
    BOOST_CHECK(a <= b);
    BOOST_CHECK(!(a > b));
    BOOST_CHECK(a >= b);

    BOOST_CHECK(a == 0_health);
    BOOST_CHECK(!(a != 0_health));
    BOOST_CHECK(!(a < 0_health));
    BOOST_CHECK(a <= 0_health);
    BOOST_CHECK(!(a > 0_health));
    BOOST_CHECK(a >= 0_health);

    BOOST_CHECK(!(d == e));
    BOOST_CHECK(d != e);
    BOOST_CHECK(!(d < e));
    BOOST_CHECK(e < d);
    BOOST_CHECK(!(d <= e));
    BOOST_CHECK(d > e);
    BOOST_CHECK(d >= e);

    BOOST_CHECK(!(d == -123_health));
    BOOST_CHECK(d != -123_health);
    BOOST_CHECK(!(d < -123_health));
    BOOST_CHECK(-123_health < d);
    BOOST_CHECK(!(d <= -123_health));
    BOOST_CHECK(d > -123_health);
    BOOST_CHECK(d >= -123_health);

    BOOST_CHECK_EQUAL((a + a), 0_health);
    BOOST_CHECK_EQUAL((a - a), 0_health);
    BOOST_CHECK_EQUAL((a * a), 0_health);

    // divide or mod by zero equals zero
    {
        BOOST_CHECK_EQUAL((a / a), 0_health);
        BOOST_CHECK_EQUAL((a / 0_health), 0_health);
        BOOST_CHECK_EQUAL((d / a), 0_health);
        BOOST_CHECK_EQUAL((d / 0_health), 0_health);
        //
        BOOST_CHECK_EQUAL((a % a), 0_health);
        BOOST_CHECK_EQUAL((a % 0_health), 0_health);
        BOOST_CHECK_EQUAL((d % a), 0_health);
        BOOST_CHECK_EQUAL((d % 0_health), 0_health);

        auto temp = 0_health;
        temp %= 0_health;
        BOOST_CHECK_EQUAL(temp, 0_health);

        temp = 0_health;
        temp /= 0_health;
        BOOST_CHECK_EQUAL(temp, 0_health);

        temp = 100_health;
        temp %= 0_health;
        BOOST_CHECK_EQUAL(temp, 0_health);

        temp = 100_health;
        temp /= 0_health;
        BOOST_CHECK_EQUAL(temp, 0_health);

        temp = 100_health;
        temp %= 10_health;
        BOOST_CHECK_EQUAL(temp, 0_health);

        temp = 100_health;
        temp /= 10_health;
        BOOST_CHECK_EQUAL(temp, 10_health);
    }

    BOOST_CHECK_EQUAL((d + e).Get(), 0);
    BOOST_CHECK_EQUAL((e + d).Get(), 0);
    BOOST_CHECK_EQUAL((d / e).Get(), -1);
    BOOST_CHECK_EQUAL((d * e).Get(), (123 * -123));

    BOOST_CHECK_EQUAL((d + -123_health).Get(), 0);
    BOOST_CHECK_EQUAL((-123_health + d).Get(), 0);
    BOOST_CHECK_EQUAL((d / -123_health).Get(), -1);
    BOOST_CHECK_EQUAL((d * -123_health).Get(), (123 * -123));

    BOOST_CHECK_EQUAL(Abs(a), 0_health);
    BOOST_CHECK_EQUAL(Abs(123_health), 123_health);
    BOOST_CHECK_EQUAL(Abs(-123_health), 123_health);

    BOOST_CHECK_EQUAL(Min(d, e), -123_health);
    BOOST_CHECK_EQUAL(Min(d, e, a), -123_health);
    BOOST_CHECK_EQUAL(Min(d, e, c, a), -123_health);

    BOOST_CHECK_EQUAL(Max(d, e), 123_health);
    BOOST_CHECK_EQUAL(Max(d, e, c), 123_health);
    BOOST_CHECK_EQUAL(Max(d, e, c, a), 123_health);

    BOOST_CHECK_EQUAL(Max(-321_health, -123_health, 0_health, 123_health, 321_health), 321_health);
    BOOST_CHECK_EQUAL(Min(-321_health, -123_health, 0_health, 123_health, 321_health), -321_health);

    BOOST_CHECK_EQUAL(a.Get(), 0);
    BOOST_CHECK_EQUAL((++a).Get(), 1);
    BOOST_CHECK_EQUAL(a.Get(), 1);

    BOOST_CHECK_EQUAL((a++).Get(), 1);
    BOOST_CHECK_EQUAL(a.Get(), 2);

    BOOST_CHECK_EQUAL((--a).Get(), 1);
    BOOST_CHECK_EQUAL(a.Get(), 1);

    BOOST_CHECK_EQUAL((a--).Get(), 1);
    BOOST_CHECK_EQUAL(a.Get(), 0);

    BOOST_CHECK_EQUAL((d + e).Get(), 0);
    BOOST_CHECK_EQUAL(d, 123_health);
    BOOST_CHECK_EQUAL(e, -123_health);

    BOOST_CHECK_EQUAL((d - e), 246_health);
    BOOST_CHECK_EQUAL(d, 123_health);
    BOOST_CHECK_EQUAL(e, -123_health);

    BOOST_CHECK_EQUAL((d * e).Get(), (d.Get() * e.Get()));
    BOOST_CHECK_EQUAL(d, 123_health);
    BOOST_CHECK_EQUAL(e, -123_health);

    BOOST_CHECK_EQUAL((d / e).Get(), (d.Get() / e.Get()));
    BOOST_CHECK_EQUAL(d, 123_health);
    BOOST_CHECK_EQUAL(e, -123_health);

    d += e;
    BOOST_CHECK_EQUAL(d, 0_health);

    d -= e;
    BOOST_CHECK_EQUAL(d, 123_health);

    d *= e;
    BOOST_CHECK_EQUAL(d, -15129_health);

    d /= e;
    BOOST_CHECK_EQUAL(d, 123_health);

    Health_t g(-978_health);
    BOOST_CHECK_EQUAL(g, -978_health);

    Health_t h(+978_health);
    BOOST_CHECK_EQUAL(h, +978_health);

    // test that signed literals obey same rules as int
    //  +-X  == -X
    //  -+X  == -X
    // +-+1  == -X
    // -+-1  == +X
    BOOST_CHECK_EQUAL((10_health + (+-1_health)).Get(), (10 + (+-1)));
    BOOST_CHECK_EQUAL((10_health + (-+1_health)).Get(), (10 + (-+1)));
    BOOST_CHECK_EQUAL((10_health + (+-+1_health)).Get(), (10 + (+-+1)));
    BOOST_CHECK_EQUAL((10_health + (-+-1_health)).Get(), (10 + (-+-1)));

    //
    // none of these should compile
    //
    // Coin_t coin;
    //++123_health;
    // 123_health ++;
    //--123_health;
    // 321_health --;
    // 321_health += 123_health;
    // 321_health -= 123_health;
    // 321_health *= 123_health;
    // 321_health /= 123_health;
    // Health_t d1(int(123));
    // Health_t d2(Health_t::value_type(123));
    // Health_t e1 = int(123);
    // Health_t e2 = Health_t::value_type();
    // Health_t f = Health_t::Make(a);
    // Health_t g(coin);
    // auto b5 = (a == int(123));
    // auto b6 = (a == int());
    // auto b7 = (a == Health_t::value_type("1234"));
    // auto b8 = (a == Health_t::value_type());
    //{
    //    auto bb1 = (a == coin);
    //    auto bb2 = (a != coin);
    //    auto bb3 = (a < coin);
    //    auto bb4 = (a <= coin);
    //    auto bb5 = (a > coin);
    //    auto bb6 = (a >= coin);
    //    auto mm1 = (a + coin);
    //    auto mm2 = (a - coin);
    //    auto mm3 = (a / coin);
    //    auto mm4 = (a * coin);
    //    a += coin;
    //    a -= coin;
    //    a *= coin;
    //    a /= coin;
    //}
    //{
    //    auto bb1 = (a == int(123));
    //    auto bb2 = (a != int(123));
    //    auto bb3 = (a < int(123));
    //    auto bb4 = (a <= int(123));
    //    auto bb5 = (a > int(123));
    //    auto bb6 = (a >= int(123));
    //    auto mm1 = (a + int(123));
    //    auto mm2 = (a - int(123));
    //    auto mm3 = (a / int(123));
    //    auto mm4 = (a * int(123));
    //    a += int(123);
    //    a -= int(123);
    //    a *= int(123);
    //    a /= int(123);
    //}
    //{
    //    auto bb1 = (a == int());
    //    auto bb2 = (a != int());
    //    auto bb3 = (a < int());
    //    auto bb4 = (a <= int());
    //    auto bb5 = (a > int());
    //    auto bb6 = (a >= int());
    //    auto mm1 = (a + int());
    //    auto mm2 = (a - int());
    //    auto mm3 = (a / int());
    //    auto mm4 = (a * int());
    //    a += int();
    //    a -= int();
    //    a *= int();
    //    a /= int();
    //}
    //{
    //    auto bb1 = (a == Health_t::value_type(123));
    //    auto bb2 = (a != Health_t::value_type(123));
    //    auto bb3 = (a < Health_t::value_type(123));
    //    auto bb4 = (a <= Health_t::value_type(123));
    //    auto bb5 = (a > Health_t::value_type(123));
    //    auto bb6 = (a >= Health_t::value_type(123));
    //    auto mm1 = (a + Health_t::value_type(123));
    //    auto mm2 = (a - Health_t::value_type(123));
    //    auto mm3 = (a / Health_t::value_type(123));
    //    auto mm4 = (a * Health_t::value_type(123));
    //    a += Health_t::value_type(123);
    //    a -= Health_t::value_type(123);
    //    a *= Health_t::value_type(123);
    //    a /= Health_t::value_type(123);
    //}
    //{
    //    auto bb1 = (a == Health_t::value_type());
    //    auto bb2 = (a != Health_t::value_type());
    //    auto bb3 = (a < Health_t::value_type());
    //    auto bb4 = (a <= Health_t::value_type());
    //    auto bb5 = (a > Health_t::value_type());
    //    auto bb6 = (a >= Health_t::value_type());
    //    auto mm1 = (a + Health_t::value_type());
    //    auto mm2 = (a - Health_t::value_type());
    //    auto mm3 = (a / Health_t::value_type());
    //    auto mm4 = (a * Health_t::value_type());
    //    a += Health_t::value_type();
    //    a -= Health_t::value_type();
    //    a *= Health_t::value_type();
    //    a /= Health_t::value_type();
    //}
}
