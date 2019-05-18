// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-strings.cpp
//
#define BOOST_TEST_MODULE "strings"

#include <boost/test/unit_test.hpp>

#include "misc/nameof.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"
#include "test/util/common.hpp"
#include "test/util/game-engine-global-fixture.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <SFML/System/Clock.hpp>

using namespace heroespath;
using namespace heroespath::test;
using namespace heroespath::misc;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestWithOnlyLogAndConfig;
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

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

BOOST_AUTO_TEST_CASE(are_same_or_only_differs_by_case)
{
    BOOST_CHECK(AreSameOrOnlyDifferByCase('1', '1'));

    BOOST_CHECK(!AreSameOrOnlyDifferByCase('1', '2'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('1', 'a'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('1', 'A'));

    BOOST_CHECK(!AreSameOrOnlyDifferByCase('2', '1'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('a', '1'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('A', '1'));

    BOOST_CHECK(AreSameOrOnlyDifferByCase('a', 'a'));
    BOOST_CHECK(AreSameOrOnlyDifferByCase('A', 'A'));

    BOOST_CHECK(AreSameOrOnlyDifferByCase('x', 'x'));
    BOOST_CHECK(AreSameOrOnlyDifferByCase('X', 'X'));

    BOOST_CHECK(AreSameOrOnlyDifferByCase('a', 'A'));
    BOOST_CHECK(AreSameOrOnlyDifferByCase('A', 'a'));

    BOOST_CHECK(AreSameOrOnlyDifferByCase('x', 'X'));
    BOOST_CHECK(AreSameOrOnlyDifferByCase('X', 'x'));

    BOOST_CHECK(!AreSameOrOnlyDifferByCase('a', 'X'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('X', 'a'));

    BOOST_CHECK(!AreSameOrOnlyDifferByCase('A', 'X'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('X', 'A'));

    BOOST_CHECK(!AreSameOrOnlyDifferByCase('a', 'x'));
    BOOST_CHECK(!AreSameOrOnlyDifferByCase('x', 'a'));
}

BOOST_AUTO_TEST_CASE(constexpr_equal_and_are_equal_case_insensitive)
{
    auto testAreEqualCaseInsensitive = [](const auto & A, const auto & B, const bool EXPECTED) {
        {
            const auto ACTUAL
                = AreEqualCaseInsensitive(std::begin(A), std::end(A), std::begin(B), std::end(B));

            BOOST_CHECK_MESSAGE(
                (ACTUAL == EXPECTED),
                "AreEqualCaseInsensitive<"
                    << NAMEOF_VAR_TYPE(A) << ", " << NAMEOF_VAR_TYPE(B) << ">(A=\"" << Join(A)
                    << "\", B=\"" << Join(B) << "\", BIN_PRED=(X==Y), EXP=" << std::boolalpha
                    << EXPECTED << ")[ia,ia,ib,ib] but = " << ACTUAL << " instead.");
        }

        {
            const auto ACTUAL = AreEqualCaseInsensitive(std::begin(A), std::end(A), B);

            BOOST_CHECK_MESSAGE(
                (ACTUAL == EXPECTED),
                "AreEqualCaseInsensitive<"
                    << NAMEOF_VAR_TYPE(A) << ", " << NAMEOF_VAR_TYPE(B) << ">(A=\"" << Join(A)
                    << "\", B=\"" << Join(B) << "\", BIN_PRED=(X==Y), EXP=" << std::boolalpha
                    << EXPECTED << ")[ia,ia,b] but = " << ACTUAL << " instead.");
        }

        {
            const auto ACTUAL = AreEqualCaseInsensitive(A, B);

            BOOST_CHECK_MESSAGE(
                (ACTUAL == EXPECTED),
                "AreEqualCaseInsensitive<" << NAMEOF_VAR_TYPE(A) << ", " << NAMEOF_VAR_TYPE(B)
                                           << ">(A=\"" << Join(A) << "\", B=\"" << Join(B)
                                           << "\", BIN_PRED=(X==Y), EXP=" << std::boolalpha
                                           << EXPECTED << ")[a,b] but = " << ACTUAL << " instead.");
        }
    };

    auto testAreEqualCaseInsensitiveAllVariations
        = [&](const std::string & A, const std::string & B, const bool EXPECTED) {
              testAreEqualCaseInsensitive(A, B, EXPECTED);
              testAreEqualCaseInsensitive(std::string_view(A), std::string_view(B), EXPECTED);
              testAreEqualCaseInsensitive(A, std::string_view(B), EXPECTED);
              testAreEqualCaseInsensitive(std::string_view(A), B, EXPECTED);
          };

    auto testConstexprEqualAny = [](const auto & A, const auto & B, const bool EXPECTED) {
        {
            const auto ACTUAL = ConstexprEqual(
                std::begin(A),
                std::end(A),
                std::begin(B),
                std::end(B),
                [](const auto & X, const auto & Y) { return (X == Y); });

            BOOST_CHECK_MESSAGE(
                (ACTUAL == EXPECTED),
                "testConstexprEqualAny<" << NAMEOF_VAR_TYPE(A) << ", " << NAMEOF_VAR_TYPE(B)
                                         << ">(A=\"" << Join(A) << "\", B=\"" << Join(B)
                                         << "\", BIN_PRED=(X==Y), EXP=" << std::boolalpha
                                         << EXPECTED << ") but = " << ACTUAL << " instead.");
        }

        {
            const auto ACTUAL
                = ConstexprEqual(std::begin(A), std::end(A), std::begin(B), std::end(B));

            BOOST_CHECK_MESSAGE(
                (ACTUAL == EXPECTED),
                "testConstexprEqualAny<" << NAMEOF_VAR_TYPE(A) << ", " << NAMEOF_VAR_TYPE(B)
                                         << ">(A=\"" << Join(A) << "\", B=\"" << Join(B)
                                         << "\", BIN_PRED=default, EXP=" << std::boolalpha
                                         << EXPECTED << ") but = " << ACTUAL << " instead.");
        }
    };

    auto testConstexprEqualForStrings
        = [&](const std::string & A, const std::string & B, const bool EXPECTED) {
              testConstexprEqualAny(A, B, EXPECTED);
              testConstexprEqualAny(std::string_view(A), std::string_view(B), EXPECTED);
              testConstexprEqualAny(A, std::string_view(B), EXPECTED);
              testConstexprEqualAny(std::string_view(A), B, EXPECTED);
              //
              testAreEqualCaseInsensitiveAllVariations(A, B, EXPECTED);
          };

    testConstexprEqualForStrings("", "", true);
    testConstexprEqualForStrings(" ", " ", true);

    testConstexprEqualForStrings("", " ", false);
    testConstexprEqualForStrings(" ", "", false);

    const std::string STR_J { "\t!@#$%^&*12 345 6789]{}\\|;':\",.<_()_+-=[>\r\n?/`~"

    };

    const std::string STR_M { "This StrinG is just a stupid sentence OF characters." };

    testConstexprEqualForStrings(STR_J, STR_J, true);
    testConstexprEqualForStrings(STR_J, "", false);
    testConstexprEqualForStrings("", STR_J, false);
    testConstexprEqualForStrings(STR_J, " ", false);
    testConstexprEqualForStrings(" ", STR_J, false);

    testConstexprEqualForStrings(STR_M, STR_M, true);
    testConstexprEqualForStrings(STR_M, "", false);
    testConstexprEqualForStrings("", STR_M, false);
    testConstexprEqualForStrings(STR_M, " ", false);
    testConstexprEqualForStrings(" ", STR_M, false);

    testConstexprEqualForStrings(STR_J, STR_M, false);
    testConstexprEqualForStrings(STR_M, STR_J, false);

    const std::vector<int> CONTAINER_A = { 1, 2, 3, 4 };
    const std::array<int, 5> CONTAINER_B = { 1, 2, 3, 4, 5 };
    const std::list<int> CONTAINER_C = { 1, 2, 33, 4 };

    testConstexprEqualAny(CONTAINER_A, CONTAINER_A, true);
    testConstexprEqualAny(CONTAINER_B, CONTAINER_B, true);
    testConstexprEqualAny(CONTAINER_C, CONTAINER_C, true);

    testConstexprEqualAny(CONTAINER_A, CONTAINER_B, false);
    testConstexprEqualAny(CONTAINER_A, CONTAINER_C, false);
    testConstexprEqualAny(CONTAINER_B, CONTAINER_A, false);
    testConstexprEqualAny(CONTAINER_B, CONTAINER_C, false);
    testConstexprEqualAny(CONTAINER_C, CONTAINER_A, false);
    testConstexprEqualAny(CONTAINER_C, CONTAINER_B, false);

    const std::vector<int> CONTAINER_EMPTY_A;
    const std::list<int> CONTAINER_EMPTY_B;

    testConstexprEqualAny(CONTAINER_EMPTY_A, CONTAINER_EMPTY_A, true);
    testConstexprEqualAny(CONTAINER_EMPTY_A, CONTAINER_EMPTY_B, true);
    testConstexprEqualAny(CONTAINER_EMPTY_B, CONTAINER_EMPTY_A, true);
    testConstexprEqualAny(CONTAINER_EMPTY_B, CONTAINER_EMPTY_B, true);

    const std::string STR_U { "THIS STRING IS JUST A STUPID SENTENCE OF CHARACTERS." };
    const std::string STR_L { "this string is just a stupid sentence of characters." };

    testAreEqualCaseInsensitiveAllVariations(STR_J, STR_J, true);
    testAreEqualCaseInsensitiveAllVariations(STR_M, STR_M, true);
    testAreEqualCaseInsensitiveAllVariations(STR_U, STR_U, true);
    testAreEqualCaseInsensitiveAllVariations(STR_L, STR_L, true);

    testAreEqualCaseInsensitiveAllVariations(STR_M, STR_U, true);
    testAreEqualCaseInsensitiveAllVariations(STR_M, STR_L, true);
    testAreEqualCaseInsensitiveAllVariations(STR_U, STR_M, true);
    testAreEqualCaseInsensitiveAllVariations(STR_U, STR_L, true);
    testAreEqualCaseInsensitiveAllVariations(STR_L, STR_M, true);
    testAreEqualCaseInsensitiveAllVariations(STR_L, STR_U, true);

    testAreEqualCaseInsensitiveAllVariations(STR_J, STR_U, false);
    testAreEqualCaseInsensitiveAllVariations(STR_J, STR_L, false);
    testAreEqualCaseInsensitiveAllVariations(STR_J, STR_M, false);
    testAreEqualCaseInsensitiveAllVariations(STR_L, STR_J, false);
    testAreEqualCaseInsensitiveAllVariations(STR_M, STR_J, false);
    testAreEqualCaseInsensitiveAllVariations(STR_U, STR_J, false);
}

BOOST_AUTO_TEST_CASE(case_querries)
{
    // test all individual chars
    for (char ch(std::numeric_limits<char>::lowest());; ++ch)
    {
        if (IsAlpha(ch))
        {
            BOOST_CHECK(IsUpper(ch) != IsLower(ch));
            BOOST_CHECK(IsUpper(ToUpperCopy(ch)));
            BOOST_CHECK(IsLower(ToLowerCopy(ch)));
        }
        else
        {
            BOOST_CHECK(IsUpper(ch) == false);
            BOOST_CHECK(IsLower(ch) == false);
            BOOST_CHECK(IsUpper(ToUpperCopy(ch)) == false);
            BOOST_CHECK(IsLower(ToLowerCopy(ch)) == false);
        }

        if (std::numeric_limits<char>::max() == ch)
        {
            break;
        }
    }

    // test whole strings -empty
    const std::string EMPTY_STR { "" };

    BOOST_CHECK(ToUpperCopy(EMPTY_STR) == EMPTY_STR);
    BOOST_CHECK(ToLowerCopy(EMPTY_STR) == EMPTY_STR);

    // test whole strings -without letters
    const std::string NON_LETTER_STR {
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n \n\r"
    };

    BOOST_CHECK(ToUpperCopy(NON_LETTER_STR) == NON_LETTER_STR);
    BOOST_CHECK(ToLowerCopy(NON_LETTER_STR) == NON_LETTER_STR);

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

    BOOST_CHECK(ToUpperCopy(MIXED_CASE_TEST_STR) == UPPER_CASE_TEST_STR);
    BOOST_CHECK(ToUpperCopy(UPPER_CASE_TEST_STR) == UPPER_CASE_TEST_STR);
    BOOST_CHECK(ToUpperCopy(LOWER_CASE_TEST_STR) == UPPER_CASE_TEST_STR);

    BOOST_CHECK(ToLowerCopy(MIXED_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
    BOOST_CHECK(ToLowerCopy(UPPER_CASE_TEST_STR) == LOWER_CASE_TEST_STR);
    BOOST_CHECK(ToLowerCopy(LOWER_CASE_TEST_STR) == LOWER_CASE_TEST_STR);

    BOOST_CHECK(AreEqualCaseInsensitive("", ""));
    BOOST_CHECK(!AreEqualCaseInsensitive("", " "));
    BOOST_CHECK(!AreEqualCaseInsensitive(" ", ""));
    BOOST_CHECK(AreEqualCaseInsensitive(" ", " "));
    BOOST_CHECK(!AreEqualCaseInsensitive(NON_LETTER_STR, ""));
    BOOST_CHECK(!AreEqualCaseInsensitive(NON_LETTER_STR, " "));

    BOOST_CHECK(AreEqualCaseInsensitive(NON_LETTER_STR, NON_LETTER_STR));

    BOOST_CHECK(AreEqualCaseInsensitive(
        NON_LETTER_STR,
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n "
        "\n\r"));

    BOOST_CHECK(!AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, ""));
    BOOST_CHECK(!AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, " "));

    BOOST_CHECK(AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, MIXED_CASE_TEST_STR));
    BOOST_CHECK(AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, ToUpperCopy(MIXED_CASE_TEST_STR)));
    BOOST_CHECK(AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, ToLowerCopy(MIXED_CASE_TEST_STR)));

    BOOST_CHECK(AreEqualCaseInsensitive(UPPER_CASE_TEST_STR, UPPER_CASE_TEST_STR));
    BOOST_CHECK(AreEqualCaseInsensitive(UPPER_CASE_TEST_STR, ToUpperCopy(UPPER_CASE_TEST_STR)));
    BOOST_CHECK(AreEqualCaseInsensitive(UPPER_CASE_TEST_STR, ToLowerCopy(UPPER_CASE_TEST_STR)));

    BOOST_CHECK(AreEqualCaseInsensitive(LOWER_CASE_TEST_STR, LOWER_CASE_TEST_STR));
    BOOST_CHECK(AreEqualCaseInsensitive(LOWER_CASE_TEST_STR, ToUpperCopy(LOWER_CASE_TEST_STR)));
    BOOST_CHECK(AreEqualCaseInsensitive(LOWER_CASE_TEST_STR, ToLowerCopy(LOWER_CASE_TEST_STR)));

    BOOST_CHECK(AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, UPPER_CASE_TEST_STR));
    BOOST_CHECK(AreEqualCaseInsensitive(MIXED_CASE_TEST_STR, LOWER_CASE_TEST_STR));
    BOOST_CHECK(AreEqualCaseInsensitive(UPPER_CASE_TEST_STR, LOWER_CASE_TEST_STR));

    BOOST_CHECK(AreEqualCaseInsensitive("a-b", "A-B"));
    BOOST_CHECK(!AreEqualCaseInsensitive("a_b", "A-B"));
}

/*
 * only run this when checking speed of strings functions
  *
BOOST_AUTO_TEST_CASE(misc_strings__Case_SpeedTestsComparedToBoost)
{
    auto makeRandomChar = []() { return static_cast<char>(Random(32, 90)); };

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
            // RandomShuffle(strings);

            trash.clear();

            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(ToLowerCopy(STR_TO_TEST));
            }
            sf::Clock clock;
            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(ToLowerCopy(STR_TO_TEST));
            }
            result.boost_times.emplace_back(clock.getElapsedTime().asMilliseconds());

            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(ToLowerCopy(STR_TO_TEST));
            }
            clock.restart();
            for (const std::string & STR_TO_TEST : strings)
            {
                trash.emplace_back(ToLowerCopy(STR_TO_TEST));
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
            [&](const std::string & WHICH_RESULTS, const std::vector<float> & TIMES, float &
sum) { auto min { TIMES.at(0) }; auto max { 0.0f }; sum = 0.0f;

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

                const float STANDARD_DEVIATION { StandardDeviation(
                    TIMES, TIMES.size(), AVERAGE, false) };

                std::ostringstream ssReport;

                ssReport << "\n\t\t" << WHICH_RESULTS << "\tx" << TIMES.size() << "\tsum=" <<
sum
                         << "\t[" << min << ", " << AVERAGE << ", " << max << "] ("
                         << STANDARD_DEVIATION << ")";

                return ssReport.str();
            };

        float sum { 0.0f };

        const auto BOOST_TEST_STR { makeTestReportString("boost", TEST_RESULT.boost_times, sum)
}; const auto BOOST_TEST_SUM { sum }; boostTotalTimeMS += BOOST_TEST_SUM;

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
*/

BOOST_AUTO_TEST_CASE(camel_to)
{
    // test empty cases
    BOOST_CHECK(CamelTo("", '_') == "");
    BOOST_CHECK(CamelTo("", "") == "");

    // test non-letter cases
    const std::string NON_LETTER_STR {
        "\t \r\n 0 12 345 6789 \t \r [ ] - _ !@#$%^&*(())_+-=[]{}\\|;':\",.<> / ? `~  \n \n\r"
    };

    const char SEPARATOR_CHAR { '_' };
    const std::string SEPARATOR_STR { std::string(1, SEPARATOR_CHAR) };

    BOOST_CHECK(CamelTo(NON_LETTER_STR, SEPARATOR_CHAR) == NON_LETTER_STR);
    BOOST_CHECK(CamelTo(NON_LETTER_STR, "") == NON_LETTER_STR);
    BOOST_CHECK(CamelTo(NON_LETTER_STR, SEPARATOR_STR) == NON_LETTER_STR);

    // test valid cases

    auto test = [](const std::string & INPUT,
                   const std::string & SEPARATOR,
                   const CaseChange CASE_CHANGE,
                   const std::string & EXPECTED_OUTPUT,
                   const std::string & CASE_MESSAGE = "normal") {
        //
        auto makeErrorMessage = [&](const std::string & ACTUAL_OUTPUT) {
            const auto CASE_CHANGE_STR = [CASE_CHANGE]() {
                if (CASE_CHANGE == CaseChange::Both)
                {
                    return "Both";
                }
                else if (CASE_CHANGE == CaseChange::UpperToLower)
                {
                    return "UpperToLower";
                }
                else
                {
                    return "LowerToUpper";
                }
            }();

            return "(" + Quoted(CASE_MESSAGE) + " case)  CamelTo(\"" + INPUT
                + "\", sep=" + Quoted(SEPARATOR) + ", case_change=" + CASE_CHANGE_STR
                + ")!=" + Quoted(EXPECTED_OUTPUT) + "\"  actual=" + Quoted(ACTUAL_OUTPUT);
        };

        const auto ACTUAL_OUTPUT { CamelTo(INPUT, SEPARATOR, CASE_CHANGE) };
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
            ToLowerCopy(INPUT),
            SEPARATOR_STR,
            ToLowerCopy(INPUT),
            CASE_MESSAGE + ", with input lower case");

        testEachCaseChange(
            ToUpperCopy(INPUT),
            SEPARATOR_STR,
            ToUpperCopy(INPUT),
            CASE_MESSAGE + ", with input upper case");

        testEachCaseChange(INPUT, "", INPUT, CASE_MESSAGE + ", with empty separator");

        testEachCaseChange(
            ToLowerCopy(INPUT),
            "",
            ToLowerCopy(INPUT),
            CASE_MESSAGE + ", with input lower case" + " with empty separator");

        testEachCaseChange(
            ToUpperCopy(INPUT),
            "",
            ToUpperCopy(INPUT),
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

BOOST_AUTO_TEST_CASE(to_string_and_to_number)
{
    const int ERROR_NUMBER_INT { -321 };

    const std::vector<NumStr<int>> NUM_STRS = { { 0, "0" },
                                                { 1, "1" },
                                                { -1, "-1" },
                                                { 123456789, "123456789" },
                                                { -123456789, "-123456789" } };

    for (const auto & NUM_STR : NUM_STRS)
    {
        auto makeErrorStr = [ERROR_NUMBER_INT](
                                const auto NUM_ORIG,
                                const std::string & STR_ORIG,
                                const std::string & MESSAGE,
                                const auto LEFT,
                                const auto RIGHT) {
            std::ostringstream sss;
            sss << "NUM_ORIG=\"" << NUM_ORIG << "\", STR_ORIG=\"" << STR_ORIG
                << "\", ERROR_INT=" << ERROR_NUMBER_INT << ", \"" << MESSAGE << "\""
                << " : (" << LEFT << "!=" << RIGHT << ")";

            return sss.str();
        };

        const auto NUM_ORIG(NUM_STR.num);
        const auto STR_ORIG(NUM_STR.str);
        const auto TO_STRING_FROM_NUM(ToString(NUM_STR.num));
        const auto TO_NUMBER_FROM_STR(ToNumberOr(NUM_STR.str, ERROR_NUMBER_INT));

        BOOST_CHECK_MESSAGE(
            (TO_STRING_FROM_NUM == STR_ORIG),
            makeErrorStr(
                NUM_ORIG, STR_ORIG, "(to_str_from_num==str_orig)", TO_STRING_FROM_NUM, STR_ORIG));

        BOOST_CHECK_MESSAGE(
            (TO_NUMBER_FROM_STR == NUM_ORIG),
            makeErrorStr(
                NUM_ORIG, STR_ORIG, "(to_num_from_str==num_orig)", TO_NUMBER_FROM_STR, NUM_ORIG));

        BOOST_CHECK_MESSAGE(
            (ToString(TO_NUMBER_FROM_STR) == STR_ORIG),
            makeErrorStr(
                NUM_ORIG,
                STR_ORIG,
                "(to_str(to_num_from_str)==str_orig)",
                ToString(TO_NUMBER_FROM_STR),
                STR_ORIG));

        BOOST_CHECK_MESSAGE(
            (ToNumberOr(TO_STRING_FROM_NUM, ERROR_NUMBER_INT) == NUM_ORIG),
            makeErrorStr(
                NUM_ORIG,
                STR_ORIG,
                "(to_num(to_str_from_num)==num_orig)",
                ToNumberOr(TO_STRING_FROM_NUM, ERROR_NUMBER_INT),
                NUM_ORIG));
    }

    const float ERROR_NUMBER_FLOAT { -321.0 };

    const std::vector<NumStr<float>> FLOAT_STRS
        = { { 0, "0" },      { 1, "1" },      { -1.0f, "-1" }, { 0.0f, "0" },    { 1.0f, "1" },
            { -1.0f, "-1" }, { 0.1f, "0.1" }, { 1.1f, "1.1" }, { -1.1f, "-1.1" } };

    for (const auto & NUM_STR : FLOAT_STRS)
    {
        BOOST_CHECK_EQUAL(ToString(NUM_STR.num), NUM_STR.str);

        BOOST_CHECK_MESSAGE(
            IsRealClose(ToNumberOr(NUM_STR.str, ERROR_NUMBER_FLOAT), NUM_STR.num),
            "ToNumberOr(\"" << NUM_STR.str << "\", err_def=\"" << ERROR_NUMBER_FLOAT
                            << "\") = " << ToNumberOr(NUM_STR.str, ERROR_NUMBER_FLOAT)
                            << " != " << NUM_STR.num);

        BOOST_CHECK_EQUAL(ToString(ToNumberOr(NUM_STR.str, ERROR_NUMBER_FLOAT)), NUM_STR.str);

        BOOST_CHECK(
            IsRealClose(ToNumberOr(ToString(NUM_STR.num), ERROR_NUMBER_FLOAT), NUM_STR.num));
    }

    BOOST_CHECK_EQUAL(ToString(true), "true");
    BOOST_CHECK_EQUAL(ToString(false), "false");
    BOOST_CHECK_EQUAL(ToString(""), "");
    BOOST_CHECK_EQUAL(ToString("abc 123"), "abc 123");

    std::ostringstream ss;
    ss << "abc 123";
    BOOST_CHECK(ToString(ss.str()) == "abc 123");

    BOOST_CHECK_EQUAL(ToNumberOr("", ERROR_NUMBER_INT), ERROR_NUMBER_INT);
    BOOST_CHECK_EQUAL(ToNumberOr("a", ERROR_NUMBER_INT), ERROR_NUMBER_INT);
    BOOST_CHECK_EQUAL(ToNumberOr("abc", ERROR_NUMBER_INT), ERROR_NUMBER_INT);

    BOOST_CHECK(IsRealClose(ToNumberOr("", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));
    BOOST_CHECK(IsRealClose(ToNumberOr("a", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));
    BOOST_CHECK(IsRealClose(ToNumberOr("abc", ERROR_NUMBER_FLOAT), ERROR_NUMBER_FLOAT));

    BOOST_CHECK_EQUAL(ToNumberOr("-0", ERROR_NUMBER_INT), 0);
    BOOST_CHECK_EQUAL(ToNumberOr("0000", ERROR_NUMBER_INT), 0);
    BOOST_CHECK_EQUAL(ToNumberOr("00001", ERROR_NUMBER_INT), 1);
    BOOST_CHECK_EQUAL(ToNumberOr("1 2", ERROR_NUMBER_INT), ERROR_NUMBER_INT);

    BOOST_CHECK_EQUAL(ToNumberOr("1.9", ERROR_NUMBER_INT), 1);

    auto testToNumber = [&](const std::string & STR, const auto EXPECTED) {
        const auto VALUE_IF_ERROR = static_cast<decltype(EXPECTED)>(696969.696969);

        BOOST_CHECK_MESSAGE(
            !IsRealClose(EXPECTED, VALUE_IF_ERROR),
            "Tried to test misc::ToNumber() but the default value to use on error was equal to "
            "the "
            "expected!  *** SANITY CHECK FAIL");

        const auto ACTUAL(ToNumberOr(STR, VALUE_IF_ERROR));

        std::ostringstream sss;
        sss << "STR=\"" << STR << "\", VALUE_IF_ERROR=" << VALUE_IF_ERROR
            << ", EXPECTED=" << EXPECTED << ", ACTUAL=" << ACTUAL << "IsRealClose<"
            << NAMEOF_TYPE(decltype(VALUE_IF_ERROR)) << ">(\"" << ACTUAL << "\", \"" << EXPECTED
            << "\") != \"" << EXPECTED << "\"";

        BOOST_CHECK_MESSAGE(IsRealClose(ACTUAL, EXPECTED), sss.str());
    };

    auto testToNumberWillFailWith = [&](const std::string & STR, const auto ERROR_VALUE) {
        BOOST_CHECK_EQUAL(ToNumberOr(STR, ERROR_VALUE), ERROR_VALUE);
    };

    auto testToNumberWillFail
        = [&](const std::string & STR) { testToNumberWillFailWith(STR, int(696969)); };

    testToNumber("999999", 999999);

    testToNumber("2147483647", std::int32_t(2147483647));
    testToNumber("2147483647.", std::int32_t(2147483647));
    testToNumber("2147483647.0", std::int32_t(2147483647));
    testToNumber("2147483647.0000", std::int32_t(2147483647));
    testToNumber("2147483647.999", std::int32_t(2147483647));

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

    testToNumber("-2147483648", std::int32_t(-2147483648));
    testToNumber("-2147483648.", std::int32_t(-2147483648));
    testToNumber("-2147483648.0", std::int32_t(-2147483648));
    testToNumber("-2147483648.0000", std::int32_t(-2147483648));
    testToNumber("-2147483648.999", std::int32_t(-2147483648));

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
    // sign mismatches
    testToNumberWillFailWith("-1", 1u);
    testToNumberWillFailWith("-10", 10u);
    testToNumberWillFailWith("-0", 0u);
    testToNumber("-0.0", -0.0);
    testToNumber("-0", -0.0);

    // overflows
    short s(0);
    BOOST_CHECK(!ToNumber("999999", s));
    BOOST_CHECK(!ToNumber("-999999", s));

    // unusual
    testToNumber(" 0", 0);
    testToNumber("0 ", 0);
    testToNumber(" 0 ", 0);

    testToNumber(" 00", 0);
    testToNumber("00 ", 0);
    testToNumber(" 00 ", 0);

    testToNumber(" 000", 0);
    testToNumber("000 ", 0);
    testToNumber(" 000 ", 0);

    testToNumber(" 1", 1);
    testToNumber("1 ", 1);
    testToNumber(" 1 ", 1);

    testToNumber(" 123", 123);
    testToNumber("123 ", 123);
    testToNumber(" 123 ", 123);

    testToNumber(" 1.23", 1.23);
    testToNumber("3.21 ", 3.21);
    testToNumber(" 9.87 ", 9.87);

    testToNumber("  1", 1);
    testToNumber("1  ", 1);
    testToNumber("  1  ", 1);

    testToNumber("   1", 1);
    testToNumber("1   ", 1);
    testToNumber("   1   ", 1);

    testToNumber("\t0", 0);
    testToNumber("0\t", 0);
    testToNumber("\t0\t", 0);

    testToNumber("00", 0);
    testToNumber("000", 0);
    testToNumber("0000", 0);

    testToNumber("0001", 1);
    testToNumber("0010", 10);
    testToNumber("0100", 100);
    testToNumber("1000", 1000);
    testToNumber("01000", 1000);
    testToNumber("001000", 1000);
    testToNumber("0001000", 1000);

    testToNumber("000123", 123);
    testToNumber("001230", 1230);
    testToNumber("012300", 12300);
    testToNumber("123000", 123000);
    testToNumber("0123000", 123000);
    testToNumber("00123000", 123000);
    testToNumber("000123000", 123000);

    testToNumber("000123.", 123.0);
    testToNumber("001230.", 1230.0);
    testToNumber("012300.", 12300.0);
    testToNumber("123000.", 123000.0);
    testToNumber("0123000.", 123000.0);
    testToNumber("00123000.", 123000.0);
    testToNumber("000123000.", 123000.0);

    testToNumber("000123.0", 123.0);
    testToNumber("001230.0", 1230.0);
    testToNumber("012300.0", 12300.0);
    testToNumber("123000.0", 123000.0);
    testToNumber("012300.0", 12300.0);
    testToNumber("00123000.0", 123000.0);
    testToNumber("000123000.0", 123000.0);

    testToNumber("0.000123", 0.000123);
    testToNumber("0.001230", 0.00123);
    testToNumber("0.012300", 0.0123);
    testToNumber("0.123000", 0.123);
    testToNumber("0.0123000", 0.0123);
    testToNumber("0.00123000", 0.00123);
    testToNumber("0.000123000", 0.000123);

    testToNumber("000123.000123", 123.000123);
    testToNumber("001230.001230", 1230.00123);
    testToNumber("012300.012300", 12300.0123);
    testToNumber("123000.123000", 123000.123);
    testToNumber("0123000.0123000", 123000.0123);
    testToNumber("00123000.00123000", 123000.00123);
    testToNumber("000123000.000123000", 123000.000123);

    testToNumber("000.", 0.0);
    testToNumber("00.0", 0.0);
    testToNumber("0.00", 0.0);
    testToNumber(".000", 0.0);
    testToNumber("0.000", 0.0);
    testToNumber("00.000", 0.0);
    testToNumber("000.000", 0.0);

    testToNumber("100.", 100.0);
    testToNumber("100.0", 100.0);
    testToNumber("10.00", 10.0);
    testToNumber("1.000", 1.0);
    testToNumber("10.000", 10.0);
    testToNumber("100.000", 100.0);
    testToNumber("1000.000", 1000.0);

    testToNumber(".0", 0);
    testToNumber("0.", 0);
    testToNumber(".0123", 0.0123);
    testToNumber("1230.", 1230);

    testToNumber("+.0", 0);
    testToNumber("+0.", 0);
    testToNumber("+.0123", 0.0123);
    testToNumber("+1230.", 1230);

    testToNumber("-.0", 0);
    testToNumber("-0.", 0);
    testToNumber("-.0123", -0.0123);
    testToNumber("-1230.", -1230);

    // messy leading plus sign cases (some should work)
    testToNumber(" +\t++ 0", 0);
    testToNumber(" +\t++ -0", 0);

    testToNumberWillFail(" +\t++ --0");

    testToNumber(" +\t++0", 0);
    testToNumber(" +\t++-0", 0);

    testToNumberWillFail(" +\t++--0");

    testToNumber(" +\t++ .0", 0.0);
    testToNumber(" +\t++ -.0", -0.0);

    testToNumberWillFail(" +\t++ --.0");

    testToNumber(" +\t++.0", 0.0);
    testToNumber(" +\t++-.0", -0.0);
    testToNumberWillFail(" +\t++--.0");

    // messy leading plus and minus sign cases (all should fail)
    testToNumberWillFail(" +\t+-+ 0");
    testToNumberWillFail(" +\t+-+ -0");

    testToNumberWillFail(" +\t+-+ --0");

    testToNumberWillFail(" +\t+-+0");
    testToNumberWillFail(" +\t+-+-0");

    testToNumberWillFail(" +\t+-+--0");

    testToNumberWillFail(" +\t+-+ .0");
    testToNumberWillFail(" +\t+-+ -.0");

    testToNumberWillFail(" +\t+-+ --.0");

    testToNumberWillFail(" +\t+-+.0");
    testToNumberWillFail(" +\t+-+-.0");
    testToNumberWillFail(" +\t+-+--.0");

    //

    // invalid
    testToNumberWillFail("");
    testToNumberWillFail(" ");
    testToNumberWillFail("  ");
    testToNumberWillFail("   ");
    testToNumberWillFail("\t");
    testToNumberWillFail("\t\t");
    testToNumberWillFail(" \t  \t ");

    testToNumberWillFail("a");
    testToNumberWillFail("ab");
    testToNumberWillFail("abc");

    testToNumberWillFail(" a\tb  c  ");

    testToNumberWillFail("a1");
    testToNumberWillFail("a12");
    testToNumberWillFail("a123");

    testToNumberWillFail("1a");
    testToNumberWillFail("12a");
    testToNumberWillFail("123a");
    testToNumberWillFail("1a23");
    testToNumberWillFail("12a3");
    testToNumberWillFail("1a23");
    testToNumberWillFail("1aa23");
    testToNumberWillFail("12a3");
    testToNumberWillFail("12aa3");

    testToNumberWillFail("1+");
    testToNumberWillFail("12-");
    testToNumberWillFail("123+");
    testToNumberWillFail("1+23");
    testToNumberWillFail("12-3");
    testToNumberWillFail("1-+23");
    testToNumberWillFail("1+-23");
    testToNumberWillFail("12--3");
    testToNumberWillFail("12++3");

    testToNumberWillFail(".");
    testToNumberWillFail(" .");
    testToNumberWillFail(". ");
    testToNumberWillFail(" . ");

    testToNumberWillFail("..");
    testToNumberWillFail(" ..");
    testToNumberWillFail(".. ");
    testToNumberWillFail(" .. ");

    testToNumberWillFail("-");
    testToNumberWillFail(" -");
    testToNumberWillFail("- ");
    testToNumberWillFail(" - ");

    testToNumberWillFail("--");
    testToNumberWillFail(" --");
    testToNumberWillFail("-- ");
    testToNumberWillFail(" -- ");

    testToNumberWillFail("+");
    testToNumberWillFail(" +");
    testToNumberWillFail("+ ");
    testToNumberWillFail(" + ");

    testToNumberWillFail("++");
    testToNumberWillFail(" ++");
    testToNumberWillFail("++ ");
    testToNumberWillFail(" ++ ");

    testToNumberWillFail("-.");
    testToNumberWillFail(".-");
    testToNumberWillFail("-.-");
    testToNumberWillFail(".-.");

    testToNumberWillFail("-..");
    testToNumberWillFail("..-");
    testToNumberWillFail("-..-");
    testToNumberWillFail("..-..");

    testToNumberWillFail("--.");
    testToNumberWillFail(".--");
    testToNumberWillFail("--.--");
    testToNumberWillFail(".--.");

    testToNumberWillFail("+.");
    testToNumberWillFail(".+");
    testToNumberWillFail("+.+");
    testToNumberWillFail(".+.");

    testToNumberWillFail("+..");
    testToNumberWillFail("..+");
    testToNumberWillFail("+..+");
    testToNumberWillFail("..+..");

    testToNumberWillFail("++.");
    testToNumberWillFail(".++");
    testToNumberWillFail("++.++");
    testToNumberWillFail(".++.");

    testToNumberWillFail("++---+-++-+-");
    testToNumberWillFail("+2+345662-357-3467-3674+-267+358+649-4i+-");
    testToNumberWillFail("+ + - - -+ - + +-+-");
    testToNumberWillFail("+ 236+ - -327 37-38+489 49- 3+ 839+-3+3-3");

    testToNumberWillFail("-");
    testToNumberWillFail("-.");
    testToNumberWillFail("0-");
    testToNumberWillFail("1--");
    testToNumberWillFail("0--0");
    testToNumberWillFail("1--0");

    testToNumberWillFail("0 0");
    testToNumberWillFail("1 0");
    testToNumberWillFail("0 1");

    testToNumberWillFail(" 0 0");
    testToNumberWillFail("1 0 ");
    testToNumberWillFail(" 0 1 ");

    testToNumberWillFail("0\t0");
    testToNumberWillFail("1\t0");
    testToNumberWillFail("0\t1");

    testToNumberWillFail("0..0");
    testToNumberWillFail("..0");
    testToNumberWillFail("0..");

    testToNumberWillFail("0...0");
    testToNumberWillFail("...0");
    testToNumberWillFail("0...");

    testToNumberWillFail("-0..0");
    testToNumberWillFail("-..0");
    testToNumberWillFail("-0..");

    testToNumberWillFail("+0..0");
    testToNumberWillFail("+..0");
    testToNumberWillFail("+0..");

    testToNumberWillFail("000..");
    testToNumberWillFail("00..0");
    testToNumberWillFail("0..00");
    testToNumberWillFail("..000");
    testToNumberWillFail("0..000");
    testToNumberWillFail("00..000");
    testToNumberWillFail("000..000");

    testToNumberWillFail("123..");
    testToNumberWillFail("12..3");
    testToNumberWillFail("1..23");
    testToNumberWillFail("..321");
    testToNumberWillFail("1..234");
    testToNumberWillFail("12..345");
    testToNumberWillFail("123..456");

    testToNumberWillFail(".000.000");
    testToNumberWillFail("0.00.000");
    testToNumberWillFail("00.0.000");
    testToNumberWillFail("000.0.00");
    testToNumberWillFail("000.00.0");
    testToNumberWillFail("000.000.");

    testToNumberWillFail(".123.456");
    testToNumberWillFail("1.23.456");
    testToNumberWillFail("12.3.456");
    testToNumberWillFail("123.4.56");
    testToNumberWillFail("123.45.6");
    testToNumberWillFail("123.456.");

    testToNumberWillFail("1.23.4..567...8.9.....0.");

    testToNumberWillFail("cat");
    testToNumberWillFail("cat and dog");
    testToNumberWillFail("This looks like a typical sentence.");
    testToNumberWillFail("../../src/dir/thing/text.txt");
    testToNumberWillFail("C:\\Program Files\\SomeApp\\SomeApp.exe");
}

BOOST_AUTO_TEST_CASE(number_to_string_with_ordinal_suffix)
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

BOOST_AUTO_TEST_CASE(contains_any_of)
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

BOOST_AUTO_TEST_CASE(find_number)
{
    // const int ERROR_NUMBER { -1 };

    // test empty input cases
    {
        BOOST_CHECK(FindNumber(-1, "") < 0);
        BOOST_CHECK(FindNumber(0, "") < 0);
        BOOST_CHECK(FindNumber(1, "") < 0);
        BOOST_CHECK(FindNumber(2, "") < 0);
        BOOST_CHECK(FindNumber(3, "") < 0);
        BOOST_CHECK(FindNumber(10, "") < 0);
        BOOST_CHECK(FindNumber(std::numeric_limits<int>::max(), "") < 0);
        BOOST_CHECK(FindNumber(std::numeric_limits<int>::lowest(), "") < 0);
    }

    // strings with no numbers tests
    {
        const std::string TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS {
            "The numbers are , , and , the one in this filename log-.txt, and finally a large "
            "one "
            "."
        };

        BOOST_CHECK_MESSAGE(
            FindNumber(-1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0,
            "FindNumber(-1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) = " << FindNumber(
                -1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS));

        BOOST_CHECK_MESSAGE(
            FindNumber(0, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0,
            "FindNumber(0, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS)  = " << FindNumber(
                0, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS));

        BOOST_CHECK_MESSAGE(
            FindNumber(1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0,
            "FindNumber(1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS)  = " << FindNumber(
                1, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS));

        BOOST_CHECK_MESSAGE(
            FindNumber(2, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0,
            "FindNumber(2, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS)  = " << FindNumber(
                2, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS));

        BOOST_CHECK_MESSAGE(
            FindNumber(3, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0,
            "FindNumber(3, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS)  = " << FindNumber(
                3, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS));

        BOOST_CHECK_MESSAGE(
            FindNumber(10, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0,
            "FindNumber(10, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) = " << FindNumber(
                10, TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS));

        BOOST_CHECK(
            FindNumber(std::numeric_limits<int>::max(), TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS) < 0);

        BOOST_CHECK(
            FindNumber(std::numeric_limits<int>::lowest(), TEST_STRING_TO_SEARCH_WITHOUT_NUMBERS)
            < 0);
    }

    // test cases where strings are there to be found
    {
        const std::string TEST_STRING_TO_SEARCH_WITH_NUMBERS {
            "The numbers are 0.1, and inside the filename log-234.txt, an impossibly "
            "large one 9999999999999999999999999999999, and a more plausible large one "
            "123456789."
        };

        BOOST_CHECK_EQUAL(FindNumber(0, TEST_STRING_TO_SEARCH_WITH_NUMBERS), 0);
        BOOST_CHECK_EQUAL(FindNumber(1, TEST_STRING_TO_SEARCH_WITH_NUMBERS), 1);
        BOOST_CHECK_EQUAL(FindNumber(2, TEST_STRING_TO_SEARCH_WITH_NUMBERS), 234);
        BOOST_CHECK(FindNumber(3, TEST_STRING_TO_SEARCH_WITH_NUMBERS) < 0);
        BOOST_CHECK_EQUAL(FindNumber(4, TEST_STRING_TO_SEARCH_WITH_NUMBERS), 123456789);
        BOOST_CHECK(FindNumber(5, TEST_STRING_TO_SEARCH_WITH_NUMBERS) < 0);
        BOOST_CHECK(FindNumber(6, TEST_STRING_TO_SEARCH_WITH_NUMBERS) < 0);
        BOOST_CHECK(FindNumber(7, TEST_STRING_TO_SEARCH_WITH_NUMBERS) < 0);
        BOOST_CHECK(FindNumber(8, TEST_STRING_TO_SEARCH_WITH_NUMBERS) < 0);

        BOOST_CHECK(
            FindNumber(std::numeric_limits<int>::max(), TEST_STRING_TO_SEARCH_WITH_NUMBERS) < 0);

        // test helper functions
        BOOST_CHECK_EQUAL(FindNumberFirst(TEST_STRING_TO_SEARCH_WITH_NUMBERS), 0);

        BOOST_CHECK_EQUAL(FindNumberLast(TEST_STRING_TO_SEARCH_WITH_NUMBERS), 123456789);
    }
}

BOOST_AUTO_TEST_CASE(trim)
{
    BOOST_CHECK(TrimWhitespaceCopy("") == "");
    BOOST_CHECK(TrimWhitespaceCopy(" ") == "");
    BOOST_CHECK(TrimWhitespaceCopy("\t") == "");
    BOOST_CHECK(TrimWhitespaceCopy("\r") == "");
    BOOST_CHECK(TrimWhitespaceCopy("\n") == "");
    BOOST_CHECK(TrimWhitespaceCopy("  ") == "");
    BOOST_CHECK(TrimWhitespaceCopy("   ") == "");
    BOOST_CHECK(TrimWhitespaceCopy("     ") == "");
    BOOST_CHECK(TrimWhitespaceCopy(" \r \n\n\t\t \t \n\r ") == "");
    BOOST_CHECK(TrimWhitespaceCopy(" \r \n\n\ta\t \t \n\r ") == "a");
    BOOST_CHECK(TrimWhitespaceCopy(" \r \n\n\ta\tb \t \n\r ") == "a\tb");
    BOOST_CHECK(TrimWhitespaceCopy("a \r \n\n\ta\t \t \n\r b") == "a \r \n\n\ta\t \t \n\r b");

    BOOST_CHECK(TrimNonDisplayableCopy("") == "");
    BOOST_CHECK(TrimNonDisplayableCopy(" ") == " ");
    BOOST_CHECK(TrimNonDisplayableCopy("\t") == "\t");
    BOOST_CHECK(TrimNonDisplayableCopy("\r") == "\r");
    BOOST_CHECK(TrimNonDisplayableCopy("\n") == "\n");
    BOOST_CHECK(TrimNonDisplayableCopy("  ") == "  ");
    BOOST_CHECK(TrimNonDisplayableCopy("   ") == "   ");
    BOOST_CHECK(TrimNonDisplayableCopy("     ") == "     ");
    BOOST_CHECK(TrimNonDisplayableCopy(" \r \n\n\t\t \t \n\r ") == " \r \n\n\t\t \t \n\r ");
    BOOST_CHECK(TrimNonDisplayableCopy(" \r \n\n\ta\t \t \n\r ") == " \r \n\n\ta\t \t \n\r ");
    BOOST_CHECK(TrimNonDisplayableCopy(" \r \n\n\ta\tb \t \n\r ") == " \r \n\n\ta\tb \t \n\r ");
    BOOST_CHECK(TrimNonDisplayableCopy("a \r \n\n\ta\t \t \n\r b") == "a \r \n\n\ta\t \t \n\r b");

    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(" ") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("\t") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("\r") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("\n") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("  ") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("   ") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy("     ") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(" \r \n\n\t\t \t \n\r ") == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(" \r \n\n\ta\t \t \n\r ") == "a");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(" \r \n\n\ta\tb \t \n\r ") == "a\tb");

    BOOST_CHECK(
        TrimWhitespaceAndNonDisplayableCopy("a \r \n\n\ta\t \t \n\r b")
        == "a \r \n\n\ta\t \t \n\r b");

    std::string str;
    BOOST_CHECK(TrimNonDisplayableCopy(str) == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == "");
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);

    str += static_cast<char>(0);
    BOOST_CHECK(TrimNonDisplayableCopy(str) == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == "");
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);

    str += static_cast<char>(-1);
    str += static_cast<char>(127);
    str += static_cast<char>(31);
    str += static_cast<char>(15);
    str += static_cast<char>(0);
    BOOST_CHECK(TrimNonDisplayableCopy(str) == "");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == "");
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);

    str += 'a';
    BOOST_CHECK(TrimNonDisplayableCopy(str) == "a");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == "a");
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);

    str.pop_back();
    str.insert(str.begin(), 'a');
    BOOST_CHECK(TrimNonDisplayableCopy(str) == "a");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == "a");
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);

    str += static_cast<char>(-1);
    str += static_cast<char>(127);
    str += static_cast<char>(31);
    str += static_cast<char>(15);
    str += static_cast<char>(0);
    BOOST_CHECK(TrimNonDisplayableCopy(str) == "a");
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == "a");
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);

    str += 'b';
    BOOST_CHECK(TrimNonDisplayableCopy(str) == str);
    BOOST_CHECK(TrimWhitespaceAndNonDisplayableCopy(str) == str);
    BOOST_CHECK(TrimWhitespaceCopy(str) == str);
}

