// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-combat.cpp
//
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_TypeHelpers"

#include <boost/test/unit_test.hpp>

#include "misc/type-helpers.hpp"

#include <iostream>

using namespace heroespath::misc;

BOOST_AUTO_TEST_CASE(unit_tests__misc__type_helpers__are_number)
{
    // fundamental number types
    BOOST_CHECK((are_arithmetic_nobool_v<char>));
    BOOST_CHECK((are_arithmetic_nobool_v<short>));
    BOOST_CHECK((are_arithmetic_nobool_v<int>));
    BOOST_CHECK((are_arithmetic_nobool_v<long>));
    BOOST_CHECK((are_arithmetic_nobool_v<long long>));

    // repeat above only unsigned
    BOOST_CHECK((are_arithmetic_nobool_v<unsigned char>));
    BOOST_CHECK((are_arithmetic_nobool_v<unsigned short>));
    BOOST_CHECK((are_arithmetic_nobool_v<unsigned int>));
    BOOST_CHECK((are_arithmetic_nobool_v<unsigned long>));
    BOOST_CHECK((are_arithmetic_nobool_v<unsigned long long>));

    // repeat BOTH above only with const
    BOOST_CHECK((are_arithmetic_nobool_v<const char>));
    BOOST_CHECK((are_arithmetic_nobool_v<const short>));
    BOOST_CHECK((are_arithmetic_nobool_v<const int>));
    BOOST_CHECK((are_arithmetic_nobool_v<const long>));
    BOOST_CHECK((are_arithmetic_nobool_v<const long long>));
    //
    BOOST_CHECK((are_arithmetic_nobool_v<const unsigned char>));
    BOOST_CHECK((are_arithmetic_nobool_v<const unsigned short>));
    BOOST_CHECK((are_arithmetic_nobool_v<const unsigned int>));
    BOOST_CHECK((are_arithmetic_nobool_v<const unsigned long>));
    BOOST_CHECK((are_arithmetic_nobool_v<const unsigned long long>));

    // standard library types
    BOOST_CHECK((are_arithmetic_nobool_v<std::int8_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<std::int16_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<std::int32_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<std::int64_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<char16_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<char32_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<wchar_t>));

    // repeat above only unsigned
    BOOST_CHECK((are_arithmetic_nobool_v<std::uint8_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<std::uint16_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<std::uint32_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<std::uint64_t>));

    // repeat BOTH above only with const
    BOOST_CHECK((are_arithmetic_nobool_v<const std::int8_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const std::int16_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const std::int32_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const std::int64_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const char16_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const char32_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const wchar_t>));
    //
    BOOST_CHECK((are_arithmetic_nobool_v<const std::uint8_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const std::uint16_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const std::uint32_t>));
    BOOST_CHECK((are_arithmetic_nobool_v<const std::uint64_t>));

    // floating point types
    BOOST_CHECK((are_arithmetic_nobool_v<float>));
    BOOST_CHECK((are_arithmetic_nobool_v<double>));
    BOOST_CHECK((are_arithmetic_nobool_v<long double>));

    // misc fail cases
    BOOST_CHECK((are_arithmetic_nobool_v<bool>) == false);
    BOOST_CHECK((are_arithmetic_nobool_v<const bool>) == false);
    BOOST_CHECK((are_arithmetic_nobool_v<std::string>) == false);
    BOOST_CHECK((are_arithmetic_nobool_v<const std::string>) == false);
}

BOOST_AUTO_TEST_CASE(unit_tests__misc__type_helpers__are_integral)
{
    // fundamental number types
    BOOST_CHECK((are_integral_v<char>));
    BOOST_CHECK((are_integral_v<short>));
    BOOST_CHECK((are_integral_v<int>));
    BOOST_CHECK((are_integral_v<long>));
    BOOST_CHECK((are_integral_v<long long>));

    // repeat above only unsigned
    BOOST_CHECK((are_integral_v<unsigned char>));
    BOOST_CHECK((are_integral_v<unsigned short>));
    BOOST_CHECK((are_integral_v<unsigned int>));
    BOOST_CHECK((are_integral_v<unsigned long>));
    BOOST_CHECK((are_integral_v<unsigned long long>));

    // repeat BOTH above only with const
    BOOST_CHECK((are_integral_v<const char>));
    BOOST_CHECK((are_integral_v<const short>));
    BOOST_CHECK((are_integral_v<const int>));
    BOOST_CHECK((are_integral_v<const long>));
    BOOST_CHECK((are_integral_v<const long long>));
    //
    BOOST_CHECK((are_integral_v<const unsigned char>));
    BOOST_CHECK((are_integral_v<const unsigned short>));
    BOOST_CHECK((are_integral_v<const unsigned int>));
    BOOST_CHECK((are_integral_v<const unsigned long>));
    BOOST_CHECK((are_integral_v<const unsigned long long>));

    // standard library types
    BOOST_CHECK((are_integral_v<std::int8_t>));
    BOOST_CHECK((are_integral_v<std::int16_t>));
    BOOST_CHECK((are_integral_v<std::int32_t>));
    BOOST_CHECK((are_integral_v<std::int64_t>));
    BOOST_CHECK((are_integral_v<char16_t>));
    BOOST_CHECK((are_integral_v<char32_t>));
    BOOST_CHECK((are_integral_v<wchar_t>));

    // repeat above only unsigned
    BOOST_CHECK((are_integral_v<std::uint8_t>));
    BOOST_CHECK((are_integral_v<std::uint16_t>));
    BOOST_CHECK((are_integral_v<std::uint32_t>));
    BOOST_CHECK((are_integral_v<std::uint64_t>));

    // repeat BOTH above only with const
    BOOST_CHECK((are_integral_v<const std::int8_t>));
    BOOST_CHECK((are_integral_v<const std::int16_t>));
    BOOST_CHECK((are_integral_v<const std::int32_t>));
    BOOST_CHECK((are_integral_v<const std::int64_t>));
    BOOST_CHECK((are_integral_v<const char16_t>));
    BOOST_CHECK((are_integral_v<const char32_t>));
    BOOST_CHECK((are_integral_v<const wchar_t>));
    //
    BOOST_CHECK((are_integral_v<const std::uint8_t>));
    BOOST_CHECK((are_integral_v<const std::uint16_t>));
    BOOST_CHECK((are_integral_v<const std::uint32_t>));
    BOOST_CHECK((are_integral_v<const std::uint64_t>));

    // floating point types (should always return false)
    BOOST_CHECK((are_integral_v<float>) == false);
    BOOST_CHECK((are_integral_v<double>) == false);
    BOOST_CHECK((are_integral_v<long double>) == false);

    // misc fail cases
    BOOST_CHECK((are_integral_nobool_v<bool>) == false);
    BOOST_CHECK((are_integral_nobool_v<const bool>) == false);
    BOOST_CHECK((are_integral_v<std::string>) == false);
    BOOST_CHECK((are_integral_v<const std::string>) == false);
}
