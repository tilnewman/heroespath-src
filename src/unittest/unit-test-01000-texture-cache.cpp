// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// unit-test-01000-texture-cache.cpp
//
#define BOOST_TEST_MODULE "texture_cache"

#include <boost/test/unit_test.hpp>

#include "gui/cached-texture.hpp"
#include "gui/texture-cache.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "testutil/game-engine-global-fixture.hpp"
#include "testutil/single-image-displayer.hpp"

#include <cstdlib>
#include <stdexcept>

using namespace heroespath;
using namespace heroespath::test;
using namespace heroespath::gui;
using namespace heroespath::misc;

void GameEngineGlobalFixture::setupBeforeAllTests()
{
    m_unitTestFilename = __FILE__;
    m_subsystemsToSetup = game::SubsystemCollection::TestAll;
    m_iDisplayerUPtr = std::make_unique<SingleImageDisplayer>();
}

BOOST_TEST_GLOBAL_FIXTURE(GameEngineGlobalFixture);

inline void
    LoadTexture(sf::Texture & texture, const std::string & PATH_STR_ORIG, const bool WILL_SMOOTH)
{
    const auto PATH_STR_COMPLETE { misc::filesystem::CleanPath(PATH_STR_ORIG) };

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (misc::filesystem::ExistsAndIsFile(PATH_STR_COMPLETE)),
        "LoadTexture(\""
            << PATH_STR_COMPLETE
            << "\") failed because that file either does not exist or is not a regular file.");

    M_HP_ASSERT_OR_LOG_AND_THROW(
        texture.loadFromFile(PATH_STR_COMPLETE),
        "LoadTexture(), sf::(Image or Texture).loadFromFile(\""
            << PATH_STR_COMPLETE << "\") failed.  Check console output for information.");

    if (WILL_SMOOTH)
    {
        texture.setSmooth(true);
    }
}

inline bool areImagesEqual(const sf::Image & A, const sf::Image & B)
{
    if (A.getSize() == B.getSize())
    {
        return (
            memcmp(
                A.getPixelsPtr(),
                B.getPixelsPtr(),
                (static_cast<std::size_t>(A.getSize().x) * static_cast<std::size_t>(A.getSize().y)
                 * static_cast<std::size_t>(4)))
            == 0);
    }
    else
    {
        return false;
    }
}

inline bool areImagesEqual(const sf::Texture & T, const sf::Image & I)
{
    return areImagesEqual(T.copyToImage(), I);
}

inline bool areImagesEqual(const sf::Texture & A, const sf::Texture & B)
{
    return areImagesEqual(A.copyToImage(), B.copyToImage());
}

inline const sf::Image
    quickLoadByPath(const std::string & PATH, const ImageOptions & OPTIONS = ImageOptions())
{
    sf::Texture texture;
    LoadTexture(texture, PATH, (OPTIONS.option_enum & ImageOpt::Smooth));
    sf::Image image(texture.copyToImage());
    OPTIONS.Apply(image);
    return image;
}

inline const sf::Image
    quickLoadByKey(const std::string & KEY, const ImageOptions & OPTIONS = ImageOptions())
{
    return quickLoadByPath(misc::ConfigFile::Instance()->GetMediaPath(KEY), OPTIONS);
}

BOOST_AUTO_TEST_CASE(helper_functions)
{
    const std::string IMAGE1_PATH_KEY("media-image-misc-todo");
    const std::string IMAGE2_PATH_KEY("media-image-misc-x");

    BOOST_CHECK(areImagesEqual(quickLoadByKey(IMAGE1_PATH_KEY), quickLoadByKey(IMAGE1_PATH_KEY)));

    BOOST_CHECK(
        areImagesEqual(quickLoadByKey(IMAGE1_PATH_KEY), quickLoadByKey(IMAGE2_PATH_KEY)) == false);

    BOOST_CHECK(areImagesEqual(quickLoadByKey(IMAGE1_PATH_KEY), quickLoadByKey(IMAGE1_PATH_KEY)));

    BOOST_CHECK(areImagesEqual(quickLoadByKey(IMAGE1_PATH_KEY), quickLoadByKey(IMAGE1_PATH_KEY)));

    BOOST_CHECK(
        areImagesEqual(
            quickLoadByKey(IMAGE1_PATH_KEY),
            quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz)))
        == false);

    BOOST_CHECK(
        areImagesEqual(
            quickLoadByKey(IMAGE1_PATH_KEY),
            quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert)))
        == false);

    BOOST_CHECK(
        areImagesEqual(
            quickLoadByKey(IMAGE1_PATH_KEY),
            quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::Invert)))
        == false);

    sf::Texture texture1;
    LoadTexture(texture1, misc::ConfigFile::Instance()->GetMediaPath(IMAGE1_PATH_KEY), false);

    BOOST_CHECK(areImagesEqual(texture1, quickLoadByKey(IMAGE1_PATH_KEY)));

    sf::Texture texture2;
    texture2.loadFromImage(quickLoadByKey(IMAGE1_PATH_KEY));
    BOOST_CHECK(areImagesEqual(texture1, texture2));
}

