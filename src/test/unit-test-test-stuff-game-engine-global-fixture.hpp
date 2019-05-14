// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_UNIT_TEST_TEST_STUFF_GAME_ENGINE_GLOBAL_FIXTURE_INCLUDED
#define HEROESPATH_UNIT_TEST_TEST_STUFF_GAME_ENGINE_GLOBAL_FIXTURE_INCLUDED
//
// unit-test-test-stuff-game-engine-global-fixture.hpp
//
#include <sstream>
#include <stdexcept>
#include <string>

#include "game/startup-shutdown.hpp"
#include "gui/cached-texture.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/texture-cache.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "test/unit-test-test-stuff-i-displayer.hpp"

#include <SFML/Graphics.hpp>

// this has to be in the global scope or else boost_unit_test complains...whatever.
//
// pretty boost_unit_test wants this to be a struct for some damn reason...whatever
//
// this fixture can be decalred in any unit-test.cpp file and it will cause the game engine to fully
// start up before all the tests (including the display window) and then shutdown after all tests
// complete.
struct GameEngineGlobalFixture
{
    GameEngineGlobalFixture() { setDisplayer(); }

    static void setDisplayer();

    void setup()
    {
        m_startupShutdownUPtr = std::make_unique<heroespath::game::StartupShutdown>(
            "HP Test Image Loading", 0, nullptr, true);

        auto & window = *heroespath::gui::Display::Instance();

        window.PollEvents();

        if (!window.IsOpen())
        {
            throw std::runtime_error(
                "GameEngineGlobalFixture::setup() failed to open a display window.");
        }

        heroespath::gui::FontManager::Instance()->Load(heroespath::gui::GuiFont::Default);

        if (!m_iDisplayerUPtr)
        {
            throw std::runtime_error(
                "GameEngineGlobalFixture::setup() called when m_iDisplayerUPtr was null.  "
                "Implement the setDisplayer() function at the top of your unit-test-whatever.cpp "
                "file before the line \"BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);\".");
        }

        m_iDisplayerUPtr->setup(window.FullScreenRect());
        draw();
        window.PollEvents();
    }

    void teardown()
    {
        heroespath::gui::Display::Instance()->PollEvents();
        m_iDisplayerUPtr.reset();
        m_startupShutdownUPtr.reset();
    }

    static IDisplayer & displayer()
    {
        if (!m_iDisplayerUPtr)
        {
            throw std::runtime_error(
                "GameEngineGlobalFixture::displayer() called when m_iDisplayerUPtr was null.");
        }

        return *m_iDisplayerUPtr;
    }

    static void draw()
    {
        auto & window = *heroespath::gui::Display::Instance();
        window.ClearToBlack();

        if (m_iDisplayerUPtr)
        {
            window.TestDraw(*m_iDisplayerUPtr);
        }

        window.DisplayFrameBuffer();

        if (m_delayAfterEachDrawSec > 0.0f)
        {
            sf::sleep(sf::seconds(m_delayAfterEachDrawSec));
        }
    }

    static void drawAndHoldUntilMouseOrKeyOrDuration(const float DURATION_SEC = 3.0f)
    {
        if (!(DURATION_SEC > 0.0f))
        {
            return;
        }

        heroespath::gui::Display::Instance()->PollEvents();

        sf::Clock timer;

        while (timer.getElapsedTime().asSeconds() < DURATION_SEC)
        {
            const float timeElapsedPercent { (timer.getElapsedTime().asSeconds() / DURATION_SEC)
                                             * 100.0f };

            m_iDisplayerUPtr->setProgress(static_cast<std::size_t>(timeElapsedPercent));

            draw();

            for (auto const & EVENT : heroespath::gui::Display::Instance()->PollEvents())
            {
                if ((EVENT.type == sf::Event::KeyPressed)
                    || (EVENT.type == sf::Event::MouseButtonPressed)
                    || (EVENT.type == sf::Event::Closed)
                    || !heroespath::gui::Display::Instance()->IsOpen())
                {
                    return;
                }
            }

            sf::sleep(sf::seconds(0.05f));
        }
    }

private:
    static inline std::unique_ptr<heroespath::game::StartupShutdown> m_startupShutdownUPtr {};
    static inline std::unique_ptr<IDisplayer> m_iDisplayerUPtr {};
    static inline float m_delayAfterEachDrawSec = 0.0f;
};

#endif // HEROESPATH_UNIT_TEST_TEST_STUFF_GAME_ENGINE_GLOBAL_FIXTURE_INCLUDED
