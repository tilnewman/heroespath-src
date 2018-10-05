// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_Strings"

#include <boost/test/unit_test.hpp>

#include "misc/strings.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace heroespath;
using namespace heroespath::misc;

template <typename Number_t>
struct NumStr
{
    NumStr(const Number_t NUMBER, const std::string & STRING)
        : num(NUMBER)
        , str(STRING)
    {}

    Number_t num;
    std::string str;
};

BOOST_AUTO_TEST_CASE(misc_strings__ToString_and_ToNumber)
{
    const int ERROR_NUMBER_INT { -321 };
    const float ERROR_NUMBER_FLOAT { -321.0 };

    const std::vector<NumStr<int>> NUM_STRS = { { 0, "0" },
                                                { 1, "1" },
                                                { -1, "-1" },
                                                { 123456789, "123456789" },
                                                { -123456789, "-123456789" } };

    for (const auto & NUM_STR : NUM_STRS)
    {
        BOOST_CHECK(ToString(NUM_STR.num) == NUM_STR.str);
        BOOST_CHECK(ToNumber(NUM_STR.str, ERROR_NUMBER_INT) == NUM_STR.num);
        BOOST_CHECK(ToString(ToNumber(NUM_STR.str, ERROR_NUMBER_INT)) == NUM_STR.str);
        BOOST_CHECK(ToNumber(ToString(NUM_STR.num), ERROR_NUMBER_INT) == NUM_STR.num);
    }

    const std::vector<NumStr<float>> FLOAT_STRS
        = { { 0, "0" },      { 1, "1" },      { -1.0f, "-1" }, { 0.0f, "0" },    { 1.0f, "1" },
            { -1.0f, "-1" }, { 0.1f, "0.1" }, { 1.1f, "1.1" }, { -1.1f, "-1.1" } };

    for (const auto & NUM_STR : FLOAT_STRS)
    {
        BOOST_CHECK(ToString(NUM_STR.num) == NUM_STR.str);
        BOOST_CHECK(misc::IsRealClose(ToNumber(NUM_STR.str, ERROR_NUMBER_FLOAT), NUM_STR.num));
        BOOST_CHECK(ToString(ToNumber(NUM_STR.str, ERROR_NUMBER_FLOAT)) == NUM_STR.str);

        BOOST_CHECK(
            misc::IsRealClose(ToNumber(ToString(NUM_STR.num), ERROR_NUMBER_FLOAT), NUM_STR.num));
    }

    BOOST_CHECK(ToString(true) == "true");
    BOOST_CHECK(ToString(false) == "false");
    BOOST_CHECK(ToString("") == "");
    BOOST_CHECK(ToString("abc 123") == "abc 123");

    std::ostringstream ss;
    ss << "abc 123";
    BOOST_CHECK(ToString(ss.str()) == "abc 123");

    BOOST_CHECK(ToNumber("", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);
    BOOST_CHECK(ToNumber("a", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);
    BOOST_CHECK(ToNumber("abc", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);

    BOOST_CHECK(misc::IsRealClose(ToNumber("", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));
    BOOST_CHECK(misc::IsRealClose(ToNumber("a", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));
    BOOST_CHECK(misc::IsRealClose(ToNumber("abc", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));

    BOOST_CHECK(ToNumber("-0", ERROR_NUMBER_INT) == 0);
    BOOST_CHECK(ToNumber("0000", ERROR_NUMBER_INT) == 0);
    BOOST_CHECK(ToNumber("00001", ERROR_NUMBER_INT) == 1);
    BOOST_CHECK(ToNumber("1 2", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);

    BOOST_CHECK(ToNumber("1.9", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);
}

BOOST_AUTO_TEST_CASE(misc_strings__NumberToStringWithOrdinalSuffix)
{
    BOOST_CHECK(NumberToStringWithOrdinalSuffix(0) == "0th");

    BOOST_CHECK(NumberToStringWithOrdinalSuffix(1) == "1st");
    BOOST_CHECK(NumberToStringWithOrdinalSuffix(2) == "2nd");
    BOOST_CHECK(NumberToStringWithOrdinalSuffix(3) == "3rd");

    for (short i(4); i <= 12; ++i)
    {
        BOOST_CHECK(NumberToStringWithOrdinalSuffix(i) == ToString(i) + "th");
    }
}

BOOST_AUTO_TEST_CASE(misc_strings__ContainsAnyOf)
{
    const std::string TEST_STRING_TO_SEARCH {
        "Now IS THE TIME FOR ALL good coders to write unit tests. 7777777 1 23 456"
    };

    const std::vector<std::string> ALL_ALWAYS_MATCH_STRINGS {
        "N",      "Now",   "Now ",
        "456",    " 456",  "6",
        "IS",     " THE ", "Now IS THE TIME FOR ALL good coders",
        "7",      "77",    "777",
        "7777777"
    };

    // empty input tests (all should always return false)
    BOOST_CHECK(ContainsAnyOf("", {}, true) == false);
    BOOST_CHECK(ContainsAnyOf("", {}, false) == false);
    //
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, {}, true) == false);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, {}, false) == false);
    //
    BOOST_CHECK(ContainsAnyOf("", ALL_ALWAYS_MATCH_STRINGS, true) == false);
    BOOST_CHECK(ContainsAnyOf("", ALL_ALWAYS_MATCH_STRINGS, false) == false);
    //
    BOOST_CHECK(ContainsAnyOf("", { "" }, false) == false);
    BOOST_CHECK(ContainsAnyOf("", { "" }, true) == false);
    BOOST_CHECK(ContainsAnyOf("", { "", "", "" }, false) == false);
    BOOST_CHECK(ContainsAnyOf("", { "", "", "" }, true) == false);
    //
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "" }, false) == false);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "" }, true) == false);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "", "", "" }, false) == false);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "", "", "" }, true) == false);

    // single never match tests

    // all strings that always match tests
    for (const auto & STRING_TO_SEARCH_FOR : ALL_ALWAYS_MATCH_STRINGS)
    {
        BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { STRING_TO_SEARCH_FOR }, true) == true);
        BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { STRING_TO_SEARCH_FOR }, false) == true);
    }

    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, ALL_ALWAYS_MATCH_STRINGS, false) == true);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, ALL_ALWAYS_MATCH_STRINGS, true) == true);

    // some strings match tests
    const std::vector<std::string> SOME_ALWAYS_MATCH_STRINGS = [&]() {
        std::vector<std::string> vec { ALL_ALWAYS_MATCH_STRINGS };
        vec.emplace_back("xyz");
        return vec;
    }();

    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, SOME_ALWAYS_MATCH_STRINGS, false) == true);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, SOME_ALWAYS_MATCH_STRINGS, true) == true);

    // none ever match strings tests
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz" }, false) == false);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz" }, true) == false);
    //
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz" }, false) == false);
    BOOST_CHECK(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz" }, true) == false);

    // only match if case insensitive
    BOOST_CHECK(
        ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz", "NOW" }, false) == true);

    BOOST_CHECK(
        ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz", "NOW" }, true) == false);
}

BOOST_AUTO_TEST_CASE(misc_strings__is_number_typeTest)
{
    // fundamental number types
    BOOST_CHECK((type_helpers::is_number_type<char>::value));
    BOOST_CHECK((type_helpers::is_number_type<short>::value));
    BOOST_CHECK((type_helpers::is_number_type<int>::value));
    BOOST_CHECK((type_helpers::is_number_type<long>::value));
    BOOST_CHECK((type_helpers::is_number_type<long long>::value));

    // repeat above only unsigned
    BOOST_CHECK((type_helpers::is_number_type<unsigned char>::value));
    BOOST_CHECK((type_helpers::is_number_type<unsigned short>::value));
    BOOST_CHECK((type_helpers::is_number_type<unsigned int>::value));
    BOOST_CHECK((type_helpers::is_number_type<unsigned long>::value));
    BOOST_CHECK((type_helpers::is_number_type<unsigned long long>::value));

    // repeat BOTH above only with const
    BOOST_CHECK((type_helpers::is_number_type<const char>::value));
    BOOST_CHECK((type_helpers::is_number_type<const short>::value));
    BOOST_CHECK((type_helpers::is_number_type<const int>::value));
    BOOST_CHECK((type_helpers::is_number_type<const long>::value));
    BOOST_CHECK((type_helpers::is_number_type<const long long>::value));
    //
    BOOST_CHECK((type_helpers::is_number_type<const unsigned char>::value));
    BOOST_CHECK((type_helpers::is_number_type<const unsigned short>::value));
    BOOST_CHECK((type_helpers::is_number_type<const unsigned int>::value));
    BOOST_CHECK((type_helpers::is_number_type<const unsigned long>::value));
    BOOST_CHECK((type_helpers::is_number_type<const unsigned long long>::value));

    // standard library types
    BOOST_CHECK((type_helpers::is_number_type<std::int8_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<std::int16_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<std::int32_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<std::int64_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<char16_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<char32_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<wchar_t>::value));

    // repeat above only unsigned
    BOOST_CHECK((type_helpers::is_number_type<std::uint8_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<std::uint16_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<std::uint32_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<std::uint64_t>::value));

    // repeat BOTH above only with const
    BOOST_CHECK((type_helpers::is_number_type<const std::int8_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const std::int16_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const std::int32_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const std::int64_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const char16_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const char32_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const wchar_t>::value));
    //
    BOOST_CHECK((type_helpers::is_number_type<const std::uint8_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const std::uint16_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const std::uint32_t>::value));
    BOOST_CHECK((type_helpers::is_number_type<const std::uint64_t>::value));

    // floating point types
    BOOST_CHECK((type_helpers::is_number_type<float>::value));
    BOOST_CHECK((type_helpers::is_number_type<double>::value));
    BOOST_CHECK((type_helpers::is_number_type<long double>::value));

    // misc fail cases
    BOOST_CHECK((type_helpers::is_number_type<bool>::value) == false);
    BOOST_CHECK((type_helpers::is_number_type<const bool>::value) == false);
    BOOST_CHECK((type_helpers::is_number_type<std::string>::value) == false);
    BOOST_CHECK((type_helpers::is_number_type<const std::string>::value) == false);
}

BOOST_AUTO_TEST_CASE(misc_strings__is_non_floating_point_number_typeTEST)
{
    // fundamental number types
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<char>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<short>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<int>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<long>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<long long>::value));

    // repeat above only unsigned
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<unsigned char>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<unsigned short>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<unsigned int>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<unsigned long>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<unsigned long long>::value));

    // repeat BOTH above only with const
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const char>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const short>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const int>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const long>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const long long>::value));
    //
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const unsigned char>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const unsigned short>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const unsigned int>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const unsigned long>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const unsigned long long>::value));

    // standard library types
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::int8_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::int16_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::int32_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::int64_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<char16_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<char32_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<wchar_t>::value));

    // repeat above only unsigned
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::uint8_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::uint16_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::uint32_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::uint64_t>::value));

    // repeat BOTH above only with const
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::int8_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::int16_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::int32_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::int64_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const char16_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const char32_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const wchar_t>::value));
    //
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::uint8_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::uint16_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::uint32_t>::value));
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const std::uint64_t>::value));

    // floating point types (should always return false)
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<float>::value) == false);
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<double>::value) == false);
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<long double>::value) == false);

    // misc fail cases
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<bool>::value) == false);
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<const bool>::value) == false);
    BOOST_CHECK((type_helpers::is_non_floating_point_number_type<std::string>::value) == false);

    BOOST_CHECK(
        (type_helpers::is_non_floating_point_number_type<const std::string>::value) == false);
}