BOOST_AUTO_TEST_CASE(all_texture_cache_tests)
{
    TextureCache & tc = *gui::TextureCache::Instance();

    const std::string IMAGE1_PATH_KEY("media-image-misc-todo");
    const std::string IMAGE2_PATH_KEY("media-image-misc-x");
    const std::string IMAGE3_PATH_KEY("media-image-misc-talk");

    const std::string IMAGE1_PATH { misc::filesystem::CleanPath(
        misc::ConfigFile::Instance()->GetMediaPath(IMAGE1_PATH_KEY)) };

    const std::string IMAGE2_PATH(
        misc::filesystem::CleanPath(misc::ConfigFile::Instance()->GetMediaPath(IMAGE2_PATH_KEY)));

    const std::string IMAGE3_PATH(
        misc::filesystem::CleanPath(misc::ConfigFile::Instance()->GetMediaPath(IMAGE3_PATH_KEY)));

    const std::size_t IMAGES1_COUNT(12);
    const std::size_t IMAGES2_COUNT(6);

    const std::string IMAGES1_DIR_KEY("media-image-spell-dir");
    const std::string IMAGES2_DIR_KEY("media-image-song-dir");

    const std::string IMAGES1_DIR_PATH(
        misc::filesystem::CleanPath(misc::ConfigFile::Instance()->GetMediaPath(IMAGES1_DIR_KEY)));

    const std::string IMAGES2_DIR_PATH(
        misc::filesystem::CleanPath(misc::ConfigFile::Instance()->GetMediaPath(IMAGES2_DIR_KEY)));

    const ImageOptions ALL_OPTIONS { (
        ImageOpt::Smooth | ImageOpt::FlipHoriz | ImageOpt::FlipVert | ImageOpt::Invert) };

    //
    // BOOST_CHECK_THROW(tc.AddByKey(""), std::exception);
    // BOOST_CHECK_THROW(tc.AddByPath(""), std::exception);
    // BOOST_CHECK_THROW(tc.AddByPathFake("", sf::Texture()), std::exception);
    // BOOST_CHECK_THROW(tc.AddDirectoryByKey(""), std::exception);
    // BOOST_CHECK_THROW(tc.AddDirectoryByPath(""), std::exception);
    // BOOST_CHECK_THROW(tc.GetByIndex(0), std::exception);
    // BOOST_CHECK_THROW(tc.GetByIndex(1), std::exception);
    // BOOST_CHECK_THROW(tc.GetOptionsByIndex(0), std::exception);
    // BOOST_CHECK_THROW(tc.GetPathByIndex(0), std::exception);
    // BOOST_CHECK_THROW(tc.GetRefCountByIndex(0), std::exception);
    // BOOST_CHECK_THROW(tc.RemoveByKey(""), std::exception);
    // BOOST_CHECK_THROW(tc.RemoveByKey("", ALL_OPTIONS), std::exception);
    // BOOST_CHECK_THROW(tc.RemoveByPath(""), std::exception);
    // BOOST_CHECK_THROW(tc.RemoveByPath("", ALL_OPTIONS), std::exception);
    BOOST_CHECK_NO_THROW(tc.RemoveAll());

    {
        const auto IMAGE1_INDEX(tc.AddByKey(IMAGE1_PATH_KEY));

        BOOST_CHECK(IMAGE1_INDEX == 0);
        BOOST_CHECK(tc.GetOptionsByIndex(IMAGE1_INDEX) == ImageOptions());
        BOOST_CHECK(tc.GetPathByIndex(IMAGE1_INDEX) == IMAGE1_PATH);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 1);

        BOOST_CHECK(areImagesEqual(tc.GetByIndex(IMAGE1_INDEX), quickLoadByKey(IMAGE1_PATH_KEY)));

        BOOST_CHECK(areImagesEqual(
            tc.GetByIndex(IMAGE1_INDEX), quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions())));

        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 1);

        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1_INDEX + 1), std::exception);
        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1_INDEX - 1), std::exception);
        // BOOST_CHECK_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY, ALL_OPTIONS), std::exception);
        // BOOST_CHECK_THROW(tc.RemoveByPath(IMAGE1_PATH, ALL_OPTIONS), std::exception);

        BOOST_CHECK_NO_THROW(tc.RemoveAll());

        // BOOST_CHECK_THROW(tc.GetOptionsByIndex(IMAGE1_INDEX), std::exception);
        // BOOST_CHECK_THROW(tc.GetPathByIndex(IMAGE1_INDEX), std::exception);
        // BOOST_CHECK_THROW(tc.GetRefCountByIndex(IMAGE1_INDEX), std::exception);
        // BOOST_CHECK_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY), std::exception);
        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1_INDEX), std::exception);
    }

    {
        const auto IMAGE1_INDEX(tc.AddByKey(IMAGE1_PATH_KEY));
        BOOST_CHECK(IMAGE1_INDEX == 0);

        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::None)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::Invert)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert)), std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz |
        //    ImageOpt::FlipVert)), std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz | ImageOpt::Invert)),
        //    std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert | ImageOpt::Invert)),
        //    std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::None)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::Invert)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::FlipHoriz)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::FlipVert)), std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::FlipHoriz | ImageOpt::FlipVert)),
        //    std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::FlipHoriz | ImageOpt::Invert)),
        //    std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByPath(IMAGE1_PATH, ImageOptions(ImageOpt::FlipVert | ImageOpt::Invert)),
        //    std::exception);

        const auto IMAGE1B_INDEX(tc.AddByPath(IMAGE1_PATH, ALL_OPTIONS));

        BOOST_CHECK(IMAGE1_INDEX != IMAGE1B_INDEX);
        BOOST_CHECK(IMAGE1B_INDEX == 1);
        BOOST_CHECK(tc.GetOptionsByIndex(IMAGE1B_INDEX) == ALL_OPTIONS);
        BOOST_CHECK(tc.GetPathByIndex(IMAGE1B_INDEX) == IMAGE1_PATH);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 1);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1B_INDEX) == 1);

        BOOST_CHECK(areImagesEqual(
            tc.GetByIndex(IMAGE1B_INDEX), quickLoadByKey(IMAGE1_PATH_KEY, ALL_OPTIONS)));

        BOOST_CHECK(areImagesEqual(
            tc.GetByIndex(IMAGE1B_INDEX), quickLoadByPath(IMAGE1_PATH, ALL_OPTIONS)));

        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 1);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1B_INDEX) == 1);

        BOOST_CHECK(
            areImagesEqual(tc.GetByIndex(IMAGE1_INDEX), tc.GetByIndex(IMAGE1B_INDEX)) == false);

        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::Invert)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz)), std::exception);
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert)), std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz |
        //    ImageOpt::FlipVert)), std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz | ImageOpt::Invert)),
        //    std::exception);
        //
        // BOOST_CHECK_THROW(
        //    tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert | ImageOpt::Invert)),
        //    std::exception);

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY, ALL_OPTIONS));

        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1B_INDEX), std::exception);

        BOOST_CHECK_NO_THROW(tc.GetByIndex(IMAGE1_INDEX));

        BOOST_CHECK(tc.GetOptionsByIndex(IMAGE1_INDEX) == ImageOptions());
        BOOST_CHECK(tc.GetPathByIndex(IMAGE1_INDEX) == IMAGE1_PATH);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 1);

        BOOST_CHECK(tc.AddByKey(IMAGE1_PATH_KEY) == IMAGE1_INDEX);

        BOOST_CHECK(tc.GetOptionsByIndex(IMAGE1_INDEX) == ImageOptions());
        BOOST_CHECK(tc.GetPathByIndex(IMAGE1_INDEX) == IMAGE1_PATH);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 2);

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY, ImageOptions()));

        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX) == 1);

        BOOST_CHECK_NO_THROW(tc.RemoveByPath(IMAGE1_PATH));

        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1_INDEX), std::exception);
    }

    {
        const auto IMAGE1_INDEX(tc.AddByKey(IMAGE1_PATH_KEY));
        BOOST_CHECK(IMAGE1_INDEX == 0);

        const auto IMAGE2_INDEX(tc.AddByKey(IMAGE2_PATH_KEY));
        BOOST_CHECK(IMAGE2_INDEX == 1);

        const auto IMAGE3_INDEX(tc.AddByKey(IMAGE3_PATH_KEY));
        BOOST_CHECK(IMAGE3_INDEX == 2);

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE2_PATH_KEY));

        BOOST_CHECK(tc.AddByKey(IMAGE2_PATH_KEY) == 1);
    }

    {
        const auto IMAGE1_INDEX(tc.AddByKey(IMAGE1_PATH_KEY));
        BOOST_CHECK(IMAGE1_INDEX == 0);

        const auto IMAGE2_INDEX(tc.AddByKey(IMAGE2_PATH_KEY));
        BOOST_CHECK(IMAGE2_INDEX == (IMAGE1_INDEX + 1));

        BOOST_CHECK_NO_THROW(tc.RemoveAll());

        // BOOST_CHECK_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY), std::exception);
        // BOOST_CHECK_THROW(tc.RemoveByKey(IMAGE2_PATH_KEY), std::exception);

        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1_INDEX), std::exception);
        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE2_INDEX), std::exception);

        const auto IMAGE3_INDEX(tc.AddByKey(IMAGE3_PATH_KEY));
        BOOST_CHECK(IMAGE3_INDEX == 0);

        BOOST_CHECK_NO_THROW(tc.RemoveAll());

        // BOOST_CHECK_THROW(tc.RemoveByKey(IMAGE3_PATH_KEY), std::exception);
    }

    {
        const auto IMAGE1_INDEX1(tc.AddByKey(IMAGE1_PATH_KEY));
        BOOST_CHECK(IMAGE1_INDEX1 == 0);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX1) == 1);

        const auto IMAGE1_INDEX2(tc.AddByPath(IMAGE1_PATH, ImageOptions()));
        BOOST_CHECK(IMAGE1_INDEX2 == IMAGE1_INDEX1);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX1) == 2);

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY));

        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX1) == 1);

        BOOST_CHECK_NO_THROW(
            areImagesEqual(tc.GetByIndex(IMAGE1_INDEX1), quickLoadByKey(IMAGE1_PATH_KEY)));

        BOOST_CHECK_NO_THROW(
            areImagesEqual(tc.GetByIndex(IMAGE1_INDEX2), quickLoadByKey(IMAGE1_PATH_KEY)));

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY));
    }

    {
        const auto IMAGE1_INDEX1(tc.AddByKey(IMAGE1_PATH_KEY));
        BOOST_CHECK(IMAGE1_INDEX1 == 0);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX1) == 1);

        sf::Texture texture;
        const auto IMAGE1_INDEX2(tc.AddByPathFake(IMAGE1_PATH, texture, ImageOptions()));
        BOOST_CHECK(IMAGE1_INDEX2 == IMAGE1_INDEX1);
        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX1) == 2);

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY));

        BOOST_CHECK(tc.GetRefCountByIndex(IMAGE1_INDEX1) == 1);

        BOOST_CHECK_NO_THROW(
            areImagesEqual(tc.GetByIndex(IMAGE1_INDEX1), quickLoadByKey(IMAGE1_PATH_KEY)));

        BOOST_CHECK_NO_THROW(
            areImagesEqual(tc.GetByIndex(IMAGE1_INDEX1), quickLoadByKey(IMAGE1_PATH_KEY)));

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGE1_PATH_KEY));
    }

    {
        const auto IMAGE1(quickLoadByKey(IMAGE1_PATH_KEY, ALL_OPTIONS));

        sf::Texture texture;
        texture.loadFromImage(IMAGE1);

        const std::string FAKE_PATH("fake path");
        const auto IMAGE1_INDEX(tc.AddByPathFake(FAKE_PATH, texture, ALL_OPTIONS));
        BOOST_CHECK(IMAGE1_INDEX == 0);

        BOOST_CHECK_NO_THROW(areImagesEqual(tc.GetByIndex(IMAGE1_INDEX), texture));

        BOOST_CHECK_NO_THROW(tc.RemoveByPath(FAKE_PATH, ALL_OPTIONS));

        // BOOST_CHECK_THROW(tc.GetByIndex(IMAGE1_INDEX), std::exception);
    }

    {
        const auto IMAGE_INDEXES1(tc.AddDirectoryByKey(IMAGES1_DIR_KEY));
        BOOST_CHECK(IMAGE_INDEXES1.size() == IMAGES1_COUNT);
        for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        {
            const auto INDEX(IMAGE_INDEXES1.at(i));
            BOOST_CHECK(INDEX == i);
            BOOST_CHECK_NO_THROW(tc.GetByIndex(INDEX));
            BOOST_CHECK(tc.GetRefCountByIndex(INDEX) == 1);
        }

        // BOOST_CHECK_THROW(tc.AddByPath(IMAGES1_DIR_PATH), std::exception);
        // BOOST_CHECK_THROW(tc.AddByPathFake(IMAGES1_DIR_PATH, sf::Texture()), std::exception);

        const auto IMAGE_INDEXES2(tc.AddDirectoryByPath(IMAGES1_DIR_PATH));
        BOOST_CHECK(IMAGE_INDEXES1 == IMAGE_INDEXES2);
        for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        {
            const auto INDEX(IMAGE_INDEXES2.at(i));
            BOOST_CHECK_NO_THROW(tc.GetByIndex(INDEX));
            BOOST_CHECK(tc.GetRefCountByIndex(INDEX) == 2);
        }

        BOOST_CHECK_NO_THROW(tc.RemoveByPath(IMAGES1_DIR_PATH));

        for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        {
            const auto INDEX(IMAGE_INDEXES1.at(i));
            BOOST_CHECK_NO_THROW(tc.GetByIndex(INDEX));
            BOOST_CHECK(tc.GetRefCountByIndex(INDEX) == 1);
        }

        const auto IMAGE_INDEXES3(tc.AddDirectoryByKey(IMAGES1_DIR_KEY, ALL_OPTIONS));
        BOOST_CHECK(IMAGE_INDEXES3.size() == IMAGES1_COUNT);
        BOOST_CHECK(IMAGE_INDEXES1 != IMAGE_INDEXES3);
        for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        {
            const auto INDEX(IMAGE_INDEXES3.at(i));
            BOOST_CHECK_NO_THROW(tc.GetByIndex(INDEX));

            BOOST_CHECK(
                areImagesEqual(tc.GetByIndex(INDEX), tc.GetByIndex(IMAGE_INDEXES1.at(i))) == false);
        }

        BOOST_CHECK_NO_THROW(tc.RemoveByKey(IMAGES1_DIR_KEY, ImageOptions()));

        // for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        //{
        //    //BOOST_CHECK_THROW(tc.GetByIndex(IMAGE_INDEXES1.at(i)), std::exception);
        //}
    }

    {
        CachedTexture ct1A(IMAGE1_PATH_KEY);

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.Options() == ImageOptions());
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(ct1A.Path() == IMAGE1_PATH);

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct1B(ct1A);

            BOOST_CHECK(ct1B.RefCount() == 2);
            BOOST_CHECK(ct1A.RefCount() == 2);

            BOOST_CHECK(ct1B == ct1A);

            BOOST_CHECK(ct1B.Index() == 0);
            BOOST_CHECK(ct1B.Index() == ct1A.Index());
            BOOST_CHECK(ct1B.Options() == ct1A.Options());
            BOOST_CHECK(ct1B.Options() == ImageOptions());
            BOOST_CHECK(ct1B.RefCount() == ct1A.RefCount());
            BOOST_CHECK(ct1B.RefCount() == 2);
            BOOST_CHECK(ct1B.Path() == ct1A.Path());
            BOOST_CHECK(ct1B.Path() == IMAGE1_PATH);

            BOOST_CHECK(areImagesEqual(ct1A.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(ct1B.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1B.Get()));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1B.Index()), ct1A.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1B.Get()));

            CachedTexture ct1C(ct1B);

            BOOST_CHECK(ct1C.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1A.RefCount() == 3);

            BOOST_CHECK(ct1A == ct1B);
            BOOST_CHECK(ct1B == ct1C);
            BOOST_CHECK(ct1C == ct1A);

            BOOST_CHECK(ct1C.Index() == 0);
            BOOST_CHECK(ct1C.Index() == ct1A.Index());
            BOOST_CHECK(ct1C.Options() == ct1A.Options());
            BOOST_CHECK(ct1C.Options() == ImageOptions());
            BOOST_CHECK(ct1C.RefCount() == ct1A.RefCount());
            BOOST_CHECK(ct1C.RefCount() == 3);
            BOOST_CHECK(ct1C.Path() == ct1A.Path());
            BOOST_CHECK(ct1C.Path() == IMAGE1_PATH);

            BOOST_CHECK(areImagesEqual(ct1A.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(ct1C.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1C.Get()));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1C.Index()), ct1A.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1C.Get()));
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct1B(ct1A);

            BOOST_CHECK(ct1A.RefCount() == 2);
            BOOST_CHECK(ct1B.RefCount() == 2);
            BOOST_CHECK(ct1B.Index() == 0);

            CachedTexture ct1C(std::move(ct1B));

            BOOST_CHECK(ct1A.RefCount() == 2);
            BOOST_CHECK(ct1C.RefCount() == 2);

            BOOST_CHECK(ct1C == ct1A);

            BOOST_CHECK(ct1C.Index() == 0);
            BOOST_CHECK(ct1C.Index() == ct1A.Index());
            BOOST_CHECK(ct1C.Options() == ct1A.Options());
            BOOST_CHECK(ct1C.Options() == ImageOptions());
            BOOST_CHECK(ct1C.RefCount() == ct1A.RefCount());
            BOOST_CHECK(ct1C.RefCount() == 2);
            BOOST_CHECK(ct1C.Path() == ct1A.Path());
            BOOST_CHECK(ct1C.Path() == IMAGE1_PATH);

            BOOST_CHECK(areImagesEqual(ct1A.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(ct1C.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1C.Get()));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1C.Index()), ct1A.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1C.Get()));
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct2 { PathWrapper(IMAGE2_PATH) };

            BOOST_CHECK(ct2.Index() == 1);
            BOOST_CHECK(ct2.Options() == ImageOptions());
            BOOST_CHECK(ct2.RefCount() == 1);
            BOOST_CHECK(ct2.Path() == IMAGE2_PATH);
            BOOST_CHECK(ct2 != ct1A);

            BOOST_CHECK(areImagesEqual(ct2.Get(), quickLoadByKey(IMAGE2_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct2.Index()), ct2.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct2.Get()) == false);

            CachedTexture ct3(IMAGE3_PATH_KEY);

            BOOST_CHECK(ct3.Index() == 2);
            BOOST_CHECK(ct3.Options() == ImageOptions());
            BOOST_CHECK(ct3.RefCount() == 1);
            BOOST_CHECK(ct3.Path() == IMAGE3_PATH);
            BOOST_CHECK(ct3 != ct1A);
            BOOST_CHECK(ct3 != ct2);

            BOOST_CHECK(areImagesEqual(ct3.Get(), quickLoadByKey(IMAGE3_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct3.Index()), ct3.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct3.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct2.Get()) == false);

            M_HP_LOG_WRN("ct2=" << &ct2.Get());
            ct2 = ct3;
            M_HP_LOG_WRN("ct2=" << &ct2.Get());

            BOOST_CHECK(ct1A.Index() == 0);

            BOOST_CHECK(ct2.RefCount() == 2);
            BOOST_CHECK(ct3.RefCount() == 2);

            BOOST_CHECK(ct2 == ct3);

            BOOST_CHECK(ct2.Index() == 2);
            BOOST_CHECK(ct2.Index() == ct3.Index());
            BOOST_CHECK(ct2.Options() == ct3.Options());
            BOOST_CHECK(ct2.Options() == ImageOptions());
            BOOST_CHECK(ct2.RefCount() == ct3.RefCount());
            BOOST_CHECK(ct2.RefCount() == 2);
            BOOST_CHECK(ct2.Path() == ct3.Path());
            BOOST_CHECK(ct2.Path() == IMAGE3_PATH);
            BOOST_CHECK(ct2 != ct1A);
            BOOST_CHECK(ct3 != ct1A);

            BOOST_CHECK(areImagesEqual(ct2.Get(), quickLoadByKey(IMAGE3_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct3.Index()), ct2.Get()));
            BOOST_CHECK(areImagesEqual(ct2.Get(), ct3.Get()));
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct2 { PathWrapper(IMAGE2_PATH) };

            BOOST_CHECK(ct2.Index() == 1);
            BOOST_CHECK(ct2.RefCount() == 1);

            CachedTexture ct3(IMAGE3_PATH_KEY);

            BOOST_CHECK(ct2.Index() == 1);
            BOOST_CHECK(ct2.RefCount() == 1);

            BOOST_CHECK(ct3.Index() == 2);
            BOOST_CHECK(ct3.RefCount() == 1);

            ct2 = std::move(ct3);

            BOOST_CHECK(ct2.Index() == 2);
            BOOST_CHECK(ct2.RefCount() == 1);

            BOOST_CHECK(ct2 != ct1A);

            BOOST_CHECK(ct2.Index() == 2);
            BOOST_CHECK(ct2.Options() == ImageOptions());
            BOOST_CHECK(ct2.RefCount() == 1);
            BOOST_CHECK(ct2.Path() == IMAGE3_PATH);

            BOOST_CHECK(areImagesEqual(ct2.Get(), quickLoadByKey(IMAGE3_PATH_KEY)));
            BOOST_CHECK(areImagesEqual(ct2.Get(), ct1A.Get()) == false);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct1B(IMAGE1_PATH_KEY);
            const CachedTexture ct1C(IMAGE1_PATH_KEY);

            BOOST_CHECK(ct1A.Index() == 0);
            BOOST_CHECK(ct1B.Index() == 0);
            BOOST_CHECK(ct1C.Index() == 0);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            auto testFunction1 = [](const CachedTexture & A, const CachedTexture B) {
                BOOST_CHECK(A == B);
                BOOST_CHECK(A.RefCount() == 4);
                BOOST_CHECK(B.RefCount() == 4);
            };

            testFunction1(ct1A, ct1A);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            testFunction1(ct1A, ct1B);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            testFunction1(ct1A, ct1C);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            testFunction1(ct1B, ct1A);
            testFunction1(ct1B, ct1B);
            testFunction1(ct1B, ct1C);

            testFunction1(ct1C, ct1A);
            testFunction1(ct1C, ct1B);
            testFunction1(ct1C, ct1C);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            auto testFunction2A = [](const CachedTexture & A, const CachedTexture B) {
                BOOST_CHECK(A == B);
                BOOST_CHECK(A.RefCount() == 4);
                BOOST_CHECK(B.RefCount() == 4);
            };

            testFunction2A(ct1A, CachedTexture(ct1A));

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            auto testFunction2B = [](const CachedTexture & A, const CachedTexture B) {
                BOOST_CHECK(A == B);
                BOOST_CHECK(A.RefCount() == 5);
                BOOST_CHECK(B.RefCount() == 5);
            };

            testFunction2B(CachedTexture(ct1B), ct1B);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            auto testFunction2C = [](const CachedTexture & A, const CachedTexture B) {
                BOOST_CHECK(A == B);
                BOOST_CHECK(A.RefCount() == 5);
                BOOST_CHECK(B.RefCount() == 5);
            };

            testFunction2C(CachedTexture(ct1A), ct1B);

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            auto testFunction3A = [](const CachedTexture & A, CachedTexture && b) {
                BOOST_CHECK(A == b);
                BOOST_CHECK(A.RefCount() == 4);
                BOOST_CHECK(b.RefCount() == 4);
            };

            testFunction3A(ct1A, CachedTexture(ct1A));

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);

            auto testFunction3B = [](const CachedTexture & A, CachedTexture && b) {
                BOOST_CHECK(A == b);
                BOOST_CHECK(A.RefCount() == 3);
                BOOST_CHECK(b.RefCount() == 3);
            };

            testFunction3B(ct1A, std::move(ct1B));

            BOOST_CHECK(ct1A.RefCount() == 3);
            BOOST_CHECK(ct1B.RefCount() == 3);
            BOOST_CHECK(ct1C.RefCount() == 3);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            const CachedTexture ct1B { PathWrapper(IMAGE1_PATH) };

            BOOST_CHECK(ct1B.Index() == 0);
            BOOST_CHECK(ct1B.Options() == ImageOptions());
            BOOST_CHECK(ct1B.RefCount() == 2);
            BOOST_CHECK(ct1B.Path() == ct1A.Path());
            BOOST_CHECK(ct1B == ct1A);

            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1B.Index()), ct1B.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1B.Get()));
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            const CachedTexture ct1B(PathWrapper(IMAGE1_PATH), ALL_OPTIONS);

            BOOST_CHECK(ct1B.Index() == 1);
            BOOST_CHECK(ct1B.Options() == ALL_OPTIONS);
            BOOST_CHECK(ct1A.Options() != ct1B.Options());
            BOOST_CHECK(ct1B.RefCount() == 1);
            BOOST_CHECK(ct1A.RefCount() == 1);
            BOOST_CHECK(ct1B.Path() == ct1A.Path());
            BOOST_CHECK(ct1B != ct1A);

            BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1B.Index()), ct1B.Get()));
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1B.Get()) == false);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct2(IMAGE2_PATH_KEY, ALL_OPTIONS);
            BOOST_CHECK(ct2.Index() == 1);

            BOOST_CHECK(ct1A.Options() != ct2.Options());
            BOOST_CHECK(ct1A.Path() != ct2.Path());
            BOOST_CHECK(ct1A != ct2);

            BOOST_CHECK(areImagesEqual(ct2.Get(), tc.GetByIndex(ct2.Index())));

            BOOST_CHECK(areImagesEqual(ct2.Get(), quickLoadByKey(IMAGE2_PATH_KEY, ALL_OPTIONS)));

            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct2.Get()) == false);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            CachedTexture ct1B { PathWrapper(IMAGE1_PATH) };

            BOOST_CHECK(ct1A == ct1B);
            BOOST_CHECK(ct1A.RefCount() == 2);
            BOOST_CHECK(ct1B.RefCount() == 2);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            const CachedTexture ct1B(PathWrapper(IMAGE1_PATH), ALL_OPTIONS);
            BOOST_CHECK(ct1A != ct1B);

            BOOST_CHECK(ct1A.Index() != ct1B.Index());
            BOOST_CHECK(ct1A.Options() != ct1B.Options());
            BOOST_CHECK(ct1A.Path() == ct1B.Path());
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        {
            const CachedTexture ct1B(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz));
            BOOST_CHECK(ct1B.Index() == 1);

            const CachedTexture ct1C(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert));
            BOOST_CHECK(ct1C.Index() == 2);

            BOOST_CHECK(ct1B != ct1C);
            BOOST_CHECK(areImagesEqual(ct1B.Get(), ct1C.Get()) == false);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        // create each way with different options and verify the options were applied
        {
            CachedTexture ct1B(IMAGE1_PATH_KEY.c_str(), ImageOptions(ImageOpt::FlipHoriz));
            CachedTexture ct1C(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert));
            CachedTexture ct1D { PathWrapper(IMAGE1_PATH), ImageOptions(ImageOpt::Invert) };
            sf::Texture texture;
            texture.loadFromImage(quickLoadByKey(IMAGE1_PATH_KEY));
            CachedTexture ct1E("FAKE PATH", texture, ALL_OPTIONS);

            BOOST_CHECK(ct1A.Index() == 0);
            BOOST_CHECK(ct1B.Index() == 1);
            BOOST_CHECK(ct1C.Index() == 2);
            BOOST_CHECK(ct1D.Index() == 3);
            BOOST_CHECK(ct1E.Index() == 4);

            BOOST_CHECK(ct1A.RefCount() == 1);
            BOOST_CHECK(ct1B.RefCount() == 1);
            BOOST_CHECK(ct1C.RefCount() == 1);
            BOOST_CHECK(ct1D.RefCount() == 1);
            BOOST_CHECK(ct1E.RefCount() == 1);

            BOOST_CHECK(areImagesEqual(ct1A.Get(), quickLoadByKey(IMAGE1_PATH_KEY)));

            BOOST_CHECK(areImagesEqual(
                ct1B.Get(), quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz))));

            BOOST_CHECK(areImagesEqual(
                ct1C.Get(), quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert))));

            BOOST_CHECK(areImagesEqual(
                ct1D.Get(), quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::Invert))));

            BOOST_CHECK(areImagesEqual(ct1E.Get(), quickLoadByKey(IMAGE1_PATH_KEY, ALL_OPTIONS)));

            BOOST_CHECK(areImagesEqual(ct1A.Get().copyToImage(), quickLoadByKey(IMAGE1_PATH_KEY)));

            BOOST_CHECK(areImagesEqual(
                ct1B.Get().copyToImage(),
                quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipHoriz))));

            BOOST_CHECK(areImagesEqual(
                ct1C.Get().copyToImage(),
                quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::FlipVert))));

            BOOST_CHECK(areImagesEqual(
                ct1D.Get().copyToImage(),
                quickLoadByKey(IMAGE1_PATH_KEY, ImageOptions(ImageOpt::Invert))));

            BOOST_CHECK(areImagesEqual(
                ct1E.Get().copyToImage(), quickLoadByKey(IMAGE1_PATH_KEY, ALL_OPTIONS)));

            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1B.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1C.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1D.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1A.Get(), ct1E.Get()) == false);

            BOOST_CHECK(areImagesEqual(ct1B.Get(), ct1C.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1B.Get(), ct1D.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1B.Get(), ct1E.Get()) == false);

            BOOST_CHECK(areImagesEqual(ct1C.Get(), ct1D.Get()) == false);
            BOOST_CHECK(areImagesEqual(ct1C.Get(), ct1E.Get()) == false);

            BOOST_CHECK(areImagesEqual(ct1D.Get(), ct1E.Get()) == false);
        }

        BOOST_CHECK(ct1A.Index() == 0);
        BOOST_CHECK(ct1A.RefCount() == 1);
        BOOST_CHECK(areImagesEqual(tc.GetByIndex(ct1A.Index()), ct1A.Get()));

        // BOOST_CHECK_THROW(tc.GetByIndex(0), std::exception);
    }

    {
        CachedTextures cs1A(IMAGES1_DIR_KEY);
        BOOST_CHECK((cs1A.Path() == IMAGES1_DIR_PATH));
        BOOST_CHECK((cs1A.Options() == ImageOptions()));
        BOOST_CHECK((cs1A.RefCount() == 1));
        auto INDEXES1 { cs1A.Indexes() };
        BOOST_CHECK((INDEXES1.size() == IMAGES1_COUNT));
        BOOST_CHECK(cs1A.Size() == IMAGES1_COUNT);
        BOOST_CHECK(cs1A == cs1A);
        BOOST_CHECK((cs1A != cs1A) == false);
        for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        {
            BOOST_CHECK(INDEXES1[i] == i);
            BOOST_CHECK(areImagesEqual(cs1A[i], cs1A.At(i)));
        }
        BOOST_CHECK((cs1A.Size() == IMAGES1_COUNT));
        BOOST_CHECK(areImagesEqual(cs1A.Front(), cs1A[0]));
        BOOST_CHECK(areImagesEqual(cs1A.Back(), cs1A[cs1A.Size() - 1]));

        CachedTextures cs1B { PathWrapper(IMAGES1_DIR_PATH) };
        BOOST_CHECK((cs1B.Path() == IMAGES1_DIR_PATH));
        BOOST_CHECK((cs1B.Options() == ImageOptions()));
        BOOST_CHECK((cs1B.RefCount() == 2));
        auto INDEXES2 { cs1B.Indexes() };
        BOOST_CHECK(INDEXES1 == INDEXES2);
        BOOST_CHECK((INDEXES2.size() == IMAGES1_COUNT));
        BOOST_CHECK(cs1B.Size() == IMAGES1_COUNT);
        BOOST_CHECK(cs1B == cs1A);
        BOOST_CHECK((cs1B != cs1A) == false);
        for (std::size_t i(0); i < IMAGES1_COUNT; ++i)
        {
            BOOST_CHECK(INDEXES1[i] == INDEXES2[i]);
            BOOST_CHECK(areImagesEqual(cs1B[i], cs1B.At(i)));
            BOOST_CHECK(areImagesEqual(cs1B[i], cs1A.At(i)));
        }
        BOOST_CHECK((cs1B.Size() == IMAGES1_COUNT));
        BOOST_CHECK(areImagesEqual(cs1B.Front(), cs1B[0]));
        BOOST_CHECK(areImagesEqual(cs1B.Back(), cs1B[cs1B.Size() - 1]));

        CachedTextures cs1C(cs1A);
        BOOST_CHECK((cs1A == cs1C));

        BOOST_CHECK((cs1A.RefCount() == 3));
        BOOST_CHECK((cs1B.RefCount() == 3));
        BOOST_CHECK((cs1C.RefCount() == 3));

        CachedTextures cs1D(std::move(cs1C));
        BOOST_CHECK((cs1A == cs1D));

        BOOST_CHECK((cs1A.RefCount() == 3));
        BOOST_CHECK((cs1B.RefCount() == 3));
        BOOST_CHECK((cs1D.RefCount() == 3));

        CachedTextures cs2 { PathWrapper(IMAGES2_DIR_PATH) };

        BOOST_CHECK(cs2.Size() == IMAGES2_COUNT);
        BOOST_CHECK(cs1D != cs2);

        cs2 = cs1D;
        BOOST_CHECK(cs2 == cs1D);

        BOOST_CHECK((cs1A.RefCount() == 4));
        BOOST_CHECK((cs1B.RefCount() == 4));
        BOOST_CHECK((cs2.RefCount() == 4));

        CachedTextures cs3(PathWrapper(IMAGES2_DIR_PATH), ALL_OPTIONS);
        cs3 = std::move(cs1B);
        BOOST_CHECK(cs1A == cs3);
        BOOST_CHECK(cs2 == cs3);

        BOOST_CHECK((cs1A.RefCount() == 4));
        BOOST_CHECK((cs2.RefCount() == 4));
    }
}
