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
#define BOOST_TEST_MODULE "HeroesPathTestModule_sfml-util_sliders_test"

#include <boost/test/unit_test.hpp>

#include "misc/real.hpp"
#include "sfml-util/sliders.hpp"

#include <exception>

#include "Test-stuff.hpp"

using namespace misc;
using namespace sfml_util;

namespace ts = test_stuff;


BOOST_AUTO_TEST_CASE(Sliders_ZeroSliderOnce_DefaultConstruction)
{
    ts::Constants constants;

    sliders::ZeroSliderOnce<double> slider;
    BOOST_CHECK(slider.GetIsDone() == false);
    BOOST_CHECK(IsRealClose(slider.GetCur(), 0.0, constants.CLOSE_ENOUGH_EPSILON));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), 1.0, constants.CLOSE_ENOUGH_EPSILON));

    slider.Update(0.0);
    BOOST_CHECK(slider.GetIsDone() == false);
    BOOST_CHECK(IsRealClose(slider.GetCur(), 0.0, constants.CLOSE_ENOUGH_EPSILON));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), 1.0, constants.CLOSE_ENOUGH_EPSILON));
}


BOOST_AUTO_TEST_CASE(Sliders_ZeroSliderOnce_Updates)
{
    ts::Constants constants;

    sliders::ZeroSliderOnce<double> slider;
    BOOST_CHECK(slider.GetIsDone() == false);

    slider.Update(0.1);
    BOOST_CHECK(slider.GetIsDone() == false);
    BOOST_CHECK(IsRealClose(slider.GetCur(), 0.0, constants.CLOSE_ENOUGH_EPSILON) == false);
    BOOST_CHECK(IsRealClose(slider.GetSpd(), 1.0, constants.CLOSE_ENOUGH_EPSILON));

    while (slider.GetIsDone() == false)
    {
        slider.Update(0.1);
    }
    BOOST_CHECK(IsRealOne(slider.GetCur()));
}


BOOST_AUTO_TEST_CASE(Sliders_SliderOnce_DefaultConstruction)
{
    ts::Constants constants;

    auto const BEGIN{ 123.123 };
    auto const END{ 456.456 };
    auto const SPEED{ 1.0 };

    sliders::SliderOnce<double> slider(BEGIN, END, SPEED);

    BOOST_CHECK(slider.GetIsDone() == false);
    BOOST_CHECK(IsRealClose(slider.GetBeg(), BEGIN));
    BOOST_CHECK(IsRealClose(slider.GetEnd(), END));
    BOOST_CHECK(IsRealClose(slider.GetCur(), BEGIN, constants.CLOSE_ENOUGH_EPSILON));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), SPEED));

    slider.Update(0.0);
    BOOST_CHECK(slider.GetIsDone() == false);
    BOOST_CHECK(IsRealClose(slider.GetBeg(), BEGIN));
    BOOST_CHECK(IsRealClose(slider.GetEnd(), END));
    BOOST_CHECK(IsRealClose(slider.GetCur(), BEGIN, constants.CLOSE_ENOUGH_EPSILON));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), SPEED));
}


BOOST_AUTO_TEST_CASE(Sliders_SliderOnce_Updates)
{
    auto const BEGIN{ 123.123 };
    auto const END{ 456.456 };
    auto const SPEED{ 1.0 };

    sliders::SliderOnce<double> slider(BEGIN, END, SPEED);

    while (slider.GetIsDone() == false)
    {
        slider.Update(0.1);
    }
    BOOST_CHECK(IsRealClose(slider.GetBeg(), BEGIN));
    BOOST_CHECK(IsRealClose(slider.GetEnd(), END));
    BOOST_CHECK(IsRealClose(slider.GetCur(), END));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), SPEED));
}


BOOST_AUTO_TEST_CASE(Sliders_Slider_DefaultConstruction)
{
    ts::Constants constants;

    auto const BEGIN{ 123.123 };
    auto const END{ 456.456 };
    auto const SPEED{ 1.0 };

    sliders::Slider<double> slider(BEGIN, END, SPEED, BEGIN);

    BOOST_CHECK(IsRealClose(slider.GetMin(), BEGIN));
    BOOST_CHECK(IsRealClose(slider.GetMax(), END));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), SPEED));

    BOOST_CHECK(IsRealClose(slider.Update(0.0), BEGIN, constants.CLOSE_ENOUGH_EPSILON));
    BOOST_CHECK(IsRealClose(slider.GetMin(), BEGIN));
    BOOST_CHECK(IsRealClose(slider.GetMax(), END));
    BOOST_CHECK(IsRealClose(slider.GetSpd(), SPEED));
}


BOOST_AUTO_TEST_CASE(Sliders_Slider_Updates)
{
    ts::Constants constants;

    auto const BEGIN{ 123.123 };
    auto const END{ 456.456 };
    auto const SPEED{ 1.0 };

    sliders::Slider<double> slider(BEGIN, END, SPEED, BEGIN);

    auto const ITERATIONS{ 1000 };
    for (int i(0); i<ITERATIONS; ++i)
    {
        auto const CURRENT{ slider.Update(0.1) };

        BOOST_CHECK(IsRealClose(CURRENT, BEGIN, constants.CLOSE_ENOUGH_EPSILON) ||
                    IsRealClose(CURRENT, END, constants.CLOSE_ENOUGH_EPSILON) ||
                    ((CURRENT > BEGIN) && (CURRENT < END)));

        BOOST_CHECK(IsRealClose(slider.GetMin(), BEGIN));
        BOOST_CHECK(IsRealClose(slider.GetMax(), END));
        BOOST_CHECK(IsRealClose(slider.GetSpd(), SPEED));
    }
}
