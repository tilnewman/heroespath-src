// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer sin return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-gui-elements.cpp
//
#define BOOST_TEST_MODULE "gui_elements"

#include <boost/test/unit_test.hpp>

#include "gui/border.hpp"
#include "gui/font-enum.hpp"
#include "gui/font-manager.hpp"
#include "gui/gold-bar.hpp"
#include "gui/gui-images.hpp"
#include "gui/text-info.hpp"
#include "misc/random.hpp"
#include "sfutil/center.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/primitives.hpp"
#include "test/util/common.hpp"
#include "test/util/drawables-displayer.hpp"
#include "test/util/game-engine-global-fixture.hpp"

#include <SFML/Graphics.hpp>

using namespace heroespath;
using namespace heroespath::test;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestAll;
    m_iDisplayerUPtr = std::make_unique<DrawablesDisplayer>();
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

BOOST_AUTO_TEST_CASE(gold_bars_version_1)
{
    GameEngineGlobalFixture::displayer().beginDrawablesSet("Gold Bars Version #1");
    const auto CONTENT_REGION = GameEngineGlobalFixture::displayer().contentRegion();

    const auto SCREEN_EDGE_PAD { std::sqrt((CONTENT_REGION.width * CONTENT_REGION.height))
                                 * 0.092f };

    const auto BETWEEN_PAD { SCREEN_EDGE_PAD * 0.025f };

    auto createGoldBars = [&](const float LEFT,
                              const float TOP,
                              const gui::Orientation::Enum ORIENTATION,
                              const gui::Side::Enum SIDE,
                              const bool WILL_CAP_ENDS) {
        float posX { LEFT };
        float posY { TOP };

        auto length { 0.0f };
        while (((posX - LEFT) < SCREEN_EDGE_PAD) && ((posY - TOP) < SCREEN_EDGE_PAD))
        {
            gui::GoldBar goldBar(posX, posY, length, ORIENTATION, SIDE, WILL_CAP_ENDS);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<gui::GoldBar>(goldBar));

            const auto GOLD_BAR_REGION { goldBar.OuterRegion() };

            sf::RectangleShape rectangleOutline
                = sfutil::MakeRectangleHollow(GOLD_BAR_REGION, sf::Color::Red, 1.0f, true);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(rectangleOutline));

            if (ORIENTATION == gui::Orientation::Horiz)
            {
                posY += GOLD_BAR_REGION.height + BETWEEN_PAD;
            }
            else
            {
                posX += GOLD_BAR_REGION.width + BETWEEN_PAD;
            }

            if (length < 3.0f)
            {
                length += misc::Random(1.0f);
            }
            else
            {
                length += misc::Random(1.0f, length);
            }
        }

        return sf::Vector2f(posX, posY);
    };

    const auto BETWEEN_SERIES_PAD { BETWEEN_PAD * 2.0f };
    const auto BETWEEN_SET_PAD { BETWEEN_SERIES_PAD * 2.0f };

    auto createGoldBarsOfAllSides
        = [&](const sf::Vector2f & POS_V, const gui::Orientation::Enum ORIENTATION) {
              sf::Vector2f posV { POS_V };
              EnumUnderlying_t flag(1);
              while (flag <= gui::Side::Last)
              {
                  posV = createGoldBars(posV.x, posV.y, ORIENTATION, gui::Side::Enum(flag), false);

                  flag <<= 1;

                  if (ORIENTATION == gui::Orientation::Horiz)
                  {
                      posV.y += BETWEEN_SERIES_PAD;
                      posV.x = POS_V.x;
                  }
                  else
                  {
                      posV.x += BETWEEN_SERIES_PAD;
                      posV.y = POS_V.y;
                  }
              }

              if (ORIENTATION == gui::Orientation::Horiz)
              {
                  posV.y += BETWEEN_SET_PAD;
                  posV.x = POS_V.x;
              }
              else
              {
                  posV.x += BETWEEN_SET_PAD;
                  posV.y = POS_V.y;
              }

              const auto START_POS_V { posV };

              flag = 1;
              while (flag <= gui::Side::Last)
              {
                  posV = createGoldBars(posV.x, posV.y, ORIENTATION, gui::Side::Enum(flag), true);

                  flag <<= 1;

                  if (ORIENTATION == gui::Orientation::Horiz)
                  {
                      posV.y += BETWEEN_SERIES_PAD;
                      posV.x = START_POS_V.x;
                  }
                  else
                  {
                      posV.x += BETWEEN_SERIES_PAD;
                      posV.y = START_POS_V.y;
                  }
              }
          };

    createGoldBarsOfAllSides(
        sf::Vector2f(SCREEN_EDGE_PAD, (CONTENT_REGION.top + (SCREEN_EDGE_PAD * 1.9f))),
        gui::Orientation::Horiz);

    createGoldBarsOfAllSides(
        sf::Vector2f((SCREEN_EDGE_PAD * 1.9f), (CONTENT_REGION.top + SCREEN_EDGE_PAD)),
        gui::Orientation::Vert);

    const auto guiElementsCachedTexture = gui::CachedTexture(gui::GuiImages::PathKey());
    sf::Sprite sprite(guiElementsCachedTexture.Get());
    sfutil::CenterTo(sprite, CONTENT_REGION);
    GameEngineGlobalFixture::displayer().appendDrawable(std::make_unique<sf::Sprite>(sprite));

    GameEngineGlobalFixture::drawAndHoldUntilMouseOrKeyOrDuration();
    GameEngineGlobalFixture::displayer().endDrawablesSet();
}

