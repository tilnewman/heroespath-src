// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_GAME_ENGINE_GLOBAL_FIXTURE_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_GAME_ENGINE_GLOBAL_FIXTURE_HPP_INCLUDED
//
// game-engine-global-fixture.hpp
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
#include "sfutil/keyboard.hpp"
#include "testutil/i-displayer.hpp"

#include <SFML/Graphics.hpp>

namespace heroespath
{
namespace test
{

    // this fixture can be decalred in any unit-test.cpp file and it will cause the game engine to
    // fully start up before all the tests (including the display window) and then shutdown after
    // all tests complete.
    struct GameEngineGlobalFixture
    {
        GameEngineGlobalFixture() { setDisplayer(); }

        static void setDisplayer();

        void setup()
        {
            m_startupShutdownUPtr = std::make_unique<game::StartupShutdown>(
                "HP Test Image Loading", 0, nullptr, true);

            auto & window = *gui::Display::Instance();

            window.PollEvents();

            if (!window.IsOpen())
            {
                throw std::runtime_error(
                    "GameEngineGlobalFixture::setup() failed to open a display window.");
            }

            gui::FontManager::Instance()->Load(gui::GuiFont::Default);

            if (!m_iDisplayerUPtr)
            {
                throw std::runtime_error(
                    "GameEngineGlobalFixture::setup() called when m_iDisplayerUPtr was null.  "
                    "Implement the setDisplayer() function at the top of your "
                    "unit-test-whatever.cpp "
                    "file before the line "
                    "\"BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);\".");
            }

            m_iDisplayerUPtr->setup(window.FullScreenRect());
            draw();
            window.PollEvents();
        }

        void teardown()
        {
            gui::Display::Instance()->PollEvents();
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
            auto & window = *gui::Display::Instance();
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

            checkEvents();
        }

        static void drawAndHoldUntilMouseOrKeyOrDuration(const float DURATION_SEC = 1.5f)
        {
            if (!(DURATION_SEC > 0.0f))
            {
                return;
            }

            gui::Display::Instance()->PollEvents();

            const float timeStepSec = 0.05f;
            bool isPaused = false;
            float currentElapsedTime = 0.0f;
            while (currentElapsedTime < DURATION_SEC)
            {
                if (!isPaused)
                {
                    currentElapsedTime += timeStepSec;

                    const float timeElapsedPercent { (currentElapsedTime / DURATION_SEC) * 100.0f };

                    m_iDisplayerUPtr->setProgress(static_cast<std::size_t>(timeElapsedPercent));
                }

                const auto EVENT_FLAGS = checkEvents();

                if (EVENT_FLAGS & EventFlags::PauseKey)
                {
                    isPaused = !isPaused;
                }
                else if (EVENT_FLAGS & EventFlags::OtherInput)
                {
                    return;
                }

                draw();
                sf::sleep(sf::seconds(timeStepSec));
            }
        }

    private:
        struct EventFlags : public EnumBaseBitField<>
        {
            enum Enum : EnumUnderlying_t
            {
                None = 0,
                PauseKey = 1 << 0,
                OtherInput = 1 << 1
            };
        };

        static EventFlags::Enum checkEvents()
        {
            EventFlags::Enum flags = EventFlags::None;

            if (!gui::Display::Instance()->IsOpen())
            {
                throw std::runtime_error(
                    "GameEngineGlobalFixture::checkEvents() found the window is not open anymore.");
            }

            const auto EVENTS = gui::Display::Instance()->PollEvents();
            for (auto const & EVENT : EVENTS)
            {
                if (EVENT.type == sf::Event::KeyPressed)
                {
                    flags |= EventFlags::OtherInput;

                    if (EVENT.key.code == sf::Keyboard::F1)
                    {
                        throw std::runtime_error("GameEngineGlobalFixture::checkEvents() found the "
                                                 "secret escape key F1 pressed.");
                    }
                    else if (EVENT.key.code == sf::Keyboard::Escape)
                    {
                        throw std::runtime_error(
                            "GameEngineGlobalFixture::checkEvents() found the "
                            "ESCAPE key pressed.  Throwing to fail this test.");
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

    private:
        static inline std::unique_ptr<game::StartupShutdown> m_startupShutdownUPtr {};
        static inline std::unique_ptr<IDisplayer> m_iDisplayerUPtr {};
        static inline float m_delayAfterEachDrawSec = 0.0f;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_GAME_ENGINE_GLOBAL_FIXTURE_HPP_INCLUDED
