// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#include "unit-test-test-stuff.hpp"

using namespace heroespath;
using namespace heroespath::test;

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_ImageLoad"

#include <boost/test/unit_test.hpp>

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

template <typename EnumWrapper_t>
void TestEnumImageLoading()
{
    for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
    {
        const auto ENUM { static_cast<typename EnumWrapper_t::Enum>(index) };

        M_HP_LOG(
            "Unit Test \"image_loading__enum_image_loading\" TestImageLoading<"
            << NAMEOF_TYPE(EnumWrapper_t) << "> on image with index/value=" << index
            << ", enum=" << NAMEOF_ENUM(ENUM) << "...");

        GameEngineGlobalFixture::displayNextImage(gui::LoadAndCacheImage(ENUM));
    }
}

BOOST_AUTO_TEST_CASE(image_loading__enum_image_loading)
{
    GameEngineGlobalFixture::startUnitTestSeries(
        "Enum Images Test",
        (creature::Conditions::Count + creature::Titles ::Count + gui::CombatImageType::Count
         + song::Songs ::Count + spell::Spells ::Count));

    TestEnumImageLoading<creature::Conditions>();
    TestEnumImageLoading<creature::Titles>();
    TestEnumImageLoading<gui::CombatImageType>();
    TestEnumImageLoading<song::Songs>();
    TestEnumImageLoading<spell::Spells>();
}

BOOST_AUTO_TEST_CASE(image_loading__load_each_image_found_in_config_file)
{
    auto & configFile = *misc::ConfigFile::Instance();

    struct KeyPath
    {
        KeyPath(const std::string & KEY = "", const std::string & PATH = "")
            : key(KEY)
            , path(PATH)
        {}

        std::string key;
        std::string path;
    };

    // create a container of keys and paths to images
    std::vector<KeyPath> keyPaths;
    {
        std::vector<std::string> keys = configFile.FindAllKeysWithPrefix("media-image-");

        BOOST_CHECK_MESSAGE(
            !keys.empty(), "configFile.FindAllKeysWithPrefix(\"media-image-\") found no keys.");

        for (const auto & KEY : keys)
        {
            BOOST_CHECK_MESSAGE(
                !KEY.empty(),
                "configFile.FindAllKeysWithPrefix(\"media-image-\") returned "
                    << keys.size() << " keys but one was an empty string.");

            // skip if not a key to an actual image
            if (boost::algorithm::ends_with(KEY, "-dir")
                || boost::algorithm::ends_with(KEY, "-rect"))
            {
                M_HP_LOG(
                    "skipping key=\"" << KEY << "\" because it is not a key to an actual image.");

                continue;
            }

            const std::string PATH(configFile.GetMediaPath(KEY));

            BOOST_CHECK_MESSAGE(!PATH.empty(), "The key=\"" << KEY << "\" had an empty path.");

            if (std::find_if(
                    std::begin(keyPaths),
                    std::end(keyPaths),
                    [&PATH](const auto & KEYPATH) { return (KEYPATH.path.compare(PATH) == 0); })
                == std::end(keyPaths))
            {
                keyPaths.emplace_back(KEY, PATH);
            }
            else
            {
                M_HP_LOG(
                    "skipping key=\"" << KEY << "\" because its path=\"" << PATH
                                      << "\" is a duplicate.");
            }
        }
    }

    BOOST_CHECK_MESSAGE(
        !keyPaths.empty(),
        "Even though configFile.FindAllKeysWithPrefix(\"media-image-\") found keys, none of "
        "them were actual valid images.");

    GameEngineGlobalFixture::startUnitTestSeries(
        "All Images in the Config File, Load and Display Test", keyPaths.size());

    for (const auto & KEYPATH : keyPaths)
    {
        M_HP_LOG(
            "About to test key=\"" << KEYPATH.key << "\" with path=\"" << KEYPATH.path << "\"");

        GameEngineGlobalFixture::displayNextImage(gui::CachedTexture(KEYPATH.key));
    }
}