BOOST_AUTO_TEST_CASE(gold_bars_version_2)
{
    GameEngineGlobalFixture::displayer().beginDrawablesSet("Gold Bars Version 2");
    const auto CONTENT_REGION = GameEngineGlobalFixture::displayer().contentRegion();

    const auto SCREEN_EDGE_PAD { std::sqrt((CONTENT_REGION.width * CONTENT_REGION.height))
                                 * 0.12f };

    const auto BETWEEN_SPACER { SCREEN_EDGE_PAD * 0.025f };
    const auto BETWEEN_ROW_VERT_SPACER { BETWEEN_SPACER * 4.0f };

    auto makeHorizRowOfGoldBarBoxes = [&](const float LEFT,
                                          const float TOP,
                                          const bool WILL_MAKE_SMALL,
                                          const bool WILL_GROW_BORDER) {
        const auto GROWTH_MIN { 0.333f };
        const auto GROWTH_MAX { 10.0f };

        float posX { LEFT };
        float posY { TOP };

        auto width { 0.0f };
        auto height { 0.0f };

        sf::FloatRect finalOuterRegion;

        auto horizGrowthPrev { GROWTH_MIN };
        auto vertGrowthPrev { GROWTH_MIN };
        while (posX < (CONTENT_REGION.width - SCREEN_EDGE_PAD))
        {
            const sf::FloatRect REGION_INITIAL(posX, posY, width, height);

            gui::GoldBar goldBar(REGION_INITIAL, gui::Orientation::Count, WILL_GROW_BORDER);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<gui::GoldBar>(goldBar));

            const auto REGION_OUTER = goldBar.OuterRegion();
            finalOuterRegion = REGION_OUTER;

            sf::RectangleShape outlineRectangleOuter
                = sfutil::MakeRectangleHollow(REGION_OUTER, sf::Color::Red, 1.0f, true);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(outlineRectangleOuter));

            const auto REGION_INNER = goldBar.InnerRegion();

            sf::RectangleShape outlineRectangleInner
                = sfutil::MakeRectangleHollow(REGION_INNER, sf::Color::Green, 1.0f, true);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(outlineRectangleInner));

            posX += REGION_OUTER.width + BETWEEN_SPACER;

            if (WILL_MAKE_SMALL)
            {
                const auto GROWTH_HORIZ { misc::Random(
                    GROWTH_MIN, misc::Max(GROWTH_MIN, (1.0f - horizGrowthPrev))) };

                horizGrowthPrev = GROWTH_HORIZ;

                width += GROWTH_HORIZ;

                const auto GROWTH_VERT { misc::Random(
                    GROWTH_MIN, misc::Max(GROWTH_MIN, (1.0f - vertGrowthPrev))) };

                vertGrowthPrev = GROWTH_VERT;

                height += GROWTH_VERT;
            }
            else
            {
                width += misc::Random(1.0f, GROWTH_MAX);
                height += misc::Random(1.0f, GROWTH_MAX);
            }
        }

        return TOP + finalOuterRegion.height;
    };

    const auto BOTTOM_OF_FIRST_ROW { makeHorizRowOfGoldBarBoxes(
        SCREEN_EDGE_PAD, (CONTENT_REGION.top + SCREEN_EDGE_PAD), true, false) };

    const auto BOTTOM_OF_SECOND_ROW { makeHorizRowOfGoldBarBoxes(
        SCREEN_EDGE_PAD, (BOTTOM_OF_FIRST_ROW + BETWEEN_ROW_VERT_SPACER), false, false) };

    const auto BOTTOM_OF_THIRD_ROW { makeHorizRowOfGoldBarBoxes(
        SCREEN_EDGE_PAD, (BOTTOM_OF_SECOND_ROW + (BETWEEN_ROW_VERT_SPACER * 2.0f)), true, true) };

    makeHorizRowOfGoldBarBoxes(
        SCREEN_EDGE_PAD, (BOTTOM_OF_THIRD_ROW + BETWEEN_ROW_VERT_SPACER), false, true);

    GameEngineGlobalFixture::drawAndHoldUntilMouseOrKeyOrDuration();
    GameEngineGlobalFixture::displayer().endDrawablesSet();
}

