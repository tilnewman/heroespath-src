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

#include "misc/real.hpp"

using namespace heroespath::misc;

BOOST_AUTO_TEST_CASE(Real_IsRealZero)
{
    BOOST_CHECK(IsRealZero(0.0));
    BOOST_CHECK(IsRealZero(0.0f));

    BOOST_CHECK(IsRealZero(1.0) == false);
    BOOST_CHECK(IsRealZero(1.0f) == false);

    BOOST_CHECK(IsRealZero(0.00001) == false);
    BOOST_CHECK(IsRealZero(-0.00001f) == false);

    BOOST_CHECK(IsRealZero(0.0 + std::numeric_limits<double>::epsilon()) == false);
    BOOST_CHECK(IsRealZero(0.0 - std::numeric_limits<double>::epsilon()) == false);
}

BOOST_AUTO_TEST_CASE(Real_IsRealOne)
{
    BOOST_CHECK(IsRealOne(1.0));
    BOOST_CHECK(IsRealOne(1.0f));

    BOOST_CHECK(IsRealOne(0.0) == false);
    BOOST_CHECK(IsRealOne(0.0f) == false);

    BOOST_CHECK(IsRealOne(1.00001) == false);
    BOOST_CHECK(IsRealOne(0.9999f) == false);

    BOOST_CHECK(IsRealOne(1.0 + std::numeric_limits<double>::epsilon()) == false);
    BOOST_CHECK(IsRealOne(1.0 - std::numeric_limits<double>::epsilon()) == false);
}

BOOST_AUTO_TEST_CASE(Real_IsRealClose)
{
    BOOST_CHECK(IsRealClose(0.0, 0.0));
    BOOST_CHECK(IsRealClose(1.0, 1.0));
    BOOST_CHECK(IsRealClose(123.456, 123.456));

    BOOST_CHECK(IsRealClose(0.0f, 0.0f));
    BOOST_CHECK(IsRealClose(1.0f, 1.0f));
    BOOST_CHECK(IsRealClose(123.456f, 123.456f));

    BOOST_CHECK(IsRealClose(0.0, 1.0) == false);
    BOOST_CHECK(IsRealClose(0.0, 0.00001) == false);
    BOOST_CHECK(IsRealClose(0.0, std::numeric_limits<double>::epsilon()) == false);
    BOOST_CHECK(IsRealClose(std::numeric_limits<double>::epsilon(), 0.0) == false);
}
