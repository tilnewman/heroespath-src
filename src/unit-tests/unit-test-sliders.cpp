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
#include <boost/type_index.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include "misc/real.hpp"
#include "sfml-util/sliders.hpp"

#include <exception>
#include <iostream>
#include <numeric>

#include "unit-test-test-stuff.hpp"

using namespace heroespath;
using namespace heroespath::misc;
using namespace heroespath::sfml_util;

namespace ts = test_stuff;

template <typename Value_t, typename Slider_t>
void testSlider(
    Slider_t slider,
    const Value_t FROM,
    const Value_t TO,
    const Value_t START_AT,
    const float ADJUSTMENT,
    const std::size_t ITERATION_COUNT_MIN)
{
    std::cout << "testSlider<Value_t=" << boost::typeindex::type_id<Value_t>().pretty_name()
              << ", Slider_t=" << boost::typeindex::type_id<Slider_t>().pretty_name()
              << ">(from=" << FROM << ", to=" << TO << ", start_at=" << START_AT
              << ", adjustment=" << ADJUSTMENT << ", iteration_count_min=" << ITERATION_COUNT_MIN
              << ")" << std::endl;

    BOOST_CHECK(IsRealClose(slider.From(), FROM));
    BOOST_CHECK(IsRealClose(slider.To(), TO));
    BOOST_CHECK(IsRealOne(slider.Speed()));
    BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
    BOOST_CHECK(slider.IsStopped() == false);
    BOOST_CHECK(IsRealClose(slider.Value(), START_AT));

    Value_t prevValue { START_AT };
    std::size_t iterationCounter { 0 };
    while (slider.IsMoving())
    {
        const auto CURRENT_VALUE { slider.Update(ADJUSTMENT) };
        ++iterationCounter;

        BOOST_CHECK(IsRealClose(slider.From(), FROM));
        BOOST_CHECK(IsRealClose(slider.To(), TO));
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());

        BOOST_CHECK(CURRENT_VALUE >= prevValue);
        prevValue = CURRENT_VALUE;

        // update of zero double check

        BOOST_CHECK(misc::IsRealClose(slider.Update(0.0f), CURRENT_VALUE));
    }

    if (ITERATION_COUNT_MIN > 0)
    {
        BOOST_CHECK(iterationCounter > ITERATION_COUNT_MIN);
    }

    BOOST_CHECK(slider.IsStopped());
    BOOST_CHECK(IsRealClose(slider.Value(), TO));

    // updates after stop (positive)

    for (int i(0); i < 100; ++i)
    {
        slider.Update(9999.9999f);

        BOOST_CHECK(IsRealClose(slider.From(), FROM));
        BOOST_CHECK(IsRealClose(slider.To(), TO));
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealClose(slider.Value(), TO));
    }

    // updates after stop (negative)

    for (int i(0); i < 100; ++i)
    {
        slider.Update(-9999.9999f);

        BOOST_CHECK(IsRealClose(slider.From(), FROM));
        BOOST_CHECK(IsRealClose(slider.To(), TO));
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealClose(slider.Value(), TO));
    }
}

