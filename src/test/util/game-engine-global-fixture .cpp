// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game-engine-global-fixture.cpp
//
#include "game-engine-global-fixture.hpp"

#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "misc/filesystem.hpp"
#include "misc/log.hpp"
#include "sfutil/keyboard.hpp"
#include "test/util/i-displayer.hpp"

#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <stdexcept>

namespace heroespath
{
namespace test
{

    bool GameEngineGlobalFixture::hasDisplay()
    {
        return (
            (m_subsystemsToSetup == game::SubsystemCollection::All)
            || (m_subsystemsToSetup == game::SubsystemCollection::TestAll));
    }

    void GameEngineGlobalFixture::setup()
    {
        misc::Log::setUnitTestName("log-" + misc::filesystem::Filename(m_unitTestFilename, true));

        m_startupShutdownUPtr
            = std::make_unique<game::SetupTeardown>("Unit Test: " + name(), m_subsystemsToSetup);

        if (hasDisplay())
        {
            auto & window = *gui::Display::Instance();

            window.PollEvents();

            gui::FontManager::Instance()->Load(gui::GuiFont::Default);

            if (!m_iDisplayerUPtr)
            {
                throw std::runtime_error(
                    name()
                    + "::setup() called when m_iDisplayerUPtr was null.  "
                      "Implement the setDisplayer() function at the top of your  "
                      "unit-test-whatever.cpp  file before the line "
                      "\"BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);\".");
            }

            m_iDisplayerUPtr->setup(window.FullScreenRect());

            draw();
        }
    }

    void GameEngineGlobalFixture::teardown()
    {
        if (hasDisplay())
        {
            gui::Display::Instance()->PollEvents();
        }

        m_iDisplayerUPtr.reset();
        m_startupShutdownUPtr.reset();
    }

    void GameEngineGlobalFixture::setDisplayer(std::unique_ptr<IDisplayer> displayerUPtr)
    {
        m_iDisplayerUPtr = std::move(displayerUPtr);
    }

    IDisplayer & GameEngineGlobalFixture::displayer()
    {
        if (!hasDisplay() || !m_iDisplayerUPtr)
        {
            throw std::runtime_error(
                name() + "::displayer() called when !hasDisplay() or m_iDisplayerUPtr was null.");
        }

        return *m_iDisplayerUPtr;
    }

    void GameEngineGlobalFixture::draw(const bool WILL_CHECK_EVENTS)
    {
        if (!hasDisplay() || !m_iDisplayerUPtr)
        {
            throw std::runtime_error(
                name() + "::draw() called when !hasDisplay() or m_iDisplayerUPtr was null.");
        }

        auto & window = *gui::Display::Instance();

        window.ClearToBlack();
        window.TestDraw(*m_iDisplayerUPtr);
        window.DisplayFrameBuffer();

        if (m_delayAfterEachDrawSec > 0.0f)
        {
            sf::sleep(sf::seconds(m_delayAfterEachDrawSec));
        }

        if (WILL_CHECK_EVENTS)
        {
            checkEvents();
        }
    }

    void GameEngineGlobalFixture::drawAndHoldUntilMouseOrKeyOrDuration(const float DURATION_SEC)
    {
        if (!hasDisplay() || !m_iDisplayerUPtr)
        {
            throw std::runtime_error(
                name()
                + "::drawAndHoldUntilMouseOrKeyOrDuration() called when !hasDisplay() or "
                  "m_iDisplayerUPtr was null.");
        }

        if (!(DURATION_SEC > 0.0f))
        {
            return;
        }

        gui::Display::Instance()->PollEvents();

        m_iDisplayerUPtr->setProgressMax(100);

        const auto DELAY_AFTER_EACH_DRAW_ORIG = m_delayAfterEachDrawSec;
        m_delayAfterEachDrawSec = 0.0f;

        sf::Clock timer;
        const float timeStepSec = 0.02f;
        bool isPaused = false;
        float currentElapsedTime = 0.0f;
        while (currentElapsedTime < DURATION_SEC)
        {
            timer.restart();

            if (!isPaused)
            {
                currentElapsedTime += timeStepSec;

                const float timeElapsedPercent { (currentElapsedTime / DURATION_SEC) * 100.0f };

                m_iDisplayerUPtr->setProgress(static_cast<std::size_t>(timeElapsedPercent));
            }

            draw(false);

            const auto EVENT_FLAGS = checkEvents();

            if (EVENT_FLAGS & EventFlags::PauseKey)
            {
                isPaused = !isPaused;
            }
            else if (EVENT_FLAGS & EventFlags::OtherInput)
            {
                break;
            }

            const auto TIME_ELAPSED_SEC = timer.getElapsedTime().asSeconds();

            if (TIME_ELAPSED_SEC < timeStepSec)
            {
                sf::sleep(sf::seconds(timeStepSec - TIME_ELAPSED_SEC));
            }
        }

        m_delayAfterEachDrawSec = DELAY_AFTER_EACH_DRAW_ORIG;
    }

    const std::string GameEngineGlobalFixture::name()
    {
        const auto CURRENT_TEST_MODULE_NAME
            = boost::unit_test::framework::current_auto_test_suite().full_name();

        return "GameEngineGlobalFixture_" + std::string(NAMEOF_ENUM(m_subsystemsToSetup)) + "_"
            + CURRENT_TEST_MODULE_NAME + "_";
    }

    EventFlags::Enum GameEngineGlobalFixture::checkEvents()
    {
        if (!hasDisplay())
        {
            return EventFlags::None;
        }

        auto & window = *gui::Display::Instance();

        EventFlags::Enum flags = EventFlags::None;

        if (!window.IsOpen())
        {
            throw std::runtime_error(
                name() + "::checkEvents() found the window is not open anymore.");
        }

        const auto EVENTS = window.PollEvents();
        for (auto const & EVENT : EVENTS)
        {
            if (EVENT.type == sf::Event::KeyPressed)
            {
                flags |= EventFlags::OtherInput;

                if (EVENT.key.code == sf::Keyboard::F1)
                {
                    throw std::runtime_error(
                        name() + "::checkEvents() found the secret escape key F1 pressed.");
                }
                else if (EVENT.key.code == sf::Keyboard::Escape)
                {
                    throw std::runtime_error(
                        name()
                        + "::checkEvents() found the  ESCAPE key pressed.  Throwing to fail "
                          "this test.");
                }
                else if (EVENT.key.code == sf::Keyboard::P)
                {
                    flags |= EventFlags::PauseKey;
                }
            }
            else if (EVENT.type == sf::Event::MouseButtonPressed)
            {
                flags |= EventFlags::OtherInput;
            }
        }

        return flags;
    }

} // namespace test
} // namespace heroespath
