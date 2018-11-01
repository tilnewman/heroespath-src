// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_sfml-util_sfml-util_test"

#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4266)
#endif

#include <boost/test/unit_test.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include "misc/log-macros.hpp"
#include "misc/log.hpp"
#include "misc/platform.hpp"
#include "misc/random.hpp"
#include "sfml-util/display.hpp"
#include "sfutil/angles.hpp"
#include "sfutil/blend-mode.hpp"
#include "sfutil/center-of.hpp"
#include "sfutil/center-to.hpp"
#include "sfutil/center.hpp"
#include "sfutil/color.hpp"
#include "sfutil/direction.hpp"
#include "sfutil/display.hpp"
#include "sfutil/distance.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/font.hpp"
#include "sfutil/image-manip.hpp"
#include "sfutil/keyboard.hpp"
#include "sfutil/overlap.hpp"
#include "sfutil/position.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/rectangle-shape.hpp"
#include "sfutil/size-and-scale.hpp"
#include "sfutil/sprite-texture.hpp"
#include "sfutil/vector-and-rect.hpp"
#include "sfutil/vector-math.hpp"
#include "sfutil/vertex.hpp"
#include "sfutil/video-mode.hpp"

#include <SFML/Window/WindowStyle.hpp>

#include <iostream>
#include <limits>
#include <vector>

#include "unit-test-test-stuff.hpp"

using namespace test_stuff;
using namespace heroespath::gui;
using namespace heroespath::sfutil;
using namespace heroespath::misc;

using heroespath::misc::IsRealClose;

