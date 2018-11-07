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

#include <SFML/System/Clock.hpp>

#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"

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
            BOOST_CHECK(misc::IsUpper(ch) != misc::IsLower(ch));
            BOOST_CHECK(misc::IsUpper(misc::ToUpperCopy(ch)));
            BOOST_CHECK(misc::IsLower(misc::ToLowerCopy(ch)));
        }
        else
        {
            BOOST_CHECK(misc::IsUpper(ch) == false);
            BOOST_CHECK(misc::IsLower(ch) == false);
            BOOST_CHECK(misc::IsUpper(misc::ToUpperCopy(ch)) == false);
            BOOST_CHECK(misc::IsLower(misc::ToLowerCopy(ch)) == false);
        }

        if (std::numeric_limits<char>::max() == ch)
        {
            break;
        }
    }

    // test whole strings -empty
    const std::string EMPTY_STR { "" };

    BOOST_CHECK(misc::ToUpperCopy(EMPTY_STR) == EMPTY_STR);
    BOOST_CHECK(misc::ToLowerCopy(EMPTY_STR) == EMPTY_STR);

    // test whole strings -without letters
    const std::string NON_LETTER_STR {
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n \n\r"
    };

    BOOST_CHECK(misc::ToUpperCopy(NON_LETTER_STR) == NON_LETTER_STR);
    BOOST_CHECK(misc::ToLowerCopy(NON_LETTER_STR) == NON_LETTER_STR);

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

    BOOST_CHECK(misc::ToUpperCopy(MIXED_CASE_TEST_STR) == UPPER_CASE_TEST_STR);
    BOOST_CHECK(misc::ToUpperCopy(UPPER_CASE_TEST_STR) == UPPER_CASE_TEST_STR);
    BOOST_CHECK(misc::ToUpperCopy(LOWER_CASE_TEST_STR) == UPPER_CASE_TEST_STR);

    BOOST_CHECK(misc::ToLowerCopy(MIXED_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
    BOOST_CHECK(misc::ToLowerCopy(UPPER_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
    BOOST_CHECK(misc::ToLowerCopy(LOWER_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
}

BOOST_AUTO_TEST_CASE(misc_strings__Case_SpeedTestsComparedToBoost)
{
    auto makeRandomChar = []() { return static_cast<char>(misc::random::Int(32, 90)); };

    auto makeRandomString = [makeRandomChar](const std::size_t LENGTH) {
        std::string str;
        for (std::size_t index(0); index < LENGTH; ++index)
        {
            str += makeRandomChar();
        }
        return str;
    };

    const std::size_t REPEAT_TEST_COUNT { 200 };
    const std::size_t MAX_STRING_LENGTH { 257 };
    const std::size_t RAND_STR_PER_TEST_COUNT { 2000 };

    struct TestResult
    {
        TestResult(const std::size_t STR_LENGTH)
            : str_length(STR_LENGTH)
            , my_times()
            , boost_times()
        {
            my_times.reserve(REPEAT_TEST_COUNT);
            boost_times.reserve(REPEAT_TEST_COUNT);
        }

        std::size_t str_length;
        std::vector<float> my_times;
        std::vector<float> boost_times;
    };

    std::vector<TestResult> testResults;

    std::vector<std::string> strings;
    strings.reserve(RAND_STR_PER_TEST_COUNT);

    std::vector<std::string> trash;
    trash.reserve(strings.size() * 4);

    for (std::size_t strLength(32); strLength <= MAX_STRING_LENGTH;)
    {
        // M_HP_LOG_WRN("test #" << strLength);
        TestResult result(strLength);

        strings.clear();
        for (std::size_t randIndex(0); randIndex <= RAND_STR_PER_TEST_COUNT; ++randIndex)
        {
            strings.emplace_back(makeRandomString(strLength));
        }

        for (std::size_t testIteration(0); testIteration <= REPEAT_TEST_COUNT; ++testIteration)
        {
            // misc::Vector::ShuffleVec(strings);

            trash.clear();

            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(boost::algorithm::to_lower_copy(STR_TO_TEST));
            }
            sf::Clock clock;
            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(boost::algorithm::to_lower_copy(STR_TO_TEST));
            }
            result.boost_times.emplace_back(clock.getElapsedTime().asMilliseconds());

            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(misc::ToLowerCopy(STR_TO_TEST));
            }
            clock.restart();
            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(misc::ToLowerCopy(STR_TO_TEST));
            }
            result.my_times.emplace_back(clock.getElapsedTime().asMilliseconds());
        }

        testResults.emplace_back(result);

        strLength += 64;
    }

    float boostTotalWins { 0.0f };
    float myTotalWins { 0.0f };

    float boostTotalTimeMS { 0.0f };
    float myTotalTimeMS { 0.0f };

    std::ostringstream ssFinal;

    ssFinal << "\nToLower Speed Tests:";

    for (const auto & TEST_RESULT : testResults)
    {
        auto makeTestReportString =
            [&](const std::string & WHICH_RESULTS, const std::vector<float> & TIMES, float & sum) {
                auto min { TIMES.at(0) };
                auto max { 0.0f };
                sum = 0.0f;

                for (const float TIME_MS : TIMES)
                {
                    sum += TIME_MS;

                    if (TIME_MS < min)
                    {
                        min = TIME_MS;
                    }

                    if (TIME_MS > max)
                    {
                        max = TIME_MS;
                    }
                }

                const auto AVERAGE { sum / static_cast<float>(TIMES.size()) };

                const float STANDARD_DEVIATION { misc::Vector::StandardDeviation(
                    TIMES, TIMES.size(), AVERAGE, false) };

                std::ostringstream ssReport;

                ssReport << "\n\t\t" << WHICH_RESULTS << "\tx" << TIMES.size() << "\tsum=" << sum
                         << "\t[" << min << ", " << AVERAGE << ", " << max << "] ("
                         << STANDARD_DEVIATION << ")";

                return ssReport.str();
            };

        float sum { 0.0f };

        const auto BOOST_TEST_STR { makeTestReportString("boost", TEST_RESULT.boost_times, sum) };
        const auto BOOST_TEST_SUM { sum };
        boostTotalTimeMS += BOOST_TEST_SUM;

        const auto MY_TEST_STR { makeTestReportString(" mine", TEST_RESULT.my_times, sum) };
        const auto MY_TEST_SUM { sum };
        myTotalTimeMS += MY_TEST_SUM;

        if ((BOOST_TEST_SUM > 0.0f) && (MY_TEST_SUM > 0.0f))
        {
            ssFinal << "\n\t length=" << TEST_RESULT.str_length << BOOST_TEST_STR << MY_TEST_STR
                    << "\n\t\t";

            if (BOOST_TEST_SUM > MY_TEST_SUM)
            {
                ssFinal << "MINE wins by " << (BOOST_TEST_SUM - MY_TEST_SUM) << "ms or "
                        << (BOOST_TEST_SUM / MY_TEST_SUM) << "x";

                myTotalWins += 1.0f;
            }
            else
            {
                ssFinal << "BOOST wins by " << (MY_TEST_SUM - BOOST_TEST_SUM) << "ms or "
                        << (MY_TEST_SUM / BOOST_TEST_SUM) << "x";

                boostTotalWins += 1.0f;
            }
        }

        ssFinal << "\n\n";
    }

    ssFinal << "\n\t Total Time BOOST=" << boostTotalTimeMS
            << "\n\t Total Time MINE=" << myTotalTimeMS;

    if (boostTotalTimeMS > myTotalTimeMS)
    {
        ssFinal << "\n\tI win by " << (boostTotalTimeMS - myTotalTimeMS) << "ms or "
                << (boostTotalTimeMS / myTotalTimeMS) << "x";
    }
    else
    {
        ssFinal << "\n\tBOOST wins by " << (myTotalTimeMS - boostTotalTimeMS) << "ms or "
                << (myTotalTimeMS / boostTotalTimeMS) << "x";
    }

    ssFinal << "\n\t Total Score BOOST=" << boostTotalWins
            << "\n\t Total Score MINE=" << myTotalWins;

    if (boostTotalWins > myTotalWins)
    {
        ssFinal << "\n\tBOOST wins by " << (boostTotalWins - myTotalWins);
    }
    else
    {
        ssFinal << "\n\tI win by " << (myTotalWins - boostTotalWins);
    }

    M_HP_LOG(ssFinal.str());
}

BOOST_AUTO_TEST_CASE(misc_strings__CamelTo)
{
    // test empty cases
    BOOST_CHECK(misc::CamelTo("", '_') == "");
    BOOST_CHECK(misc::CamelTo("", "") == "");

    // test non-letter cases
    const std::string NON_LETTER_STR {
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n \n\r"
    };

    const char SEPARATOR_CHAR { '_' };
    const std::string SEPARATOR_STR { std::string(1, SEPARATOR_CHAR) };

    BOOST_CHECK(misc::CamelTo(NON_LETTER_STR, SEPARATOR_CHAR) == NON_LETTER_STR);
    BOOST_CHECK(misc::CamelTo(NON_LETTER_STR, "") == NON_LETTER_STR);
    BOOST_CHECK(misc::CamelTo(NON_LETTER_STR, SEPARATOR_STR) == NON_LETTER_STR);

    // test valid cases

    auto test = [](const std::string & INPUT,
                   const std::string & SEPARATOR,
                   const misc::CaseChange CASE_CHANGE,
                   const std::string & EXPECTED_OUTPUT,
                   const std::string & CASE_MESSAGE = "normal") {
        //
        auto makeErrorMessage = [&](const std::string & ACTUAL_OUTPUT) {
            const auto CASE_CHAGE_STR = [CASE_CHANGE]() {
                if (CASE_CHANGE == misc::CaseChange::Both)
                {
                    return "Both";
                }
                else if (CASE_CHANGE == misc::CaseChange::UpperToLower)
                {
                    return "UpperToLower";
                }
                else
                {
                    return "LowerToUpper";
                }
            }();

            return "(" + misc::Quoted(CASE_MESSAGE) + " case)  CamelTo(\"" + INPUT
                + "\", sep=" + misc::Quoted(SEPARATOR) + ", case_change=" + CASE_CHAGE_STR + ")!="
                + misc::Quoted(EXPECTED_OUTPUT) + "\"  actual=" + misc::Quoted(ACTUAL_OUTPUT);
        };

        const auto ACTUAL_OUTPUT { misc::CamelTo(INPUT, SEPARATOR, CASE_CHANGE) };
        BOOST_TEST(ACTUAL_OUTPUT == EXPECTED_OUTPUT, makeErrorMessage(ACTUAL_OUTPUT));
    };

    auto testEachCaseChange = [test](
                                  const std::string & INPUT,
                                  const std::string & SEPARATOR,
                                  const std::string & EXPECTED_OUTPUT,
                                  const std::string & CASE_MESSAGE) {
        test(INPUT, SEPARATOR, CaseChange::LowerToUpper, EXPECTED_OUTPUT, CASE_MESSAGE);
        test(INPUT, SEPARATOR, CaseChange::UpperToLower, EXPECTED_OUTPUT, CASE_MESSAGE);
        test(INPUT, SEPARATOR, CaseChange::Both, EXPECTED_OUTPUT, CASE_MESSAGE);
    };

    auto testAllCasesTheSame = [testEachCaseChange, SEPARATOR_STR](
                                   const std::string & INPUT,
                                   const std::string & CASE_MESSAGE = "input should equal output") {
        testEachCaseChange(INPUT, SEPARATOR_STR, INPUT, CASE_MESSAGE);

        testEachCaseChange(INPUT, SEPARATOR_STR, INPUT, CASE_MESSAGE + ", as is");

        testEachCaseChange(
            misc::ToLowerCopy(INPUT),
            SEPARATOR_STR,
            misc::ToLowerCopy(INPUT),
            CASE_MESSAGE + ", with input lower case");

        testEachCaseChange(
            misc::ToUpperCopy(INPUT),
            SEPARATOR_STR,
            misc::ToUpperCopy(INPUT),
            CASE_MESSAGE + ", with input upper case");

        testEachCaseChange(INPUT, "", INPUT, CASE_MESSAGE + ", with empty separator");

        testEachCaseChange(
            misc::ToLowerCopy(INPUT),
            "",
            misc::ToLowerCopy(INPUT),
            CASE_MESSAGE + ", with input lower case" + " with empty separator");

        testEachCaseChange(
            misc::ToUpperCopy(INPUT),
            "",
            misc::ToUpperCopy(INPUT),
            CASE_MESSAGE + ", with input upper case" + " with empty separator");
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

    // in this test, all letters are separated by numbers so there should be no changes
    testAllCasesTheSame("A a1B A1b x1Y1z X1y1Z x1Y1Z X1yz a1B1c1D A1b1C1d A");

    test(
        "A aB Ab xYz XyZ xYZ Xyz aBcD AbCd A",
        SEPARATOR_STR,
        CaseChange::LowerToUpper,
        "A a_B Ab x_Yz Xy_Z x_YZ Xyz a_Bc_D Ab_Cd A",
        "complex, LowerToUpper");

    test(
        "A aB Ab xYz XyZ xYZ Xyz aBcD AbCd A",
        SEPARATOR_STR,
        CaseChange::UpperToLower,
        "A aB A_b xY_z X_yZ xYZ X_yz aB_cD A_bC_d A",
        "complex, UpperToLower");

    test(
        "A aB Ab xYz XyZ xYZ Xyz aBcD AbCd A",
        SEPARATOR_STR,
        CaseChange::Both,
        "A a_B A_b x_Y_z X_y_Z x_YZ X_yz a_B_c_D A_b_C_d A",
        "complex, Both");
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

        BOOST_CHECK(
            FindNumber(static_cast<std::size_t>(std::numeric_limits<int>::max()), "", ERROR_NUMBER)
            == ERROR_NUMBER);
    }

    // strings with no numbers tests
    {
        const std::string TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS {
            "The numbers are , , and , the one in this filename log-.txt, and finally a large "
            "one "
            "."
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
            "large one 9999999999999999999999999999999, and a more plausible large one "
            "123456789."
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
