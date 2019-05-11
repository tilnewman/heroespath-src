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

struct TestStuff
{
    TestStuff()
    //: window(
    //      sf::VideoMode(800, 600, sf::VideoMode::getDesktopMode().bitsPerPixel),
    //      "HP Test Image Loading")
    //, windowSize(window.getSize())
    //, windowRect(sf::Vector2f(), windowSize)
    {
        // window.clear(sf::Color::Red);
    }

    template <typename T>
    void drawAndDisplay(const T & drawableThing)
    {
        // window.clear();
        // window.draw(drawableThing);
        // window.display();
        // sf::sleep(sf::seconds(0.01f));
    }

    void handleEvents()
    {
        // sf::Event event;
        // while (window.pollEvent(event))
        //{
        //    BOOST_CHECK(event.type != sf::Event::Closed);
        //}
    }

    // sf::RenderWindow window;
    // sf::Vector2f windowSize;
    // sf::FloatRect windowRect;
    std::unique_ptr<gui::CachedTexture> cachedTextureUPtr;
};

struct Fixture
{
    void setup()
    {
        startupShutdownUPtr
            = std::make_unique<game::StartupShutdown>("HP Test Image Loading", 0, nullptr);

        stuffUPtr = std::make_unique<TestStuff>();
    }

    void teardown()
    {
        stuffUPtr.reset();
        startupShutdownUPtr.reset();
    }

    static TestStuff & stuff()
    {
        BOOST_CHECK(stuffUPtr.get() != nullptr);
        return *stuffUPtr;
    }

    template <typename T>
    static void drawAndDisplay(const T & drawableThing)
    {
        gui::Display::Instance()->PollEvents();
        gui::Display::Instance()->ClearToBlack();
        gui::Display::Instance()->TestDraw(drawableThing);
        gui::Display::Instance()->DisplayFrameBuffer();
        sf::sleep(sf::seconds(0.01f));
    }

    // template<typename T>
    // static void updateTextureAndSprite(T && cachedTexture)
    //{
    //
    //}

    static std::unique_ptr<game::StartupShutdown> startupShutdownUPtr;
    static std::unique_ptr<TestStuff> stuffUPtr;
};

std::unique_ptr<heroespath::game::StartupShutdown> Fixture::startupShutdownUPtr;
std::unique_ptr<TestStuff> Fixture::stuffUPtr;

BOOST_TEST_GLOBAL_FIXTURE(Fixture);

template <typename EnumWrapper_t>
void TestEnumImageLoading(sf::Sprite & sprite)
{
    for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
    {
        const auto ENUM_VALUE { static_cast<typename EnumWrapper_t::Enum>(index) };

        M_HP_LOG(
            "Unit Test \"image_loading__enum_image_loading\" TestImageLoading<"
            << NAMEOF_TYPE(EnumWrapper_t) << "> on image with index/value=" << index
            << ", enum=" << NAMEOF_ENUM(ENUM_VALUE) << "...");

        Fixture::stuff().cachedTextureUPtr
            = std::make_unique<gui::CachedTexture>(gui::LoadAndCacheImage(ENUM_VALUE));

        BOOST_CHECK(Fixture::stuff().cachedTextureUPtr->Get().getSize().x > 0);
        BOOST_CHECK(Fixture::stuff().cachedTextureUPtr->Get().getSize().y > 0);

        sprite.setTexture(Fixture::stuff().cachedTextureUPtr->Get(), true);
        sprite.setPosition(0.0f, 0.0f);
        Fixture::drawAndDisplay(sprite);
        // Fixture::stuff().handleEvents();
        // Fixture::stuff().drawAndDisplay(sprite);
    }
}

BOOST_AUTO_TEST_CASE(image_loading__enum_image_loading)
{
    sf::Sprite sprite;
    TestEnumImageLoading<creature::Conditions>(sprite);
    TestEnumImageLoading<creature::Titles>(sprite);
    TestEnumImageLoading<gui::CombatImageType>(sprite);
    TestEnumImageLoading<song::Songs>(sprite);
    TestEnumImageLoading<spell::Spells>(sprite);
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
        {
            const auto KEY_STR { imagePathKeys.at(imageIndex++) };

            M_HP_LOG(
                "Unit Test \"image_loading__enum_image_loading\" about to test image index #"
                << imageIndex << ", key=\"" << KEY_STR << "\"...");

            BOOST_CHECK(!KEY_STR.empty());

            const auto IMAGE_PATH_STR { misc::ConfigFile::Instance()->GetMediaPath(KEY_STR) };
            M_HP_LOG("...which has the media path=\"" << IMAGE_PATH_STR << "\"...");
            BOOST_CHECK(!IMAGE_PATH_STR.empty());

            Fixture::stuff().cachedTextureUPtr = std::make_unique<gui::CachedTexture>(KEY_STR);
            sprite.setTexture(Fixture::stuff().cachedTextureUPtr->Get(), true);

            const auto INITIAL_GLOBAL_BOUNDS = sprite.getGlobalBounds();

            BOOST_CHECK(INITIAL_GLOBAL_BOUNDS.width > 0.0f);
            BOOST_CHECK(INITIAL_GLOBAL_BOUNDS.height > 0.0f);
            sprite.setPosition(0.0f, 0.0f);
            Fixture::drawAndDisplay(sprite);
            // if ((INITIAL_GLOBAL_BOUNDS.width > Fixture::stuff().windowSize.x)
            //    || (INITIAL_GLOBAL_BOUNDS.height > Fixture::stuff().windowSize.y))
            //{
            //    sfutil::FitAndCenterTo(sprite, Fixture::stuff().windowRect);
            //}
            // else
            //{
            //    sfutil::CenterTo(sprite, Fixture::stuff().windowRect);
            //}
        }

        // Fixture::stuff().handleEvents();
        // Fixture::stuff().drawAndDisplay(sprite);
    }

    BOOST_CHECK(imageIndex == imagePathKeys.size());
}
