// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-audio.cpp
#define BOOST_TEST_MODULE "audio"

#include <boost/test/unit_test.hpp>

#include "game/setup-teardown.hpp"
#include "gui/display.hpp"
#include "gui/sound-manager.hpp"
#include "misc/log-macros.hpp"
#include "test/util/common.hpp"
#include "test/util/drawables-displayer.hpp"
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

inline void delay(SingleImageDisplayerScoped & displayer, const float SECONDS = 0.01f)
{
    sf::Clock clock;
    const auto sleepSfSec = sf::seconds(0.01f);

    displayer.get().incrememntProgress();
    displayer.draw();

    do
    {
        GameEngineGlobalFixture::checkEvents();
        gui::SoundManager::Instance()->UpdateTime(0.1f);

        if (clock.getElapsedTime().asSeconds() < SECONDS)
        {
            sf::sleep(sleepSfSec);
        }

    } while (clock.getElapsedTime().asSeconds() < SECONDS);
}

inline void stopAll()
{
    GameEngineGlobalFixture::checkEvents();

    gui::SoundManager::Instance()->ClearSoundEffectsCache(true);
    gui::SoundManager::Instance()->MusicStop(gui::music::All);

    for (int i(0); i < 10; ++i)
    {
        GameEngineGlobalFixture::checkEvents();
        gui::SoundManager::Instance()->UpdateTime(10.0f);
    }
}

BOOST_AUTO_TEST_CASE(specific)
{
    SingleImageDisplayerScoped displayerScoped("specific", 7);

    gui::SoundManager::Instance()->SoundEffectVolumeSet(25.0f);

    const float DELAY_DURATION_SEC(0.2f);

    M_HP_LOG("About to test PlaySfx_AckMajor()...");
    gui::SoundManager::Instance()->PlaySfx_AckMajor();
    delay(displayerScoped, DELAY_DURATION_SEC);

    M_HP_LOG("About to test PlaySfx_AckMinor()...");
    gui::SoundManager::Instance()->PlaySfx_AckMinor();
    delay(displayerScoped, DELAY_DURATION_SEC);

    M_HP_LOG("About to test PlaySfx_Keypress()...");
    gui::SoundManager::Instance()->PlaySfx_Keypress();
    delay(displayerScoped, DELAY_DURATION_SEC);

    M_HP_LOG("About to test PlaySfx_MouseClick()...");
    gui::SoundManager::Instance()->PlaySfx_MouseClick();
    delay(displayerScoped, DELAY_DURATION_SEC);

    M_HP_LOG("About to test PlaySfx_Reject()...");
    gui::SoundManager::Instance()->PlaySfx_Reject();
    delay(displayerScoped, DELAY_DURATION_SEC);

    M_HP_LOG("About to test PlaySfx_TickOff()...");
    gui::SoundManager::Instance()->PlaySfx_TickOff();
    delay(displayerScoped, DELAY_DURATION_SEC);

    M_HP_LOG("About to test PlaySfx_TickOn()...");
    gui::SoundManager::Instance()->PlaySfx_TickOn();
    delay(displayerScoped, DELAY_DURATION_SEC);

    stopAll();
}

BOOST_AUTO_TEST_CASE(sfx)
{
    SingleImageDisplayerScoped displayerScoped("sfx", gui::sound_effect::Count);

    gui::SoundManager::Instance()->SoundEffectVolumeSet(25.0f);

    for (EnumUnderlying_t index(0); index < gui::sound_effect::Count; ++index)
    {
        const auto ENUM { static_cast<typename gui::sound_effect::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };
        M_HP_LOG("About to test sfx \"" << ENUM_STR << "\" #" << index << "...");
        gui::SoundManager::Instance()->SoundEffectPlay(ENUM);
        delay(displayerScoped);
    }

    stopAll();
}

BOOST_AUTO_TEST_CASE(music)
{
    SingleImageDisplayerScoped displayerScoped("music", gui::music::Count);

    gui::SoundManager::Instance()->MusicVolumeSet(25.0f);

    auto & soundManager = *gui::SoundManager::Instance();

    for (EnumUnderlying_t index(0); index < gui::music::Count; ++index)
    {
        const auto ENUM { static_cast<typename gui::music::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };
        M_HP_LOG("About to start music \"" << ENUM_STR << "\" #" << index << "...");
        soundManager.MusicStart(ENUM);
        delay(displayerScoped, 0.2f);
    }

    stopAll();
}
