// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_sfutil_test"

#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4266)
#endif

#include <boost/test/unit_test.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include "gui/display.hpp"
#include "misc/log-macros.hpp"
#include "misc/log.hpp"
#include "misc/platform.hpp"
#include "misc/random.hpp"
#include "sfutil/angles.hpp"
#include "sfutil/blend-mode.hpp"

#include "sfutil/center.hpp"
#include "sfutil/color.hpp"
#include "sfutil/common.hpp"
#include "sfutil/direction.hpp"
#include "sfutil/display.hpp"
#include "sfutil/distance.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/font.hpp"
#include "sfutil/image-manip.hpp"
#include "sfutil/keyboard.hpp"
#include "sfutil/overlap.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/rectangle-shape.hpp"
#include "sfutil/scale.hpp"
#include "sfutil/sprite.hpp"
#include "sfutil/vector-and-rect.hpp"
#include "sfutil/vector-math.hpp"
#include "sfutil/vertex.hpp"
#include "sfutil/video-mode.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include <iostream>
#include <limits>
#include <vector>

#include "unit-test-test-stuff.hpp"

using namespace test_stuff;
using namespace heroespath;
using namespace heroespath::gui;
using namespace heroespath::sfutil;
using namespace heroespath::misc;

namespace intersects_test_stuff
{

#define M_TEST_INTERSECTION(rect_a, rect_b, expected)                                              \
    {                                                                                              \
        BOOST_CHECK_MESSAGE(                                                                       \
            Intersects(RI_##rect_a, RI_##rect_b) == (RI_##expected != sf::IntRect(0, 0, 0, 0)),    \
            "Intersects==(Exp==R0)(RI_" #rect_a "__tion__RI_" #rect_b "==RI_" #expected ")");      \
                                                                                                   \
        BOOST_CHECK_MESSAGE(                                                                       \
            Intersection(RI_##rect_a, RI_##rect_b) == RI_##expected,                               \
            "Expected(RI_" #rect_a "__tion__RI_" #rect_b "==RI_" #expected ")");                   \
                                                                                                   \
        BOOST_CHECK_MESSAGE(                                                                       \
            Intersection(RI_##rect_a, RI_##rect_b) == Intersection(RI_##rect_b, RI_##rect_a),      \
            "Transitive(RI_" #rect_a "__tion__RI_" #rect_b "==RI_" #expected ")");                 \
    }

#define M_TEST_EQUAL_INTERSECTS(rect_a, rect_b, expected)                                          \
    {                                                                                              \
        BOOST_CHECK_MESSAGE(                                                                       \
            (RI_##rect_a.intersects(RI_##rect_b)) == expected,                                     \
            "SFML==Expected(RI_" #rect_a "__ect__RI_" #rect_b " " #expected ")");                  \
                                                                                                   \
        BOOST_CHECK_MESSAGE(                                                                       \
            Intersects(RI_##rect_a, RI_##rect_b) == expected,                                      \
            "New==Expected(RI_" #rect_a "__ect__RI_" #rect_b " " #expected ")");                   \
                                                                                                   \
        BOOST_CHECK_MESSAGE(                                                                       \
            (RI_##rect_a.intersects(RI_##rect_b)) == (RI_##rect_b.intersects(RI_##rect_a)),        \
            "SFML_Transitive(RI_" #rect_a "__ect__RI_" #rect_b " " #expected ")");                 \
                                                                                                   \
        BOOST_CHECK_MESSAGE(                                                                       \
            Intersects(RF_##rect_a, RF_##rect_b) == Intersects(RF_##rect_b, RF_##rect_a),          \
            "New_Transitive(RF_" #rect_a "__ect__RF_" #rect_b " " #expected ")");                  \
                                                                                                   \
        BOOST_CHECK_MESSAGE(                                                                       \
            (RI_##rect_a.intersects(RI_##rect_b)) == Intersects(RI_##rect_a, RI_##rect_b),         \
            "SFML==New(RI_" #rect_a "__ect__RI_" #rect_b " " #expected ")");                       \
    }

} // namespace intersects_test_stuff

namespace minimally_enclosing_test_stuff
{

template <typename T1, typename T2>
void test(
    const sf::Rect<T1> & RECT_A,
    const sf::Rect<T2> & RECT_B,
    const bool WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS,
    const sf::Rect<T1> & RECT_EXPECTED,
    const std::string & TEST_NAME)
{
    using Rect_t = sf::Rect<T1>;
    using RectVec_t = std::vector<Rect_t>;

    const RectVec_t V { RECT_A, Rect_t { RECT_B } };
    const RectVec_t V_REV { Rect_t { RECT_B }, RECT_A };

    const auto RESULT { MinimallyEnclosing(
        RECT_A, RECT_B, WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS) };

    const auto RESULT_REV { MinimallyEnclosing(
        RECT_B, RECT_A, WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS) };

    const auto RESULT_VEC { MinimallyEnclosing(V, WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS) };

    const auto RESULT_VEC_REV { MinimallyEnclosing(V_REV, WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS) };

    std::ostringstream ss;
    ss << "{" << TEST_NAME << "}, RECT_A=" << RECT_A << ", RECT_B=" << RECT_B
       << ", WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS=" << WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS
       << ", EXPECTED=" << RECT_EXPECTED;

    const auto MESSAGE_STR = ss.str();

    BOOST_CHECK_MESSAGE(RESULT == RECT_EXPECTED, MESSAGE_STR);
    BOOST_CHECK_MESSAGE(RESULT_REV == RECT_EXPECTED, MESSAGE_STR);
    BOOST_CHECK_MESSAGE(RESULT_VEC == RECT_EXPECTED, MESSAGE_STR);
    BOOST_CHECK_MESSAGE(RESULT_VEC_REV == RECT_EXPECTED, MESSAGE_STR);

    if (WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS)
    {
        const auto RESULT_VEC_WITH_ZEROS1 { MinimallyEnclosing(
            RectVec_t { Rect_t {}, RECT_A, Rect_t { RECT_B } }, true) };

        const auto RESULT_VEC_WITH_ZEROS2 { MinimallyEnclosing(
            RectVec_t { RECT_A, Rect_t {}, Rect_t { RECT_B } }, true) };

        const auto RESULT_VEC_WITH_ZEROS3 { MinimallyEnclosing(
            RectVec_t { RECT_A, Rect_t { RECT_B }, Rect_t {} }, true) };

        const auto RESULT_VEC_WITH_ZEROS1B { MinimallyEnclosing(
            RectVec_t { Rect_t {}, Rect_t {}, RECT_A, Rect_t { RECT_B } }, true) };

        const auto RESULT_VEC_WITH_ZEROS2B { MinimallyEnclosing(
            RectVec_t { RECT_A, Rect_t {}, Rect_t {}, Rect_t { RECT_B } }, true) };

        const auto RESULT_VEC_WITH_ZEROS3B { MinimallyEnclosing(
            RectVec_t { RECT_A, Rect_t { RECT_B }, Rect_t {}, Rect_t {} }, true) };

        const auto RESULT_VEC_WITH_ZEROS4 { MinimallyEnclosing(
            RectVec_t { Rect_t {}, RECT_A, Rect_t {}, Rect_t { RECT_B } }, true) };

        const auto RESULT_VEC_WITH_ZEROS5 { MinimallyEnclosing(
            RectVec_t { Rect_t {}, RECT_A, Rect_t { RECT_B }, Rect_t {} }, true) };

        const auto RESULT_VEC_WITH_ZEROS6 { MinimallyEnclosing(
            RectVec_t { RECT_A, Rect_t {}, Rect_t { RECT_B }, Rect_t {} }, true) };

        const auto RESULT_VEC_WITH_ZEROS4B { MinimallyEnclosing(
            RectVec_t { Rect_t {}, Rect_t {}, RECT_A, Rect_t {}, Rect_t {}, Rect_t { RECT_B } },
            true) };

        const auto RESULT_VEC_WITH_ZEROS5B { MinimallyEnclosing(
            RectVec_t { Rect_t {}, Rect_t {}, RECT_A, Rect_t { RECT_B }, Rect_t {}, Rect_t {} },
            true) };

        const auto RESULT_VEC_WITH_ZEROS6B { MinimallyEnclosing(
            RectVec_t { RECT_A, Rect_t {}, Rect_t {}, Rect_t { RECT_B }, Rect_t {}, Rect_t {} },
            true) };

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS1 == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS2 == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS3 == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS1B == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS2B == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS3B == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS4 == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS5 == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS6 == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS4B == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS5B == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);

        BOOST_CHECK_MESSAGE(
            RESULT_VEC_WITH_ZEROS6B == RECT_EXPECTED,
            "{" << TEST_NAME << "}, WITH_ZEROS, EXPECTED=" << RECT_EXPECTED);
    }
}

template <typename T1, typename T2>
void testWithAndWithoutIgnore(
    const sf::Rect<T1> & RECT_A,
    const sf::Rect<T2> & RECT_B,
    const sf::Rect<T1> & RECT_EXPECTED,
    const std::string & TEST_NAME)
{
    test(RECT_A, RECT_B, false, RECT_EXPECTED, TEST_NAME + " (both=false)");
    test(RECT_A, RECT_B, true, RECT_EXPECTED, TEST_NAME + " (both=true)");
}

} // namespace minimally_enclosing_test_stuff

BOOST_AUTO_TEST_CASE(Type_Helpers_Tests)
{
    BOOST_CHECK_MESSAGE(
        (misc::are_any_v<sf::Sprite, sf::Sprite, sf::Image, sf::Texture, sf::RectangleShape>),
        "are_any_v with two sf::Sprites should be true");

    BOOST_CHECK_MESSAGE(!(has_getglobalbounds_v<int>), "should be false");

    BOOST_CHECK_MESSAGE(
        !(has_getglobalbounds_v<sf::FloatRect>),
        "should be false even though it is an sf::FloatRect");

    BOOST_CHECK_MESSAGE(
        !(has_getglobalbounds_v<sf::IntRect>),
        "should be false even though it is an sf::Rect<int>");

    BOOST_CHECK((has_getglobalbounds_v<sf::Sprite>));
    BOOST_CHECK((has_getglobalbounds_v<sf::RectangleShape>));
}

BOOST_AUTO_TEST_CASE(SFML_Default_Values_Tests)
{
    auto areVectorValuesZero = [](const sf::Vector2f & V) {
        return !((V.x < 0.0f) || (V.x > 0.0f) || (V.y < 0.0f) || (V.y > 0.0f));
    };

    auto areRectValuesZero = [](const sf::FloatRect & R) {
        return !(
            (R.left < 0.0f) || (R.left > 0.0f) || (R.top < 0.0f) || (R.top > 0.0f)
            || (R.width < 0.0f) || (R.width > 0.0f) || (R.height < 0.0f) || (R.height > 0.0f));
    };

    const sf::Vector2f VZ(0.0f, 0.0f);
    BOOST_CHECK(areVectorValuesZero(VZ));

    const sf::FloatRect RZ(0.0f, 0.0f, 0.0f, 0.0f);
    BOOST_CHECK(areRectValuesZero(RZ));

    //

    BOOST_CHECK(areVectorValuesZero(sf::Vector2f()));
    BOOST_CHECK(sf::Vector2f() == VZ);

    BOOST_CHECK(areRectValuesZero(sf::FloatRect()));
    BOOST_CHECK(sf::FloatRect() == RZ);

    //

    BOOST_CHECK(areVectorValuesZero(sf::Vector2f {}));
    BOOST_CHECK(sf::Vector2f {} == VZ);

    BOOST_CHECK(areRectValuesZero(sf::FloatRect {}));
    BOOST_CHECK(sf::FloatRect {} == RZ);

    //

    sf::Vector2f v1;
    BOOST_CHECK(areVectorValuesZero(v1));
    BOOST_CHECK(v1 == VZ);

    sf::FloatRect r1;
    BOOST_CHECK(areRectValuesZero(r1));
    BOOST_CHECK(r1 == RZ);

    //

    sf::Vector2f v2 {};
    BOOST_CHECK(areVectorValuesZero(v2));
    BOOST_CHECK(v2 == VZ);

    sf::FloatRect r2 {};
    BOOST_CHECK(areRectValuesZero(r2));
    BOOST_CHECK(r2 == RZ);

    //

    {
        struct NoInit
        {
            sf::Vector2f v;
            sf::FloatRect r;
        };
        NoInit ni;

        BOOST_CHECK(areVectorValuesZero(ni.v));
        BOOST_CHECK(ni.v == VZ);

        BOOST_CHECK(areRectValuesZero(ni.r));
        BOOST_CHECK(ni.r == RZ);

        {
            std::vector<NoInit> vec;
            vec.resize(4096);
            for (const auto & NI : vec)
            {
                BOOST_CHECK(areVectorValuesZero(NI.v));
                BOOST_CHECK(NI.v == VZ);

                BOOST_CHECK(areRectValuesZero(NI.r));
                BOOST_CHECK(NI.r == RZ);
            }
        }

        {
            std::vector<NoInit> vec {};
            vec.resize(4096);
            for (const auto & NI : vec)
            {
                BOOST_CHECK(areVectorValuesZero(NI.v));
                BOOST_CHECK(NI.v == VZ);

                BOOST_CHECK(areRectValuesZero(NI.r));
                BOOST_CHECK(NI.r == RZ);
            }
        }
    }

    {
        struct DefaultInit
        {
            DefaultInit()
                : v()
                , r()
            {}

            sf::Vector2f v;
            sf::FloatRect r;
        };

        {
            DefaultInit di;

            BOOST_CHECK(areVectorValuesZero(di.v));
            BOOST_CHECK(di.v == VZ);

            BOOST_CHECK(areRectValuesZero(di.r));
            BOOST_CHECK(di.r == RZ);
        }

        {
            DefaultInit di2 {};

            BOOST_CHECK(areVectorValuesZero(di2.v));
            BOOST_CHECK(di2.v == VZ);

            BOOST_CHECK(areRectValuesZero(di2.r));
            BOOST_CHECK(di2.r == RZ);
        }

        {
            BOOST_CHECK(areVectorValuesZero(DefaultInit().v));
            BOOST_CHECK(DefaultInit().v == VZ);

            BOOST_CHECK(areRectValuesZero(DefaultInit().r));
            BOOST_CHECK(DefaultInit().r == RZ);
        }

        {
            BOOST_CHECK(areVectorValuesZero(DefaultInit {}.v));
            BOOST_CHECK(DefaultInit {}.v == VZ);

            BOOST_CHECK(areRectValuesZero(DefaultInit {}.r));
            BOOST_CHECK(DefaultInit {}.r == RZ);
        }
    }

    {
        struct UniformInit
        {
            UniformInit()
                : v {}
                , r {}
            {}

            sf::Vector2f v;
            sf::FloatRect r;
        };

        {
            UniformInit ui;

            BOOST_CHECK(areVectorValuesZero(ui.v));
            BOOST_CHECK(ui.v == VZ);

            BOOST_CHECK(areRectValuesZero(ui.r));
            BOOST_CHECK(ui.r == RZ);
        }

        {
            UniformInit ui {};

            BOOST_CHECK(areVectorValuesZero(ui.v));
            BOOST_CHECK(ui.v == VZ);

            BOOST_CHECK(areRectValuesZero(ui.r));
            BOOST_CHECK(ui.r == RZ);
        }

        {
            BOOST_CHECK(areVectorValuesZero(UniformInit().v));
            BOOST_CHECK(UniformInit().v == VZ);

            BOOST_CHECK(areRectValuesZero(UniformInit().r));
            BOOST_CHECK(UniformInit().r == RZ);
        }

        {
            BOOST_CHECK(areVectorValuesZero(UniformInit {}.v));
            BOOST_CHECK(UniformInit {}.v == VZ);

            BOOST_CHECK(areRectValuesZero(UniformInit {}.r));
            BOOST_CHECK(UniformInit {}.r == RZ);
        }
    }
}

BOOST_AUTO_TEST_CASE(vector_mult)
{
    const sf::Vector2f V0F(0.0f, 0.0f);
    const sf::Vector2i V0I(V0F);

    const sf::Vector2f V1F(1.0f, 1.0f);
    const sf::Vector2i V1I(V1F);

    const sf::Vector2f VN1F(-1.0f, -1.0f);
    const sf::Vector2i VN1I(VN1F);

    const float HALF(0.5f);
    const float DOUBLE(2.0f);

    const sf::Vector2f VHF(HALF, HALF);

    const sf::Vector2f VDF(DOUBLE, DOUBLE);
    const sf::Vector2i VDI(VDF);

    const sf::Vector2f VRF(123.123f, 789.789f);
    const sf::Vector2i VRI(V1F);

    const sf::Vector2f VRHF(VRF * HALF);
    const sf::Vector2i VRHI(VRI / 2);

    const sf::Vector2f VRDF(VRF * DOUBLE);
    const sf::Vector2i VRDI(VRI * 2);

    const sf::Vector2f VRRF(VRF.x * VRF.x, VRF.y * VRF.y);
    const sf::Vector2i VRRI(VRI.x * VRI.x, VRI.x * VRI.x);

    BOOST_CHECK(VectorMult(V0F, V0F) == V0F);
    BOOST_CHECK(VectorMult(V1F, V0F) == V0F);
    BOOST_CHECK(VectorMult(VHF, V0F) == V0F);
    BOOST_CHECK(VectorMult(VDF, V0F) == V0F);
    BOOST_CHECK(VectorMult(V0F, V1F) == V0F);
    BOOST_CHECK(VectorMult(V1F, V1F) == V1F);
    BOOST_CHECK(VectorMult(VHF, V1F) == VHF);
    BOOST_CHECK(VectorMult(VDF, V1F) == VDF);
    BOOST_CHECK(VectorMult(VRF, V0F) == V0F);
    BOOST_CHECK(VectorMult(VRF, V1F) == VRF);
    BOOST_CHECK(VectorMult(VRF, VHF) == VRHF);
    BOOST_CHECK(VectorMult(VRF, VDF) == VRDF);
    BOOST_CHECK(VectorMult(VRF, VRF) == VRRF);

    BOOST_CHECK(VectorMult(V0I, V0F) == V0I);
    BOOST_CHECK(VectorMult(V1I, V0F) == V0I);
    BOOST_CHECK(VectorMult(VDI, V0F) == V0I);
    BOOST_CHECK(VectorMult(V0I, V1F) == V0I);
    BOOST_CHECK(VectorMult(V1I, V1F) == V1I);
    BOOST_CHECK(VectorMult(VDI, V1F) == VDI);
    BOOST_CHECK(VectorMult(VRI, V0F) == V0I);
    BOOST_CHECK(VectorMult(VRI, V1F) == VRI);
    BOOST_CHECK(VectorMult(VRI, VHF) == VRHI);
    BOOST_CHECK(VectorMult(VRI, VDF) == VRDI);

    const auto RIRFIx { int(double(VRI.x) * double(VRF.x)) };
    const auto RIRFIy { int(double(VRI.y) * double(VRF.y)) };
    const sf::Vector2i V_RIRFI(RIRFIx, RIRFIy);
    BOOST_CHECK(VectorMult(VRI, VRF) == V_RIRFI);
    BOOST_CHECK(VectorMult(sf::Vector2i(10, 1000000), sf::Vector2f(0.1f, 0.000001f)) == V1I);
    BOOST_CHECK(VectorMult(sf::Vector2i(10, 1000000), sf::Vector2f(-0.1f, -0.000001f)) == VN1I);
    BOOST_CHECK(VectorMult(sf::Vector2i(-10, -1000000), sf::Vector2f(0.1f, 0.000001f)) == VN1I);
    BOOST_CHECK(VectorMult(sf::Vector2i(-10, -1000000), sf::Vector2f(-0.1f, -0.000001f)) == V1I);
}

BOOST_AUTO_TEST_CASE(ColorMathTests)
{
    const sf::Color C0(0, 0, 0, 0);
    const sf::Color C1(1, 1, 1, 1);
    const sf::Color C254(254, 254, 254, 254);
    const sf::Color C255(255, 255, 255, 255);

    // sanity
    BOOST_CHECK((C0 + C0) == C0);
    BOOST_CHECK((C0 - C0) == C0);
    BOOST_CHECK((C0 + C1) == C1);
    BOOST_CHECK((C1 + C0) == C1);
    BOOST_CHECK((C1 - C1) == C0);
    BOOST_CHECK((C254 + C1) == C255);
    BOOST_CHECK((C255 - C1) == C254);

    // overflow and underflow should NOT wrap
    BOOST_CHECK((C255 + C1) == C255);
    BOOST_CHECK((C254 + C255) == C255);
    BOOST_CHECK((C0 - C1) == C0);
    BOOST_CHECK((C0 - C255) == C0);
}

BOOST_AUTO_TEST_CASE(VectorAndRectTests)
{
    const sf::Vector2f V0(0.0f, 0.0f);
    const sf::Vector2f V1(1.0f, 1.0f);
    const sf::Vector2f VN(-1.0f, -1.0f);
    const sf::FloatRect R0(V0, V0);
    const sf::FloatRect R1(V1, V1);
    const sf::FloatRect RN(VN, VN);

    {
        auto testRect { R0 };
        SetPosition(testRect, V1);
        BOOST_CHECK(testRect == sf::FloatRect(V1, V0));
    }

    {
        auto testRect { R0 };
        SetSize(testRect, V1);
        BOOST_CHECK(testRect == sf::FloatRect(V0, V1));
    }

    {
        auto testRect { RN };
        AdjustPosition(testRect, V1);
        BOOST_CHECK(testRect == sf::FloatRect(V0, VN));
    }

    {
        auto testRect { R1 };
        AdjustSize(testRect, VN);
        BOOST_CHECK(testRect == sf::FloatRect(V1, V0));
    }

    {
        auto testRect { R1 };
        Move(testRect, VN);
        BOOST_CHECK(testRect == sf::FloatRect(V0, V1));
    }
}

BOOST_AUTO_TEST_CASE(Vertex_Tests)
{
    const sf::Vector2f V0(0.0f, 0.0f);
    const sf::Vector2f V1(1.0f, 1.0f);
    const sf::Vector2f VN1(-1.0f, -1.0f);
    const sf::Color CT(sf::Color::Transparent);
    const sf::Vertex VX0(V0, CT, V0);
    const sf::Vertex VX1(V1, CT, V1);

    BOOST_CHECK(VX0 == VX0);
    BOOST_CHECK((VX0 != VX0) == false);
    BOOST_CHECK(VX0 != VX1);
    BOOST_CHECK((VX0 == VX1) == false);

    BOOST_CHECK(VX0 < VX1);
    BOOST_CHECK((VX1 < VX0) == false);
    BOOST_CHECK((VX0 < VX0) == false);

    const sf::VertexArray VA_Q_E(sf::PrimitiveType::Quads);
    BOOST_CHECK(VA_Q_E == VA_Q_E);
    BOOST_CHECK((VA_Q_E != VA_Q_E) == false);
    BOOST_CHECK((VA_Q_E < VA_Q_E) == false);

    BOOST_CHECK(VA_Q_E.getBounds() == sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f));

    const sf::VertexArray VA_L_E(sf::PrimitiveType::Lines);
    BOOST_CHECK(VA_Q_E != VA_L_E);
    BOOST_CHECK((VA_Q_E == VA_L_E) == false);
    BOOST_CHECK(VA_L_E < VA_Q_E);
    BOOST_CHECK((VA_Q_E < VA_L_E) == false);

    auto makeVertexArray = [&](const sf::PrimitiveType PRIMITIVE_TYPE,
                               const sf::Vector2f & V,
                               const std::size_t SIZE) {
        sf::VertexArray vertexArray(PRIMITIVE_TYPE, SIZE);
        for (std::size_t i(0); i < SIZE; ++i)
        {
            vertexArray[i] = sf::Vertex(V, CT, V);
        }
        return vertexArray;
    };

    const sf::VertexArray VA_Q_0_10(makeVertexArray(sf::PrimitiveType::Quads, V0, 10));

    BOOST_CHECK(VA_Q_E < VA_Q_0_10);
    BOOST_CHECK(VA_Q_E != VA_Q_0_10);
    BOOST_CHECK(VA_L_E < VA_Q_0_10);
    BOOST_CHECK(VA_L_E != VA_Q_0_10);

    const sf::VertexArray VA_Q_0_11(makeVertexArray(sf::PrimitiveType::Quads, V0, 11));

    BOOST_CHECK(VA_Q_0_10 != VA_Q_0_11);
    BOOST_CHECK(VA_Q_0_10 < VA_Q_0_11);

    const sf::VertexArray VA_L_0_10(makeVertexArray(sf::PrimitiveType::Lines, V0, 10));

    BOOST_CHECK(VA_L_0_10 != VA_Q_0_10);
    BOOST_CHECK(VA_L_0_10 < VA_Q_0_10);

    const sf::VertexArray VA_Q_1_10(makeVertexArray(sf::PrimitiveType::Quads, V1, 10));

    BOOST_CHECK(VA_Q_0_10 != VA_Q_1_10);
    BOOST_CHECK(VA_Q_0_10 < VA_Q_1_10);

    //

    BOOST_CHECK(CreateVector(Orientation::Horiz, 0.0f, 1.0f) == sf::Vector2f(0.0f, 1.0f));
    BOOST_CHECK(CreateVector(Orientation::Vert, 0.0f, 1.0f) == sf::Vector2f(1.0f, 0.0f));
    BOOST_CHECK(CreateVector(Orientation::Both, 0.0f, 1.0f) == sf::Vector2f(0.0f, 0.0f));
    BOOST_CHECK(CreateVector(Orientation::Count, 0.0f, 1.0f) == sf::Vector2f(1.0f, 1.0f));

    //

    {
        const sf::Vector2i TEST_V_I(123, 456);
        BOOST_CHECK(ChangeVectorCopy(TEST_V_I, Orientation::Horiz, 789) == sf::Vector2i(789, 456));
        BOOST_CHECK(ChangeVectorCopy(TEST_V_I, Orientation::Vert, 789) == sf::Vector2i(123, 789));
        BOOST_CHECK(ChangeVectorCopy(TEST_V_I, Orientation::Both, 789) == sf::Vector2i(789, 789));
        BOOST_CHECK(ChangeVectorCopy(TEST_V_I, Orientation::Count, 789) == TEST_V_I);
    }

    //

    const sf::Vector2f POS_10_V { 10.0f, 10.0f };
    const sf::Vector2f SIZE_10_V { 10.0f, 10.0f };
    const sf::Vector2f SIZE_20_V { 20.0f, 20.0f };
    const sf::FloatRect RECT_10 { 10.0f, 10.0f, 10.0f, 10.0f };

    const std::vector<sf::Vector2f> POS_VEC_10_10 {
        { 10.0f, 10.0f }, { 20.0f, 10.0f }, { 20.0f, 20.0f }, { 10.0f, 20.0f }
    };
    const std::vector<sf::Vector2f> POS_VEC_EMPTY;

    for (heroespath::EnumUnderlying_t i(0); i <= Orientation::Count; ++i)
    {
        const Orientation::Enum ORIENTATION { i };

        BOOST_CHECK(MakeQuadVertexPositions(POS_10_V, SIZE_10_V, V0, ORIENTATION) == POS_VEC_10_10);

        BOOST_CHECK(MakeQuadVertexPositions(POS_10_V, SIZE_10_V, VN1, ORIENTATION).empty());

        BOOST_CHECK(
            MakeQuadVertexPositions(POS_10_V, SIZE_10_V, SIZE_10_V, ORIENTATION) == POS_VEC_10_10);

        BOOST_CHECK(
            MakeQuadVertexPositions(POS_10_V, SIZE_10_V, SIZE_20_V, ORIENTATION) == POS_VEC_10_10);
    }

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, SIZE_10_V, Orientation::Count)
        == POS_VEC_10_10);

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, SIZE_20_V, Orientation::Count)
        == POS_VEC_10_10);

    {
        const std::vector<sf::Vector2f> POS_VEC_10_10_LIMIT_0_1 {
            { 10.0f, 10.0f }, { 20.0f, 10.0f }, { 20.0f, 11.0f }, { 10.0f, 11.0f }
        };

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(0.0f, 1.0f), Orientation::Count)
            == POS_VEC_10_10_LIMIT_0_1);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(999.0f, 1.0f), Orientation::Count)
            == POS_VEC_10_10_LIMIT_0_1);
    }

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(0.0f, 10.0f), Orientation::Horiz)
        == POS_VEC_10_10);

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(0.0f, 999.0f), Orientation::Horiz)
        == POS_VEC_10_10);

    {
        const std::vector<sf::Vector2f> POS_VEC_10_10_LIMIT_1_0 {
            { 10.0f, 10.0f }, { 11.0f, 10.0f }, { 11.0f, 20.0f }, { 10.0f, 20.0f }
        };

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(1.0f, 0.0f), Orientation::Count)
            == POS_VEC_10_10_LIMIT_1_0);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(1.0f, 999.0f), Orientation::Count)
            == POS_VEC_10_10_LIMIT_1_0);
    }

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(10.0f, 0.0f), Orientation::Vert)
        == POS_VEC_10_10);

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(999.0f, 0.0f), Orientation::Vert)
        == POS_VEC_10_10);

    //

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(2.0f, 3.0f), Orientation::Count)
        == (std::vector<sf::Vector2f> {
               { 10.0f, 10.0f }, { 12.0f, 10.0f }, { 12.0f, 13.0f }, { 10.0f, 13.0f } }));

    {
        const std::vector<sf::Vector2f> POS_VEC_10_10_LIMIT_E4_0 {
            { 16.0f, 10.0f }, { 20.0f, 10.0f }, { 20.0f, 20.0f }, { 16.0f, 20.0f }
        };

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 0.0f), Orientation::Horiz)
            == POS_VEC_10_10_LIMIT_E4_0);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 0.0f), Orientation::Both)
            == POS_VEC_10_10_LIMIT_E4_0);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 999.0f), Orientation::Horiz)
            == POS_VEC_10_10_LIMIT_E4_0);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 999.0f), Orientation::Both)
            == POS_VEC_10_10_LIMIT_E4_0);
    }

    {
        const std::vector<sf::Vector2f> POS_VEC_10_10_LIMIT_0_E4 {
            { 10.0f, 16.0f }, { 20.0f, 16.0f }, { 20.0f, 20.0f }, { 10.0f, 20.0f }
        };

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(0.0f, 4.0f), Orientation::Vert)
            == POS_VEC_10_10_LIMIT_0_E4);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(0.0f, 4.0f), Orientation::Both)
            == POS_VEC_10_10_LIMIT_0_E4);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(999.0f, 4.0f), Orientation::Vert)
            == POS_VEC_10_10_LIMIT_0_E4);

        BOOST_CHECK(
            MakeQuadVertexPositions(
                POS_10_V, SIZE_10_V, sf::Vector2f(999.0f, 4.0f), Orientation::Both)
            == POS_VEC_10_10_LIMIT_0_E4);
    }

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 6.0f), Orientation::Horiz)
        == (std::vector<sf::Vector2f> {
               { 16.0f, 10.0f }, { 20.0f, 10.0f }, { 20.0f, 16.0f }, { 16.0f, 16.0f } }));

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 6.0f), Orientation::Vert)
        == (std::vector<sf::Vector2f> {
               { 10.0f, 14.0f }, { 14.0f, 14.0f }, { 14.0f, 20.0f }, { 10.0f, 20.0f } }));

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 6.0f), Orientation::Both)
        == (std::vector<sf::Vector2f> {
               { 16.0f, 14.0f }, { 20.0f, 14.0f }, { 20.0f, 20.0f }, { 16.0f, 20.0f } }));

    BOOST_CHECK(
        MakeQuadVertexPositions(POS_10_V, SIZE_10_V, sf::Vector2f(4.0f, 6.0f), Orientation::Count)
        == (std::vector<sf::Vector2f> {
               { 10.0f, 10.0f }, { 14.0f, 10.0f }, { 14.0f, 16.0f }, { 10.0f, 16.0f } }));

    //

    sf::VertexArray va;

    //

    AppendVertexesForQuad(
        va, POS_10_V, RECT_10, sf::Color::Black, sf::Vector2f(-4.0f, 6.0f), Orientation::Count);

    BOOST_CHECK(va.getVertexCount() == 0);

    AppendVertexesForQuad(
        va, POS_10_V, RECT_10, sf::Color::Black, sf::Vector2f(4.0f, -6.0f), Orientation::Count);

    BOOST_CHECK(va.getVertexCount() == 0);

    //

    va.clear();

    AppendVertexesForQuad(
        va, POS_10_V, RECT_10, sf::Color::Black, sf::Vector2f(4.0f, 6.0f), Orientation::Count);

    BOOST_CHECK(va.getVertexCount() == 4);

    BOOST_CHECK(va[0].color == sf::Color::Black);
    BOOST_CHECK(va[1].color == sf::Color::Black);
    BOOST_CHECK(va[2].color == sf::Color::Black);
    BOOST_CHECK(va[3].color == sf::Color::Black);

    BOOST_CHECK(va[0].position == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].position == sf::Vector2f(14.0f, 10.0f));
    BOOST_CHECK(va[2].position == sf::Vector2f(14.0f, 16.0f));
    BOOST_CHECK(va[3].position == sf::Vector2f(10.0f, 16.0f));

    BOOST_CHECK(va[0].texCoords == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].texCoords == sf::Vector2f(14.0f, 10.0f));
    BOOST_CHECK(va[2].texCoords == sf::Vector2f(14.0f, 16.0f));
    BOOST_CHECK(va[3].texCoords == sf::Vector2f(10.0f, 16.0f));

    //

    va.clear();

    AppendVertexesForQuad(
        va, POS_10_V, RECT_10, sf::Color::Green, sf::Vector2f(4.0f, 6.0f), Orientation::Both);

    BOOST_CHECK(va.getVertexCount() == 4);

    BOOST_CHECK(va[0].color == sf::Color::Green);
    BOOST_CHECK(va[1].color == sf::Color::Green);
    BOOST_CHECK(va[2].color == sf::Color::Green);
    BOOST_CHECK(va[3].color == sf::Color::Green);

    BOOST_CHECK(va[0].position == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].position == sf::Vector2f(14.0f, 10.0f));
    BOOST_CHECK(va[2].position == sf::Vector2f(14.0f, 16.0f));
    BOOST_CHECK(va[3].position == sf::Vector2f(10.0f, 16.0f));

    BOOST_CHECK(va[0].texCoords == sf::Vector2f(16.0f, 14.0f));
    BOOST_CHECK(va[1].texCoords == sf::Vector2f(20.0f, 14.0f));
    BOOST_CHECK(va[2].texCoords == sf::Vector2f(20.0f, 20.0f));
    BOOST_CHECK(va[3].texCoords == sf::Vector2f(16.0f, 20.0f));

    //

    va.clear();

    AppendVertexesForQuadRepeatedOverLength(va, POS_10_V, RECT_10, ::Orientation::Count, 25.0f);

    BOOST_CHECK(va.getVertexCount() == 0);

    AppendVertexesForQuadRepeatedOverLength(va, POS_10_V, RECT_10, ::Orientation::Horiz, 0.0f);

    BOOST_CHECK(va.getVertexCount() == 0);

    //

    va.clear();

    AppendVertexesForQuadRepeatedOverLength(
        va, POS_10_V, RECT_10, ::Orientation::Horiz, 5.0f, sf::Color::Red, 5.0f);

    BOOST_CHECK(va.getVertexCount() == 4);

    BOOST_CHECK(va[0].color == sf::Color::Red);
    BOOST_CHECK(va[1].color == sf::Color::Red);
    BOOST_CHECK(va[2].color == sf::Color::Red);
    BOOST_CHECK(va[3].color == sf::Color::Red);

    BOOST_CHECK(va[0].position == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].position == sf::Vector2f(15.0f, 10.0f));
    BOOST_CHECK(va[2].position == sf::Vector2f(15.0f, 15.0f));
    BOOST_CHECK(va[3].position == sf::Vector2f(10.0f, 15.0f));

    BOOST_CHECK(va[0].texCoords == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].texCoords == sf::Vector2f(15.0f, 10.0f));
    BOOST_CHECK(va[2].texCoords == sf::Vector2f(15.0f, 15.0f));
    BOOST_CHECK(va[3].texCoords == sf::Vector2f(10.0f, 15.0f));

    //

    va.clear();

    AppendVertexesForQuadRepeatedOverLength(
        va,
        POS_10_V,
        RECT_10,
        ::Orientation::Horiz,
        5.0f,
        sf::Color::White,
        999.0f,
        Orientation::Both);

    BOOST_CHECK(va.getVertexCount() == 4);

    BOOST_CHECK(va[0].position == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].position == sf::Vector2f(15.0f, 10.0f));
    BOOST_CHECK(va[2].position == sf::Vector2f(15.0f, 20.0f));
    BOOST_CHECK(va[3].position == sf::Vector2f(10.0f, 20.0f));

    BOOST_CHECK(va[0].texCoords == sf::Vector2f(15.0f, 10.0f));
    BOOST_CHECK(va[1].texCoords == sf::Vector2f(20.0f, 10.0f));
    BOOST_CHECK(va[2].texCoords == sf::Vector2f(20.0f, 20.0f));
    BOOST_CHECK(va[3].texCoords == sf::Vector2f(15.0f, 20.0f));

    //

    va.clear();

    AppendVertexesForQuadRepeatedOverLength(
        va,
        POS_10_V,
        RECT_10,
        ::Orientation::Horiz,
        25.0f,
        sf::Color::White,
        5.0f,
        Orientation::Vert);

    BOOST_CHECK(va.getVertexCount() == 12);

    BOOST_CHECK(va[0].position == sf::Vector2f(10.0f, 10.0f));
    BOOST_CHECK(va[1].position == sf::Vector2f(20.0f, 10.0f));
    BOOST_CHECK(va[2].position == sf::Vector2f(20.0f, 15.0f));
    BOOST_CHECK(va[3].position == sf::Vector2f(10.0f, 15.0f));
    //
    BOOST_CHECK(va[4].position == sf::Vector2f(20.0f, 10.0f));
    BOOST_CHECK(va[5].position == sf::Vector2f(30.0f, 10.0f));
    BOOST_CHECK(va[6].position == sf::Vector2f(30.0f, 15.0f));
    BOOST_CHECK(va[7].position == sf::Vector2f(20.0f, 15.0f));
    //
    BOOST_CHECK(va[8].position == sf::Vector2f(30.0f, 10.0f));
    BOOST_CHECK(va[9].position == sf::Vector2f(35.0f, 10.0f));
    BOOST_CHECK(va[10].position == sf::Vector2f(35.0f, 15.0f));
    BOOST_CHECK(va[11].position == sf::Vector2f(30.0f, 15.0f));

    BOOST_CHECK(va[0].texCoords == sf::Vector2f(10.0f, 15.0f));
    BOOST_CHECK(va[1].texCoords == sf::Vector2f(20.0f, 15.0f));
    BOOST_CHECK(va[2].texCoords == sf::Vector2f(20.0f, 20.0f));
    BOOST_CHECK(va[3].texCoords == sf::Vector2f(10.0f, 20.0f));
    //
    BOOST_CHECK(va[4].texCoords == sf::Vector2f(10.0f, 15.0f));
    BOOST_CHECK(va[5].texCoords == sf::Vector2f(20.0f, 15.0f));
    BOOST_CHECK(va[6].texCoords == sf::Vector2f(20.0f, 20.0f));
    BOOST_CHECK(va[7].texCoords == sf::Vector2f(10.0f, 20.0f));
    //
    BOOST_CHECK(va[8].texCoords == sf::Vector2f(10.0f, 15.0f));
    BOOST_CHECK(va[9].texCoords == sf::Vector2f(15.0f, 15.0f));
    BOOST_CHECK(va[10].texCoords == sf::Vector2f(15.0f, 20.0f));
    BOOST_CHECK(va[11].texCoords == sf::Vector2f(10.0f, 20.0f));
}