BOOST_AUTO_TEST_CASE(SFML_Default_Values_Tests)
{
    auto areVectorValuesZero = [](const sf::Vector2f & V) {
        return (!(V.x < 0.0f) && !(V.x > 0.0f) && !(V.y < 0.0f) && !(V.y > 0.0f));
    };

    auto areRectValuesZero = [](const sf::FloatRect & R) {
        return (
            !(R.left < 0.0f) && !(R.left > 0.0f) && !(R.top < 0.0f) && !(R.top > 0.0f)
            && !(R.width < 0.0f) && !(R.width > 0.0f) && !(R.height < 0.0f) && !(R.height > 0.0f));
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

    for (heroespath::misc::EnumUnderlying_t i(0); i <= Orientation::Count; ++i)
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

    BOOST_CHECK(IsZeroOrLessEither(ZNV));
    BOOST_CHECK(IsZeroOrLessEither(ZZV));
    BOOST_CHECK(IsZeroOrLessEither(ZPV));
    BOOST_CHECK(IsZeroOrLessEither(ZOV));
    BOOST_CHECK(IsZeroOrLessEither(ZLV));
    BOOST_CHECK(IsZeroOrLessEither(NNV));
    BOOST_CHECK(IsZeroOrLessEither(NZV));
    BOOST_CHECK(IsZeroOrLessEither(NPV));
    BOOST_CHECK(IsZeroOrLessEither(NOV));
    BOOST_CHECK(IsZeroOrLessEither(NLV));
    BOOST_CHECK(IsZeroOrLessEither(PNV));
    BOOST_CHECK(IsZeroOrLessEither(PZV));
    BOOST_CHECK(IsZeroOrLessEither(PPV) == false);
    BOOST_CHECK(IsZeroOrLessEither(POV) == false);
    BOOST_CHECK(IsZeroOrLessEither(PLV) == false);
    BOOST_CHECK(IsZeroOrLessEither(ONV));
    BOOST_CHECK(IsZeroOrLessEither(OZV));
    BOOST_CHECK(IsZeroOrLessEither(OPV) == false);
    BOOST_CHECK(IsZeroOrLessEither(OOV) == false);
    BOOST_CHECK(IsZeroOrLessEither(OLV) == false);
    BOOST_CHECK(IsZeroOrLessEither(LNV));
    BOOST_CHECK(IsZeroOrLessEither(LZV));
    BOOST_CHECK(IsZeroOrLessEither(LPV) == false);
    BOOST_CHECK(IsZeroOrLessEither(LOV) == false);
    BOOST_CHECK(IsZeroOrLessEither(LLV) == false);

    BOOST_CHECK(IsZeroOrLessBoth(ZNV));
    BOOST_CHECK(IsZeroOrLessBoth(ZZV));
    BOOST_CHECK(IsZeroOrLessBoth(ZPV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(ZOV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(ZLV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(NNV));
    BOOST_CHECK(IsZeroOrLessBoth(NZV));
    BOOST_CHECK(IsZeroOrLessBoth(NPV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(NOV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(NLV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(PNV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(PZV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(PPV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(POV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(PLV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(ONV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(OZV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(OPV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(OOV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(OLV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(LNV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(LZV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(LPV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(LOV) == false);
    BOOST_CHECK(IsZeroOrLessBoth(LLV) == false);

    BOOST_CHECK(IsLessThanOneEither(ZNV));
    BOOST_CHECK(IsLessThanOneEither(ZZV));
    BOOST_CHECK(IsLessThanOneEither(ZPV));
    BOOST_CHECK(IsLessThanOneEither(ZOV));
    BOOST_CHECK(IsLessThanOneEither(ZLV));
    BOOST_CHECK(IsLessThanOneEither(NNV));
    BOOST_CHECK(IsLessThanOneEither(NZV));
    BOOST_CHECK(IsLessThanOneEither(NPV));
    BOOST_CHECK(IsLessThanOneEither(NOV));
    BOOST_CHECK(IsLessThanOneEither(NLV));
    BOOST_CHECK(IsLessThanOneEither(PNV));
    BOOST_CHECK(IsLessThanOneEither(PZV));
    BOOST_CHECK(IsLessThanOneEither(PPV));
    BOOST_CHECK(IsLessThanOneEither(POV));
    BOOST_CHECK(IsLessThanOneEither(PLV));
    BOOST_CHECK(IsLessThanOneEither(ONV));
    BOOST_CHECK(IsLessThanOneEither(OZV));
    BOOST_CHECK(IsLessThanOneEither(OPV));
    BOOST_CHECK(IsLessThanOneEither(OOV) == false);
    BOOST_CHECK(IsLessThanOneEither(OLV) == false);
    BOOST_CHECK(IsLessThanOneEither(LNV));
    BOOST_CHECK(IsLessThanOneEither(LZV));
    BOOST_CHECK(IsLessThanOneEither(LPV));
    BOOST_CHECK(IsLessThanOneEither(LOV) == false);
    BOOST_CHECK(IsLessThanOneEither(LLV) == false);

    BOOST_CHECK(IsLessThanOneBoth(ZNV));
    BOOST_CHECK(IsLessThanOneBoth(ZZV));
    BOOST_CHECK(IsLessThanOneBoth(ZPV));
    BOOST_CHECK(IsLessThanOneBoth(ZOV) == false);
    BOOST_CHECK(IsLessThanOneBoth(ZLV) == false);
    BOOST_CHECK(IsLessThanOneBoth(NNV));
    BOOST_CHECK(IsLessThanOneBoth(NZV));
    BOOST_CHECK(IsLessThanOneBoth(NPV));
    BOOST_CHECK(IsLessThanOneBoth(NOV) == false);
    BOOST_CHECK(IsLessThanOneBoth(NLV) == false);
    BOOST_CHECK(IsLessThanOneBoth(PNV));
    BOOST_CHECK(IsLessThanOneBoth(PZV));
    BOOST_CHECK(IsLessThanOneBoth(PPV));
    BOOST_CHECK(IsLessThanOneBoth(POV) == false);
    BOOST_CHECK(IsLessThanOneBoth(PLV) == false);
    BOOST_CHECK(IsLessThanOneBoth(ONV) == false);
    BOOST_CHECK(IsLessThanOneBoth(OZV) == false);
    BOOST_CHECK(IsLessThanOneBoth(OPV) == false);
    BOOST_CHECK(IsLessThanOneBoth(OOV) == false);
    BOOST_CHECK(IsLessThanOneBoth(OLV) == false);
    BOOST_CHECK(IsLessThanOneBoth(LNV) == false);
    BOOST_CHECK(IsLessThanOneBoth(LZV) == false);
    BOOST_CHECK(IsLessThanOneBoth(LPV) == false);
    BOOST_CHECK(IsLessThanOneBoth(LOV) == false);
    BOOST_CHECK(IsLessThanOneBoth(LLV) == false);

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

    BOOST_CHECK(IsSizeZeroOrLessEither(ZNR));
    BOOST_CHECK(IsSizeZeroOrLessEither(ZZR));
    BOOST_CHECK(IsSizeZeroOrLessEither(ZPR));
    BOOST_CHECK(IsSizeZeroOrLessEither(ZOR));
    BOOST_CHECK(IsSizeZeroOrLessEither(ZLR));
    BOOST_CHECK(IsSizeZeroOrLessEither(NNR));
    BOOST_CHECK(IsSizeZeroOrLessEither(NZR));
    BOOST_CHECK(IsSizeZeroOrLessEither(NPR));
    BOOST_CHECK(IsSizeZeroOrLessEither(NOR));
    BOOST_CHECK(IsSizeZeroOrLessEither(NLR));
    BOOST_CHECK(IsSizeZeroOrLessEither(PNR));
    BOOST_CHECK(IsSizeZeroOrLessEither(PZR));
    BOOST_CHECK(IsSizeZeroOrLessEither(PPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(POR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(PLR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(ONR));
    BOOST_CHECK(IsSizeZeroOrLessEither(OZR));
    BOOST_CHECK(IsSizeZeroOrLessEither(OPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(OOR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(OLR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(LNR));
    BOOST_CHECK(IsSizeZeroOrLessEither(LZR));
    BOOST_CHECK(IsSizeZeroOrLessEither(LPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(LOR) == false);
    BOOST_CHECK(IsSizeZeroOrLessEither(LLR) == false);

    BOOST_CHECK(IsSizeZeroOrLessBoth(ZNR));
    BOOST_CHECK(IsSizeZeroOrLessBoth(ZZR));
    BOOST_CHECK(IsSizeZeroOrLessBoth(ZPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(ZOR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(ZLR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(NNR));
    BOOST_CHECK(IsSizeZeroOrLessBoth(NZR));
    BOOST_CHECK(IsSizeZeroOrLessBoth(NPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(NOR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(NLR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(PNR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(PZR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(PPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(POR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(PLR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(ONR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(OZR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(OPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(OOR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(OLR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(LNR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(LZR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(LPR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(LOR) == false);
    BOOST_CHECK(IsSizeZeroOrLessBoth(LLR) == false);

    BOOST_CHECK(IsSizeLessThanOneEither(ZNR));
    BOOST_CHECK(IsSizeLessThanOneEither(ZZR));
    BOOST_CHECK(IsSizeLessThanOneEither(ZPR));
    BOOST_CHECK(IsSizeLessThanOneEither(ZOR));
    BOOST_CHECK(IsSizeLessThanOneEither(ZLR));
    BOOST_CHECK(IsSizeLessThanOneEither(NNR));
    BOOST_CHECK(IsSizeLessThanOneEither(NZR));
    BOOST_CHECK(IsSizeLessThanOneEither(NPR));
    BOOST_CHECK(IsSizeLessThanOneEither(NOR));
    BOOST_CHECK(IsSizeLessThanOneEither(NLR));
    BOOST_CHECK(IsSizeLessThanOneEither(PNR));
    BOOST_CHECK(IsSizeLessThanOneEither(PZR));
    BOOST_CHECK(IsSizeLessThanOneEither(PPR));
    BOOST_CHECK(IsSizeLessThanOneEither(POR));
    BOOST_CHECK(IsSizeLessThanOneEither(PLR));
    BOOST_CHECK(IsSizeLessThanOneEither(ONR));
    BOOST_CHECK(IsSizeLessThanOneEither(OZR));
    BOOST_CHECK(IsSizeLessThanOneEither(OPR));
    BOOST_CHECK(IsSizeLessThanOneEither(OOR) == false);
    BOOST_CHECK(IsSizeLessThanOneEither(OLR) == false);
    BOOST_CHECK(IsSizeLessThanOneEither(LNR));
    BOOST_CHECK(IsSizeLessThanOneEither(LZR));
    BOOST_CHECK(IsSizeLessThanOneEither(LPR));
    BOOST_CHECK(IsSizeLessThanOneEither(LOR) == false);
    BOOST_CHECK(IsSizeLessThanOneEither(LLR) == false);

    BOOST_CHECK(IsSizeLessThanOneBoth(ZNR));
    BOOST_CHECK(IsSizeLessThanOneBoth(ZZR));
    BOOST_CHECK(IsSizeLessThanOneBoth(ZPR));
    BOOST_CHECK(IsSizeLessThanOneBoth(ZOR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(ZLR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(NNR));
    BOOST_CHECK(IsSizeLessThanOneBoth(NZR));
    BOOST_CHECK(IsSizeLessThanOneBoth(NPR));
    BOOST_CHECK(IsSizeLessThanOneBoth(NOR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(NLR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(PNR));
    BOOST_CHECK(IsSizeLessThanOneBoth(PZR));
    BOOST_CHECK(IsSizeLessThanOneBoth(PPR));
    BOOST_CHECK(IsSizeLessThanOneBoth(POR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(PLR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(ONR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(OZR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(OPR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(OOR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(OLR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(LNR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(LZR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(LPR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(LOR) == false);
    BOOST_CHECK(IsSizeLessThanOneBoth(LLR) == false);
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

    BOOST_CHECK(CenterCopy(ZERO_V_I) == SCREEN_CENTER_V);
    BOOST_CHECK(CenterCopy(ZERO_V_F) == SCREEN_CENTER_V);
    BOOST_CHECK(CenterCopy(SIZE_V_F) == SCREEN_CENTER_V - (SIZE_V_F * 0.5f));
    BOOST_CHECK(CenterCopy(SIZE_V_I) == (SCREEN_CENTER_V - (sf::Vector2f(SIZE_V_I) * 0.5f)));

    //
    BOOST_CHECK(CenterCopy(ZERO_RECT_F) == ZERO_RECT_CENTERED_F);
    BOOST_CHECK(CenterCopy(ZERO_RECT_I) == ZERO_RECT_CENTERED_F);
    BOOST_CHECK(CenterCopy(ZERO_RECT_F, SCALE_V_F) == ZERO_RECT_CENTERED_F);

    // THIS SHOULD NOT COMPILE
    // BOOST_CHECK(CenterCopy(ZERO_RECT_F, ORIGIN_WRAP_RECT_F) == ZERO_RECT_CENTERED_F);

    BOOST_CHECK(
        CenterCopy(RECT_I)
        == sf::FloatRect(
            SCREEN_CENTER_V - (sf::Vector2f(300.0f, 400.0f) * 0.5f), sf::Vector2f(300.0f, 400.0f)));

    BOOST_CHECK(
        CenterCopy(RECT_I, SCALE_V_F)
        == sf::FloatRect(
            SCREEN_CENTER_V - ScaleCopy(sf::Vector2f(300.0f, 400.0f), SCALE_V_F * 0.5f),
            ScaleCopy(sf::Vector2f(300.0f, 400.0f), SCALE_V_F)));

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

    BOOST_CHECK((FitCopy(10.0f, 100.0f, 0.0, 0.0) == TALL_V_F));
    BOOST_CHECK((FitCopy(10.0f, 100.0f, -1.0, -0.1) == TALL_V_F));

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
    BOOST_CHECK(FitAndCenterCopy(TALL_RECT_F, WIDE_RECT_I) == TALL_FITANDCTO_WIDE_F);

    tallRectFTemp = TALL_RECT_F;
    FitAndCenterTo(tallRectFTemp, WIDE_RECT_F);
    BOOST_CHECK(tallRectFTemp == TALL_FITANDCTO_WIDE_F);

    /*{
        // if FitAndCenterTo() to a rect with either width/height zero then only the other non-zero
        // dimmension is used to scale
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(0, 0, 20, 40), sf::IntRect(10, 10, 20, 0))
            == sf::IntRect(0, 0, 100, 200));

        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-60, -220, 100, 200));

        // if FitAndCenterTo() to a rect with both width/height zero then the resulting size is zero
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 0))
            == sf::IntRect(-10, -20, 0, 0));

        //...even if scaled
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 0), 69.0f)
            == sf::IntRect(-10, -20, 0, 0));

        // if FitAndCenterTo() with opposite dimmensions zero result in size of zero
        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 50, 0), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-10, -120, 0, 0));

        BOOST_CHECK(
            FitAndCenterCopy(sf::IntRect(10, 20, 0, 100), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(-60, -20, 0, 0));

        // if FitAndCenterTo() with matching dimmensions zero results in a scale based on the
    non-zero
        // dimmension
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
        // if FitAndReCenter() to a rect with either width/height zero then only the other non-zero
        // dimmension is used to scale
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(-65, -130, 100, 200));

        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-65, -130, 100, 200));

        // if FitAndReCenter() to a rect with both width/height zero then the resulting size is zero
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 0))
            == sf::IntRect(-15, -30, 0, 0));

        //...even if scaled
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 100), sf::IntRect(-10, -20, 0, 0), 69.0f)
            == sf::IntRect(-15, -30, 0, 0));

        // if FitAndReCenter() with opposite dimmensions zero result in size of zero
        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 50, 0), sf::IntRect(-10, -20, 0, 200))
            == sf::IntRect(-15, 20, 0, 0));

        BOOST_CHECK(
            FitAndReCenterCopy(sf::IntRect(10, 20, 0, 100), sf::IntRect(-10, -20, 100, 0))
            == sf::IntRect(10, -30, 0, 0));

        // if FitAndReCenter() with matching dimmensions zero results in a scale based on the
        // non-zero dimmension
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

    const sf::Vector2f SCALEV1(0.5f, 2.0f);

    auto tempRect1(R1F);
    ScaleAndReCenter(tempRect1, 1.0f);
    BOOST_CHECK(tempRect1 == R1F);
    tempRect1 = R1F;
    ScaleAndReCenter(tempRect1, 1.0f);
    BOOST_CHECK(tempRect1 == R1F);
    //
    tempRect1 = R1F;
    ScaleAndReCenter(tempRect1, HALF);
    BOOST_CHECK(tempRect1 == sf::FloatRect(1.25f, 1.25f, 0.5f, 0.5f));
    ScaleAndReCenter(tempRect1, DOUBLE);
    BOOST_CHECK(tempRect1 == R1F);
    //
    tempRect1 = R1F;
    ScaleAndReCenter(tempRect1, sf::Vector2f(HALF, DOUBLE));
    BOOST_CHECK(tempRect1 == sf::FloatRect(1.25f, 0.5f, 0.5f, 2.0f));
    ScaleAndReCenter(tempRect1, sf::Vector2f(DOUBLE, HALF));
    BOOST_CHECK(tempRect1 == R1F);

    // test of ScaleCopy(Vector, SCALE)
    BOOST_CHECK(ScaleCopy(V0F, 0.0f) == V0F);
    BOOST_CHECK(ScaleCopy(V0F, 1.0f) == V0F);
    BOOST_CHECK(ScaleCopy(V0F, 0.0) == V0F);
    BOOST_CHECK(ScaleCopy(V0F, 1.0) == V0F);
    //
    BOOST_CHECK(ScaleCopy(V0I, 0.0f) == V0I);
    BOOST_CHECK(ScaleCopy(V0I, 1.0f) == V0I);
    BOOST_CHECK(ScaleCopy(V0I, 0.0) == V0I);
    BOOST_CHECK(ScaleCopy(V0I, 1.0) == V0I);
    ////
    BOOST_CHECK(ScaleCopy(V1F, 0.0f) == V0F);
    BOOST_CHECK(ScaleCopy(V1F, 1.0f) == V1F);
    BOOST_CHECK(ScaleCopy(V1F, 0.0) == V0F);
    BOOST_CHECK(ScaleCopy(V1F, 1.0) == V1F);
    //
    BOOST_CHECK(ScaleCopy(V1I, 0.0f) == V0I);
    BOOST_CHECK(ScaleCopy(V1I, 1.0f) == V1I);
    BOOST_CHECK(ScaleCopy(V1I, 0.0) == V0I);
    BOOST_CHECK(ScaleCopy(V1I, 1.0) == V1I);
    ////
    BOOST_CHECK(ScaleCopy(VRF, 0.0f) == V0F);
    BOOST_CHECK(ScaleCopy(VRF, 1.0f) == VRF);
    BOOST_CHECK(ScaleCopy(VRF, 0.0) == V0F);
    BOOST_CHECK(ScaleCopy(VRF, 1.0) == VRF);
    //
    BOOST_CHECK(ScaleCopy(VRI, 0.0f) == V0I);
    BOOST_CHECK(ScaleCopy(VRI, 1.0f) == VRI);
    BOOST_CHECK(ScaleCopy(VRI, 0.0) == V0I);
    BOOST_CHECK(ScaleCopy(VRI, 1.0) == VRI);
    ////
    const auto XF { 123.789f };
    const int XI { static_cast<int>(XF) };
    const sf::Vector2f VXF(XF, XF);
    const sf::Vector2f VXXF(XF * XF, XF * XF);
    const sf::Vector2i VXI(XI, XI);
    BOOST_CHECK(ScaleCopy(V0F, XF) == V0F);
    BOOST_CHECK(ScaleCopy(V1F, XF) == VXF);
    BOOST_CHECK(ScaleCopy(VXF, XF) == VXXF);

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

    auto temp1VRF { VRF };
    Scale(temp1VRF, 0.0f);
    BOOST_CHECK(temp1VRF == V0F);

    auto temp2VRF { VRF };
    Scale(temp2VRF, 1.0f);
    BOOST_CHECK(temp2VRF == VRF);

    auto temp3VRF { VRF };
    Scale(temp3VRF, HALF);
    BOOST_CHECK(temp3VRF == VRHF);

    auto temp4VRF { VRF };
    Scale(temp4VRF, DOUBLE);
    BOOST_CHECK(temp4VRF == VRDF);

    auto temp5VRF { VRF };
    Scale(temp5VRF, VRF);
    BOOST_CHECK(temp5VRF == VRRF);

    // ScaleRectLinearCopy() tests
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
    //
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, 0.0f) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, 1.0f) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, HALF) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, DOUBLE) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, 0.0f) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, 1.0f) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, HALF) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, DOUBLE) == FLOAT_RECT_0);
    //
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, 0.0f) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, 1.0f) == INT_RECT_10);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, HALF) == INT_RECT_5);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, DOUBLE) == INT_RECT_20);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, 0.0f) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, 1.0f) == FLOAT_RECT_10);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, HALF) == FLOAT_RECT_5);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, DOUBLE) == FLOAT_RECT_20);
    //
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, V0F) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, V1F) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, VHF) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_0, VDF) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, V0F) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, V1F) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, VHF) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_0, VDF) == FLOAT_RECT_0);
    //
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, V0F) == INT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, V1F) == INT_RECT_10);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, VHF) == INT_RECT_5);
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, VDF) == INT_RECT_20);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, V0F) == FLOAT_RECT_0);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, V1F) == FLOAT_RECT_10);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, VHF) == FLOAT_RECT_5);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, VDF) == FLOAT_RECT_20);
    //
    BOOST_CHECK(ScaleRectLinearCopy(INT_RECT_10, SCALEV1) == INT_RECT_10_SCALEV1);
    BOOST_CHECK(ScaleRectLinearCopy(FLOAT_RECT_10, SCALEV1) == FLOAT_RECT_10_SCALEV1);
    //
    auto tempIntRect10 { INT_RECT_10 };
    ScaleRectLinear(tempIntRect10, SCALEV1);
    BOOST_CHECK(tempIntRect10 == INT_RECT_10_SCALEV1);
    //
    auto tempFloatRect10 { FLOAT_RECT_10 };
    ScaleRectLinear(tempFloatRect10, SCALEV1);
    BOOST_CHECK(tempFloatRect10 == FLOAT_RECT_10_SCALEV1);

    // ScaleSize() tests
    BOOST_CHECK(ScaleSizeCopy(R0F, 0.0f) == R0F);
    BOOST_CHECK(ScaleSizeCopy(R0F, 1.0f) == R0F);
    BOOST_CHECK(ScaleSizeCopy(R0F, HALF) == R0F);
    BOOST_CHECK(ScaleSizeCopy(R0F, DOUBLE) == R0F);
    //
    BOOST_CHECK(ScaleSizeCopy(R0I, 0.0f) == R0I);
    BOOST_CHECK(ScaleSizeCopy(R0I, 1.0f) == R0I);
    BOOST_CHECK(ScaleSizeCopy(R0I, HALF) == R0I);
    BOOST_CHECK(ScaleSizeCopy(R0I, DOUBLE) == R0I);
    //
    BOOST_CHECK(ScaleSizeCopy(R1F, 0.0f) == sf::FloatRect(V1F, V0F));
    BOOST_CHECK(ScaleSizeCopy(R1F, 1.0f) == R1F);
    //
    BOOST_CHECK(ScaleSizeCopy(R1F, V0F) == sf::FloatRect(V1F, V0F));
    BOOST_CHECK(ScaleSizeCopy(R1F, V1F) == R1F);
    //
    BOOST_CHECK(ScaleSizeCopy(RRF, V0F) == sf::FloatRect(VRF, V0F));
    BOOST_CHECK(ScaleSizeCopy(RRF, V1F) == RRF);
    BOOST_CHECK(ScaleSizeCopy(RRF, VHF) == RRHF);
    BOOST_CHECK(ScaleSizeCopy(RRF, VDF) == RRDF);

    auto tempFloatRectRRF { RRF };
    ScaleSize(tempFloatRectRRF, 1.0f);
    BOOST_CHECK(tempFloatRectRRF == RRF);

    auto temp2FloatRectRRF { RRF };
    ScaleSize(temp2FloatRectRRF, VHF);
    BOOST_CHECK(temp2FloatRectRRF == RRHF);

    // ScaleSizeAndReCenterCopy() tests
    const sf::Rect<int> SSARC_RECT_ORIG(10, 10, 10, 10);
    const sf::Rect<int> SSARC_RECT_DOUBLE_SIZE_AND_RECENTERED(5, 5, 20, 20);

    BOOST_CHECK(
        ScaleSizeAndReCenterCopy(SSARC_RECT_ORIG, DOUBLE) == SSARC_RECT_DOUBLE_SIZE_AND_RECENTERED);

    BOOST_CHECK(
        ScaleSizeAndReCenterCopy(SSARC_RECT_ORIG, VDF) == SSARC_RECT_DOUBLE_SIZE_AND_RECENTERED);

    auto ssarcRectOrig1 { SSARC_RECT_ORIG };
    auto ssarcRectOrig2 { SSARC_RECT_ORIG };
    ScaleSizeAndReCenter(ssarcRectOrig1, DOUBLE);
    ScaleSizeAndReCenter(ssarcRectOrig2, VDF);
    BOOST_CHECK(ssarcRectOrig1 == SSARC_RECT_DOUBLE_SIZE_AND_RECENTERED);
    BOOST_CHECK(ssarcRectOrig2 == SSARC_RECT_DOUBLE_SIZE_AND_RECENTERED);

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

        BOOST_CHECK(
            MinimallyEnclosing(sprite1, sprite2) == sf::FloatRect(-10.0f, -10.0f, 30.0f, 30.0f));
    }

    {
        sf::Sprite spriteDefault1;
        sf::Sprite spriteDefault2;

        BOOST_CHECK(MinimallyEnclosing(spriteDefault1, spriteDefault2) == R0F);
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
    ::GrowToSquare(gsRectITemp, 0.5f);
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

    // Grow/Shrink AndReCenter()

    //
    const sf::IntRect GS_RECT_GROWN_RECENTERED_I(5, 20, 40, 40);
    BOOST_CHECK(GrowToSquareAndReCenterCopy(GS_RECT_I) == GS_RECT_GROWN_RECENTERED_I);

    gsRectITemp = GS_RECT_I;
    ::GrowToSquareAndReCenter(gsRectITemp);
    BOOST_CHECK(gsRectITemp == GS_RECT_GROWN_RECENTERED_I);

    //
    const sf::IntRect GS_RECT_GROWN_RECENTERED_HALF_I(15, 30, 20, 20);
    BOOST_CHECK(GrowToSquareAndReCenterCopy(GS_RECT_I, 0.5f) == GS_RECT_GROWN_RECENTERED_HALF_I);

    gsRectITemp = GS_RECT_I;
    ::GrowToSquareAndReCenter(gsRectITemp, 0.5f);
    BOOST_CHECK(gsRectITemp == GS_RECT_GROWN_RECENTERED_HALF_I);

    //
    const sf::IntRect GS_RECT_SHRUNK_RECENTERED_I(10, 25, 30, 30);
    BOOST_CHECK(ShrinkToSquareAndReCenterCopy(GS_RECT_I) == GS_RECT_SHRUNK_RECENTERED_I);

    gsRectITemp = GS_RECT_I;
    ShrinkToSquareAndReCenter(gsRectITemp);
    BOOST_CHECK(gsRectITemp == GS_RECT_SHRUNK_RECENTERED_I);

    //
    const sf::IntRect GS_RECT_SHRUNK_RECENTERED_DOUBLE_I(-5, 10, 60, 60);

    BOOST_CHECK(
        ShrinkToSquareAndReCenterCopy(GS_RECT_I, 2.0f) == GS_RECT_SHRUNK_RECENTERED_DOUBLE_I);

    gsRectITemp = GS_RECT_I;
    ShrinkToSquareAndReCenter(gsRectITemp, 2.0f);
    BOOST_CHECK(gsRectITemp == GS_RECT_SHRUNK_RECENTERED_DOUBLE_I);
}

BOOST_AUTO_TEST_CASE(ToStringTests)
{
    BOOST_CHECK(ToString(sf::Vector2i(0, 0)) == "(0,0)");
    BOOST_CHECK(ToString(sf::Vector2i(123, 456)) == "(123,456)");
    BOOST_CHECK(ToString(sf::Rect<int>(0, 0, 0, 0)) == "(0,0/0x0)");
    BOOST_CHECK(ToString(sf::Rect<int>(1, 2, 3, 4)) == "(1,2/3x4)");
    BOOST_CHECK(ToString(sf::VideoMode(1, 2, 8)) == "(1x2:8)");
}

BOOST_AUTO_TEST_CASE(SetSizeAndPosTests)
{
    srand(static_cast<unsigned>(time(nullptr)));
    heroespath::misc::random::MersenneTwister::Seed();

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
