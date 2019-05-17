// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-01200-image-loading.cpp
//
#define BOOST_TEST_MODULE "image_loading"
#include <boost/test/unit_test.hpp>

#include "avatar/avatar-enum.hpp"
#include "creature/condition.hpp"
#include "creature/title.hpp"
#include "gui/combat-image-enum.hpp"
#include "gui/image-loaders.hpp"
#include "misc/boost-string-includes.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "testutil/common.hpp"
#include "testutil/i-displayer.hpp"
#include "testutil/single-image-displayer-scoped.hpp"

using namespace heroespath;
using namespace heroespath::test;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestAll;
    m_iDisplayerUPtr = std::make_unique<SingleImageDisplayer>();
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

template <typename EnumWrapper_t>
void TestEnumImageLoading(SingleImageDisplayerScoped & displayerScoped)
{
    for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
    {
        const auto ENUM { static_cast<typename EnumWrapper_t::Enum>(index) };

        M_HP_LOG(
            displayerScoped.name() << ", about to load and display " << NAMEOF_TYPE(EnumWrapper_t)
                                   << " #" << index << " \"" << NAMEOF_ENUM(ENUM) << "\"...");

        displayerScoped.appendImage(gui::LoadAndCacheImage(ENUM));
    }
}

BOOST_AUTO_TEST_CASE(enums)
{
    const auto IMAGE_COUNT_TOTAL = static_cast<std::size_t>(
        creature::Conditions::Count + creature::Titles::Count + gui::CombatImageType::Count
        + song::Songs::Count + spell::Spells::Count + gui::CombatImageType::Count);

    SingleImageDisplayerScoped displayerScoped(IMAGE_COUNT_TOTAL);

    TestEnumImageLoading<creature::Conditions>(displayerScoped);
    TestEnumImageLoading<creature::Titles>(displayerScoped);
    TestEnumImageLoading<gui::CombatImageType>(displayerScoped);
    TestEnumImageLoading<song::Songs>(displayerScoped);
    TestEnumImageLoading<spell::Spells>(displayerScoped);
    TestEnumImageLoading<gui::CombatImageType>(displayerScoped);
}

BOOST_AUTO_TEST_CASE(avatar_enums)
{
    SingleImageDisplayerScoped displayerScoped(static_cast<std::size_t>(avatar::Avatar::Count));

    for (EnumUnderlying_t index(0); index < avatar::Avatar::Count; ++index)
    {
        const auto ENUM { static_cast<typename avatar::Avatar::Enum>(index) };

        M_HP_LOG(
            displayerScoped.name() << ", about to load and display " << NAMEOF_TYPE(avatar::Avatar)
                                   << " #" << index << " \"" << NAMEOF_ENUM(ENUM) << "\"...");

        displayerScoped.appendImage(
            gui::CachedTexture(PathWrapper(avatar::Avatar::ImagePath(ENUM))));
    }
}

BOOST_AUTO_TEST_CASE(all_media_image_in_config_file)
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
        }
    }

    BOOST_CHECK_MESSAGE(
        !keyPaths.empty(),
        "Even though configFile.FindAllKeysWithPrefix(\"media-image-\") found keys, none of "
        "them were actual valid images.");

    SingleImageDisplayerScoped displayerScoped(keyPaths.size());

    for (const auto & KEYPATH : keyPaths)
    {
        M_HP_LOG(
            displayerScoped.name() << ", about to load and display key=\"" << KEYPATH.key
                                   << "\", path=\"" << KEYPATH.path << "\"...");

        displayerScoped.appendImage(gui::CachedTexture(KEYPATH.key));
    }
}
