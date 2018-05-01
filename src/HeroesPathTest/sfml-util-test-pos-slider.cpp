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
#define BOOST_TEST_MODULE "HeroesPathTestModule_sfml-util_texture_cache_test"

#include <boost/test/unit_test.hpp>

#include "misc/real.hpp"
#include "sfml-util/pos-slider.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include "Test-stuff.hpp"

using namespace test_stuff;
using namespace heroespath::sfml_util;
using namespace heroespath::misc;

inline bool AreVectorsClose(const sf::Vector2f & A, const sf::Vector2f & B)
{
    return IsRealClose(A.x, B.x) && IsRealClose(A.y, B.y);
}

inline bool AreVectorsCloseEnough(const sf::Vector2f & A, const sf::Vector2f & B)
{
    Constants constants;
    return IsRealClose(A.x, B.x) && IsRealClose(A.y, B.y);
}

BOOST_AUTO_TEST_CASE(PosSlider_Construction_DefaultValues)
{
    sliders::PosSlider pSlider;

    const sf::Vector2f ZERO_ZERO{ 0.0f, 0.0f };

    BOOST_CHECK(AreVectorsClose(pSlider.Position(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), ZERO_ZERO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealOne(pSlider.Speed()));

    pSlider.Setup(ZERO_ZERO, ZERO_ZERO, 1.0f);

    BOOST_CHECK(AreVectorsClose(pSlider.Position(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), ZERO_ZERO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealOne(pSlider.Speed()));

    pSlider.ChangeDirection();

    BOOST_CHECK(AreVectorsClose(pSlider.Position(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), ZERO_ZERO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Away);
    BOOST_CHECK(IsRealOne(pSlider.Speed()));

    pSlider.ChangeDirection();

    BOOST_CHECK(AreVectorsClose(pSlider.Position(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), ZERO_ZERO));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), ZERO_ZERO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealOne(pSlider.Speed()));
}

BOOST_AUTO_TEST_CASE(PosSlider_ConstructionAndSetup_SpecificValues)
{
    const sf::Vector2f FROM{ 123.123f, 456.456f };
    const sf::Vector2f TO{ 1234.56789f, -1234.56789f };
    auto const SPEED{ 12.34f };

    sliders::PosSlider pSlider(FROM, TO, SPEED);

    BOOST_CHECK(AreVectorsClose(pSlider.Position(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), TO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED));

    const sf::Vector2f FROM_ALT{ 321.321f, 654.654f };
    const sf::Vector2f TO_ALT{ -1234.56789f, 1234.56789f };
    auto const SPEED_ALT{ 43.21f };

    pSlider.Setup(FROM_ALT, TO_ALT, SPEED_ALT);

    BOOST_CHECK(AreVectorsClose(pSlider.Position(), FROM_ALT));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), FROM_ALT));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), TO_ALT));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED_ALT));
}

BOOST_AUTO_TEST_CASE(PosSlider_Moving)
{
    const sf::Vector2f FROM{ 50.0f, 50.0f };
    const sf::Vector2f TO{ 100.0f, 100.0f };
    auto const SPEED{ 1.0f };

    // setup and test valid initial state
    sliders::PosSlider pSlider(FROM, TO, SPEED);
    BOOST_CHECK(AreVectorsClose(pSlider.Position(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), TO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED));

    // verify that updating time when not moving does nothing
    BOOST_CHECK(pSlider.UpdateTime(1000.f) == false);
    BOOST_CHECK(AreVectorsClose(pSlider.Position(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), TO));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving() == false);
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED));

    // move toward target
    auto const DIFF_X_ORIG{ pSlider.To().x - pSlider.Position().x };
    auto const DIFF_Y_ORIG{ pSlider.To().y - pSlider.Position().y };
    pSlider.Start();
    BOOST_CHECK(pSlider.UpdateTime(0.1f));
    BOOST_CHECK(AreVectorsClose(pSlider.Position(), FROM) == false);
    BOOST_CHECK(AreVectorsClose(pSlider.From(), FROM));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), TO));
    BOOST_CHECK(pSlider.ProgressRatio() > 0.0f);
    BOOST_CHECK(pSlider.IsMoving());
    BOOST_CHECK(pSlider.Direction() == Moving::Toward);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED));

    auto const DIFF_X_AFTER_MOVING_TOWARD{ pSlider.To().x - pSlider.Position().x };
    auto const DIFF_Y_AFTER_MOVING_TOWARD{ pSlider.To().y - pSlider.Position().y };

    BOOST_CHECK(DIFF_X_AFTER_MOVING_TOWARD < DIFF_X_ORIG);
    BOOST_CHECK(DIFF_Y_AFTER_MOVING_TOWARD < DIFF_Y_ORIG);

    // change direction
    auto const POSITION_BEFORE_CHANGING_DIRECTION{ pSlider.Position() };
    pSlider.ChangeDirection();
    BOOST_CHECK(AreVectorsClose(pSlider.Position(), POSITION_BEFORE_CHANGING_DIRECTION));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), POSITION_BEFORE_CHANGING_DIRECTION));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), FROM));
    BOOST_CHECK(IsRealZero(pSlider.ProgressRatio()));
    BOOST_CHECK(pSlider.IsMoving());
    BOOST_CHECK(pSlider.Direction() == Moving::Away);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED));

    // move away from target (more than prev moved toward)
    BOOST_CHECK(pSlider.UpdateTime(0.5f));
    BOOST_CHECK(AreVectorsClose(pSlider.From(), POSITION_BEFORE_CHANGING_DIRECTION));
    BOOST_CHECK(AreVectorsClose(pSlider.To(), FROM));
    BOOST_CHECK(pSlider.ProgressRatio() > 0.0f);
    BOOST_CHECK(pSlider.IsMoving());
    BOOST_CHECK(pSlider.Direction() == Moving::Away);
    BOOST_CHECK(IsRealClose(pSlider.Speed(), SPEED));

    auto const DIFF_X_AFTER_MOVING_AWAY{ pSlider.To().x - pSlider.Position().x };
    auto const DIFF_Y_AFTER_MOVING_AWAY{ pSlider.To().y - pSlider.Position().y };

    BOOST_CHECK(DIFF_X_AFTER_MOVING_AWAY < DIFF_X_ORIG);
    BOOST_CHECK(DIFF_Y_AFTER_MOVING_AWAY < DIFF_Y_ORIG);

    BOOST_CHECK(DIFF_X_AFTER_MOVING_AWAY < DIFF_X_AFTER_MOVING_TOWARD);
    BOOST_CHECK(DIFF_Y_AFTER_MOVING_AWAY < DIFF_Y_AFTER_MOVING_TOWARD);
}
