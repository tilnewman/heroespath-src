// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-image-loading.cpp
//
#define BOOST_TEST_MODULE "image_loading"
#include <boost/test/unit_test.hpp>

#include "avatar/avatar-enum.hpp"
#include "creature/condition.hpp"
#include "creature/title.hpp"
#include "game/setup-teardown.hpp"
#include "gui/animation-factory.hpp"
#include "gui/animation.hpp"
#include "gui/combat-image-enum.hpp"
#include "gui/display.hpp"
#include "gui/image-loaders.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/scale.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "test/util/common.hpp"
#include "test/util/drawables-displayer.hpp"
#include "test/util/game-engine-global-fixture.hpp"
#include "test/util/single-image-displayer-scoped.hpp"
#include "test/util/single-image-displayer.hpp"

#include <SFML/Graphics.hpp>

using namespace heroespath;
using namespace heroespath::test;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestAll;
    m_iDisplayerUPtr = std::make_unique<SingleImageDisplayer>();
    // m_delayAfterEachDrawSec = 0.25f;
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

template <typename EnumWrapper_t>
void TestEnumImageLoading(const bool WILL_ENSURE_ALL_IMAGES_SAME_SIZE)
{
    SingleImageDisplayerScoped displayerScoped(
        std::string(NAMEOF_TYPE(EnumWrapper_t)) + "::Enum",
        EnumWrapper_t::Count,
        WILL_ENSURE_ALL_IMAGES_SAME_SIZE);

    for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
    {
        const auto ENUM { static_cast<typename EnumWrapper_t::Enum>(index) };

        M_HP_LOG(
            displayerScoped.get().name()
            << ", about to load and display " << NAMEOF_TYPE(EnumWrapper_t) << " #" << index
            << " \"" << NAMEOF_ENUM(ENUM) << "\"...");

        displayerScoped.get().appendImageToSeries(gui::LoadAndCacheImage(ENUM));
        displayerScoped.draw();
    }
}

BOOST_AUTO_TEST_CASE(enums)
{
    TestEnumImageLoading<creature::Conditions>(true);
    TestEnumImageLoading<creature::Titles>(true);
    TestEnumImageLoading<song::Songs>(true);
    TestEnumImageLoading<spell::Spells>(true);
    TestEnumImageLoading<gui::CombatImageType>(false);
}

BOOST_AUTO_TEST_CASE(avatar_enums)
{
    SingleImageDisplayerScoped displayerScoped(
        "avatar::Avatar::Enum", static_cast<std::size_t>(avatar::Avatar::Count), true);

    for (EnumUnderlying_t index(0); index < avatar::Avatar::Count; ++index)
    {
        const auto ENUM { static_cast<typename avatar::Avatar::Enum>(index) };

        M_HP_LOG(
            displayerScoped.get().name()
            << ", about to load and display " << NAMEOF_TYPE(avatar::Avatar) << " #" << index
            << " \"" << NAMEOF_ENUM(ENUM) << "\"...");

        displayerScoped.get().appendImageToSeries(
            gui::CachedTexture(PathWrapper(avatar::Avatar::ImagePath(ENUM))));

        displayerScoped.draw();
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

    SingleImageDisplayerScoped displayerScoped("AllMediaImageConfigKeys", keyPaths.size());

    for (const auto & KEYPATH : keyPaths)
    {
        M_HP_LOG(
            displayerScoped.get().name() << ", about to load and display key=\"" << KEYPATH.key
                                         << "\", path=\"" << KEYPATH.path << "\"...");

        displayerScoped.get().appendImageToSeries(gui::CachedTexture(KEYPATH.key));
        displayerScoped.draw();
    }
}

BOOST_AUTO_TEST_CASE(animations)
{
    for (EnumUnderlying_t index(0); index < gui::Animations::Count; ++index)
    {
        const auto ENUM { static_cast<typename gui::Animations::Enum>(index) };
        const std::string ENUM_STR { NAMEOF_ENUM(ENUM) };

        auto animUPtr = gui::AnimationFactory::Make(ENUM);

        SingleImageDisplayerScoped displayerScoped(ENUM_STR, animUPtr->FrameCount(), true);

        const auto CONTENT_REGION = displayerScoped.get().contentRegion();

        animUPtr->SetEntityRegion(
            sfutil::FitAndCenterToCopy(animUPtr->GetEntityRegion(), CONTENT_REGION, 0.5f));

        M_HP_LOG(
            displayerScoped.get().name()
            << ", about to load and display animation #" << index << ", enum=\"" << ENUM_STR
            << "\", entity=\"" << animUPtr->GetEntityName() << "\", which has "
            << animUPtr->FrameCount() << " frames...");

        displayerScoped.get().appendEntity(gui::IEntityPtr_t(animUPtr.get()));

        do
        {
            displayerScoped.draw(true);

            if (!displayerScoped.isPaused())
            {
                std::size_t frameCounter = animUPtr->CurrentFrame();

                while ((animUPtr->CurrentFrame() == frameCounter) && !animUPtr->IsFinished())
                {
                    animUPtr->UpdateTime(animUPtr->TimePerFrame() + 0.001f);
                }

                displayerScoped.get().incrememntProgress();
            }

        } while (!animUPtr->IsFinished());
    }
}

BOOST_AUTO_TEST_CASE(all_images_in_media_directory)
{
    const auto IMAGE_DIR_PATHS = misc::filesystem::FindDirectories(
        true, misc::ConfigFile::Instance()->GetMediaPath("media-image-dir"));

    BOOST_TEST(!IMAGE_DIR_PATHS.empty());

    for (const auto & DIR_PATH_STR : IMAGE_DIR_PATHS)
    {
        const auto IMAGE_FILE_PATHS = misc::filesystem::FindFiles(
            false, DIR_PATH_STR, "", "", misc::filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_);

        SingleImageDisplayerScoped displayerScoped(DIR_PATH_STR, IMAGE_FILE_PATHS.size(), false);
        GameEngineGlobalFixture::delayAfterEachDrawSet(0.0f);

        if (IMAGE_FILE_PATHS.empty())
        {
            M_HP_LOG_WRN(
                displayerScoped.get().name()
                << ", found an image directory with no images in it: \"" << DIR_PATH_STR << "\"");
        }

        for (const auto & FILE_PATH_STR : IMAGE_FILE_PATHS)
        {
            displayerScoped.get().appendImageToSeries(
                gui::CachedTexture(PathWrapper(FILE_PATH_STR)));

            displayerScoped.draw();
        }
    }
}
