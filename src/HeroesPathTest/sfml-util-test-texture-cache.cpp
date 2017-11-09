// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE "HeroesPathTestModule_sfml-util_texture_cache_test"

#include <boost/test/unit_test.hpp>

#include "sfml-util/texture-cache.hpp"
#include "game/game-data-file.hpp"

#include <cstdlib>
#include <exception>


using namespace heroespath::sfml_util;


BOOST_AUTO_TEST_CASE(TextureCache_InvalidFunctionCallsAfterDefaultConstruction)
{
    TextureCache tc;
    auto const INVALID_PATH_OR_KEY{ "xyz" };

    BOOST_CHECK_THROW(tc.AddByKey(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.AddByPath(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.AddAllInDirectoryByKey(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.AddAllInDirectoryByPath(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.RemoveByKey(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.RemoveByPath(INVALID_PATH_OR_KEY), std::exception);

    BOOST_REQUIRE_NO_THROW(tc.GetByIndex(0));

    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndex(0));

    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndexVec({ 0 }));

    //empty vectors result in no operations, so no exception is raised
    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndexVec({}));

    //RemoveAll() should never throw
    BOOST_REQUIRE_NO_THROW(tc.RemoveAll());
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadAndRemoveSingle_RemoveBykey_and_RemoveByIndex_and_CheckSlotReuse)
{
    TextureCache tc;
    auto const KEY{ "media-images-gui-elements" };
    auto const ID0{ tc.AddByKey(KEY) };
    BOOST_CHECK(ID0 == 1);
    BOOST_CHECK(tc.GetByIndex(ID0).getSize().x > 0.0f);
    BOOST_CHECK(tc.GetByIndex(ID0).isSmooth() == true);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByKey(KEY));
    BOOST_CHECK(tc.GetByIndex(ID0).getSize().x < 1.0f);
    auto const ID1{ tc.AddByKey(KEY, false) };
    BOOST_CHECK_MESSAGE(ID1 == 1, "the first/original slot should be reused");
    BOOST_CHECK(tc.GetByIndex(ID1).getSize().x > 0.0f);
    BOOST_CHECK(tc.GetByIndex(ID1).isSmooth() == false);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndex(ID1));
    BOOST_CHECK(tc.GetByIndex(ID1).getSize().x < 1.0f);
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadAndRemoveSingle_RemoveByPath)
{
    TextureCache tc;
    auto const KEY{ "media-images-gui-elements" };
    auto const ID2{ tc.AddByKey(KEY) };
    auto const PATH{ heroespath::game::GameDataFile::Instance()->GetMediaPath(KEY) };
    BOOST_CHECK(ID2 == 1);
    BOOST_CHECK(tc.GetByIndex(ID2).getSize().x > 0.0f);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByPath(PATH));
    BOOST_CHECK(tc.GetByIndex(ID2).getSize().x < 1.0f);
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadAndRemoveSingle_RemoveByIndexVec)
{
    TextureCache tc;
    auto const KEY{ "media-images-gui-elements" };
    auto const ID3{ tc.AddByKey(KEY) };
    BOOST_CHECK(ID3 == 1);
    BOOST_CHECK(tc.GetByIndex(ID3).getSize().x > 0.0f);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndexVec({ ID3 }));
    BOOST_CHECK(tc.GetByIndex(ID3).getSize().x < 1.0f);
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadAndRemoveSingle_RemoveAll)
{
    TextureCache tc;
    auto const KEY{ "media-images-gui-elements" };
    auto const ID4{ tc.AddByKey(KEY) };
    BOOST_CHECK(ID4 == 1);
    BOOST_CHECK(tc.GetByIndex(ID4).getSize().x > 0.0f);
    BOOST_REQUIRE_NO_THROW(tc.RemoveAll());
    BOOST_CHECK(tc.GetByIndex(ID4).getSize().x < 1.0f);
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveIndividually)
{
    TextureCache tc;
    auto const KEY{ "media-anim-images-dir-inferno" };
    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at one and didn't skip any
    for (std::size_t i(1); i <= IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify and remove one-at-a-time
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
        BOOST_REQUIRE_NO_THROW(tc.RemoveByIndex(ID));
    }

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x < 1.0f);
    }
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveAll)
{
    TextureCache tc;
    auto const KEY{ "media-anim-images-dir-inferno" };
    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at one and didn't skip any
    for (std::size_t i(1); i <= IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify each
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
    }

    tc.RemoveAll();

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x < 1.0f);
    }
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveByIndexVec)
{
    TextureCache tc;
    auto const KEY{ "media-anim-images-dir-inferno" };
    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at one and didn't skip any
    for (std::size_t i(1); i <= IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify each
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
    }

    tc.RemoveByIndexVec(IDS);

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x < 1.0f);
    }
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveByKey)
{
    TextureCache tc;
    auto const KEY{ "media-anim-images-dir-inferno" };
    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at one and didn't skip any
    for (std::size_t i(1); i <= IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify each
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
    }

    tc.RemoveByKey(KEY);

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x < 1.0f);
    }
}