BOOST_AUTO_TEST_CASE(ZeroOrLessTests)
{
    const float NEGATIVE(-0.0001f);
    const float ZERO(0.0f);
    const float POSITIVE(0.0001f);
    const float ONE(1.0f);
    const float LARGER(1.0001f);

    const sf::Vector2f ZNV(ZERO, NEGATIVE);
    const sf::Vector2f ZZV(ZERO, ZERO);
    const sf::Vector2f ZPV(ZERO, POSITIVE);
    const sf::Vector2f ZOV(ZERO, ONE);
    const sf::Vector2f ZLV(ZERO, LARGER);
    const sf::Vector2f NNV(NEGATIVE, NEGATIVE);
    const sf::Vector2f NZV(NEGATIVE, ZERO);
    const sf::Vector2f NPV(NEGATIVE, POSITIVE);
    const sf::Vector2f NOV(NEGATIVE, ONE);
    const sf::Vector2f NLV(NEGATIVE, LARGER);
    const sf::Vector2f PNV(POSITIVE, NEGATIVE);
    const sf::Vector2f PZV(POSITIVE, ZERO);
    const sf::Vector2f PPV(POSITIVE, POSITIVE);
    const sf::Vector2f POV(POSITIVE, ONE);
    const sf::Vector2f PLV(POSITIVE, LARGER);
    const sf::Vector2f ONV(ONE, NEGATIVE);
    const sf::Vector2f OZV(ONE, ZERO);
    const sf::Vector2f OPV(ONE, POSITIVE);
    const sf::Vector2f OOV(ONE, ONE);
    const sf::Vector2f OLV(ONE, LARGER);
    const sf::Vector2f LNV(LARGER, NEGATIVE);
    const sf::Vector2f LZV(LARGER, ZERO);
    const sf::Vector2f LPV(LARGER, POSITIVE);
    const sf::Vector2f LOV(LARGER, ONE);
    const sf::Vector2f LLV(LARGER, LARGER);

    BOOST_CHECK(IsZeroOrLessAny(ZNV));
    BOOST_CHECK(IsZeroOrLessAny(ZZV));
    BOOST_CHECK(IsZeroOrLessAny(ZPV));
    BOOST_CHECK(IsZeroOrLessAny(ZOV));
    BOOST_CHECK(IsZeroOrLessAny(ZLV));
    BOOST_CHECK(IsZeroOrLessAny(NNV));
    BOOST_CHECK(IsZeroOrLessAny(NZV));
    BOOST_CHECK(IsZeroOrLessAny(NPV));
    BOOST_CHECK(IsZeroOrLessAny(NOV));
    BOOST_CHECK(IsZeroOrLessAny(NLV));
    BOOST_CHECK(IsZeroOrLessAny(PNV));
    BOOST_CHECK(IsZeroOrLessAny(PZV));
    BOOST_CHECK(IsZeroOrLessAny(PPV) == false);
    BOOST_CHECK(IsZeroOrLessAny(POV) == false);
    BOOST_CHECK(IsZeroOrLessAny(PLV) == false);
    BOOST_CHECK(IsZeroOrLessAny(ONV));
    BOOST_CHECK(IsZeroOrLessAny(OZV));
    BOOST_CHECK(IsZeroOrLessAny(OPV) == false);
    BOOST_CHECK(IsZeroOrLessAny(OOV) == false);
    BOOST_CHECK(IsZeroOrLessAny(OLV) == false);
    BOOST_CHECK(IsZeroOrLessAny(LNV));
    BOOST_CHECK(IsZeroOrLessAny(LZV));
    BOOST_CHECK(IsZeroOrLessAny(LPV) == false);
    BOOST_CHECK(IsZeroOrLessAny(LOV) == false);
    BOOST_CHECK(IsZeroOrLessAny(LLV) == false);

    BOOST_CHECK(IsZeroOrLessAll(ZNV));
    BOOST_CHECK(IsZeroOrLessAll(ZZV));
    BOOST_CHECK(IsZeroOrLessAll(ZPV) == false);
    BOOST_CHECK(IsZeroOrLessAll(ZOV) == false);
    BOOST_CHECK(IsZeroOrLessAll(ZLV) == false);
    BOOST_CHECK(IsZeroOrLessAll(NNV));
    BOOST_CHECK(IsZeroOrLessAll(NZV));
    BOOST_CHECK(IsZeroOrLessAll(NPV) == false);
    BOOST_CHECK(IsZeroOrLessAll(NOV) == false);
    BOOST_CHECK(IsZeroOrLessAll(NLV) == false);
    BOOST_CHECK(IsZeroOrLessAll(PNV) == false);
    BOOST_CHECK(IsZeroOrLessAll(PZV) == false);
    BOOST_CHECK(IsZeroOrLessAll(PPV) == false);
    BOOST_CHECK(IsZeroOrLessAll(POV) == false);
    BOOST_CHECK(IsZeroOrLessAll(PLV) == false);
    BOOST_CHECK(IsZeroOrLessAll(ONV) == false);
    BOOST_CHECK(IsZeroOrLessAll(OZV) == false);
    BOOST_CHECK(IsZeroOrLessAll(OPV) == false);
    BOOST_CHECK(IsZeroOrLessAll(OOV) == false);
    BOOST_CHECK(IsZeroOrLessAll(OLV) == false);
    BOOST_CHECK(IsZeroOrLessAll(LNV) == false);
    BOOST_CHECK(IsZeroOrLessAll(LZV) == false);
    BOOST_CHECK(IsZeroOrLessAll(LPV) == false);
    BOOST_CHECK(IsZeroOrLessAll(LOV) == false);
    BOOST_CHECK(IsZeroOrLessAll(LLV) == false);

    BOOST_CHECK(IsLessThanOneAny(ZNV));
    BOOST_CHECK(IsLessThanOneAny(ZZV));
    BOOST_CHECK(IsLessThanOneAny(ZPV));
    BOOST_CHECK(IsLessThanOneAny(ZOV));
    BOOST_CHECK(IsLessThanOneAny(ZLV));
    BOOST_CHECK(IsLessThanOneAny(NNV));
    BOOST_CHECK(IsLessThanOneAny(NZV));
    BOOST_CHECK(IsLessThanOneAny(NPV));
    BOOST_CHECK(IsLessThanOneAny(NOV));
    BOOST_CHECK(IsLessThanOneAny(NLV));
    BOOST_CHECK(IsLessThanOneAny(PNV));
    BOOST_CHECK(IsLessThanOneAny(PZV));
    BOOST_CHECK(IsLessThanOneAny(PPV));
    BOOST_CHECK(IsLessThanOneAny(POV));
    BOOST_CHECK(IsLessThanOneAny(PLV));
    BOOST_CHECK(IsLessThanOneAny(ONV));
    BOOST_CHECK(IsLessThanOneAny(OZV));
    BOOST_CHECK(IsLessThanOneAny(OPV));
    BOOST_CHECK(IsLessThanOneAny(OOV) == false);
    BOOST_CHECK(IsLessThanOneAny(OLV) == false);
    BOOST_CHECK(IsLessThanOneAny(LNV));
    BOOST_CHECK(IsLessThanOneAny(LZV));
    BOOST_CHECK(IsLessThanOneAny(LPV));
    BOOST_CHECK(IsLessThanOneAny(LOV) == false);
    BOOST_CHECK(IsLessThanOneAny(LLV) == false);

    BOOST_CHECK(IsLessThanOneAll(ZNV));
    BOOST_CHECK(IsLessThanOneAll(ZZV));
    BOOST_CHECK(IsLessThanOneAll(ZPV));
    BOOST_CHECK(IsLessThanOneAll(ZOV) == false);
    BOOST_CHECK(IsLessThanOneAll(ZLV) == false);
    BOOST_CHECK(IsLessThanOneAll(NNV));
    BOOST_CHECK(IsLessThanOneAll(NZV));
    BOOST_CHECK(IsLessThanOneAll(NPV));
    BOOST_CHECK(IsLessThanOneAll(NOV) == false);
    BOOST_CHECK(IsLessThanOneAll(NLV) == false);
    BOOST_CHECK(IsLessThanOneAll(PNV));
    BOOST_CHECK(IsLessThanOneAll(PZV));
    BOOST_CHECK(IsLessThanOneAll(PPV));
    BOOST_CHECK(IsLessThanOneAll(POV) == false);
    BOOST_CHECK(IsLessThanOneAll(PLV) == false);
    BOOST_CHECK(IsLessThanOneAll(ONV) == false);
    BOOST_CHECK(IsLessThanOneAll(OZV) == false);
    BOOST_CHECK(IsLessThanOneAll(OPV) == false);
    BOOST_CHECK(IsLessThanOneAll(OOV) == false);
    BOOST_CHECK(IsLessThanOneAll(OLV) == false);
    BOOST_CHECK(IsLessThanOneAll(LNV) == false);
    BOOST_CHECK(IsLessThanOneAll(LZV) == false);
    BOOST_CHECK(IsLessThanOneAll(LPV) == false);
    BOOST_CHECK(IsLessThanOneAll(LOV) == false);
    BOOST_CHECK(IsLessThanOneAll(LLV) == false);

    const sf::FloatRect ZNR(ZZV, ZNV);
    const sf::FloatRect ZZR(ZZV, ZZV);
    const sf::FloatRect ZPR(ZZV, ZPV);
    const sf::FloatRect ZOR(ZZV, ZOV);
    const sf::FloatRect ZLR(ZZV, ZLV);
    const sf::FloatRect NNR(ZZV, NNV);
    const sf::FloatRect NZR(ZZV, NZV);
    const sf::FloatRect NPR(ZZV, NPV);
    const sf::FloatRect NOR(ZZV, NOV);
    const sf::FloatRect NLR(ZZV, NLV);
    const sf::FloatRect PNR(ZZV, PNV);
    const sf::FloatRect PZR(ZZV, PZV);
    const sf::FloatRect PPR(ZZV, PPV);
    const sf::FloatRect POR(ZZV, POV);
    const sf::FloatRect PLR(ZZV, PLV);
    const sf::FloatRect ONR(ZZV, ONV);
    const sf::FloatRect OZR(ZZV, OZV);
    const sf::FloatRect OPR(ZZV, OPV);
    const sf::FloatRect OOR(ZZV, OOV);
    const sf::FloatRect OLR(ZZV, OLV);
    const sf::FloatRect LNR(ZZV, LNV);
    const sf::FloatRect LZR(ZZV, LZV);
    const sf::FloatRect LPR(ZZV, LPV);
    const sf::FloatRect LOR(ZZV, LOV);
    const sf::FloatRect LLR(ZZV, LLV);

    BOOST_CHECK(IsZeroOrLessAny(Size(ZNR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(ZZR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(ZPR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(ZOR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(ZLR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(NNR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(NZR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(NPR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(NOR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(NLR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(PNR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(PZR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(PPR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(POR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(PLR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(ONR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(OZR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(OPR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(OOR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(OLR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(LNR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(LZR)));
    BOOST_CHECK(IsZeroOrLessAny(Size(LPR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(LOR)) == false);
    BOOST_CHECK(IsZeroOrLessAny(Size(LLR)) == false);

    BOOST_CHECK(IsZeroOrLessAll(Size(ZNR)));
    BOOST_CHECK(IsZeroOrLessAll(Size(ZZR)));
    BOOST_CHECK(IsZeroOrLessAll(Size(ZPR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(ZOR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(ZLR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(NNR)));
    BOOST_CHECK(IsZeroOrLessAll(Size(NZR)));
    BOOST_CHECK(IsZeroOrLessAll(Size(NPR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(NOR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(NLR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(PNR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(PZR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(PPR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(POR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(PLR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(ONR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(OZR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(OPR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(OOR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(OLR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(LNR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(LZR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(LPR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(LOR)) == false);
    BOOST_CHECK(IsZeroOrLessAll(Size(LLR)) == false);

    BOOST_CHECK(IsLessThanOneAny(Size(ZNR)));
    BOOST_CHECK(IsLessThanOneAny(Size(ZZR)));
    BOOST_CHECK(IsLessThanOneAny(Size(ZPR)));
    BOOST_CHECK(IsLessThanOneAny(Size(ZOR)));
    BOOST_CHECK(IsLessThanOneAny(Size(ZLR)));
    BOOST_CHECK(IsLessThanOneAny(Size(NNR)));
    BOOST_CHECK(IsLessThanOneAny(Size(NZR)));
    BOOST_CHECK(IsLessThanOneAny(Size(NPR)));
    BOOST_CHECK(IsLessThanOneAny(Size(NOR)));
    BOOST_CHECK(IsLessThanOneAny(Size(NLR)));
    BOOST_CHECK(IsLessThanOneAny(Size(PNR)));
    BOOST_CHECK(IsLessThanOneAny(Size(PZR)));
    BOOST_CHECK(IsLessThanOneAny(Size(PPR)));
    BOOST_CHECK(IsLessThanOneAny(Size(POR)));
    BOOST_CHECK(IsLessThanOneAny(Size(PLR)));
    BOOST_CHECK(IsLessThanOneAny(Size(ONR)));
    BOOST_CHECK(IsLessThanOneAny(Size(OZR)));
    BOOST_CHECK(IsLessThanOneAny(Size(OPR)));
    BOOST_CHECK(IsLessThanOneAny(Size(OOR)) == false);
    BOOST_CHECK(IsLessThanOneAny(Size(OLR)) == false);
    BOOST_CHECK(IsLessThanOneAny(Size(LNR)));
    BOOST_CHECK(IsLessThanOneAny(Size(LZR)));
    BOOST_CHECK(IsLessThanOneAny(Size(LPR)));
    BOOST_CHECK(IsLessThanOneAny(Size(LOR)) == false);
    BOOST_CHECK(IsLessThanOneAny(Size(LLR)) == false);

    BOOST_CHECK(IsLessThanOneAll(Size(ZNR)));
    BOOST_CHECK(IsLessThanOneAll(Size(ZZR)));
    BOOST_CHECK(IsLessThanOneAll(Size(ZPR)));
    BOOST_CHECK(IsLessThanOneAll(Size(ZOR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(ZLR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(NNR)));
    BOOST_CHECK(IsLessThanOneAll(Size(NZR)));
    BOOST_CHECK(IsLessThanOneAll(Size(NPR)));
    BOOST_CHECK(IsLessThanOneAll(Size(NOR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(NLR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(PNR)));
    BOOST_CHECK(IsLessThanOneAll(Size(PZR)));
    BOOST_CHECK(IsLessThanOneAll(Size(PPR)));
    BOOST_CHECK(IsLessThanOneAll(Size(POR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(PLR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(ONR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(OZR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(OPR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(OOR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(OLR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(LNR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(LZR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(LPR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(LOR)) == false);
    BOOST_CHECK(IsLessThanOneAll(Size(LLR)) == false);
}

BOOST_AUTO_TEST_CASE(CenterOfTests)
{
    BOOST_CHECK(CenterOf(sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f)) == sf::Vector2f(0.0f, 0.0f));
    BOOST_CHECK(CenterOf(sf::FloatRect(-1.0f, -1.0f, 2.0f, 2.0f)) == sf::Vector2f(0.0f, 0.0f));
    BOOST_CHECK(CenterOf(sf::FloatRect(-0.1f, -0.1f, 0.2f, 0.2f)) == sf::Vector2f(0.0f, 0.0f));
    BOOST_CHECK(CenterOf(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f)) == sf::Vector2f(0.5f, 0.5f));
    BOOST_CHECK(CenterOf(sf::Rect<int>(10, 10, 100, 100)) == sf::Vector2i(60, 60));
    BOOST_CHECK(CenterOf(sf::Rect<int>(-10, -10, 20, 20)) == sf::Vector2i(0, 0));

    sf::Image image;
    image.create(unsigned(100), unsigned(10));

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);
    sprite.setPosition(-50.0f, -5.0f);

    BOOST_CHECK(CenterOf(sprite) == sf::Vector2f(0.0f, 0.0f));

    sf::Sprite spriteDefault;
    BOOST_CHECK(spriteDefault.getGlobalBounds() == sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f));
    BOOST_CHECK(CenterOf(spriteDefault) == sf::Vector2f(0.0f, 0.0f));
}

BOOST_AUTO_TEST_CASE(CenterToTests)
{
    const float HALF { 0.5f };
    const sf::IntRect ORIGIN_WRAP_RECT_I(-1, -1, 2, 2);
    const sf::FloatRect ORIGIN_WRAP_RECT_F(-0.1f, -0.1f, 0.2f, 0.2f);
    const sf::FloatRect ZERO_RECT_F(0.0f, 0.0f, 0.0f, 0.0f);
    const sf::IntRect ZERO_RECT_I(ZERO_RECT_F);
    const sf::IntRect RECT_I(100, 200, 300, 400);
    const sf::FloatRect RECT_F(RECT_I);
    const sf::Vector2f RECT_CENTER_V_F(250.0f, 400.0f);
    const sf::Vector2f SCALE_V_F(0.5f, 2.0f);

    BOOST_CHECK(CenterToCopy(sf::Vector2i(0, 0), ORIGIN_WRAP_RECT_I) == sf::Vector2i(0, 0));
    BOOST_CHECK(CenterToCopy(sf::Vector2i(0, 0), ZERO_RECT_I) == sf::Vector2i(0, 0));
    BOOST_CHECK(CenterToCopy(sf::Vector2f(0.0f, 0.0f), RECT_I) == sf::Vector2f(250.0f, 400.0f));
    BOOST_CHECK(CenterToCopy(sf::Vector2i(50, 50), ZERO_RECT_F) == sf::Vector2i(-25, -25));
    BOOST_CHECK(CenterToCopy(sf::Vector2i(50, 50), ORIGIN_WRAP_RECT_I) == sf::Vector2i(-25, -25));

    //
    BOOST_CHECK(CenterToCopy(ZERO_RECT_F, ZERO_RECT_I) == ZERO_RECT_F);
    BOOST_CHECK(CenterToCopy(ZERO_RECT_I, ZERO_RECT_F) == ZERO_RECT_I);
    BOOST_CHECK(CenterToCopy(ZERO_RECT_F, ZERO_RECT_I, SCALE_V_F) == ZERO_RECT_F);

    //
    BOOST_CHECK(CenterToCopy(ZERO_RECT_F, ORIGIN_WRAP_RECT_F) == ZERO_RECT_F);
    BOOST_CHECK(CenterToCopy(ZERO_RECT_I, RECT_F) == sf::IntRect(250, 400, 0, 0));

    BOOST_CHECK(CenterToCopy(RECT_F, RECT_F) == RECT_F);

    BOOST_CHECK(
        CenterToCopy(RECT_F, RECT_F, HALF) == sf::FloatRect(175.0f, 300.0f, 150.0f, 200.0f));

    //
    BOOST_CHECK(CenterToCopy(RECT_I, ORIGIN_WRAP_RECT_F) == sf::IntRect(-150, -200, 300, 400));

    //
    BOOST_CHECK(
        CenterToCopy(RECT_F, ORIGIN_WRAP_RECT_I, HALF)
        == sf::FloatRect(-75.0f, -100.0f, 150.0f, 200.0f));

    BOOST_CHECK(
        CenterToCopy(RECT_F, ORIGIN_WRAP_RECT_I, SCALE_V_F)
        == sf::FloatRect(-75.0f, -400.0f, 150.0f, 800.0f));

    auto rectI { RECT_I };
    CenterTo(rectI, ORIGIN_WRAP_RECT_F);
    BOOST_CHECK(rectI == sf::IntRect(-150, -200, 300, 400));

    auto rectF { RECT_F };
    CenterTo(rectF, ORIGIN_WRAP_RECT_I, SCALE_V_F);
    BOOST_CHECK(rectF == sf::FloatRect(-75.0f, -400.0f, 150.0f, 800.0f));

    //
    const sf::Vector2f SIZE_V_F(123.0f, 456.0f);
    const sf::Vector2f IMAGE_ZEROCENTERED_V_F(SIZE_V_F * -0.5f);
    const sf::Vector2f RESIZE_V_F(SIZE_V_F.x * SCALE_V_F.x, SIZE_V_F.y * SCALE_V_F.y);
    sf::Image image;
    image.create(unsigned(SIZE_V_F.x), unsigned(SIZE_V_F.y));

    sf::Texture texture;
    texture.loadFromImage(image);

    {
        sf::Sprite sprite(texture);

        const sf::Vector2f POS_V_F(789.0f, 987.0f);
        sprite.setPosition(POS_V_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(POS_V_F, SIZE_V_F));
        CenterTo(sprite, ZERO_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));
        CenterTo(sprite, ZERO_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));

        sprite.setPosition(POS_V_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(POS_V_F, SIZE_V_F));
        CenterTo(sprite, ORIGIN_WRAP_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));
        CenterTo(sprite, ORIGIN_WRAP_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));

        sprite.setPosition(POS_V_F);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(POS_V_F, SIZE_V_F));
    }

    {
        sf::Sprite spriteDefault;
        CenterTo(spriteDefault, ZERO_RECT_F);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == ZERO_RECT_F);
        CenterTo(spriteDefault, ORIGIN_WRAP_RECT_F);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == ZERO_RECT_F);
    }
}

BOOST_AUTO_TEST_CASE(ColorTests)
{
    const sf::Color C1(1, 2, 3, 4);
    const sf::Color C2(5, 6, 7, 8);
    const sf::Color C4(4, 4, 4, 4);

    BOOST_CHECK(C1 == C1);
    BOOST_CHECK(C1 != C2);
    BOOST_CHECK((C2 - C4) == C1);
    BOOST_CHECK((C1 + C4) == C2);

    BOOST_CHECK(C1 < C2);
    BOOST_CHECK(C1 <= C2);
    BOOST_CHECK(C1 <= C1);
    BOOST_CHECK(C2 > C1);
    BOOST_CHECK(C2 >= C1);
    BOOST_CHECK(C2 >= C2);

    BOOST_CHECK(Transition(C1, C2, 0.0f) == C1);
    BOOST_CHECK(Transition(C1, C2, 1.0f) == C2);

    const sf::Color C0(0, 0, 0, 0);
    const sf::Color CD(10, 20, 40, 80);

    BOOST_CHECK(Transition(C0, CD, 0.5f) == sf::Color(5, 10, 20, 40));
}

BOOST_AUTO_TEST_CASE(DirectionTests)
{
    BOOST_CHECK(DirectionFromAToB(sf::Vector2i(-1, 0), sf::Vector2i(1, 0)) == Direction::Right);
    BOOST_CHECK(DirectionFromAToB(sf::Vector2i(1, 0), sf::Vector2i(-1, 0)) == Direction::Left);
    BOOST_CHECK(DirectionFromAToB(sf::Vector2i(0, -1), sf::Vector2i(0, 1)) == Direction::Down);
    BOOST_CHECK(DirectionFromAToB(sf::Vector2i(0, 1), sf::Vector2i(0, -1)) == Direction::Up);
    BOOST_CHECK(DirectionFromAToB(sf::Vector2i(0, 0), sf::Vector2i(0, 0)) == Direction::Count);
    BOOST_CHECK(DirectionFromAToB(sf::Vector2i(1, -1), sf::Vector2i(1, -1)) == Direction::Count);
}

BOOST_AUTO_TEST_CASE(DisplayAndCenterTests)
{
    // display tests

    heroespath::misc::Log::Acquire();
    heroespath::gui::Display::Acquire("HeroespathTestDisplay", sf::Style::None, 0);

    const sf::Vector2f SCREEN_SIZE_V(DisplaySize());
    const float HALF_SCALE(0.5f);
    const sf::Vector2f SCREEN_CENTER_V(SCREEN_SIZE_V * HALF_SCALE);
    const sf::Vector2f EMPTY_V(0.0f, 0.0f);
    const sf::FloatRect SCREEN_RECT(EMPTY_V, SCREEN_SIZE_V);
    const sf::Vector2f TEN_V(10.0f, 10.0f);
    const sf::Vector2f TEN_CENTERED_V(SCREEN_CENTER_V - (TEN_V * HALF_SCALE));
    const sf::FloatRect TEN_RECT(EMPTY_V, TEN_V);
    const sf::FloatRect TEN_CENTERED_RECT(TEN_CENTERED_V, TEN_V);

    BOOST_CHECK(DisplaySize() == SCREEN_SIZE_V);
    BOOST_CHECK(DisplayRect() == SCREEN_RECT);

    // center tests
    BOOST_CHECK(Center() == SCREEN_CENTER_V);
    BOOST_CHECK(CenterCopy(TEN_V) == TEN_CENTERED_V);
    BOOST_CHECK(CenterCopy(TEN_V.x, TEN_V.y) == TEN_CENTERED_V);

    //

    const sf::IntRect ORIGIN_WRAP_RECT_I(-1, -1, 1, 1);
    const sf::FloatRect ORIGIN_WRAP_RECT_F(-0.1f, -0.1f, 0.2f, 0.2f);
    const sf::FloatRect ZERO_RECT_F(0.0f, 0.0f, 0.0f, 0.0f);
    const sf::IntRect ZERO_RECT_I(ZERO_RECT_F);
    const sf::IntRect RECT_I(100, 200, 300, 400);
    const sf::FloatRect RECT_F(100.0f, 200.0f, 300.0f, 400.0f);
    const sf::Vector2f SCALE_V_F(0.5f, 2.0f);
    const sf::Vector2f ZERO_V_F(0.0f, 0.0f);
    const sf::Vector2i ZERO_V_I(ZERO_V_F);
    const sf::FloatRect ZERO_RECT_CENTERED_F(SCREEN_CENTER_V, ZERO_V_F);
    const sf::IntRect ZERO_RECT_CENTERED_I(ZERO_RECT_CENTERED_F);
    const sf::Vector2f SIZE_V_F { 123.456f, 789.987f };
    const sf::Vector2i SIZE_V_I { SIZE_V_F };

    BOOST_CHECK(CenterCopy(ZERO_V_F) == SCREEN_CENTER_V);
    BOOST_CHECK(CenterCopy(ZERO_V_F) == SCREEN_CENTER_V);
    BOOST_CHECK(CenterCopy(SIZE_V_F) == SCREEN_CENTER_V - (SIZE_V_F * 0.5f));
    BOOST_CHECK(CenterCopy(SIZE_V_F) == (SCREEN_CENTER_V - (sf::Vector2f(SIZE_V_F) * 0.5f)));

    //
    BOOST_CHECK(CenterCopy(ZERO_RECT_F) == ZERO_RECT_CENTERED_F);
    BOOST_CHECK(CenterCopy(ZERO_RECT_F, SCALE_V_F) == ZERO_RECT_CENTERED_F);

    // THIS SHOULD NOT COMPILE
    // BOOST_CHECK(CenterCopy(ZERO_RECT_F, ORIGIN_WRAP_RECT_F) == ZERO_RECT_CENTERED_F);

    BOOST_CHECK(
        CenterCopy(RECT_I)
        == sf::IntRect { sf::FloatRect(
               SCREEN_CENTER_V - (sf::Vector2f(300.0f, 400.0f) * 0.5f),
               sf::Vector2f(300.0f, 400.0f)) });

    BOOST_CHECK(
        CenterCopy(RECT_I, SCALE_V_F)
        == sf::IntRect { sf::FloatRect(
               SCREEN_CENTER_V - ScaleCopy(sf::Vector2f(300.0f, 400.0f), SCALE_V_F * 0.5f),
               ScaleCopy(sf::Vector2f(300.0f, 400.0f), SCALE_V_F)) });

    heroespath::gui::Display::Release();
    heroespath::misc::Log::Release();
}

BOOST_AUTO_TEST_CASE(DistanceTests)
{
    BOOST_CHECK((Distance(sf::Vector2i(0, 0), sf::Vector2f(0.0f, 0.0f)) == 0));
    BOOST_CHECK((Distance(sf::Vector2i(-10, 0), sf::Vector2f(10.0f, 0.0f)) == 20));

    BOOST_CHECK(
        (Distance(sf::Rect<int>(-1, -1, 2, 2), sf::FloatRect(2.0f, 3.0f, 2.0f, 2.0f)) == 5));

    BOOST_CHECK(
        (Distance(sf::Rect<float>(-1.0f, -1.0f, 2.0f, 2.0f), sf::IntRect(2, 3, 2, 2)) == 5.0f));

    sf::Image image;
    image.create(unsigned(10), unsigned(10));

    sf::Texture texture;
    texture.loadFromImage(image);

    {
        sf::Sprite sprite1(texture);
        sprite1.setPosition(-5.0f, -5.0f);

        sf::Sprite sprite2(texture);
        sprite2.setPosition(-2.0f, -1.0f);
        BOOST_CHECK((Distance(sprite1, sprite2) == 5.0f));
    }

    {
        sf::Sprite spriteDefault1;
        sf::Sprite spriteDefault2;
        BOOST_CHECK((Distance(spriteDefault1, spriteDefault2) == 0.0f));
    }
}

BOOST_AUTO_TEST_CASE(FitTests)
{
    const sf::Vector2i EMPTY_V_I(0, 0);
    const sf::Vector2f EMPTY_V_F(0.0f, 0.0f);

    const sf::Vector2i TALL_V_I(10, 100);
    const sf::Vector2i WIDE_V_I(100, 10);
    const sf::Vector2f TALL_V_F(10.0f, 100.0f);
    const sf::Vector2f WIDE_V_F(100.0f, 10.0f);

    const sf::Rect<int> TALL_RECT_I(EMPTY_V_I, TALL_V_I);
    const sf::Rect<int> WIDE_RECT_I(EMPTY_V_I, WIDE_V_I);
    const sf::Rect<float> TALL_RECT_F(EMPTY_V_F, TALL_V_F);
    const sf::Rect<float> WIDE_RECT_F(EMPTY_V_F, WIDE_V_F);
    const sf::FloatRect RECT_ZERO_F(0.0f, 0.0f, 0.0f, 0.0f);

    const float HALF_SCALE(0.5f);
    const float DOUBLE_SCALE(2.0f);

    BOOST_CHECK(FitCopy(10.0f, 100.0f, 0.0, 0.0) == TALL_V_F);
    BOOST_CHECK(FitCopy(10.0f, 100.0f, -1.0, -0.1) == TALL_V_F);

    BOOST_CHECK((FitCopy(10.0f, 100.0f, 10.0, 100.0) == TALL_V_F));
    BOOST_CHECK((FitCopy(10.0f, 100.0f, 10.0, 100.0, HALF_SCALE) == (TALL_V_F * HALF_SCALE)));

    BOOST_CHECK((FitCopy(10, 100, 10.0, 100.0) == TALL_V_I));
    BOOST_CHECK((FitCopy(10, 100, 10.0, 100.0, HALF_SCALE) == sf::Vector2i(5, 50)));

    BOOST_CHECK((FitCopy(10, 100, 100.0, 10.0) == sf::Vector2i(1, 10)));
    BOOST_CHECK((FitCopy(50, 100, 100.0, 10.0, HALF_SCALE) == sf::Vector2i(2, 5)));

    BOOST_CHECK((FitCopy(10, 100, 0.0, 10.0) == sf::Vector2i(1, 10)));
    BOOST_CHECK((FitCopy(100, 10, 0.0, 10.0) == sf::Vector2i(100, 10)));
    BOOST_CHECK((FitCopy(100, 100, 0.0, 10.0) == sf::Vector2i(10, 10)));
    BOOST_CHECK((FitCopy(100, 100, 10.0, 0.0) == sf::Vector2i(10, 10)));
    BOOST_CHECK((FitCopy(10, 100, 10.0, 0.0) == sf::Vector2i(10, 100)));
    BOOST_CHECK((FitCopy(100, 10, 10.0, 0.0) == sf::Vector2i(10, 1)));

    const auto TEST_RESULT_1_V { FitCopy(40.0f, 30.0f, 20.0f, 10.0f) };
    BOOST_CHECK(heroespath::misc::IsRealClose(TEST_RESULT_1_V.x, 40.0f * (1.0f / 3.0f)));
    BOOST_CHECK(heroespath::misc::IsRealClose(TEST_RESULT_1_V.y, 10.0f));

    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 40.0f) == sf::Vector2f(20.0f, 40.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 40.0f) == sf::Vector2f(0.0f, 40.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 40.0f) == sf::Vector2f(30.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 40.0f) == sf::Vector2f(20.0f, 40.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 0.0f) == sf::Vector2f(30.0f, 60.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 40.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 0.0f, 40.0f) == sf::Vector2f(0.0f, 40.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 0.0f) == sf::Vector2f(0.0f, 20.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 40.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 0.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 0.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 0.0f, 40.0f) == sf::Vector2f(10.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 0.0f) == sf::Vector2f(30.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 0.0f) == sf::Vector2f(10.0f, 20.0f)));

    // repeat of the blocks above with scaling
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(40.0f, 80.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 40.0f, HALF_SCALE) == sf::Vector2f(0.0f, 20.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(60.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 40.0f, HALF_SCALE) == sf::Vector2f(10.0f, 20.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 0.0f, DOUBLE_SCALE) == sf::Vector2f(60.0f, 120.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 40.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 0.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(0.0f, 80.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 0.0f, HALF_SCALE) == sf::Vector2f(0.0f, 10.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 0.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 0.0f, DOUBLE_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 0.0f, 40.0f, HALF_SCALE) == sf::Vector2f(5.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 0.0f, DOUBLE_SCALE) == sf::Vector2f(60.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 0.0f, HALF_SCALE) == sf::Vector2f(5.0f, 10.0f)));

    const float FIT_SCALE { 0.1f };
    const sf::Vector2f TALL_TO_WIDE_SIZE_V_F(TALL_V_F * FIT_SCALE);
    const sf::Vector2f WIDE_TO_TALL_SIZE_V_F(WIDE_V_F * FIT_SCALE);
    const float QUARTER_SCALE { 0.25f };
    const sf::FloatRect WIDE_TO_TALL_RECT_F(EMPTY_V_F, WIDE_TO_TALL_SIZE_V_F);
    const sf::Vector2f QUARTER_TALL_TO_WIDE_SIZE_V_F(TALL_TO_WIDE_SIZE_V_F * QUARTER_SCALE);
    const sf::FloatRect QUARTER_TALL_TO_WIDE_RECT_F(EMPTY_V_F, QUARTER_TALL_TO_WIDE_SIZE_V_F);
    const sf::Vector2f QUARTER_WIDE_TO_TALL_SIZE_V_F(WIDE_TO_TALL_SIZE_V_F * QUARTER_SCALE);
    const sf::FloatRect QUARTER_WIDE_TO_TALL_RECT_F(EMPTY_V_F, QUARTER_WIDE_TO_TALL_SIZE_V_F);

    BOOST_CHECK((FitCopy(TALL_V_F, WIDE_V_I) == TALL_TO_WIDE_SIZE_V_F));

    BOOST_CHECK(
        (FitCopy(TALL_V_F, WIDE_V_I, QUARTER_SCALE) == (TALL_TO_WIDE_SIZE_V_F * QUARTER_SCALE)));

    auto tallVF { TALL_V_F };
    Fit(tallVF, WIDE_V_I);
    BOOST_CHECK(tallVF == TALL_TO_WIDE_SIZE_V_F);

    tallVF = TALL_V_F;
    Fit(tallVF, WIDE_RECT_I);
    BOOST_CHECK(tallVF == TALL_TO_WIDE_SIZE_V_F);

    tallVF = TALL_V_F;
    Fit(tallVF, WIDE_V_I, QUARTER_SCALE);
    BOOST_CHECK(tallVF == QUARTER_TALL_TO_WIDE_SIZE_V_F);

    tallVF = TALL_V_F;
    Fit(tallVF, WIDE_RECT_I, QUARTER_SCALE);
    BOOST_CHECK(tallVF == QUARTER_TALL_TO_WIDE_SIZE_V_F);

    auto wideRF { WIDE_RECT_F };
    Fit(wideRF, TALL_V_I);
    BOOST_CHECK((wideRF == WIDE_TO_TALL_RECT_F));

    wideRF = WIDE_RECT_F;
    Fit(wideRF, TALL_RECT_I);
    BOOST_CHECK((wideRF == WIDE_TO_TALL_RECT_F));

    wideRF = WIDE_RECT_F;
    Fit(wideRF, TALL_V_I, QUARTER_SCALE);
    BOOST_CHECK((wideRF == QUARTER_WIDE_TO_TALL_RECT_F));

    wideRF = WIDE_RECT_F;
    Fit(wideRF, TALL_RECT_I, QUARTER_SCALE);
    BOOST_CHECK((wideRF == QUARTER_WIDE_TO_TALL_RECT_F));

    {
        sf::Image image;
        image.create(unsigned(TALL_V_I.x), unsigned(TALL_V_I.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite1(texture);
        const sf::Vector2f RAND_POS_V_F(123.0f, 456.0f);
        sprite1.setPosition(RAND_POS_V_F);
        BOOST_CHECK(sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, TALL_V_F));

        Fit(sprite1, WIDE_V_F);
        BOOST_CHECK((Size(sprite1.getGlobalBounds()) == Size(sprite1)));

        BOOST_CHECK(
            sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, sf::Vector2f(1.0f, 10.0f)));

        sprite1.setScale(1.0f, 1.0f);
        BOOST_CHECK(sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, TALL_V_F));
    }

    {
        sf::Sprite spriteDefault;

        Fit(spriteDefault, WIDE_V_F);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == RECT_ZERO_F);
    }

    {
        sf::Image imageTall;
        imageTall.create(unsigned(TALL_V_F.x), unsigned(TALL_V_F.y));

        sf::Texture textureTall;
        textureTall.loadFromImage(imageTall);

        sf::Sprite spriteTall(textureTall);

        sf::Image imageWide;
        imageWide.create(unsigned(WIDE_V_F.x), unsigned(WIDE_V_F.y));

        sf::Texture textureWide;
        textureWide.loadFromImage(imageWide);

        sf::Sprite spriteWide(textureWide);

        spriteTall.setScale(1.0f, 1.0f);
        const auto ORIG_POS_1_V(spriteTall.getPosition());
        Fit(spriteTall, WIDE_V_F);
        BOOST_CHECK(spriteTall.getGlobalBounds() == sf::FloatRect(0.0f, 0.0f, 1.0f, 10.0f));
        BOOST_CHECK(spriteTall.getPosition() == ORIG_POS_1_V);

        spriteWide.setScale(1.0f, 1.0f);
        const auto ORIG_POS_2_V(spriteWide.getPosition());
        Fit(spriteWide, TALL_V_F);
        BOOST_CHECK(spriteWide.getGlobalBounds() == sf::FloatRect(0.0f, 0.0f, 10.0f, 1.0f));
        BOOST_CHECK(spriteWide.getPosition() == ORIG_POS_2_V);
    }

    const sf::Rect<int> FARC_RECT_1(10, 10, 10, 10);
    const sf::Rect<int> FARC_RECT_1_RECENTERED(14, 14, 2, 2);
    BOOST_CHECK(FitAndReCenterCopy(FARC_RECT_1, sf::Vector2i(2, 2)) == FARC_RECT_1_RECENTERED);

    auto farcRectOrigTemp { FARC_RECT_1 };
    FitAndReCenter(farcRectOrigTemp, sf::Vector2i(2, 2));
    BOOST_CHECK(farcRectOrigTemp == FARC_RECT_1_RECENTERED);

    const sf::Rect<int> FARC_RECT_2(-50, -50, 100, 100);
    const sf::Rect<int> FARC_RECT_2_SIZE50_RECENTERED(-25, -25, 50, 50);
    const sf::Rect<int> FARC_RECT_2_SIZE10_RECENTERED(-5, -5, 10, 10);

    BOOST_CHECK(
        FitAndReCenterCopy(FARC_RECT_2, sf::Vector2i(50, 50)) == FARC_RECT_2_SIZE50_RECENTERED);

    BOOST_CHECK(
        FitAndReCenterCopy(FARC_RECT_2, sf::Vector2i(50, 50), 0.2f)
        == FARC_RECT_2_SIZE10_RECENTERED);

    farcRectOrigTemp = FARC_RECT_2;
    FitAndReCenter(farcRectOrigTemp, sf::Vector2i(50, 50), 0.2f);
    BOOST_CHECK(farcRectOrigTemp == FARC_RECT_2_SIZE10_RECENTERED);

    // TALL_RECT_F(0.0f, 0.0f, 10.0f, 100.0f)
    // TALL_RECT_F_CENTER(5.0f, 50.0f)
    // WIDE_RECT_F(0.0f, 0.0f, 100.0f, 10.0f)
    // FIT(TALL to WIDE)=(0.0f, 0.0f, 1.0f, 10.0f)
    // RECENTERED=(4.5f, 45.0f, 1.0f, 10.0f)
    const sf::Rect<float> TALL_FITANDRCTO_WIDE_F(4.5f, 45.0f, 1.0f, 10.0f);
    BOOST_CHECK(FitAndReCenterCopy(TALL_RECT_F, WIDE_RECT_I) == TALL_FITANDRCTO_WIDE_F);

    auto tallRectFTemp { TALL_RECT_F };
    FitAndReCenter(tallRectFTemp, WIDE_RECT_F);
    BOOST_CHECK(tallRectFTemp == TALL_FITANDRCTO_WIDE_F);

    // TALL_RECT_F(0.0f, 0.0f, 10.0f, 100.0f)
    // WIDE_RECT_F_CENTER(50.0f, 5.0f)
    // WIDE_RECT_F(0.0f, 0.0f, 100.0f, 10.0f)
    // FIT(TALL to WIDE)=(0.0f, 0.0f, 1.0f, 10.0f)
    // CENTERED=(49.5f, 0.0f, 1.0f, 10.0f)
    const sf::Rect<float> TALL_FITANDCTO_WIDE_F(49.5f, 0.0f, 1.0f, 10.0f);

    tallRectFTemp = TALL_RECT_F;
    FitAndCenterTo(tallRectFTemp, WIDE_RECT_F);
    BOOST_CHECK(tallRectFTemp == TALL_FITANDCTO_WIDE_F);

    /*{
        // if FitAndCenterTo() to a rect with either width/height zero then only the other
    non-zero
        // dimension is used to scale
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(0, 0, 20, 40), sf::IntRect(10, 10, 20, 0))
            == sf::IntRect(0, 0, 100, 200));

        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-60, -220, 100, 200));

        // if FitAndCenterTo() to a rect with both width/height zero then the resulting size is
    zero BOOST_CHECK( FitAndCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0,
    0))
            == sf::IntRect(-10, -20, 0, 0));

        //...even if scaled
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 0), 69.0f)
            == sf::IntRect(-10, -20, 0, 0));

        // if FitAndCenterTo() with opposite dimensions zero result in size of zero
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 0), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-10, -120, 0, 0));

        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 0, 100), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(-60, -20, 0, 0));

        // if FitAndCenterTo() with matching dimensions zero results in a scale based on the
    non-zero
        // dimension
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 0), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(-110, -120, 100, 200));

        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 0, 100), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-60, -220, 100, 200));

        // if FitAndCenterTo() with all sizes zero results in size of zero
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 0, 0), sf::IntRect(-10, -20, 0, 0))
            == sf::IntRect(-10, -20, 0, 0));
    }

    {
        // if FitAndReCenter() to a rect with either width/height zero then only the other
    non-zero
        // dimension is used to scale
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(-65, -130, 100, 200));

        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-65, -130, 100, 200));

        // if FitAndReCenter() to a rect with both width/height zero then the resulting size is
    zero BOOST_CHECK( FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0,
    0))
            == sf::IntRect(-15, -30, 0, 0));

        //...even if scaled
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 0), 69.0f)
            == sf::IntRect(-15, -30, 0, 0));

        // if FitAndReCenter() with opposite dimensions zero result in size of zero
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 0), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-15, 20, 0, 0));

        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 0, 100), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(10, -30, 0, 0));

        // if FitAndReCenter() with matching dimensions zero results in a scale based on the
        // non-zero dimension
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 0), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(-65, -80, 100, 200));

        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 0, 100), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-40, -130, 100, 200));

        // if FitAndReCenter() with all sizes zero results in size of zero
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 0, 0), sf::IntRect(-10, -20, 0, 0))
            == sf::IntRect(10, 20, 0, 0));
    }*/

    {
        sf::Image image;
        image.create(unsigned(10), unsigned(10));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        sprite.setPosition(10.0f, 10.0f);

        FitAndReCenter(sprite, sf::Vector2f(2.0f, 2.0f));
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(14.0f, 14.0f, 2.0f, 2.0f));

        FitAndReCenter(sprite, sf::Vector2f(2.0f, 2.0f));
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(14.0f, 14.0f, 2.0f, 2.0f));

        sf::Sprite spriteDefault;
        FitAndReCenter(spriteDefault, sf::Vector2f(2.0f, 2.0f));
        BOOST_CHECK(spriteDefault.getGlobalBounds() == RECT_ZERO_F);
    }

    {
        sf::Image image;
        image.create(unsigned(TALL_V_I.x), unsigned(TALL_V_I.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        FitAndReCenter(sprite, WIDE_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == TALL_FITANDRCTO_WIDE_F);
        FitAndReCenter(sprite, WIDE_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == TALL_FITANDRCTO_WIDE_F);

        sf::Sprite spriteDefault;
        FitAndReCenter(spriteDefault, WIDE_RECT_F);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == RECT_ZERO_F);
    }

    {
        sf::Image image;
        image.create(unsigned(TALL_V_I.x), unsigned(TALL_V_I.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        FitAndCenterTo(sprite, WIDE_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);
        FitAndCenterTo(sprite, WIDE_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);
    }

    {
        sf::Image imageTall;
        imageTall.create(unsigned(TALL_V_F.x), unsigned(TALL_V_F.y));

        sf::Texture textureTall;
        textureTall.loadFromImage(imageTall);

        sf::Sprite spriteTall(textureTall);

        sf::Image imageWide;
        imageWide.create(unsigned(WIDE_V_F.x), unsigned(WIDE_V_F.y));

        sf::Texture textureWide;
        textureWide.loadFromImage(imageWide);

        const sf::Sprite spriteWide(textureWide);

        FitAndReCenter(spriteTall, spriteWide);
        BOOST_CHECK(spriteTall.getGlobalBounds() == TALL_FITANDRCTO_WIDE_F);
        FitAndReCenter(spriteTall, spriteWide);
        BOOST_CHECK(spriteTall.getGlobalBounds() == TALL_FITANDRCTO_WIDE_F);
    }

    {
        sf::Image imageTall;
        imageTall.create(unsigned(TALL_V_F.x), unsigned(TALL_V_F.y));

        sf::Texture textureTall;
        textureTall.loadFromImage(imageTall);

        sf::Sprite spriteTall(textureTall);

        sf::Image imageWide;
        imageWide.create(unsigned(WIDE_V_F.x), unsigned(WIDE_V_F.y));

        sf::Texture textureWide;
        textureWide.loadFromImage(imageWide);

        const sf::Sprite spriteWide(textureWide);

        FitAndCenterTo(spriteTall, spriteWide);
        BOOST_CHECK(spriteTall.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);
        FitAndCenterTo(spriteTall, spriteWide);
        BOOST_CHECK(spriteTall.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);

        sf::Sprite spriteDefault1;
        sf::Sprite spriteDefault2;
        FitAndReCenter(spriteDefault1, spriteDefault2);
        BOOST_CHECK(spriteDefault1.getGlobalBounds() == RECT_ZERO_F);
    }

    BOOST_CHECK(IsRealClose(ScaleThatFits(TALL_V_F.x, TALL_V_F.y, WIDE_V_I.x, WIDE_V_I.y), 0.1f));
    BOOST_CHECK(IsRealClose(ScaleThatFits(WIDE_V_F, TALL_V_I), 0.1f));
    BOOST_CHECK(IsRealClose(ScaleThatFits(TALL_RECT_F, WIDE_V_I), 0.1f));
    BOOST_CHECK(IsRealClose(ScaleThatFits(TALL_V_F, WIDE_RECT_I), 0.1f));
    BOOST_CHECK(IsRealClose(ScaleThatFits(TALL_RECT_F, WIDE_RECT_I), 0.1f));
}

BOOST_AUTO_TEST_CASE(PositionTests)
{
    const sf::Vector2i POS_V_I(6, 8);
    const sf::Vector2f POS_V_F(POS_V_I);
    const sf::Vector2i SIZE_V_I(4, 2);
    const sf::Vector2f SIZE_V_F(SIZE_V_I);
    const sf::Rect<int> RECT_I(POS_V_I, SIZE_V_I);
    const sf::FloatRect RECT_F(RECT_I);
    const int SUM_I(10);
    const float SUM_F(SUM_I);

    sf::Image image;
    image.create(unsigned(SIZE_V_I.x), unsigned(SIZE_V_I.y));

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);
    sprite.setPosition(POS_V_F);

    BOOST_CHECK(Position(RECT_I) == POS_V_I);
    BOOST_CHECK(Position(RECT_F) == POS_V_F);

    BOOST_CHECK(Position(sprite) == POS_V_F);

    BOOST_CHECK(Right(RECT_I) == SUM_I);
    BOOST_CHECK(IsRealClose(Right(sprite), SUM_F));

    BOOST_CHECK(Bottom(RECT_I) == SUM_I);
    BOOST_CHECK(IsRealClose(Bottom(RECT_F), SUM_F));

    BOOST_CHECK(IsRealClose(Bottom(sprite), SUM_F));
}

BOOST_AUTO_TEST_CASE(SizeAndScaleTests)
{
    const sf::Vector2f V0F(0.0f, 0.0f);
    const sf::Vector2i V0I(V0F);

    const sf::Vector2f V1F(1.0f, 1.0f);
    const sf::Vector2i V1I(V1F);

    const sf::Vector2f VN1F(-1.0f, -1.0f);
    const sf::Vector2i VN1I(VN1F);

    const float HALF(0.5f);
    const float DOUBLE(2.0f);

    const sf::Vector2f VHF(HALF, HALF);

    const sf::Vector2f VDF(DOUBLE, DOUBLE);
    const sf::Vector2i VDI(VDF);

    const sf::Vector2f VRF(123.123f, 789.789f);
    const sf::Vector2i VRI(V1F);

    const sf::Vector2f VSF(456.456f, 890.890f);
    const sf::Vector2i VSI(VSF);

    const sf::Vector2f VRHF(VRF * HALF);
    const sf::Vector2i VRHI(VRI / 2);

    const sf::Vector2f VRDF(VRF * DOUBLE);
    const sf::Vector2i VRDI(VRI * 2);

    const sf::Vector2f VRRF(VRF.x * VRF.x, VRF.y * VRF.y);
    const sf::Vector2i VRRI(VRI.x * VRI.x, VRI.x * VRI.x);

    const sf::Rect<float> R0F(0.0f, 0.0f, 0.0f, 0.0f);
    const sf::Rect<int> R0I(R0F);

    const sf::Rect<float> R1F(1.0f, 1.0f, 1.0f, 1.0f);
    const sf::Rect<int> R1I(R1F);

    // these rects only have their sizes modified, as the ScaleSize() functions will do
    const sf::Rect<float> RRF(VRF, VSF);
    const sf::Rect<int> RRI(VRI, VSI);

    const sf::Rect<float> RRHF(VRF, VSF * HALF);
    const sf::Rect<int> RRHI(VRI, VSI / 2);

    const sf::Rect<float> RRDF(VRF, VSF * DOUBLE);
    const sf::Rect<int> RRDI(VRI, VSI * 2);

    const sf::Rect<float> RRRF(VRF, sf::Vector2f(VSF.x * VSF.x, VSF.y * VSF.y));
    const sf::Rect<int> RRRI(VRI, sf::Vector2i(VSI.x * VSI.x, VSI.y * VSI.y));

    BOOST_CHECK(V0F == V0F);
    BOOST_CHECK(V0F == (V1F - V1F));
    BOOST_CHECK(V0F != V1F);

    BOOST_CHECK(V0F < V1F);
    BOOST_CHECK(V0F < V1I);
    BOOST_CHECK(V0F <= V1F);
    BOOST_CHECK(V0F <= V1I);
    BOOST_CHECK(V0F <= V0F);
    BOOST_CHECK(V1F > V0F);
    BOOST_CHECK(V1F > V0I);
    BOOST_CHECK(V1F >= V0F);
    BOOST_CHECK(V1F >= V0I);

    // same as the block above only with Rects
    BOOST_CHECK(R0F == R0F);
    // BOOST_CHECK(R0F == (R1F - R1F));
    BOOST_CHECK(R0F != R1F);

    BOOST_CHECK(R0F < R1F);
    BOOST_CHECK(R0F < R1I);
    BOOST_CHECK(R0F <= R1F);
    BOOST_CHECK(R0F <= R1I);
    BOOST_CHECK(R0F <= R0F);
    BOOST_CHECK(R1F > R0F);
    BOOST_CHECK(R1F > R0I);
    BOOST_CHECK(R1F >= R0F);
    BOOST_CHECK(R1F >= R0I);

    {
        auto rectOfAllOnes(R1F);
        ScaleAndReCenter(rectOfAllOnes, 1.0f);
        BOOST_CHECK(rectOfAllOnes == R1F);
        rectOfAllOnes = R1F;
        ScaleAndReCenter(rectOfAllOnes, 1.0f);
        BOOST_CHECK(rectOfAllOnes == R1F);
        //
        rectOfAllOnes = R1F;
        ScaleAndReCenter(rectOfAllOnes, HALF);
        BOOST_CHECK(rectOfAllOnes == sf::FloatRect(1.25f, 1.25f, 0.5f, 0.5f));
        ScaleAndReCenter(rectOfAllOnes, DOUBLE);
        BOOST_CHECK(rectOfAllOnes == R1F);
        //
        rectOfAllOnes = R1F;
        ScaleAndReCenter(rectOfAllOnes, sf::Vector2f(HALF, DOUBLE));
        BOOST_CHECK(rectOfAllOnes == sf::FloatRect(1.25f, 0.5f, 0.5f, 2.0f));
        ScaleAndReCenter(rectOfAllOnes, sf::Vector2f(DOUBLE, HALF));
        BOOST_CHECK(rectOfAllOnes == R1F);
    }

    {
        const sf::IntRect FULL_RECT_ORIGIN_CENTERED_I(-100, -100, 200, 200);
        const sf::IntRect HALF_RECT_ORIGIN_CENTERED_I(-50, -50, 100, 100);
        //
        const sf::FloatRect FULL_RECT_ORIGIN_CENTERED_F(FULL_RECT_ORIGIN_CENTERED_I);
        const sf::FloatRect HALF_RECT_ORIGIN_CENTERED_F(HALF_RECT_ORIGIN_CENTERED_I);

        auto rI(FULL_RECT_ORIGIN_CENTERED_I);
        ScaleAndReCenter(rI, 0.5f);
        BOOST_CHECK(rI == HALF_RECT_ORIGIN_CENTERED_I);

        auto rF(FULL_RECT_ORIGIN_CENTERED_F);
        ScaleAndReCenter(rF, 0.5f);
        BOOST_CHECK(rF == HALF_RECT_ORIGIN_CENTERED_F);

        rI = FULL_RECT_ORIGIN_CENTERED_I;
        ScaleAndReCenter(rI, sf::Vector2f(0.1f, 0.01f));
        BOOST_CHECK(rI == sf::IntRect(-10, -1, 20, 2));

        rI = FULL_RECT_ORIGIN_CENTERED_I;
        ScaleAndReCenter(rI, sf::Vector2<double>(0.1, 0.01));
        BOOST_CHECK(rI == sf::IntRect(-10, -1, 20, 2));
    }

    // test of ScaleCopy(Vector, SCALE)
    const auto XF { 123.789f };
    const int XI { static_cast<int>(XF) };
    const sf::Vector2f VXF(XF, XF);
    const sf::Vector2f VXXF(XF * XF, XF * XF);
    const sf::Vector2i VXI(XI, XI);

    // test of ScaleCopy(Vector, Vector)
    BOOST_CHECK(ScaleCopy(V0F, V0F) == V0F);
    BOOST_CHECK(ScaleCopy(V1F, V0F) == V0F);
    BOOST_CHECK(ScaleCopy(VHF, V0F) == V0F);
    BOOST_CHECK(ScaleCopy(VDF, V0F) == V0F);
    BOOST_CHECK(ScaleCopy(V0F, V1F) == V0F);
    BOOST_CHECK(ScaleCopy(V1F, V1F) == V1F);
    BOOST_CHECK(ScaleCopy(VHF, V1F) == VHF);
    BOOST_CHECK(ScaleCopy(VDF, V1F) == VDF);
    BOOST_CHECK(ScaleCopy(VRF, V0F) == V0F);
    BOOST_CHECK(ScaleCopy(VRF, V1F) == VRF);
    BOOST_CHECK(ScaleCopy(VRF, VHF) == VRHF);
    BOOST_CHECK(ScaleCopy(VRF, VDF) == VRDF);
    BOOST_CHECK(ScaleCopy(VRF, VRF) == VRRF);

    BOOST_CHECK(ScaleCopy(V0I, V0F) == V0I);
    BOOST_CHECK(ScaleCopy(V1I, V0F) == V0I);
    // BOOST_CHECK(ScaleCopy(VHI, V0F) == V0F);
    BOOST_CHECK(ScaleCopy(VDI, V0F) == V0I);
    BOOST_CHECK(ScaleCopy(V0I, V1F) == V0I);
    BOOST_CHECK(ScaleCopy(V1I, V1F) == V1I);
    // BOOST_CHECK(ScaleCopy(VHI, V1F) == VHF);
    BOOST_CHECK(ScaleCopy(VDI, V1F) == VDI);
    BOOST_CHECK(ScaleCopy(VRI, V0F) == V0I);
    BOOST_CHECK(ScaleCopy(VRI, V1F) == VRI);
    BOOST_CHECK(ScaleCopy(VRI, VHF) == VRHI);
    BOOST_CHECK(ScaleCopy(VRI, VDF) == VRDI);

    const auto RIRFIx { int(double(VRI.x) * double(VRF.x)) };
    const auto RIRFIy { int(double(VRI.y) * double(VRF.y)) };
    const sf::Vector2i V_RIRFI(RIRFIx, RIRFIy);
    BOOST_CHECK(ScaleCopy(VRI, VRF) == V_RIRFI);

    auto temp5VRF { VRF };
    Scale(temp5VRF, VRF);
    BOOST_CHECK(temp5VRF == VRRF);

    BOOST_CHECK(ScaleCopy(sf::Vector2i(10, 1000000), sf::Vector2f(0.1f, 0.000001f)) == V1I);
    BOOST_CHECK(ScaleCopy(sf::Vector2i(10, 1000000), sf::Vector2f(-0.1f, -0.000001f)) == VN1I);
    BOOST_CHECK(ScaleCopy(sf::Vector2i(-10, -1000000), sf::Vector2f(0.1f, 0.000001f)) == VN1I);
    BOOST_CHECK(ScaleCopy(sf::Vector2i(-10, -1000000), sf::Vector2f(-0.1f, -0.000001f)) == V1I);

    // ScaleRectLinearCopy() tests
    const sf::Vector2f SCALEV1(0.5f, 2.0f);
    //
    const sf::Rect<int> INT_RECT_0(0, 0, 0, 0);
    const sf::Rect<int> INT_RECT_5(5, 5, 5, 5);
    const sf::Rect<int> INT_RECT_10(10, 10, 10, 10);
    const sf::Rect<int> INT_RECT_20(20, 20, 20, 20);
    const sf::Rect<int> INT_RECT_10_SCALEV1(5, 20, 5, 20);
    const sf::Rect<float> FLOAT_RECT_0(INT_RECT_0);
    const sf::Rect<float> FLOAT_RECT_5(INT_RECT_5);
    const sf::Rect<float> FLOAT_RECT_10(INT_RECT_10);
    const sf::Rect<float> FLOAT_RECT_20(INT_RECT_20);
    const sf::Rect<float> FLOAT_RECT_10_SCALEV1(INT_RECT_10_SCALEV1);

    {
        sf::Image image;
        const sf::Vector2f SPRITE_SIZE_V { 10.0f, 10.0f };
        image.create(unsigned(SPRITE_SIZE_V.x), unsigned(SPRITE_SIZE_V.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        const sf::Vector2f SPRITE_POS_V { 0.0f, 0.0f };
        sprite.setPosition(SPRITE_POS_V);
        const sf::FloatRect SPRITE_BOUNDS_ORIG(SPRITE_POS_V, SPRITE_SIZE_V);

        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);
        SetScaleAndReCenter(sprite, HALF);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(2.5f, 2.5f, 5.0f, 5.0f));

        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);
        SetScaleAndReCenter(sprite, VDF);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(-5.0f, -5.0f, 20.0f, 20.0f));

        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);
        SetScaleAndReCenter(sprite, SCALEV1);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(2.5f, -5.0f, 5.0f, 20.0f));
    }

    {
        sf::Sprite spriteDefault;
        SetScaleAndReCenter(spriteDefault, HALF);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == R0F);

        SetScaleAndReCenter(spriteDefault, VDF);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == R0F);

        SetScaleAndReCenter(spriteDefault, SCALEV1);
        BOOST_CHECK(spriteDefault.getGlobalBounds() == R0F);
    }

    // Size() tests
    const sf::Vector2f V5F(5.0f, 5.0f);
    const sf::Vector2f V10F(10.0f, 10.0f);
    const sf::Vector2f V20F(20.0f, 20.0f);
    const sf::Vector2i V5I(V5F);
    const sf::Vector2i V10I(V10F);
    const sf::Vector2i V20I(V20F);

    BOOST_CHECK((Size(INT_RECT_0) == V0I));
    BOOST_CHECK((Size(INT_RECT_5) == V5I));
    BOOST_CHECK((Size(INT_RECT_10) == V10I));
    BOOST_CHECK((Size(INT_RECT_20) == V20I));
    //
    BOOST_CHECK((Size(FLOAT_RECT_0) == V0F));
    BOOST_CHECK((Size(FLOAT_RECT_5) == V5F));
    BOOST_CHECK((Size(FLOAT_RECT_10) == V10F));
    BOOST_CHECK((Size(FLOAT_RECT_20) == V20F));

    // Size(RECT, SCALE) tests
    BOOST_CHECK((Size(INT_RECT_0, 0.0f) == V0I));
    BOOST_CHECK((Size(INT_RECT_0, 1.0f) == V0I));
    BOOST_CHECK((Size(FLOAT_RECT_0, 0.0f) == V0F));
    BOOST_CHECK((Size(FLOAT_RECT_0, 1.0f) == V0F));

    BOOST_CHECK((Size(INT_RECT_10, 0.0f) == V0I));
    BOOST_CHECK((Size(INT_RECT_10, 1.0f) == V10I));
    BOOST_CHECK((Size(INT_RECT_10, HALF) == V5I));
    BOOST_CHECK((Size(INT_RECT_10, DOUBLE) == V20I));
    BOOST_CHECK((Size(FLOAT_RECT_10, 0.0f) == V0F));
    BOOST_CHECK((Size(FLOAT_RECT_10, 1.0f) == V10F));
    BOOST_CHECK((Size(FLOAT_RECT_10, HALF) == V5F));
    BOOST_CHECK((Size(FLOAT_RECT_10, DOUBLE) == V20F));

    // Size(RECT, sf::Vector2<>(SCALE, SCALE)) tests
    BOOST_CHECK((Size(INT_RECT_0, V0F) == V0I));
    BOOST_CHECK((Size(INT_RECT_0, V1F) == V0I));
    //
    BOOST_CHECK((Size(FLOAT_RECT_0, V0F) == V0F));
    BOOST_CHECK((Size(FLOAT_RECT_0, V1F) == V0F));

    BOOST_CHECK((Size(INT_RECT_10, V0F) == V0I));
    BOOST_CHECK((Size(INT_RECT_10, V1F) == V10I));
    BOOST_CHECK((Size(INT_RECT_10, VHF) == V5I));
    BOOST_CHECK((Size(INT_RECT_10, VDF) == V20I));
    //
    BOOST_CHECK((Size(FLOAT_RECT_10, V0F) == V0F));
    BOOST_CHECK((Size(FLOAT_RECT_10, V1F) == V10F));
    BOOST_CHECK((Size(FLOAT_RECT_10, VHF) == V5F));
    BOOST_CHECK((Size(FLOAT_RECT_10, VDF) == V20F));

    const sf::Vector2<double> CUSTOM_SCALE_V(0.5, 2.0);
    BOOST_CHECK((Size(FLOAT_RECT_10, CUSTOM_SCALE_V) == sf::Vector2f(5.0f, 20.0f)));

    {
        sf::Image image;
        const sf::Vector2f SPRITE_SIZE_V { 10.0f, 10.0f };
        image.create(unsigned(SPRITE_SIZE_V.x), unsigned(SPRITE_SIZE_V.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        const sf::Vector2f SPRITE_POS_V { 0.0f, 0.0f };
        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        const sf::FloatRect SPRITE_BOUNDS_ORIG(SPRITE_POS_V, SPRITE_SIZE_V);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);

        BOOST_CHECK(Size(sprite) == SPRITE_SIZE_V);
        BOOST_CHECK(Size(sprite, HALF) == SPRITE_SIZE_V * HALF);
        BOOST_CHECK(Size(sprite, sf::Vector2f(CUSTOM_SCALE_V)) == sf::Vector2f(5.0f, 20.0f));
    }

    {
        sf::Sprite spriteDefault;
        BOOST_CHECK(Size(spriteDefault) == V0F);
        BOOST_CHECK(Size(spriteDefault, HALF) == V0F);
        BOOST_CHECK(Size(spriteDefault, sf::Vector2f(CUSTOM_SCALE_V)) == V0F);
    }

    // MinimallyEnclosing() tests
    BOOST_CHECK(MinimallyEnclosing(R0F, R0F) == R0F);
    BOOST_CHECK(MinimallyEnclosing(R0F, R0F, true) == R0F);
    BOOST_CHECK(MinimallyEnclosing(R1F, R1F) == R1F);
    BOOST_CHECK(MinimallyEnclosing(R1F, R1F, true) == R1F);
    BOOST_CHECK(MinimallyEnclosing(R0F, R1F) == sf::FloatRect(0.0f, 0.0f, 2.0f, 2.0f));
    BOOST_CHECK(MinimallyEnclosing(R0F, R1F, true) == R1F);

    const sf::Rect<int> RECT1(-10, -10, 10, 10);
    const sf::Rect<int> RECT2(10, 10, 10, 10);
    const sf::Rect<int> RECTE(-10, -10, 30, 30);

    BOOST_CHECK(MinimallyEnclosing(RECT1, RECT2) == RECTE);
    BOOST_CHECK(MinimallyEnclosing(RECT1, RECT2, true) == RECTE);

    const sf::Rect<int> RECT_TEST_A(0, 0, 0, 0);
    const sf::Rect<int> RECT_TEST_B(1, 1, 0, 0);
    BOOST_CHECK(MinimallyEnclosing(RECT_TEST_A, RECT_TEST_B) == sf::Rect<int>(0, 0, 1, 1));
    BOOST_CHECK(MinimallyEnclosing(RECT_TEST_A, RECT_TEST_B, true) == R0I);

    BOOST_CHECK(MinimallyEnclosing(RECT1, RECT2, R0I) == RECTE);
    BOOST_CHECK(MinimallyEnclosing(RECT1, RECT2, R0I, R1I) == RECTE);
    BOOST_CHECK(MinimallyEnclosing(RECT1, RECT2, R0I, R1I, RECTE) == RECTE);

    {
        sf::Sprite spriteDefault1;
        sf::Sprite spriteDefault2;

        BOOST_CHECK(MinimallyEnclosing(spriteDefault1, spriteDefault2) == R0F);
    }

    {
        sf::Image image;
        const sf::Vector2f SPRITE_SIZE_V { 10.0f, 10.0f };
        image.create(unsigned(SPRITE_SIZE_V.x), unsigned(SPRITE_SIZE_V.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite1(texture);
        sprite1.setPosition(-10.0f, -10.0f);

        sf::Sprite sprite2(texture);
        sprite2.setPosition(10.0f, 10.0f);

        sf::Sprite sprite3(texture);
        sprite3.setPosition(0.0f, 0.0f);

        sf::Sprite sprite4(texture);
        sprite4.setPosition(-5.0f, -5.0f);

        sf::Sprite sprite5(texture);
        sprite5.setPosition(5.0f, 5.0f);

        BOOST_CHECK(MinimallyEnclosing(sprite1, sprite2) == sf::FloatRect(RECTE));
        BOOST_CHECK(MinimallyEnclosing(sprite1, sprite2, sprite3) == sf::FloatRect(RECTE));
        BOOST_CHECK(MinimallyEnclosing(sprite1, sprite2, sprite3, sprite4) == sf::FloatRect(RECTE));

        BOOST_CHECK(
            MinimallyEnclosing(sprite1, sprite2, sprite3, sprite4, sprite5)
            == sf::FloatRect(RECTE));
    }

    const sf::IntRect GS_RECT_I(10, 20, 30, 40);

    // Grow/Shrink()

    //
    const sf::IntRect GS_RECT_GROWN_I(10, 20, 40, 40);
    BOOST_CHECK(GrowToSquareCopy(GS_RECT_I) == GS_RECT_GROWN_I);

    auto gsRectITemp { GS_RECT_I };
    ::GrowToSquare(gsRectITemp);
    BOOST_CHECK(gsRectITemp == GS_RECT_GROWN_I);

    //
    const sf::IntRect GS_RECT_GROWN_HALF_I(10, 20, 20, 20);
    BOOST_CHECK(GrowToSquareCopy(GS_RECT_I, 0.5f) == GS_RECT_GROWN_HALF_I);

    gsRectITemp = GS_RECT_I;
    GrowToSquare(gsRectITemp, 0.5f);
    BOOST_CHECK(gsRectITemp == GS_RECT_GROWN_HALF_I);

    //
    const sf::IntRect GS_RECT_SHRUNK_I(10, 20, 30, 30);
    BOOST_CHECK(ShrinkToSquareCopy(GS_RECT_I) == GS_RECT_SHRUNK_I);

    gsRectITemp = GS_RECT_I;
    ShrinkToSquare(gsRectITemp);
    BOOST_CHECK(gsRectITemp == GS_RECT_SHRUNK_I);

    //
    const sf::IntRect GS_RECT_SHRUNK_DOUBLE_I(10, 20, 60, 60);
    BOOST_CHECK(ShrinkToSquareCopy(GS_RECT_I, 2.0f) == GS_RECT_SHRUNK_DOUBLE_I);

    gsRectITemp = GS_RECT_I;
    ShrinkToSquare(gsRectITemp, 2.0f);
    BOOST_CHECK(gsRectITemp == GS_RECT_SHRUNK_DOUBLE_I);
}

BOOST_AUTO_TEST_CASE(ToStringTests)
{
    std::ostringstream ss;

    auto stringStreamTest = [&](const auto & V, const std::string & EXPECTED) {
        ss.str("");
        ss << V;
        BOOST_CHECK_EQUAL(ss.str(), EXPECTED);
    };

    stringStreamTest(sf::Vector2i(0, 0), "(0,0)");
    stringStreamTest(sf::Vector2i(123, 456), "(123,456)");
    stringStreamTest(sf::Rect<int>(0, 0, 0, 0), "(0,0/0x0)");
    stringStreamTest(sf::Rect<int>(1, 2, 3, 4), "(1,2/3x4)");
    stringStreamTest(sf::VideoMode(1, 2, 8), "(1x2:8)");
}

BOOST_AUTO_TEST_CASE(SetSizeAndPosTests)
{
    heroespath::misc::helpers::MersenneTwister19937::Setup();

    std::vector<sf::Texture> textures;

    auto makeSpriteOfPositionAndSize = [&](const sf::FloatRect & REGION) {
        sf::Image image;
        image.create(unsigned(REGION.width), unsigned(REGION.height));

        textures.push_back(sf::Texture());
        textures.back().loadFromImage(image);

        sf::Sprite sprite(textures.back());
        sprite.setPosition(Position(REGION));
        return sprite;
    };

    const sf::FloatRect SPRITE_RECT(12.0f, 34.0f, 56.0f, 78.0f);

    {
        auto sprite { makeSpriteOfPositionAndSize(SPRITE_RECT) };
        const auto SIZE_ORIG { Size(sprite) };
        const sf::FloatRect TARGET_RECT(0.0f, 0.0f, 0.0f, 0.0f);
        SetSizeAndPos(sprite, TARGET_RECT);

        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(sf::Vector2f(0.0f, 0.0f), SIZE_ORIG));
    }

    {
        auto sprite { makeSpriteOfPositionAndSize(SPRITE_RECT) };
        const sf::FloatRect TARGET_RECT(SPRITE_RECT);
        SetSizeAndPos(sprite, TARGET_RECT);
        BOOST_CHECK(SPRITE_RECT == TARGET_RECT);
        BOOST_CHECK(sprite.getGlobalBounds() == TARGET_RECT);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_RECT);
    }

    {
        auto sprite { makeSpriteOfPositionAndSize(SPRITE_RECT) };
        const auto SIZE_ORIG { Size(sprite) };
        const sf::FloatRect TARGET_RECT(-12.0f, -34.0f, 0.0f, 0.0f);
        SetSizeAndPos(sprite, TARGET_RECT);

        BOOST_CHECK(
            sprite.getGlobalBounds() == sf::FloatRect(sf::Vector2f(-12.0f, -34.0f), SIZE_ORIG));
    }

    {
        auto sprite { makeSpriteOfPositionAndSize(SPRITE_RECT) };
        const sf::FloatRect TARGET_RECT(123.0f, 456.0f, 789.0f, 101.0f);
        SetSizeAndPos(sprite, TARGET_RECT);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(TARGET_RECT));
    }

    {
        auto sprite { makeSpriteOfPositionAndSize(SPRITE_RECT) };
        const auto WIDTH_ORIG { sprite.getGlobalBounds().width };
        const sf::FloatRect TARGET_RECT(123.0f, 456.0f, 0.0f, 101.0f);
        SetSizeAndPos(sprite, TARGET_RECT);

        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(123.0f, 456.0f, WIDTH_ORIG, 101.0f));
    }

    {
        auto sprite { makeSpriteOfPositionAndSize(SPRITE_RECT) };
        const auto HEIGHT_ORIG { sprite.getGlobalBounds().height };
        const sf::FloatRect TARGET_RECT(123.0f, 456.0f, 789.0f, 0.0f);
        SetSizeAndPos(sprite, TARGET_RECT);

        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(123.0f, 456.0f, 789.0f, HEIGHT_ORIG));
    }
}

BOOST_AUTO_TEST_CASE(minimally_enclosing_tests)
{
    using namespace minimally_enclosing_test_stuff;

    // type checking up front
    {
        using Container_t = std::vector<sf::FloatRect>;

        const Container_t V = { sf::FloatRect {},
                                sf::FloatRect(1.0f, 1.0f, 1.0f, 1.0f),
                                sf::FloatRect(2.0f, 2.0f, 2.0f, 2.0f) };

        using Value_t = iterator_value_type_t<const Container_t::iterator>;
        using Rect_t = rect_type_picker_t<Value_t>;
        Rect_t minimalRect {};
    }

    const sf::Rect<float> R0F(0.0f, 0.0f, 0.0f, 0.0f);
    const sf::Rect<int> R0I { R0F };

    const sf::Rect<float> R1F(1.0f, 1.0f, 1.0f, 1.0f);
    const sf::Rect<int> R1I { R1F };

    const sf::Rect<float> R10F(10.0f, 10.0f, 10.0f, 10.0f);
    const sf::Rect<int> R10I { R10F };

    const sf::Rect<float> R10NPF(-10.0f, -10.0f, 10.0f, 10.0f);
    const sf::Rect<int> R10NPI { R10NPF };

    const sf::Rect<float> R10N30F(-10.0f, -10.0f, 30.0f, 30.0f);
    const sf::Rect<int> R10N30I { R10N30F };

    testWithAndWithoutIgnore(R0F, R0F, R0F, "R0F identity");
    testWithAndWithoutIgnore(R1F, R1F, R1F, "R1F identity");

    test(R0F, R1F, false, sf::FloatRect(0.0f, 0.0f, 2.0f, 2.0f), "R0F,R1F,false==(0,0,2,2)");

    test(R0F, R1F, true, R1F, "R0F,R1F,true==R1F");

    testWithAndWithoutIgnore(R10I, R10NPI, R10N30I, "R10I,(-10,-10,10,10)==(-10,-10,30,30)");

    const sf::Rect<int> R1_0I(1, 1, 0, 0);
    const sf::Rect<int> R0_1I(0, 0, 1, 1);
    test(R0I, R1_0I, false, R0_1I, "R0I,(1,1,0,0),false==(0,0,1,1)");
    test(R0I, R1_0I, true, R0I, "R0I,(1,1,0,0),true==R0I");

    {
        sf::Image image;
        const sf::Vector2f SPRITE_SIZE_V { 10.0f, 10.0f };
        image.create(unsigned(SPRITE_SIZE_V.x), unsigned(SPRITE_SIZE_V.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite1(texture);
        sprite1.setPosition(-10.0f, -10.0f);

        sf::Sprite sprite2(texture);
        sprite2.setPosition(10.0f, 10.0f);

        testWithAndWithoutIgnore(
            sprite1.getGlobalBounds(),
            sprite2.getGlobalBounds(),
            sf::FloatRect(-10.0f, -10.0f, 30.0f, 30.0f),
            "sprites:R10I,(-10,-10,10,10)==(-10,-10,30,30)");
    }

    {
        sf::Sprite spriteDefault1;
        sf::Sprite spriteDefault2;

        testWithAndWithoutIgnore(
            spriteDefault1.getGlobalBounds(),
            spriteDefault2.getGlobalBounds(),
            R0F,
            "default_sprites==R0F");
    }
}

BOOST_AUTO_TEST_CASE(intersects_tests)
{
    using namespace intersects_test_stuff;

    const sf::IntRect RI_0_0(0, 0, 0, 0);
    const sf::IntRect RI_1_1(1, 1, 1, 1);
    const sf::IntRect RI_N1_1(-1, -1, 1, 1);
    const sf::IntRect RI_10_1(10, 10, 1, 1);
    const sf::IntRect RI_N10_1(-10, -10, 1, 1);
    //
    const sf::FloatRect RF_0_0(RI_0_0);
    const sf::FloatRect RF_1_1(RI_1_1);
    const sf::FloatRect RF_N1_1(RI_N1_1);
    const sf::FloatRect RF_10_1(RI_10_1);
    const sf::FloatRect RF_N10_1(RI_N10_1);

    M_TEST_EQUAL_INTERSECTS(N1_1, N1_1, true);
    M_TEST_EQUAL_INTERSECTS(10_1, 10_1, true);
    M_TEST_EQUAL_INTERSECTS(N10_1, N10_1, true);

    M_TEST_INTERSECTION(0_0, 0_0, 0_0);
    M_TEST_INTERSECTION(N1_1, N1_1, N1_1);
    M_TEST_INTERSECTION(10_1, 10_1, 10_1);
    M_TEST_INTERSECTION(N10_1, N10_1, N10_1);

    M_TEST_EQUAL_INTERSECTS(1_1, 10_1, false);
    M_TEST_EQUAL_INTERSECTS(1_1, N10_1, false);
    M_TEST_EQUAL_INTERSECTS(1_1, N1_1, false);
    M_TEST_EQUAL_INTERSECTS(10_1, N10_1, false);
    M_TEST_EQUAL_INTERSECTS(10_1, N1_1, false);
    M_TEST_EQUAL_INTERSECTS(N10_1, N1_1, false);
    M_TEST_EQUAL_INTERSECTS(N1_1, 0_0, false);

    M_TEST_INTERSECTION(1_1, 10_1, 0_0);
    M_TEST_INTERSECTION(1_1, N10_1, 0_0);
    M_TEST_INTERSECTION(1_1, N1_1, 0_0);
    M_TEST_INTERSECTION(10_1, N10_1, 0_0);
    M_TEST_INTERSECTION(10_1, N1_1, 0_0);
    M_TEST_INTERSECTION(N10_1, N1_1, 0_0);
    M_TEST_INTERSECTION(N1_1, 0_0, 0_0);

    {
        const sf::IntRect RI_10_L_S(10, 10, 10, 1);
        const sf::IntRect RI_5_S_L(5, 5, 1, 10);
        //
        const sf::FloatRect RF_10_L_S(RI_10_L_S);
        const sf::FloatRect RF_5_S_L(RF_10_L_S);
        //
        M_TEST_EQUAL_INTERSECTS(10_L_S, 5_S_L, false);
        M_TEST_INTERSECTION(10_L_S, 5_S_L, 0_0);
    }

    {
        const sf::IntRect RI_N10_L_S(-10, -10, 10, 1);
        const sf::IntRect RI_N15_S_L(-15, -15, 1, 10);
        //
        const sf::FloatRect RF_N10_L_S(RI_N10_L_S);
        const sf::FloatRect RF_N15_S_L(RI_N15_S_L);
        //
        M_TEST_EQUAL_INTERSECTS(N10_L_S, N15_S_L, false);
        M_TEST_INTERSECTION(N10_L_S, N15_S_L, 0_0);
    }

    {
        const sf::IntRect RI_A(0, 0, 10, 1);
        const sf::IntRect RI_B(5, -5, 1, 10);
        //
        const sf::FloatRect RF_A(RI_A);
        const sf::FloatRect RF_B(RI_B);
        //
        M_TEST_EQUAL_INTERSECTS(A, B, true);
        //
        const sf::IntRect RI_C(5, 0, 1, 1);
        const sf::FloatRect RF_C(RI_C);
        M_TEST_INTERSECTION(A, B, C);
    }

    {
        const sf::IntRect RI_A(0, 0, 1, 10);
        const sf::IntRect RI_B(-5, 5, 10, 1);
        //
        const sf::FloatRect RF_A(RI_A);
        const sf::FloatRect RF_B(RI_B);
        //
        M_TEST_EQUAL_INTERSECTS(A, B, true);
        //
        const sf::IntRect RI_C(0, 5, 1, 1);
        const sf::FloatRect RF_C(RI_C);
        M_TEST_INTERSECTION(A, B, C);
    }

    {
        const sf::IntRect RI_Q1(0, -1, 1, 1);
        const sf::IntRect RI_Q2(-1, -1, 1, 1);
        const sf::IntRect RI_Q3(-1, 0, 1, 1);
        const sf::IntRect RI_Q4(0, 0, 1, 1);
        //
        const sf::FloatRect RF_Q1(RI_Q1);
        const sf::FloatRect RF_Q2(RI_Q2);
        const sf::FloatRect RF_Q3(RI_Q3);
        const sf::FloatRect RF_Q4(RI_Q4);
        //
        M_TEST_EQUAL_INTERSECTS(Q1, Q2, false);
        M_TEST_EQUAL_INTERSECTS(Q1, Q3, false);
        M_TEST_EQUAL_INTERSECTS(Q1, Q4, false);
        M_TEST_EQUAL_INTERSECTS(Q2, Q3, false);
        M_TEST_EQUAL_INTERSECTS(Q2, Q4, false);
        M_TEST_EQUAL_INTERSECTS(Q3, Q4, false);
        //
        M_TEST_INTERSECTION(Q1, Q2, 0_0);
        M_TEST_INTERSECTION(Q1, Q3, 0_0);
        M_TEST_INTERSECTION(Q1, Q4, 0_0);
        M_TEST_INTERSECTION(Q2, Q3, 0_0);
        M_TEST_INTERSECTION(Q2, Q4, 0_0);
        M_TEST_INTERSECTION(Q3, Q4, 0_0);
    }

    {
        const sf::IntRect RI_1_0(1, 1, 0, 0);
        const sf::IntRect RI_N1_0(-1, -1, 0, 0);
        //
        const sf::FloatRect RF_1_0(RI_1_0);
        const sf::FloatRect RF_N1_0(RI_N1_0);
        //
        M_TEST_EQUAL_INTERSECTS(0_0, 0_0, false);
        M_TEST_EQUAL_INTERSECTS(1_0, 1_0, false);
        M_TEST_EQUAL_INTERSECTS(N1_0, N1_0, false);
        //
        M_TEST_INTERSECTION(0_0, 0_0, 0_0);
        M_TEST_INTERSECTION(1_0, 1_0, 0_0);
        M_TEST_INTERSECTION(N1_0, N1_0, 0_0);
    }

    {
        const sf::IntRect RI_A(0, 0, 1, 1);
        const sf::IntRect RI_B(2, 2, -10, 1);
        const sf::IntRect RI_C(2, 2, -10, -1);
        const sf::IntRect RI_D(2, 2, -10, -10);
        const sf::IntRect RI_E(2, 2, 1, -10);
        const sf::IntRect RI_F(2, 2, -1, -10);
        //
        const sf::FloatRect RF_A(RI_A);
        const sf::FloatRect RF_B(RI_B);
        const sf::FloatRect RF_C(RI_C);
        const sf::FloatRect RF_D(RI_D);
        const sf::FloatRect RF_E(RI_E);
        const sf::FloatRect RF_F(RI_F);
        //
        M_TEST_EQUAL_INTERSECTS(A, B, false);
        M_TEST_EQUAL_INTERSECTS(A, C, false);

        // my custom function does not handle negative sizes but the SFML one does
        BOOST_CHECK_MESSAGE((RI_A.intersects(RI_D)), "SFML negative sizes test works");

        BOOST_CHECK_MESSAGE(
            !Intersects(RI_A, RI_D), "New negative sizes test fails because that is not supported");

        M_TEST_EQUAL_INTERSECTS(A, E, false);
        M_TEST_EQUAL_INTERSECTS(A, F, false);
        //
        M_TEST_INTERSECTION(A, B, 0_0);
        M_TEST_INTERSECTION(A, C, 0_0);
        M_TEST_INTERSECTION(A, D, 0_0);
        M_TEST_INTERSECTION(A, E, 0_0);
        M_TEST_INTERSECTION(A, F, 0_0);

        const sf::IntRect R(-5, -5, 10, 10);
        BOOST_CHECK(Contains(R, sf::Vector2i(-5, -5)));
        BOOST_CHECK(Contains(R, sf::Vector2i(5, -5)));
        BOOST_CHECK(Contains(R, sf::Vector2i(5, 5)));
        BOOST_CHECK(Contains(R, sf::Vector2i(-5, 5)));

        BOOST_CHECK(!Contains(R, sf::Vector2i(-6, -5)));
        BOOST_CHECK(!Contains(R, sf::Vector2i(-5, -6)));

        BOOST_CHECK(Contains(R, sf::Vector2i(-4, -4)));
        BOOST_CHECK(Contains(R, sf::Vector2i(0, 0)));
        BOOST_CHECK(Contains(R, sf::Vector2i(4, 4)));

        BOOST_CHECK(!Contains(R, sf::Vector2i(5, 6)));
        BOOST_CHECK(!Contains(R, sf::Vector2i(6, 5)));
        BOOST_CHECK(!Contains(R, sf::Vector2i(6, 6)));
    }
}
