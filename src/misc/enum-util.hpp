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
#ifndef HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
#define HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
//
// enum-util.hpp
//
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vector-map.hpp"

#include <boost/type_index.hpp>

#include <sstream>

namespace heroespath
{
namespace misc
{
    namespace enum_util
    {

        static inline unsigned CalculateLastValidValueOfFlagEnum(const unsigned HIGHEST_BIT_SET)
        {
            unsigned finalValue{ 0 };
            unsigned shiftingValue{ 1 };
            while (shiftingValue <= HIGHEST_BIT_SET)
            {
                finalValue |= shiftingValue;
                shiftingValue <<= 1;
            }
            return finalValue;
        }

        struct TestReporter
        {
            static void Log(const std::string &);
        };

        enum class AllowOneEmpty : bool
        {
            No = false,
            Yes = true
        };

        enum class AllowLastDuplicate : bool
        {
            No = false,
            Yes = true
        };

        // Responsible for testing the ToString() function over all possible enum flag values.
        template <typename Enum_t>
        struct Test
        {
            static bool ToStringForCountingEnum(const AllowOneEmpty WILL_ALLOW_ONE_EMPTY_STRING)
            {
                return ToString<int>(
                    Enum_t::Count - 1, WILL_ALLOW_ONE_EMPTY_STRING, AllowLastDuplicate::No);
            }

            static bool ToStringForFlagEnum(const AllowOneEmpty WILL_ALLOW_ONE_EMPTY_STRING)
            {
                return ToString<unsigned>(
                    CalculateLastValidValueOfFlagEnum(static_cast<unsigned>(Enum_t::Last)),
                    WILL_ALLOW_ONE_EMPTY_STRING,
                    AllowLastDuplicate::Yes);
            }

        private:
            template <typename Value_t>
            static bool ToString(
                const Value_t LAST_VALID_VALUE,
                const AllowOneEmpty WILL_ALLOW_ONE_EMPTY_STRING,
                const AllowLastDuplicate ALLOW_LAST_DUPLICATE)
            {
                static auto hasAlreadyTested{ false };

                if (hasAlreadyTested)
                {
                    return true;
                }

                std::ostringstream errorSS;
                errorSS << "misc::EnumTester<" << boost::typeindex::type_id<Enum_t>().pretty_name()
                        << ">::ToString<" << boost::typeindex::type_id<Value_t>().pretty_name()
                        << ">(last_valid_value=" << LAST_VALID_VALUE
                        << ", allow_one_empty=" << std::boolalpha
                        << bool(WILL_ALLOW_ONE_EMPTY_STRING)
                        << ", allow_last_duplicate=" << bool(ALLOW_LAST_DUPLICATE) << ") ";

                static auto hasLoggedBegin{ false };
                if (false == hasLoggedBegin)
                {
                    errorSS << "Starting...";
                    TestReporter::Log(errorSS.str());
                    hasLoggedBegin = true;
                    return false;
                }

                static auto hasZeroBeenChecked{ false };
                if (false == hasZeroBeenChecked)
                {
                    hasZeroBeenChecked = true;

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (LAST_VALID_VALUE > 0),
                        errorSS.str()
                            << "was given a last_valid_value that was not greater than zero.");

                    return false;
                }

                static Value_t LOG_AFTER_COUNT{ 100000 };
                static Value_t flagValue{ 0 };

                // this is used to verify ToString() returns a unique string for each unique enum
                static std::vector<std::string> alreadyGeneratedStrings;
                if (alreadyGeneratedStrings.capacity() < static_cast<std::size_t>(LAST_VALID_VALUE))
                {
                    alreadyGeneratedStrings.reserve(static_cast<std::size_t>(LAST_VALID_VALUE));
                }

                try
                {
                    while (flagValue <= LAST_VALID_VALUE)
                    {
                        auto const FLAG_VALUE_TO_TEST_AND_REPORT{ flagValue++ };

                        auto const STRING{ Enum_t::ToString(
                            static_cast<typename Enum_t::Enum>(FLAG_VALUE_TO_TEST_AND_REPORT)) };

                        if (WILL_ALLOW_ONE_EMPTY_STRING == AllowOneEmpty::No)
                        {
                            M_ASSERT_OR_LOGANDTHROW_SS(
                                (STRING.empty() == false),
                                errorSS.str() << "ToString() returned an empty string on value "
                                              << FLAG_VALUE_TO_TEST_AND_REPORT << ".");
                        }

                        if ((FLAG_VALUE_TO_TEST_AND_REPORT != LAST_VALID_VALUE)
                            || (ALLOW_LAST_DUPLICATE == AllowLastDuplicate::No))
                        {
                            auto const WAS_STRING_ALREADY_GENERATED{
                                std::find(
                                    std::begin(alreadyGeneratedStrings),
                                    std::end(alreadyGeneratedStrings),
                                    STRING)
                                != std::end(alreadyGeneratedStrings)
                            };

                            M_ASSERT_OR_LOGANDTHROW_SS(
                                (WAS_STRING_ALREADY_GENERATED == false),
                                errorSS.str() << "value=" << FLAG_VALUE_TO_TEST_AND_REPORT << "=\""
                                              << STRING << "\" was already generated.");
                        }

                        if ((FLAG_VALUE_TO_TEST_AND_REPORT % LOG_AFTER_COUNT) == 0)
                        {
                            std::ostringstream statusSS;

                            statusSS << "EnumTester<"
                                     << boost::typeindex::type_id<Enum_t>().pretty_name()
                                     << ">::ToString() " << std::setprecision(3)
                                     << 100.0
                                    * (static_cast<double>(FLAG_VALUE_TO_TEST_AND_REPORT)
                                       / static_cast<double>(LAST_VALID_VALUE))
                                     << "%";

                            TestReporter::Log(statusSS.str());
                            return false;
                        }
                    }
                }
                catch (const std::exception & EXCEPTION)
                {
                    --flagValue;

                    M_HP_LOG_ERR(
                        errorSS.str() << "threw exception \"" << EXCEPTION.what()
                                      << "\" on value=" << flagValue << ".");

                    throw;
                }

                static auto hasLoggedEnd{ false };
                if (false == hasLoggedEnd)
                {
                    errorSS << "Done testing all " << flagValue << " values.";
                    TestReporter::Log(errorSS.str());
                    hasLoggedEnd = true;
                }

                hasAlreadyTested = true;
                return true;
            }
        };

    } // namespace enum_util
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
