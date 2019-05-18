// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer sin return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-maps.cpp
//
#define BOOST_TEST_MODULE "maps"

#include <boost/test/unit_test.hpp>

#include "game/setup-teardown.hpp"
#include "interact/interaction-manager.hpp"
#include "map/level-enum.hpp"
#include "map/map.hpp"
#include "test/util/game-engine-global-fixture.hpp"
#include "test/util/single-image-displayer-scoped.hpp"
#include "test/util/single-image-displayer.hpp"

using namespace heroespath;
using namespace heroespath::test;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestAll;
    m_iDisplayerUPtr = std::make_unique<SingleImageDisplayer>();
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

BOOST_AUTO_TEST_CASE(all_maps_tests)
{
    interact::InteractionManager interactionManager;

    using MapLevelVec_t = std::vector<map::Level::Enum>;

    struct MapAndTransitions
    {
        explicit MapAndTransitions(
            const map::Level::Enum LEVEL = map::Level::Count,
            const MapLevelVec_t & ENTRY_LEVELS = MapLevelVec_t(),
            const MapLevelVec_t & EXIT_LEVELS = MapLevelVec_t())
            : level(LEVEL)
            , entry_levels(ENTRY_LEVELS)
            , exit_levels(EXIT_LEVELS)
        {}

        map::Level::Enum level;
        MapLevelVec_t entry_levels;
        MapLevelVec_t exit_levels;
    };

    misc::VectorMap<map::Level::Enum, MapAndTransitions> transitions;

    const auto parseMap { [&](const map::Level::Enum LEVEL_ENUM) {
        auto mapUPtr = std::make_unique<map::Map>(
            sf::FloatRect(0.0f, 0.0f, 128.0f, 256.0f), interactionManager);

        BOOST_CHECK_NO_THROW(mapUPtr->Load(LEVEL_ENUM, map::Level::Count, true));

        MapLevelVec_t entryLevels;
        MapLevelVec_t exitLevels;
        mapUPtr->EntryAndExitLevels(entryLevels, exitLevels);
        return MapAndTransitions(LEVEL_ENUM, entryLevels, exitLevels);
    } };

    for (EnumUnderlying_t mapIndex(0); mapIndex < map::Level::Count; ++mapIndex)
    {
        const auto WHICH_LEVEL { static_cast<map::Level::Enum>(mapIndex) };
        M_HP_LOG("About to test \"" + NAMEOF_ENUM_STR(WHICH_LEVEL) + "\"...");
        transitions[WHICH_LEVEL] = parseMap(WHICH_LEVEL);
    }

    for (const auto & ENUM_TRANSITIONS_PAIR : transitions)
    {
        const auto CURRENT_LEVEL { ENUM_TRANSITIONS_PAIR.first };

        M_HP_LOG("About to test \"" + NAMEOF_ENUM_STR(CURRENT_LEVEL) + "\" Map's transitions");

        for (const auto ENTRY_LEVEL : ENUM_TRANSITIONS_PAIR.second.entry_levels)
        {
            const auto ENTRY_TRANSITIONS { transitions[ENTRY_LEVEL] };

            const auto WAS_FOUND { std::find(
                                       std::begin(ENTRY_TRANSITIONS.exit_levels),
                                       std::end(ENTRY_TRANSITIONS.exit_levels),
                                       CURRENT_LEVEL)
                                   != std::end(ENTRY_TRANSITIONS.exit_levels) };

            BOOST_CHECK_MESSAGE(
                WAS_FOUND,
                "Map \"" << NAMEOF_ENUM(CURRENT_LEVEL) << "\" had an entry_level from map \""
                         << NAMEOF_ENUM(ENTRY_LEVEL)
                         << "\", but that map did not have an exit level to match.");
        }

        for (const auto EXIT_LEVEL : ENUM_TRANSITIONS_PAIR.second.exit_levels)
        {
            const auto EXIT_TRANSITIONS { transitions[EXIT_LEVEL] };

            const auto WAS_FOUND { std::find(
                                       std::begin(EXIT_TRANSITIONS.entry_levels),
                                       std::end(EXIT_TRANSITIONS.entry_levels),
                                       CURRENT_LEVEL)
                                   != std::end(EXIT_TRANSITIONS.entry_levels) };

            BOOST_CHECK_MESSAGE(
                WAS_FOUND,
                "Map \"" << NAMEOF_ENUM(CURRENT_LEVEL) << "\" had an exit level to map \""
                         << NAMEOF_ENUM(EXIT_LEVEL)
                         << "\", but that map did not have an entry level to match.");
        }
    }
}
