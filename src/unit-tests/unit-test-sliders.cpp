// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_sfml-util_sliders_test"

#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4266)
#endif

#include <boost/test/unit_test.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include "misc/real.hpp"
#include "sfml-util/sliders.hpp"

#include <exception>

#include "unit-test-test-stuff.hpp"

using namespace heroespath::misc;
using namespace heroespath::sfml_util;

namespace ts = test_stuff;

BOOST_AUTO_TEST_CASE(Sliders_ZeroSliderOnce_DefaultConstruction)
{
    ts::Constants constants;

    sliders::ZeroSliderOnce<double> slider;
    BOOST_CHECK(slider.IsDone() == false);
    BOOST_CHECK(IsRealClose(slider.Current(), 0.0));
    BOOST_CHECK(IsRealClose(slider.Speed(), 1.0));

    slider.Update(0.0);
    BOOST_CHECK(slider.IsDone() == false);
    BOOST_CHECK(IsRealClose(slider.Current(), 0.0));
    BOOST_CHECK(IsRealClose(slider.Speed(), 1.0));
}

BOOST_AUTO_TEST_CASE(Sliders_ZeroSliderOnce_Updates)
{
    ts::Constants constants;

    sliders::ZeroSliderOnce<double> slider;
    BOOST_CHECK(slider.IsDone() == false);

    slider.Update(0.1);
    BOOST_CHECK(slider.IsDone() == false);
    BOOST_CHECK(IsRealClose(slider.Current(), 0.0) == false);
    BOOST_CHECK(IsRealClose(slider.Speed(), 1.0));

    while (slider.IsDone() == false)
    {
        slider.Update(0.1);
    }
    BOOST_CHECK(IsRealOne(slider.Current()));
}

BOOST_AUTO_TEST_CASE(Sliders_SliderOnce_DefaultConstruction)
{
    ts::Constants constants;

    auto const THE_MIN { 123.123 };
    auto const THE_MAX { 456.456 };
    auto const SPEED { 1.0 };

    sliders::SliderOnce<double> slider(THE_MIN, THE_MAX, SPEED);

    BOOST_CHECK(slider.IsDone() == false);
    BOOST_CHECK(IsRealClose(slider.Min(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Max(), THE_MAX));
    BOOST_CHECK(IsRealClose(slider.Current(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Speed(), SPEED));

    slider.Update(0.0);
    BOOST_CHECK(slider.IsDone() == false);
    BOOST_CHECK(IsRealClose(slider.Min(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Max(), THE_MAX));
    BOOST_CHECK(IsRealClose(slider.Current(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Speed(), SPEED));
}

BOOST_AUTO_TEST_CASE(Sliders_SliderOnce_Updates)
{
    auto const THE_MIN { 123.123 };
    auto const THE_MAX { 456.456 };
    auto const SPEED { 1.0 };

    sliders::SliderOnce<double> slider(THE_MIN, THE_MAX, SPEED);

    while (slider.IsDone() == false)
    {
        slider.Update(0.1);
    }
    BOOST_CHECK(IsRealClose(slider.Min(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Max(), THE_MAX));
    BOOST_CHECK(IsRealClose(slider.Current(), THE_MAX));
    BOOST_CHECK(IsRealClose(slider.Speed(), SPEED));
}

BOOST_AUTO_TEST_CASE(Sliders_Slider_DefaultConstruction)
{
    ts::Constants constants;

    auto const THE_MIN { 123.123 };
    auto const THE_MAX { 456.456 };
    auto const SPEED { 1.0 };

    sliders::Slider<double> slider(THE_MIN, THE_MAX, SPEED, THE_MIN);

    BOOST_CHECK(IsRealClose(slider.Min(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Max(), THE_MAX));
    BOOST_CHECK(IsRealClose(slider.Speed(), SPEED));

    BOOST_CHECK(IsRealClose(slider.Update(0.0), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Min(), THE_MIN));
    BOOST_CHECK(IsRealClose(slider.Max(), THE_MAX));
    BOOST_CHECK(IsRealClose(slider.Speed(), SPEED));
}

BOOST_AUTO_TEST_CASE(Sliders_Slider_Updates)
{
    ts::Constants constants;

    auto const THE_MIN { 123.123 };
    auto const THE_MAX { 456.456 };
    auto const SPEED { 1.0 };

    sliders::Slider<double> slider(THE_MIN, THE_MAX, SPEED, THE_MIN);

    auto const ITERATIONS { 1000 };
    for (int i(0); i < ITERATIONS; ++i)
    {
        auto const CURRENT { slider.Update(0.1) };

        BOOST_CHECK(
            IsRealClose(CURRENT, THE_MIN) || IsRealClose(CURRENT, THE_MAX)
            || ((CURRENT > THE_MIN) && (CURRENT < THE_MAX)));

        BOOST_CHECK(IsRealClose(slider.Min(), THE_MIN));
        BOOST_CHECK(IsRealClose(slider.Max(), THE_MAX));
        BOOST_CHECK(IsRealClose(slider.Speed(), SPEED));
    }
}