BOOST_AUTO_TEST_CASE(join_when_empty)
{
    const std::vector<int> EMPTY;
    for (const auto VALUE : smallValuesWorthTestingInt)
    {
        const auto SIZET_VALUE { static_cast<std::size_t>(VALUE) };

        const auto EMPTY_RESULT_1 { Join(EMPTY, JoinHow(JoinOpt::Nothing, SIZET_VALUE)) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_1 == ""),
            "Join(empty, " << VALUE << ") result=\"" << EMPTY_RESULT_1 << "\"");

        const auto EMPTY_RESULT_2 { Join(EMPTY, JoinHow(JoinOpt::Wrap, SIZET_VALUE)) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_2 == ""),
            "Join(empty, " << VALUE << ", Wrap) result=\"" << EMPTY_RESULT_2 << "\"");

        const auto EMPTY_RESULT_3 { Join(EMPTY, JoinHow(JoinOpt::And, SIZET_VALUE)) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_3 == ""),
            "Join(empty, " << VALUE << ", And) result=\"" << EMPTY_RESULT_3 << "\"");

        const auto EMPTY_RESULT_4 { Join(EMPTY, JoinHow(JoinOpt::Ellipsis, SIZET_VALUE)) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_4 == ""),
            "Join(empty, " << VALUE << ", Ellipsis) result=\"" << EMPTY_RESULT_4 << "\"");

        const auto EMPTY_RESULT_5 { Join(
            EMPTY, JoinHow((JoinOpt::Wrap | JoinOpt::And | JoinOpt::Ellipsis), SIZET_VALUE)) };

        BOOST_CHECK_MESSAGE(
            (EMPTY_RESULT_5 == ""),
            "Join(empty, " << VALUE << ", Wrap/And/Ellipsis) result=\"" << EMPTY_RESULT_5 << "\"");
    }
}

