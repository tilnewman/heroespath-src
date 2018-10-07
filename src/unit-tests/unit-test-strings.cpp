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
#include <cctype>
#include <iomanip>
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

BOOST_AUTO_TEST_CASE(misc_strings__Case)
{
    // test all individual chars
    for (char ch(std::numeric_limits<char>::lowest());; ++ch)
    {
        if (misc::IsAlpha(ch))
        {
            BOOST_TEST(misc::IsUpper(ch) != misc::IsLower(ch));
            BOOST_TEST(misc::IsUpper(misc::ToUpperCopy(ch)));
            BOOST_TEST(misc::IsLower(misc::ToLowerCopy(ch)));
        }
        else
        {
            BOOST_TEST(misc::IsUpper(ch) == false);
            BOOST_TEST(misc::IsLower(ch) == false);
            BOOST_TEST(misc::IsUpper(misc::ToUpperCopy(ch)) == false);
            BOOST_TEST(misc::IsLower(misc::ToLowerCopy(ch)) == false);
        }

        if (std::numeric_limits<char>::max() == ch)
        {
            break;
        }
    }

    // test whole strings -empty
    const std::string EMPTY_STR { "" };

    BOOST_TEST(misc::ToUpperCopy(EMPTY_STR) == EMPTY_STR);
    BOOST_TEST(misc::ToLowerCopy(EMPTY_STR) == EMPTY_STR);

    // test whole strings -without letters
    const std::string NON_LETTER_STR {
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n \n\r"
    };

    BOOST_TEST(misc::ToUpperCopy(NON_LETTER_STR) == NON_LETTER_STR);
    BOOST_TEST(misc::ToLowerCopy(NON_LETTER_STR) == NON_LETTER_STR);

    // test whole strings -with letters
    const std::string MIXED_CASE_TEST_STR {
        "This StrinG has A mix of UPPER and lower case characters."
    };

    const std::string UPPER_CASE_TEST_STR {
        "THIS STRING HAS A MIX OF UPPER AND LOWER CASE CHARACTERS."
    };

    const std::string LOWER_CASE_TEST_STR {
        "this string has a mix of upper and lower case characters."
    };

    BOOST_TEST(misc::ToUpperCopy(MIXED_CASE_TEST_STR) == UPPER_CASE_TEST_STR);
    BOOST_TEST(misc::ToUpperCopy(UPPER_CASE_TEST_STR) == UPPER_CASE_TEST_STR);
    BOOST_TEST(misc::ToUpperCopy(LOWER_CASE_TEST_STR) == UPPER_CASE_TEST_STR);

    BOOST_TEST(misc::ToLowerCopy(MIXED_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
    BOOST_TEST(misc::ToLowerCopy(UPPER_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
    BOOST_TEST(misc::ToLowerCopy(LOWER_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
}

BOOST_AUTO_TEST_CASE(misc_strings__CamelTo)
{
    // test empty cases
    BOOST_TEST(misc::CamelTo("", '_') == "");
    BOOST_TEST(misc::CamelTo("", "") == "");

    // test non-letter cases
    const std::string NON_LETTER_STR {
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n \n\r"
    };

    const char SEPARATOR_CHAR { '_' };
    const std::string SEPARATOR_STR { std::string(1, SEPARATOR_CHAR) };

    BOOST_TEST(misc::CamelTo(NON_LETTER_STR, SEPARATOR_CHAR) == NON_LETTER_STR);
    BOOST_TEST(misc::CamelTo(NON_LETTER_STR, "") == NON_LETTER_STR);
    BOOST_TEST(misc::CamelTo(NON_LETTER_STR, SEPARATOR_STR) == NON_LETTER_STR);

    // test valid cases

    auto test = [](const std::string & INPUT,
                   const std::string & SEPARATOR,
                   const std::string & EXPECTED_OUTPUT,
                   const std::string & CASE_MESSAGE = "normal") {
        //
        auto makeErrorMessage = [&](const std::string & ACTUAL_OUTPUT) {
            return "(" + misc::Quoted(CASE_MESSAGE) + " case)  CamelTo(\"" + INPUT
                + "\", sep=" + misc::Quoted(SEPARATOR) + ")!=" + misc::Quoted(EXPECTED_OUTPUT)
                + "\"  actual=" + misc::Quoted(ACTUAL_OUTPUT);
        };

        const auto ACTUAL_OUTPUT { misc::CamelTo(INPUT, SEPARATOR) };

        BOOST_TEST(ACTUAL_OUTPUT == EXPECTED_OUTPUT, makeErrorMessage(ACTUAL_OUTPUT));
    };

    auto testAllCasesTheSame = [test, SEPARATOR_STR](
                                   const std::string & INPUT,
                                   const std::string & CASE_MESSAGE = "input should equal output") {
        test(INPUT, SEPARATOR_STR, INPUT, CASE_MESSAGE);

        test(
            misc::ToLowerCopy(INPUT),
            SEPARATOR_STR,
            INPUT,
            CASE_MESSAGE + " with input lower case");

        test(
            misc::ToUpperCopy(INPUT),
            SEPARATOR_STR,
            INPUT,
            CASE_MESSAGE + " with input upper case");

        test(INPUT, "", INPUT, CASE_MESSAGE + " with empty separator");

        test(
            misc::ToLowerCopy(INPUT),
            "",
            INPUT,
            CASE_MESSAGE + " with input lower case" + " with empty separator");

        test(
            misc::ToUpperCopy(INPUT),
            "",
            INPUT,
            CASE_MESSAGE + " with input upper case" + " with empty separator");
    };

    testAllCasesTheSame("");

    testAllCasesTheSame(NON_LETTER_STR);

    testAllCasesTheSame("a");
    testAllCasesTheSame("ab");
    testAllCasesTheSame("a a");
    testAllCasesTheSame("ab ab");

    testAllCasesTheSame("_");
    testAllCasesTheSame("__");
    testAllCasesTheSame("___");
    testAllCasesTheSame("_ __ _ ___ _ ____");

    testAllCasesTheSame("_a");
    testAllCasesTheSame("a_");
    testAllCasesTheSame("_ _a");
    testAllCasesTheSame("_ a_");
    testAllCasesTheSame("a _");
    testAllCasesTheSame("_a _");
    testAllCasesTheSame("a_ _");

    test(
        "A aB Ab xYz XyZ xYZ Xyz aBcD AbCd A",
        SEPARATOR_STR,
        "a a_b a_b x_y_z x_y_z x_yz x_yz a_b_c_d a_b_c_d a",
        "complex");

    test(
        "A a1B A1b x1Y1z X1y1Z x1Y1Z X1yz a1B1c1D A1b1C1d A",
        SEPARATOR_STR,
        "a a1b a1b x1y1z x1y1z x1y1z x1yz a1b1c1d a1b1c1d a",
        "complex with numbers canceling out separators");
}

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
        BOOST_TEST(ToString(NUM_STR.num) == NUM_STR.str);
        BOOST_TEST(ToNumber(NUM_STR.str, ERROR_NUMBER_INT) == NUM_STR.num);
        BOOST_TEST(ToString(ToNumber(NUM_STR.str, ERROR_NUMBER_INT)) == NUM_STR.str);
        BOOST_TEST(ToNumber(ToString(NUM_STR.num), ERROR_NUMBER_INT) == NUM_STR.num);
    }

    const std::vector<NumStr<float>> FLOAT_STRS
        = { { 0, "0" },      { 1, "1" },      { -1.0f, "-1" }, { 0.0f, "0" },    { 1.0f, "1" },
            { -1.0f, "-1" }, { 0.1f, "0.1" }, { 1.1f, "1.1" }, { -1.1f, "-1.1" } };

    for (const auto & NUM_STR : FLOAT_STRS)
    {
        BOOST_TEST(ToString(NUM_STR.num) == NUM_STR.str);
        BOOST_TEST(misc::IsRealClose(ToNumber(NUM_STR.str, ERROR_NUMBER_FLOAT), NUM_STR.num));
        BOOST_TEST(ToString(ToNumber(NUM_STR.str, ERROR_NUMBER_FLOAT)) == NUM_STR.str);

        BOOST_TEST(
            misc::IsRealClose(ToNumber(ToString(NUM_STR.num), ERROR_NUMBER_FLOAT), NUM_STR.num));
    }

    BOOST_TEST(ToString(true) == "true");
    BOOST_TEST(ToString(false) == "false");
    BOOST_TEST(ToString("") == "");
    BOOST_TEST(ToString("abc 123") == "abc 123");

    std::ostringstream ss;
    ss << "abc 123";
    BOOST_TEST(ToString(ss.str()) == "abc 123");

    BOOST_TEST(ToNumber("", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);
    BOOST_TEST(ToNumber("a", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);
    BOOST_TEST(ToNumber("abc", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);

    BOOST_TEST(misc::IsRealClose(ToNumber("", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));
    BOOST_TEST(misc::IsRealClose(ToNumber("a", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));
    BOOST_TEST(misc::IsRealClose(ToNumber("abc", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));

    BOOST_TEST(ToNumber("-0", ERROR_NUMBER_INT) == 0);
    BOOST_TEST(ToNumber("0000", ERROR_NUMBER_INT) == 0);
    BOOST_TEST(ToNumber("00001", ERROR_NUMBER_INT) == 1);
    BOOST_TEST(ToNumber("1 2", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);

    BOOST_TEST(ToNumber("1.9", ERROR_NUMBER_INT) == ERROR_NUMBER_INT);
}

BOOST_AUTO_TEST_CASE(misc_strings__NumberToStringWithOrdinalSuffix)
{
    BOOST_TEST(NumberToStringWithOrdinalSuffix(0) == "0th");

    BOOST_TEST(NumberToStringWithOrdinalSuffix(1) == "1st");
    BOOST_TEST(NumberToStringWithOrdinalSuffix(2) == "2nd");
    BOOST_TEST(NumberToStringWithOrdinalSuffix(3) == "3rd");

    for (short i(4); i <= 12; ++i)
    {
        BOOST_TEST(NumberToStringWithOrdinalSuffix(i) == ToString(i) + "th");
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
    BOOST_TEST(ContainsAnyOf("", {}, true) == false);
    BOOST_TEST(ContainsAnyOf("", {}, false) == false);
    //
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, {}, true) == false);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, {}, false) == false);
    //
    BOOST_TEST(ContainsAnyOf("", ALL_ALWAYS_MATCH_STRINGS, true) == false);
    BOOST_TEST(ContainsAnyOf("", ALL_ALWAYS_MATCH_STRINGS, false) == false);
    //
    BOOST_TEST(ContainsAnyOf("", { "" }, false) == false);
    BOOST_TEST(ContainsAnyOf("", { "" }, true) == false);
    BOOST_TEST(ContainsAnyOf("", { "", "", "" }, false) == false);
    BOOST_TEST(ContainsAnyOf("", { "", "", "" }, true) == false);
    //
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "" }, false) == false);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "" }, true) == false);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "", "", "" }, false) == false);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "", "", "" }, true) == false);

    // single never match tests

    // all strings that always match tests
    for (const auto & STRING_TO_SEARCH_FOR : ALL_ALWAYS_MATCH_STRINGS)
    {
        BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { STRING_TO_SEARCH_FOR }, true) == true);
        BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { STRING_TO_SEARCH_FOR }, false) == true);
    }

    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, ALL_ALWAYS_MATCH_STRINGS, false) == true);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, ALL_ALWAYS_MATCH_STRINGS, true) == true);

    // some strings match tests
    const std::vector<std::string> SOME_ALWAYS_MATCH_STRINGS = [&]() {
        std::vector<std::string> vec { ALL_ALWAYS_MATCH_STRINGS };
        vec.emplace_back("xyz");
        return vec;
    }();

    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, SOME_ALWAYS_MATCH_STRINGS, false) == true);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, SOME_ALWAYS_MATCH_STRINGS, true) == true);

    // none ever match strings tests
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz" }, false) == false);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz" }, true) == false);
    //
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz" }, false) == false);
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz" }, true) == false);

    // only match if case insensitive
    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz", "NOW" }, false) == true);

    BOOST_TEST(ContainsAnyOf(TEST_STRING_TO_SEARCH, { "xyz", "XYZ", "xYz", "NOW" }, true) == false);
}

