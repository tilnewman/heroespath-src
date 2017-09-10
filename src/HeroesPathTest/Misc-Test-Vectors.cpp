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

#include "Test-stuff.hpp"

#include "misc/vectors.hpp"


namespace ts = test_stuff;


BOOST_AUTO_TEST_CASE(Vector_Append_AppendNothingToSingle)
{
    const ts::IntVec_t EMPTY;

    for (auto const VALUE : ts::TEST_COUNTS)
    {
        ts::IntVec_t singleValue(1, VALUE);
        const ts::IntVec_t SINGLE_VALUE_BEFORE{ singleValue };
        misc::Vector::Append(EMPTY, singleValue, misc::Vector::SortOpt::None);

        BOOST_CHECK_MESSAGE(singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << ts::vectorToString(SINGLE_VALUE_BEFORE)
            << "single value vec after=" << ts::vectorToString(singleValue));


        misc::Vector::Append(EMPTY, singleValue, misc::Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(singleValue == SINGLE_VALUE_BEFORE,
            "single value vec before=" << ts::vectorToString(SINGLE_VALUE_BEFORE)
            << "single value vec after=" << ts::vectorToString(singleValue));
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

        BOOST_CHECK_MESSAGE(multipleValues == MULTIPLE_VALUES_BEFORE,
            "mutliple values vec before=" << ts::vectorToString(MULTIPLE_VALUES_BEFORE)
            << "multiple values vec after=" << ts::vectorToString(multipleValues));


        misc::Vector::Append(EMPTY, multipleValues, misc::Vector::SortOpt::SortAndUnique);

        BOOST_CHECK_MESSAGE(multipleValues == MULTIPLE_VALUES_BEFORE,
            "multiple values vec before=" << ts::vectorToString(MULTIPLE_VALUES_BEFORE)
            << "multiple values vec after=" << ts::vectorToString(multipleValues));
    }
}
