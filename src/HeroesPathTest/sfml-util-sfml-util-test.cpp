// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#define BOOST_TEST_MODULE "HeroesPathTestModule_sfml-util_sfml-util_test"

#include <boost/test/unit_test.hpp>

#include "log/logger.hpp"
#include "log/macros.hpp"
#include "misc/platform.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sfml-util.hpp"

#include "Test-stuff.hpp"

using namespace test_stuff;
using namespace heroespath::sfml_util;
using namespace heroespath::misc;

using heroespath::misc::IsRealClose;

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
    const sf::Vector2f IMAGE_RESIZED_RECTCENTERED_V_F(RECT_CENTER_V_F - (RESIZE_V_F * 0.5f));
    sf::Image image;
    image.create(unsigned(SIZE_V_F.x), unsigned(SIZE_V_F.y));

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);

    const sf::Vector2f POS_V_F(789.0f, 987.0f);
    sprite.setPosition(POS_V_F);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(POS_V_F, SIZE_V_F));
    CenterTo(sprite, ZERO_RECT_I);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));
    CenterTo(sprite, ZERO_RECT_I);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));

    sprite.setPosition(POS_V_F);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(POS_V_F, SIZE_V_F));
    CenterTo(sprite, ORIGIN_WRAP_RECT_I);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));
    CenterTo(sprite, ORIGIN_WRAP_RECT_I);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(IMAGE_ZEROCENTERED_V_F, SIZE_V_F));

    sprite.setPosition(POS_V_F);
    BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(POS_V_F, SIZE_V_F));
    CenterTo(sprite, RECT_I, SCALE_V_F);
    BOOST_CHECK(
        sprite.getGlobalBounds() == sf::FloatRect(IMAGE_RESIZED_RECTCENTERED_V_F, RESIZE_V_F));
    CenterTo(sprite, RECT_I, SCALE_V_F);
    BOOST_CHECK(
        sprite.getGlobalBounds() == sf::FloatRect(IMAGE_RESIZED_RECTCENTERED_V_F, RESIZE_V_F));
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

    heroespath::log::Logger::Acquire();
    heroespath::sfml_util::Display::Acquire("HeroespathTestDisplay", sf::Style::None, 0);

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
               SCREEN_CENTER_V - (sf::Vector2f(300.0f, 400.0f) * 0.5f),
               sf::Vector2f(300.0f, 400.0f)));

    BOOST_CHECK(
        CenterCopy(RECT_I, SCALE_V_F)
        == sf::FloatRect(
               SCREEN_CENTER_V - ScaleCopy(sf::Vector2f(300.0f, 400.0f), SCALE_V_F * 0.5f),
               ScaleCopy(sf::Vector2f(300.0f, 400.0f), SCALE_V_F)));
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

    sf::Sprite sprite1(texture);
    sprite1.setPosition(-5.0f, -5.0f);

    sf::Sprite sprite2(texture);
    sprite2.setPosition(-2.0f, -1.0f);
    BOOST_CHECK((Distance(sprite1, sprite2) == 5.0f));
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

    const float HALF_SCALE(0.5f);
    const float DOUBLE_SCALE(2.0f);

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

    BOOST_CHECK(
        (FitCopy(40.0f, 30.0f, 20.0f, 10.0f) == sf::Vector2f(40.0f * (1.0f / 3.0f), 10.0f)));

    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 40.0f) == sf::Vector2f(20.0f, 40.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 40.0f) == sf::Vector2f(0.0f, 40.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 40.0f) == sf::Vector2f(30.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 40.0f) == sf::Vector2f(20.0f, 40.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 0.0f) == sf::Vector2f(30.0f, 60.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 40.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 0.0f, 40.0f) == sf::Vector2f(0.0f, 40.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 0.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 40.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 0.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 0.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 0.0f, 40.0f) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 0.0f) == sf::Vector2f(30.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 0.0f) == sf::Vector2f(0.0f, 0.0f)));

    // repeat of the blocks above with scaling
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(40.0f, 80.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 40.0f, HALF_SCALE) == sf::Vector2f(0.0f, 20.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(60.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 40.0f, HALF_SCALE) == sf::Vector2f(10.0f, 20.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 30.0f, 0.0f, DOUBLE_SCALE) == sf::Vector2f(60.0f, 120.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 40.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 0.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(0.0f, 80.0f)));
    BOOST_CHECK((FitCopy(0.0f, 20.0f, 30.0f, 0.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 40.0f, DOUBLE_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 30.0f, 0.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(0.0f, 0.0f, 0.0f, 0.0f, DOUBLE_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 0.0f, 40.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 0.0f, 30.0f, 0.0f, DOUBLE_SCALE) == sf::Vector2f(60.0f, 0.0f)));
    BOOST_CHECK((FitCopy(10.0f, 20.0f, 0.0f, 0.0f, HALF_SCALE) == sf::Vector2f(0.0f, 0.0f)));

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

        Fit(sprite1, WIDE_V_I);
        BOOST_CHECK((Size(sprite1.getGlobalBounds()) == Size(sprite1)));

        BOOST_CHECK(
            sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, sf::Vector2f(1.0f, 10.0f)));

        Fit(sprite1, WIDE_V_I);
        BOOST_CHECK((Size(sprite1.getGlobalBounds()) == Size(sprite1)));

        BOOST_CHECK(
            sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, sf::Vector2f(1.0f, 10.0f)));

        sprite1.setScale(1.0f, 1.0f);
        BOOST_CHECK(sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, TALL_V_F));

        Fit(sprite1, WIDE_V_I, QUARTER_SCALE);
        BOOST_CHECK((Size(sprite1.getGlobalBounds()) == Size(sprite1)));

        BOOST_CHECK(
            (sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, sf::Vector2f(0.25f, 2.5f))));

        Fit(sprite1, WIDE_V_I, QUARTER_SCALE);
        BOOST_CHECK((Size(sprite1.getGlobalBounds()) == Size(sprite1)));

        BOOST_CHECK(
            (sprite1.getGlobalBounds() == sf::FloatRect(RAND_POS_V_F, sf::Vector2f(0.25f, 2.5f))));
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
        auto const ORIG_POS_1_V(spriteTall.getPosition());
        Fit(spriteTall, WIDE_V_F);
        BOOST_CHECK(spriteTall.getGlobalBounds() == sf::FloatRect(0.0f, 0.0f, 1.0f, 10.0f));
        BOOST_CHECK(spriteTall.getPosition() == ORIG_POS_1_V);
        Fit(spriteTall, WIDE_V_F);
        BOOST_CHECK(spriteTall.getGlobalBounds() == sf::FloatRect(0.0f, 0.0f, 1.0f, 10.0f));
        BOOST_CHECK(spriteTall.getPosition() == ORIG_POS_1_V);

        spriteWide.setScale(1.0f, 1.0f);
        auto const ORIG_POS_2_V(spriteWide.getPosition());
        Fit(spriteWide, TALL_V_F);
        BOOST_CHECK(spriteWide.getGlobalBounds() == sf::FloatRect(0.0f, 0.0f, 10.0f, 1.0f));
        BOOST_CHECK(spriteWide.getPosition() == ORIG_POS_2_V);
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
    FitAndCenter(tallRectFTemp, WIDE_RECT_F);
    BOOST_CHECK(tallRectFTemp == TALL_FITANDCTO_WIDE_F);

    {
        sf::Image image;
        image.create(unsigned(10), unsigned(10));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        sprite.setPosition(10.0f, 10.0f);

        FitAndReCenter(sprite, sf::Vector2i(2, 2));
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(14.0f, 14.0f, 2.0f, 2.0f));
        FitAndReCenter(sprite, sf::Vector2i(2, 2));
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(14.0f, 14.0f, 2.0f, 2.0f));
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
    }

    {
        sf::Image image;
        image.create(unsigned(TALL_V_I.x), unsigned(TALL_V_I.y));

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite(texture);
        FitAndCenter(sprite, WIDE_RECT_F);
        BOOST_CHECK(sprite.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);
        FitAndCenter(sprite, WIDE_RECT_F);
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

        FitAndCenter(spriteTall, spriteWide);
        BOOST_CHECK(spriteTall.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);
        FitAndCenter(spriteTall, spriteWide);
        BOOST_CHECK(spriteTall.getGlobalBounds() == TALL_FITANDCTO_WIDE_F);
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
    auto const XF { 123.789f };
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

    auto const RIRFIx { int(double(VRI.x) * double(VRF.x)) };
    auto const RIRFIy { int(double(VRI.y) * double(VRF.y)) };
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
    const sf::Vector2f SCALEV1(0.5f, 2.0f);
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
        ScaleSizeAndReCenter(sprite, HALF);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(2.5f, 2.5f, 5.0f, 5.0f));

        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);
        ScaleSizeAndReCenter(sprite, VDF);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(-5.0f, -5.0f, 20.0f, 20.0f));

        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);
        ScaleSizeAndReCenter(sprite, SCALEV1);
        BOOST_CHECK(sprite.getGlobalBounds() == sf::FloatRect(2.5f, -5.0f, 5.0f, 20.0f));
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

        BOOST_CHECK(Size(texture) == sf::Vector2u(SPRITE_SIZE_V));
        BOOST_CHECK(Size(texture, HALF) == sf::Vector2u(SPRITE_SIZE_V * HALF));
        BOOST_CHECK(Size(texture, CUSTOM_SCALE_V) == sf::Vector2u(5, 20));

        sf::Sprite sprite(texture);
        const sf::Vector2f SPRITE_POS_V { 0.0f, 0.0f };
        sprite.setPosition(SPRITE_POS_V);
        sprite.setScale(V1F);
        const sf::FloatRect SPRITE_BOUNDS_ORIG(SPRITE_POS_V, SPRITE_SIZE_V);
        BOOST_CHECK(sprite.getGlobalBounds() == SPRITE_BOUNDS_ORIG);

        BOOST_CHECK(Size(sprite) == SPRITE_SIZE_V);
        BOOST_CHECK(Size(sprite, HALF) == SPRITE_SIZE_V * HALF);
        BOOST_CHECK(Size(sprite, CUSTOM_SCALE_V) == sf::Vector2f(5.0f, 20.0f));
    }

    // MininallyEnclosing() tests
    BOOST_CHECK(MininallyEnclosing(R0F, R0F) == R0F);
    BOOST_CHECK(MininallyEnclosing(R1F, R1F) == R1F);
    BOOST_CHECK(MininallyEnclosing(R0F, R1F) == sf::FloatRect(0.0f, 0.0f, 2.0f, 2.0f));

    const sf::Rect<int> RECT1(-10, -10, 10, 10);
    const sf::Rect<int> RECT2(10, 10, 10, 10);
    const sf::Rect<int> RECTE(-10, -10, 30, 30);

    BOOST_CHECK(MininallyEnclosing(RECT1, RECT2) == RECTE);

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
            MininallyEnclosing(sprite1, sprite2) == sf::FloatRect(-10.0f, -10.0f, 30.0f, 30.0f));
    }

    const sf::IntRect GS_RECT_I(10, 20, 30, 40);

    // Grow/Shrink()

    //
    const sf::IntRect GS_RECT_GROWN_I(10, 20, 40, 40);
    BOOST_CHECK(GrowToSquareCopy(GS_RECT_I) == GS_RECT_GROWN_I);

    auto gsRectITemp { GS_RECT_I };
    heroespath::sfml_util::GrowToSquare(gsRectITemp);
    BOOST_CHECK(gsRectITemp == GS_RECT_GROWN_I);

    //
    const sf::IntRect GS_RECT_GROWN_HALF_I(10, 20, 20, 20);
    BOOST_CHECK(GrowToSquareCopy(GS_RECT_I, 0.5f) == GS_RECT_GROWN_HALF_I);

    gsRectITemp = GS_RECT_I;
    heroespath::sfml_util::GrowToSquare(gsRectITemp, 0.5f);
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
    heroespath::sfml_util::GrowToSquareAndReCenter(gsRectITemp);
    BOOST_CHECK(gsRectITemp == GS_RECT_GROWN_RECENTERED_I);

    //
    const sf::IntRect GS_RECT_GROWN_RECENTERED_HALF_I(15, 30, 20, 20);
    BOOST_CHECK(GrowToSquareAndReCenterCopy(GS_RECT_I, 0.5f) == GS_RECT_GROWN_RECENTERED_HALF_I);

    gsRectITemp = GS_RECT_I;
    heroespath::sfml_util::GrowToSquareAndReCenter(gsRectITemp, 0.5f);
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
