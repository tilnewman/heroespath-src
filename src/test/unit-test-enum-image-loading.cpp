// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_EnumImageLoad"

#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4266)
#endif

#include <boost/test/unit_test.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include "game/startup-shutdown.hpp"
#include "gui/display.hpp"
#include "gui/image-loaders.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/nameof.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/scale.hpp"

#include <SFML/Graphics.hpp>

using namespace heroespath;

struct DisplayStuff
{
    static sf::Sprite sprite;
};

sf::Sprite DisplayStuff::sprite;

template <typename EnumWrapper_t>
void TestEnumImageLoading()
{
    for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
    {
        const auto ENUM_VALUE { static_cast<typename EnumWrapper_t::Enum>(index) };
        const gui::CachedTexture CACHED_TEXTURE { gui::LoadAndCacheImage(ENUM_VALUE) };
        const sf::Texture & TEXTURE { CACHED_TEXTURE.Get() };

        std::ostringstream ss;
        ss << "TestImageLoading<" << NAMEOF_TYPE_T(EnumWrapper_t)
           << "> on image with index/value=" << index
           << ", enum=" << EnumWrapper_t::ToString(ENUM_VALUE);

        M_HP_LOG(ss.str() << ".");

        DisplayStuff::sprite.setTexture(TEXTURE);
        sfutil::FitAndCenterTo(DisplayStuff::sprite, gui::Display::Instance()->FullScreenRect());
        gui::Display::Instance()->ClearToBlack();
        gui::Display::Instance()->TestDraw(DisplayStuff::sprite);
        gui::Display::Instance()->DisplayFrameBuffer();
    }
}

BOOST_AUTO_TEST_CASE(Misc_EnumImageLoading_Tests)
{
    heroespath::game::StartupShutdown startStop("Heroes' Path Test", 0, nullptr);

    TestEnumImageLoading<creature::Conditions>();
    TestEnumImageLoading<creature::Titles>();
    TestEnumImageLoading<gui::CombatImageType>();
    TestEnumImageLoading<song::Songs>();
    TestEnumImageLoading<spell::Spells>();

    gui::Display::Instance()->ClearToBlack();
    gui::Display::Instance()->DisplayFrameBuffer();
}