BOOST_AUTO_TEST_CASE(misc_strings__is_number_typeTest)
{
    // fundamental number types
    BOOST_TEST((type_helpers::is_number_type<char>::value));
    BOOST_TEST((type_helpers::is_number_type<short>::value));
    BOOST_TEST((type_helpers::is_number_type<int>::value));
    BOOST_TEST((type_helpers::is_number_type<long>::value));
    BOOST_TEST((type_helpers::is_number_type<long long>::value));

    // repeat above only unsigned
    BOOST_TEST((type_helpers::is_number_type<unsigned char>::value));
    BOOST_TEST((type_helpers::is_number_type<unsigned short>::value));
    BOOST_TEST((type_helpers::is_number_type<unsigned int>::value));
    BOOST_TEST((type_helpers::is_number_type<unsigned long>::value));
    BOOST_TEST((type_helpers::is_number_type<unsigned long long>::value));

    // repeat BOTH above only with const
    BOOST_TEST((type_helpers::is_number_type<const char>::value));
    BOOST_TEST((type_helpers::is_number_type<const short>::value));
    BOOST_TEST((type_helpers::is_number_type<const int>::value));
    BOOST_TEST((type_helpers::is_number_type<const long>::value));
    BOOST_TEST((type_helpers::is_number_type<const long long>::value));
    //
    BOOST_TEST((type_helpers::is_number_type<const unsigned char>::value));
    BOOST_TEST((type_helpers::is_number_type<const unsigned short>::value));
    BOOST_TEST((type_helpers::is_number_type<const unsigned int>::value));
    BOOST_TEST((type_helpers::is_number_type<const unsigned long>::value));
    BOOST_TEST((type_helpers::is_number_type<const unsigned long long>::value));

    // standard library types
    BOOST_TEST((type_helpers::is_number_type<std::int8_t>::value));
    BOOST_TEST((type_helpers::is_number_type<std::int16_t>::value));
    BOOST_TEST((type_helpers::is_number_type<std::int32_t>::value));
    BOOST_TEST((type_helpers::is_number_type<std::int64_t>::value));
    BOOST_TEST((type_helpers::is_number_type<char16_t>::value));
    BOOST_TEST((type_helpers::is_number_type<char32_t>::value));
    BOOST_TEST((type_helpers::is_number_type<wchar_t>::value));

    // repeat above only unsigned
    BOOST_TEST((type_helpers::is_number_type<std::uint8_t>::value));
    BOOST_TEST((type_helpers::is_number_type<std::uint16_t>::value));
    BOOST_TEST((type_helpers::is_number_type<std::uint32_t>::value));
    BOOST_TEST((type_helpers::is_number_type<std::uint64_t>::value));

    // repeat BOTH above only with const
    BOOST_TEST((type_helpers::is_number_type<const std::int8_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const std::int16_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const std::int32_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const std::int64_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const char16_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const char32_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const wchar_t>::value));
    //
    BOOST_TEST((type_helpers::is_number_type<const std::uint8_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const std::uint16_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const std::uint32_t>::value));
    BOOST_TEST((type_helpers::is_number_type<const std::uint64_t>::value));

    // floating point types
    BOOST_TEST((type_helpers::is_number_type<float>::value));
    BOOST_TEST((type_helpers::is_number_type<double>::value));
    BOOST_TEST((type_helpers::is_number_type<long double>::value));

    // misc fail cases
    BOOST_TEST((type_helpers::is_number_type<bool>::value) == false);
    BOOST_TEST((type_helpers::is_number_type<const bool>::value) == false);
    BOOST_TEST((type_helpers::is_number_type<std::string>::value) == false);
    BOOST_TEST((type_helpers::is_number_type<const std::string>::value) == false);
}