BOOST_AUTO_TEST_CASE(border)
{
    GameEngineGlobalFixture::displayer().beginDrawablesSet("Borders");
    const auto CONTENT_REGION = GameEngineGlobalFixture::displayer().contentRegion();

    const auto SCREEN_EDGE_PAD { std::sqrt((CONTENT_REGION.width * CONTENT_REGION.height))
                                 * 0.12f };

    const auto BETWEEN_SPACER { SCREEN_EDGE_PAD * 0.025f };
    const auto BETWEEN_ROW_VERT_SPACER { BETWEEN_SPACER * 4.0f };

    auto makeHorizRowOfBorders = [&](const float LEFT,
                                     const float TOP,
                                     const bool WILL_DRAW_LINES,
                                     const bool WILL_MAKE_SMALL,
                                     const bool WILL_GROW_BORDER) {
        const auto GROWTH_MIN { 0.333f };
        const auto GROWTH_MAX { 10.0f };

        float posX { LEFT };
        float posY { TOP };

        auto width { 0.0f };
        auto height { 0.0f };

        sf::FloatRect finalOuterRegion;

        auto horizGrowthPrev { GROWTH_MIN };
        auto vertGrowthPrev { GROWTH_MIN };
        while (posX < (CONTENT_REGION.width - SCREEN_EDGE_PAD))
        {
            const sf::FloatRect REGION_INITIAL(posX, posY, width, height);

            gui::Border border;

            if (WILL_DRAW_LINES)
            {
                border = gui::Border(REGION_INITIAL, WILL_GROW_BORDER);
            }
            else
            {
                border = gui::Border(
                    REGION_INITIAL,
                    1.0f,
                    sf::Color::White,
                    sf::Color::Transparent,
                    WILL_GROW_BORDER);
            }

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<gui::Border>(border));

            const auto REGION_OUTER { border.OuterRegion() };
            finalOuterRegion = REGION_OUTER;

            sf::RectangleShape outlineRectangleOuter
                = sfutil::MakeRectangleHollow(REGION_OUTER, sf::Color::Red, 1.0f, true);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(outlineRectangleOuter));

            const auto REGION_INNER { border.InnerRegion() };

            sf::RectangleShape outlineRectangleInner
                = sfutil::MakeRectangleHollow(REGION_INNER, sf::Color::Green, 1.0f, true);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(outlineRectangleInner));

            posX += REGION_OUTER.width + BETWEEN_SPACER;

            if (WILL_MAKE_SMALL)
            {
                const auto GROWTH_HORIZ { misc::Random(
                    GROWTH_MIN, misc::Max(GROWTH_MIN, (1.0f - horizGrowthPrev))) };

                horizGrowthPrev = GROWTH_HORIZ;

                width += GROWTH_HORIZ;

                const auto GROWTH_VERT { misc::Random(
                    GROWTH_MIN, misc::Max(GROWTH_MIN, (1.0f - vertGrowthPrev))) };

                vertGrowthPrev = GROWTH_VERT;

                height += GROWTH_VERT;
            }
            else
            {
                width += misc::Random(1.0f, GROWTH_MAX);
                height += misc::Random(1.0f, GROWTH_MAX);
            }
        }

        return TOP + finalOuterRegion.height;
    };

    const auto BOTTOM_OF_FIRST_ROW { makeHorizRowOfBorders(
        SCREEN_EDGE_PAD, (CONTENT_REGION.top + SCREEN_EDGE_PAD), true, true, false) };

    const auto BOTTOM_OF_SECOND_ROW { makeHorizRowOfBorders(
        SCREEN_EDGE_PAD, (BOTTOM_OF_FIRST_ROW + BETWEEN_ROW_VERT_SPACER), true, false, false) };

    const auto BOTTOM_OF_THIRD_ROW { makeHorizRowOfBorders(
        SCREEN_EDGE_PAD,
        (BOTTOM_OF_SECOND_ROW + (BETWEEN_ROW_VERT_SPACER * 2.0f)),
        true,
        true,
        true) };

    const auto BOTTOM_OF_FOURTH_ROW { makeHorizRowOfBorders(
        SCREEN_EDGE_PAD, (BOTTOM_OF_THIRD_ROW + BETWEEN_ROW_VERT_SPACER), false, true, false) };

    const auto BOTTOM_OF_FIFTH_ROW { makeHorizRowOfBorders(
        SCREEN_EDGE_PAD, BOTTOM_OF_FOURTH_ROW + BETWEEN_ROW_VERT_SPACER, false, false, false) };

    makeHorizRowOfBorders(
        SCREEN_EDGE_PAD, BOTTOM_OF_FIFTH_ROW + BETWEEN_ROW_VERT_SPACER, false, true, true);

    GameEngineGlobalFixture::drawAndHoldUntilMouseOrKeyOrDuration();
    GameEngineGlobalFixture::displayer().endDrawablesSet();
}