#define M_TEST_JOIN(vec, join_options, max_count, expected_str)                                    \
    BOOST_CHECK_MESSAGE(                                                                           \
        (Join(vec, JoinHow(join_options, max_count, ", ")) == expected_str),                       \
        "MACRO_TEST_JOIN(" #vec ", " #join_options ", max_count=" #max_count                       \
        ") expected=\"" #expected_str "\", actual=\""                                              \
            << Join(vec, JoinHow(join_options, max_count, ", ")) << "\"");

BOOST_AUTO_TEST_CASE(join)
{
    const std::vector<std::string> NUM_STR_VEC { "1", "2", "3" };

    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Nothing, 0, "1, 2, 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Nothing, 1, "1");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Nothing, 2, "1, 2");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Nothing, 3, "1, 2, 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Nothing, 4, "1, 2, 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Nothing, 5, "1, 2, 3");

    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Wrap, 0, "(1, 2, 3)");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Wrap, 1, "(1)");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Wrap, 2, "(1, 2)");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Wrap, 3, "(1, 2, 3)");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Wrap, 4, "(1, 2, 3)");

    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Ellipsis, 0, "1, 2, 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Ellipsis, 1, "1...");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Ellipsis, 2, "1, 2...");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Ellipsis, 3, "1, 2, 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Ellipsis, 4, "1, 2, 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::Ellipsis, 5, "1, 2, 3");

    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::And, 0, "1, 2, and 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::And, 1, "1");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::And, 2, "1, 2");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::And, 3, "1, 2, and 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::And, 4, "1, 2, and 3");
    M_TEST_JOIN(NUM_STR_VEC, JoinOpt::And, 5, "1, 2, and 3");
}