BOOST_AUTO_TEST_CASE(misc_strings__is_non_floating_point_number_typeTEST)
{
    // fundamental number types
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<char>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<short>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<int>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<long>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<long long>::value));

    // repeat above only unsigned
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<unsigned char>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<unsigned short>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<unsigned int>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<unsigned long>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<unsigned long long>::value));

    // repeat BOTH above only with const
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const char>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const short>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const int>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const long>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const long long>::value));
    //
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const unsigned char>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const unsigned short>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const unsigned int>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const unsigned long>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const unsigned long long>::value));

    // standard library types
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::int8_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::int16_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::int32_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::int64_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<char16_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<char32_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<wchar_t>::value));

    // repeat above only unsigned
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::uint8_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::uint16_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::uint32_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::uint64_t>::value));

    // repeat BOTH above only with const
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::int8_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::int16_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::int32_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::int64_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const char16_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const char32_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const wchar_t>::value));
    //
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::uint8_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::uint16_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::uint32_t>::value));
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const std::uint64_t>::value));

    // floating point types (should always return false)
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<float>::value) == false);
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<double>::value) == false);
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<long double>::value) == false);

    // misc fail cases
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<bool>::value) == false);
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<const bool>::value) == false);
    BOOST_TEST((type_helpers::is_non_floating_point_number_type<std::string>::value) == false);

    BOOST_TEST(
        (type_helpers::is_non_floating_point_number_type<const std::string>::value) == false);
}

