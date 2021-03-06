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
#include "game/setup-teardown.hpp"
#include "misc/enum-common.hpp"
#include "test/util/i-displayer.hpp"

#include <string>

namespace heroespath
{
namespace game
{
    class SetupTeardown;
}

namespace test
{
    // this fixture can be decalred in any unit-test.cpp file and it will cause the game engine to
    // fully start up before all the tests (including the display window) and then shutdown after
    // all tests complete.
    struct GameEngineGlobalFixture
    {
        // The constructor is designed to call setDisplayer() which is intentionally not defined.
        // The point is to force the user of the class to implement it in the unit-test.cpp file and
        // set the displayer class and any other details before calling BOOST_TEST_GLOBAL_FIXTURE.
        GameEngineGlobalFixture() { setupBeforeAllTests(); }
        static void setupBeforeAllTests();

        static float delayAfterEachDraw() { return m_delayAfterEachDrawSec; }

        static void delayAfterEachDrawSet(const float NEW_DELAY)
        {
            m_delayAfterEachDrawSec = NEW_DELAY;
        }

        static bool hasDisplay();

        void setup();
        void teardown();

        static void setDisplayer(std::unique_ptr<IDisplayer>);
        static IDisplayer & displayer();
        static EventFlags::Enum draw(const bool WILL_CHECK_EVENTS = true);

        static EventFlags::Enum
            drawAndHoldUntilMouseOrKeyOrDuration(const float DURATION_SEC = 0.5f);

        static EventFlags::Enum checkEvents();

        static bool isPaused() { return m_isPaused; }
        static void isPaused(const bool IS_PAUSED) { m_isPaused = IS_PAUSED; }

    private:
        static const std::string name();

        static inline std::unique_ptr<game::SetupTeardown> m_startupShutdownUPtr {};
        static inline std::unique_ptr<IDisplayer> m_iDisplayerUPtr {};
        static inline float m_delayAfterEachDrawSec { 0.0f };
        static inline std::string m_unitTestFilename {};

        static inline game::SubsystemCollection m_subsystemsToSetup {
            game::SubsystemCollection::TestWithOnlyLogAndConfig
        };

        static inline bool m_isPaused = false;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_GAME_ENGINE_GLOBAL_FIXTURE_HPP_INCLUDED