BOOST_AUTO_TEST_CASE(text_offset_fix)
{
    GameEngineGlobalFixture::displayer().beginDrawablesSet("Text Offset Fix");
    const auto CONTENT_REGION = GameEngineGlobalFixture::displayer().contentRegion();

    auto setupTextTest =
        [&](const sf::FloatRect & REGION, const unsigned FONT_SIZE, const float SCALE) {
            sf::RectangleShape outlineRectangleBoundingRegion
                = sfutil::MakeRectangleHollow(REGION, sf::Color::Green);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(outlineRectangleBoundingRegion));

            const gui::TextInfo TEXT_INFO("This is text.", gui::GuiFont::Default, FONT_SIZE);

            gui::Text text(TEXT_INFO);
            text.setScale(sf::Vector2f(SCALE, SCALE));
            text.setPosition(sfutil::Position(REGION));

            GameEngineGlobalFixture::displayer().appendDrawable(std::make_unique<gui::Text>(text));

            sf::RectangleShape outlineRectangleTextBoundingRegion
                = sfutil::MakeRectangleHollow(text.getGlobalBounds(), sf::Color::Yellow);

            GameEngineGlobalFixture::displayer().appendDrawable(
                std::make_unique<sf::RectangleShape>(outlineRectangleTextBoundingRegion));
        };

    const float OFFSET { CONTENT_REGION.width * 0.1f };
    const auto POS = (sfutil::Position(CONTENT_REGION) + sf::Vector2f(OFFSET, OFFSET));

    const float SIZE_VALUE { CONTENT_REGION.width * 0.3f };
    const sf::Vector2f SIZE_V(SIZE_VALUE, (SIZE_VALUE * 0.5f));

    sf::FloatRect region(POS, SIZE_V);
    setupTextTest(region, 100, 1.0f);

    region.left = sfutil::ScreenRatioToPixelsHoriz(0.5f);
    setupTextTest(region, 50, 5.0f);

    GameEngineGlobalFixture::drawAndHoldUntilMouseOrKeyOrDuration();
    GameEngineGlobalFixture::displayer().endDrawablesSet();
}

