// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-holders.cpp
#define BOOST_TEST_MODULE "holders"

#include <boost/test/unit_test.hpp>

#include "creature/condition-holder.hpp"
#include "creature/condition.hpp"
#include "creature/title-holder.hpp"
#include "creature/title.hpp"
#include "game/setup-teardown.hpp"
#include "misc/log-macros.hpp"
#include "song/song-holder.hpp"
#include "song/song.hpp"
#include "spell/spell-holder.hpp"
#include "spell/spell.hpp"
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

template <typename Enum_t, typename Pointer_t>
void testCommon(const Enum_t ENUM, const Pointer_t PTR)
{
    BOOST_TEST(PTR->Which() == ENUM);
    BOOST_TEST(!PTR->Name().empty());
    BOOST_TEST(!PTR->Desc().empty());
    BOOST_TEST(!PTR->ToString().empty());
}

template <typename Pointer_t>
void testSongAndSpell(const Pointer_t PTR)
{
    BOOST_TEST(!PTR->ManaCost().IsZero());
    BOOST_TEST(!PTR->Rank().IsZero());
    BOOST_TEST(!PTR->DescDetails().empty());
    BOOST_TEST(!PTR->DescComplete().empty());
    BOOST_TEST(!PTR->DescExtra().empty());
}

BOOST_AUTO_TEST_CASE(spells)
{
    for (EnumUnderlying_t index(0); index < spell::Spells::Count; ++index)
    {
        const auto ENUM { static_cast<typename spell::Spells::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };
        M_HP_LOG("About to test Holder for " << ENUM_STR << " #" << index << "...");
        const auto PTR { spell::Holder::Get(ENUM) };
        testCommon(ENUM, PTR);
        testSongAndSpell(PTR);
    }
}

BOOST_AUTO_TEST_CASE(songs)
{
    for (EnumUnderlying_t index(0); index < song::Songs::Count; ++index)
    {
        const auto ENUM { static_cast<typename song::Songs::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };
        M_HP_LOG("About to test Holder for " << ENUM_STR << " #" << index << "...");
        const auto PTR { song::Holder::Get(ENUM) };
        testCommon(ENUM, PTR);
        testSongAndSpell(PTR);
    }
}

BOOST_AUTO_TEST_CASE(conditions)
{
    for (EnumUnderlying_t index(0); index < song::Songs::Count; ++index)
    {
        const auto ENUM { static_cast<typename creature::Conditions::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };
        M_HP_LOG("About to test Holder for " << ENUM_STR << " #" << index << "...");
        const auto PTR { creature::condition::Holder::Get(ENUM) };
        testCommon(ENUM, PTR);
    }
}

BOOST_AUTO_TEST_CASE(titles)
{
    for (EnumUnderlying_t index(0); index < creature::Titles::Count; ++index)
    {
        const auto ENUM { static_cast<typename creature::Titles::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };
        M_HP_LOG("About to test Holder for " << ENUM_STR << " #" << index << "...");
        const auto PTR { creature::title::Holder::Get(ENUM) };
        testCommon(ENUM, PTR);
        BOOST_TEST(!PTR->RolesCopy().empty());
        BOOST_TEST(!PTR->ImageFilename().empty());
    }
}
