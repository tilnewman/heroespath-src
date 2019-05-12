// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_ImageLoad"

#include <boost/test/unit_test.hpp>

#include "game/startup-shutdown.hpp"
#include "gui/display.hpp"
#include "gui/image-loaders.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/scale.hpp"

#include <SFML/Graphics.hpp>

using namespace heroespath;

struct Fixture
{
    void setup()
    {
        startupShutdownUPtr
            = std::make_unique<game::StartupShutdown>("HP Test Image Loading", 0, nullptr, true);

        gui::Display::Instance()->PollEvents();
    }

    void teardown()
    {
        gui::Display::Instance()->PollEvents();
        cachedTextureUPtr.reset();
        startupShutdownUPtr.reset();
    }

    template <typename T>
    static void display(T && cachedTexture)
    {
        cachedTextureUPtr = std::make_unique<gui::CachedTexture>(cachedTexture);

        BOOST_CHECK(cachedTextureUPtr->Get().getSize().x > 0);
        BOOST_CHECK(cachedTextureUPtr->Get().getSize().y > 0);

        sprite.setTexture(cachedTextureUPtr->Get(), true);

        const auto INITIAL_GLOBAL_BOUNDS = sprite.getGlobalBounds();

        if ((INITIAL_GLOBAL_BOUNDS.width > gui::Display::Instance()->GetWinWidth())
            || (INITIAL_GLOBAL_BOUNDS.height > gui::Display::Instance()->GetWinHeight()))
        {
            sfutil::FitAndCenterTo(sprite, gui::Display::Instance()->FullScreenRect());
        }
        else
        {
            sfutil::CenterTo(sprite, gui::Display::Instance()->FullScreenRect());
        }

        gui::Display::Instance()->ClearToBlack();
        gui::Display::Instance()->TestDraw(sprite);
        gui::Display::Instance()->DisplayFrameBuffer();
    }

    static std::unique_ptr<game::StartupShutdown> startupShutdownUPtr;
    static std::unique_ptr<gui::CachedTexture> cachedTextureUPtr;
    static sf::Sprite sprite;
    static sf::Vector2f windowSize;
    static sf::FloatRect windowRect;
};

std::unique_ptr<heroespath::game::StartupShutdown> Fixture::startupShutdownUPtr;
std::unique_ptr<gui::CachedTexture> Fixture::cachedTextureUPtr;
sf::Sprite Fixture::sprite;
sf::Vector2f Fixture::windowSize;
sf::FloatRect Fixture::windowRect;

BOOST_TEST_GLOBAL_FIXTURE(Fixture);

template <typename EnumWrapper_t>
void TestEnumImageLoading()
{
    for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
    {
        const auto ENUM_VALUE { static_cast<typename EnumWrapper_t::Enum>(index) };

        M_HP_LOG(
            "Unit Test \"image_loading__enum_image_loading\" TestImageLoading<"
            << NAMEOF_TYPE(EnumWrapper_t) << "> on image with index/value=" << index
            << ", enum=" << NAMEOF_ENUM(ENUM_VALUE) << "...");

        Fixture::display(gui::LoadAndCacheImage(ENUM_VALUE));
    }
}

BOOST_AUTO_TEST_CASE(image_loading__enum_image_loading)
{
    TestEnumImageLoading<creature::Conditions>();
    TestEnumImageLoading<creature::Titles>();
    TestEnumImageLoading<gui::CombatImageType>();
    TestEnumImageLoading<song::Songs>();
    TestEnumImageLoading<spell::Spells>();
}

BOOST_AUTO_TEST_CASE(image_loading__load_each_image_found_in_config_file)
{
    std::vector<std::string> imagePathKeys
        = misc::ConfigFile::Instance()->FindAllKeysWithPrefix("media-image-");

    // remove keys that are not for actual images
    imagePathKeys.erase(
        std::remove_if(
            std::begin(imagePathKeys),
            std::end(imagePathKeys),
            [](const auto & KEY_STR) {
                return (
                    boost::algorithm::ends_with(KEY_STR, "-dir")
                    || boost::algorithm::ends_with(KEY_STR, "-texture-rect"));
            }),
        std::end(imagePathKeys));

    BOOST_CHECK(!imagePathKeys.empty());

    std::size_t imageIndex(0);

    sf::Sprite sprite;
    while ((imageIndex < imagePathKeys.size()))
    {
        const auto KEY_STR { imagePathKeys.at(imageIndex++) };

        M_HP_LOG(
            "Unit Test \"image_loading__enum_image_loading\" about to test image index #"
            << imageIndex << ", key=\"" << KEY_STR << "\"...");

        BOOST_CHECK(!KEY_STR.empty());

        const auto IMAGE_PATH_STR { misc::ConfigFile::Instance()->GetMediaPath(KEY_STR) };
        M_HP_LOG("...which has the media path=\"" << IMAGE_PATH_STR << "\"...");
        BOOST_CHECK(!IMAGE_PATH_STR.empty());

        Fixture::display(gui::CachedTexture(KEY_STR));
    }

    BOOST_CHECK(imageIndex == imagePathKeys.size());
}