BOOST_AUTO_TEST_CASE(fonts)
{
    GameEngineGlobalFixture::displayer().beginDrawablesSet("Fonts");
    const auto CONTENT_REGION = GameEngineGlobalFixture::displayer().contentRegion();

    const auto EACH_REGION_HEIGHT
        = (CONTENT_REGION.height / static_cast<float>(gui::GuiFont::Count));

    auto getDescription = [&](const gui::GuiFont::Enum ENUM) {
        switch (ENUM)
        {
            case gui::GuiFont::Default:
                return "Euler: slightly dolphin, nice and round with ample spacing";

            case gui::GuiFont::DefaultBoldFlavor:
                return "Modern-Antiqua: bold with sharp flavorful ends, bold and tall (like a bold "
                       "Euler with more flavor)";

            case gui::GuiFont::System:
                return "Gentium-Plus: normal, tall, straight, and slightly condensed (very "
                       "similar to GoudyBookletter but slightly wider)";

            case gui::GuiFont::SystemCondensed:
                return "Goudy-Bookletter: normal and wide but very condensed with ugly numbers "
                       "(very similar to GentiumPlus but slightly more narrow)";

            case gui::GuiFont::SignThinTallNarrow:
                return "(Numbers) Quill-Sword: rigid calligraphy with medieval tails, very "
                       "condensed (good "
                       "for signs) (great flavorful numbers)";

            case gui::GuiFont::SignBoldShortWide:
                return "Queen-Country: very bold and short with medieval flavor (good for signs)";

            case gui::GuiFont::Handwriting:
                return "Valley-Forge: casual handwriting, slightly bold (good for writing on "
                       "scrolls) (used for popup window text buttons)";

            case gui::GuiFont::DialogModern: return "Square-Antiqua";

            case gui::GuiFont::DialogMedieval:
                return "Mops-Antiqua: (used for interaction text buttons)";

            case gui::GuiFont::Count:
            default: return "Error -go figure this out";
        }
    };

    for (EnumUnderlying_t index(0); index < gui::GuiFont::Count; ++index)
    {
        const sf::FloatRect REGION(
            0.0f,
            (CONTENT_REGION.top + (static_cast<float>(index) * EACH_REGION_HEIGHT)),
            CONTENT_REGION.width,
            EACH_REGION_HEIGHT);

        const auto ENUM = static_cast<gui::GuiFont::Enum>(index);

        if (ENUM != gui::GuiFont::Default)
        {
            gui::FontManager::Instance()->Load(ENUM);
        }

        std::ostringstream ss;
        ss << NAMEOF_ENUM(ENUM) << " \"" << getDescription(ENUM) << "\" 0123456789";

        gui::Text text(ss.str(), ENUM, 100);
        sfutil::FitAndCenterTo(text, REGION);

        GameEngineGlobalFixture::displayer().appendDrawable(std::make_unique<gui::Text>(text));
    }

    GameEngineGlobalFixture::drawAndHoldUntilMouseOrKeyOrDuration();
    GameEngineGlobalFixture::displayer().endDrawablesSet();
}