BOOST_AUTO_TEST_CASE(misc_strings__FindNumber)
{
    const int ERROR_NUMBER { -1 };

    // test empty input cases
    {
        BOOST_TEST(FindNumber(0, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_TEST(FindNumber(1, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_TEST(FindNumber(2, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_TEST(FindNumber(3, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_TEST(FindNumber(10, "", ERROR_NUMBER) == ERROR_NUMBER);
        BOOST_TEST(FindNumber(std::numeric_limits<int>::max(), "", ERROR_NUMBER) == ERROR_NUMBER);
    }

    // strings with no numbers tests
    {
        const std::string TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS {
            "The numbers are , , and , the one in this filename log-.txt, and finally a large one ."
        };

        BOOST_TEST(
            FindNumber(0, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
            FindNumber(1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
            FindNumber(2, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
            FindNumber(3, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
            FindNumber(10, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
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

        BOOST_TEST(FindNumber(0, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 0);
        BOOST_TEST(FindNumber(1, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 1);
        BOOST_TEST(FindNumber(2, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 234);

        BOOST_TEST(FindNumber(3, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(FindNumber(4, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 123456789);

        BOOST_TEST(FindNumber(5, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
            FindNumber(10, TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == ERROR_NUMBER);

        BOOST_TEST(
            FindNumber(
                std::numeric_limits<int>::max(), TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER)
            == ERROR_NUMBER);

        // test helper functions
        BOOST_TEST(FindFirstNumber(TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 0);
        BOOST_TEST(FindLastNumber(TEST_STRING_TO_SEARCH_WITH_NUMBERS, ERROR_NUMBER) == 123456789);
    }
}