BOOST_AUTO_TEST_CASE(SliderZeroToOne_Tests)
{
    // should not compile
    // SliderZeroToOne<int> slider;
    // SliderZeroToOne<bool> slider;
    // SliderZeroToOne<const bool> slider;

    // invalid constructions

    {
        SliderZeroToOne slider;

        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealZero(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));
        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(-0.1f);

        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealClose(slider.Speed(), -0.1f));
        BOOST_CHECK(IsRealZero(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(0.0f);

        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(IsRealZero(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(1.0f, -0.1f);

        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(1.0f, 1.0f);

        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealOne(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(1.0f, 1.1f);

        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealOne(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    // valid constructions

    {
        SliderZeroToOne slider(1.0f, 0.0f);

        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(1.0f, 0.5f);

        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealClose(slider.Value(), 0.5f));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        SliderZeroToOne slider(1.0f, 0.9f);

        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealClose(slider.Value(), 0.9f));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
    }

    {
        // zero updates

        SliderZeroToOne slider(1.0f);

        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));

        for (int i(0); i < 100; ++i)
        {
            slider.Update(0.0f);

            BOOST_CHECK(slider.IsStopped() == false);
            BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
            BOOST_CHECK(IsRealOne(slider.Speed()));
            BOOST_CHECK(IsRealZero(slider.Value()));

            BOOST_CHECK(IsRealZero(slider.From()));
            BOOST_CHECK(IsRealOne(slider.To()));
        }

        // updates after stop
        slider.Stop();

        for (int i(0); i < 100; ++i)
        {
            slider.Update(0.0f);

            BOOST_CHECK(slider.IsStopped());
            BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
            BOOST_CHECK(IsRealOne(slider.Speed()));
            BOOST_CHECK(IsRealZero(slider.Value()));

            BOOST_CHECK(IsRealZero(slider.From()));
            BOOST_CHECK(IsRealOne(slider.To()));
        }

        for (int i(0); i < 100; ++i)
        {
            slider.Update(9999.9999f);

            BOOST_CHECK(slider.IsStopped());
            BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
            BOOST_CHECK(IsRealOne(slider.Speed()));
            BOOST_CHECK(IsRealZero(slider.Value()));

            BOOST_CHECK(IsRealZero(slider.From()));
            BOOST_CHECK(IsRealOne(slider.To()));
        }
    }

    // operation with speed=1 and 60fps updates
    {
        SliderZeroToOne slider(1.0f);
        testSlider<float, SliderZeroToOne>(slider, 0.0f, 1.0f, 0.0f, 0.01667f, 100);
    }

    {
        SliderZeroToOne slider(1.0f, 0.5f);
        testSlider<float, SliderZeroToOne>(slider, 0.0f, 1.0f, 0.5f, 0.01667f, 50);
    }

    // updates that are too large
    {
        SliderZeroToOne slider(1.0f);

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));

        for (int i(0); i < 100; ++i)
        {
            slider.Update(std::numeric_limits<float>::max());

            BOOST_CHECK(IsRealZero(slider.From()));
            BOOST_CHECK(IsRealOne(slider.To()));
            BOOST_CHECK(slider.IsStopped() == true);
            BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
            BOOST_CHECK(IsRealOne(slider.Speed()));
            BOOST_CHECK(IsRealOne(slider.Value()));
        }
    }

    // updates that are too small (negative) and then too large
    {
        SliderZeroToOne slider(1.0f);

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealOne(slider.To()));
        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(IsRealZero(slider.Value()));

        for (int i(0); i < 100; ++i)
        {
            slider.Update(std::numeric_limits<float>::lowest());

            BOOST_CHECK(IsRealZero(slider.From()));
            BOOST_CHECK(IsRealOne(slider.To()));
            BOOST_CHECK(slider.IsStopped() == false);
            BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
            BOOST_CHECK(IsRealOne(slider.Speed()));
            BOOST_CHECK(IsRealZero(slider.Value()));
        }

        for (int i(0); i < 100; ++i)
        {
            slider.Update(std::numeric_limits<float>::max());

            BOOST_CHECK(IsRealZero(slider.From()));
            BOOST_CHECK(IsRealOne(slider.To()));
            BOOST_CHECK(slider.IsStopped() == true);
            BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
            BOOST_CHECK(IsRealOne(slider.Speed()));
            BOOST_CHECK(IsRealOne(slider.Value()));
        }
    }
}

BOOST_AUTO_TEST_CASE(SliderFromTo_Tests)
{
    // should not compile
    // SliderFromTo<int> slider;
    // SliderFromTo<bool> slider;
    // SliderFromTo<const bool> slider;

    // invalid constructions

    {
        SliderFromTo<int> slider;

        BOOST_CHECK(slider.From() == 0);
        BOOST_CHECK(slider.To() == 0);
        BOOST_CHECK(IsRealZero(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(slider.Value() == 0);
    }

    {
        SliderFromTo<int> slider(0, 0, 1.0f);

        BOOST_CHECK(slider.From() == 0);
        BOOST_CHECK(slider.To() == 0);
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(slider.Value() == 0);
    }

    {
        SliderFromTo<float> slider(0.0, 0.0, 1.0f);

        BOOST_CHECK(IsRealZero(slider.From()));
        BOOST_CHECK(IsRealZero(slider.To()));
        BOOST_CHECK(IsRealOne(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(IsRealZero(slider.Value()));
    }

    {
        SliderFromTo<int> slider(0, 100, -0.1f);

        BOOST_CHECK(slider.From() == 0);
        BOOST_CHECK(slider.To() == 100);
        BOOST_CHECK(IsRealClose(slider.Speed(), -0.1f));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped());
        BOOST_CHECK(slider.Value() == 0);
    }

    {
        SliderFromTo<int> slider(0, 100, 0.0f);

        BOOST_CHECK(slider.From() == 0);
        BOOST_CHECK(slider.To() == 100);
        BOOST_CHECK(IsRealZero(slider.Speed()));
        BOOST_CHECK(slider.IsMoving() != slider.IsStopped());
        BOOST_CHECK(slider.IsStopped() == false);
        BOOST_CHECK(slider.Value() == 0);
    }

    {
        SliderFromTo<int> slider(-100, 100, 1.0f);
        testSlider<int, SliderFromTo<int>>(slider, -100, 100, -100, 0.01667f, 100);
    }

    {
        SliderFromTo<int> slider(-1, 1, 1.0f);
        testSlider<int, SliderFromTo<int>>(slider, -1, 1, -1, 0.01667f, 0);
    }

    {
        SliderFromTo<int> slider(0, 1, 1.0f);
        testSlider<int, SliderFromTo<int>>(slider, 0, 1, 0, 0.01667f, 0);
    }

    {
        SliderFromTo<int> slider(-1, 0, 1.0f);
        testSlider<int, SliderFromTo<int>>(slider, -1, 0, -1, 0.01667f, 0);
    }
}