BOOST_AUTO_TEST_CASE(misc_strings__FindNumber)
{
    const int ERROR_NUMBER { -1 };

    // test empty input cases
    {
        BOOST_CHECK(FindNumber(0, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_CHECK(FindNumber(1, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_CHECK(FindNumber(2, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_CHECK(FindNumber(3, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_CHECK(FindNumber(10, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_CHECK(FindNumber(std::numeric_limits<int>::max(), "", ERROR_NUMBER) == ERROR_NUMBER);
    }

    // strings with no numbers tests
    {
        const std::string TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS {
            "The numbers are , , and , the one in this filename log-.txt, and finally a large one ."
        };

        BOOST_CHECK(
            FindNumber(0, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(2, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(3, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(10, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(
                std::numeric_limits<int>::max(),
                TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS,
                ERROR_NUMBER)
            == ERROR_NUMBER);
    }

    // test cases where strings are there to be found
    {
        const std::string TEST_STRING_TO_SEARCH_WITH_NUMBERS {
            "The numbers are 0.1, and inside the filename log-234.txt, an impossibly "
            "large one 9999999999999999999999999999999, and a more plausible large one 123456789."
        };

        BOOST_CHECK(FindNumber(0, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 0);
        BOOST_CHECK(FindNumber(1, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 1);
        BOOST_CHECK(FindNumber(2, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 234);

        BOOST_CHECK(
            FindNumber(3, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(FindNumber(4, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 123456789);

        BOOST_CHECK(
            FindNumber(5, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(10, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_CHECK(
            FindNumber(
                std::numeric_limits<int>::max(), TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER)
            == ERROR_NUMBER);

        // test helper functions
        BOOST_CHECK(FindFirstNumber(TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 0);
        BOOST_CHECK(FindLastNumber(TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 123456789);
    }
}
