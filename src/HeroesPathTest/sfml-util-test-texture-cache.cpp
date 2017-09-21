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

using namespace sfml_util;


BOOST_AUTO_TEST_CASE(TextureCache_InvalidFunctionCallsAfterDefaultConstruction)
{
    TextureCache tc;
    tc.Acquire();

    auto const INVALID_PATH_OR_KEY{ "xyz" };

    BOOST_CHECK_THROW(tc.AddByKey(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.AddByPath(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.AddAllInDirectoryByKey(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.AddAllInDirectoryByPath(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.RemoveByKey(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.RemoveByPath(INVALID_PATH_OR_KEY), std::exception);
    BOOST_CHECK_THROW(tc.GetPtrByIndex(0), std::exception);
    BOOST_CHECK_THROW(tc.GetByIndex(0), std::exception);

    BOOST_CHECK_THROW(tc.RemoveByIndex(0), std::exception);
    BOOST_CHECK_THROW(tc.RemoveByIndexVec({ 0 }), std::exception);

    //empty vectors result in no operations, so no exception is raised
    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndexVec({}));

    //RemoveAll() should never throw
    BOOST_REQUIRE_NO_THROW(tc.RemoveAll());

    tc.Release();
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadAndRemoveSingle)
{
    TextureCache tc;
    tc.Acquire();

    auto const KEY{ "media-images-gui-elements" };

    //add and remove with RemoveByKey()
    auto const ID0{ tc.AddByKey(KEY) };
    BOOST_CHECK(ID0 == 0);
    BOOST_CHECK(tc.GetPtrByIndex(ID0) != nullptr);
    BOOST_CHECK(tc.GetByIndex(ID0).getSize().x > 0.0f);
    BOOST_CHECK(tc.GetByIndex(ID0).isSmooth() == true);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByKey(KEY));
    BOOST_CHECK_THROW(tc.GetPtrByIndex(ID0), std::exception);
    BOOST_CHECK_THROW(tc.GetByIndex(ID0), std::exception);

    //add and remove with RemoveByIndex()
    auto const ID1{ tc.AddByKey(KEY, false) };
    BOOST_CHECK_MESSAGE(ID1 == 0, "the first/original slot should be reused");
    BOOST_CHECK(tc.GetPtrByIndex(ID1) != nullptr);
    BOOST_CHECK(tc.GetByIndex(ID1).getSize().x > 0.0f);
    BOOST_CHECK(tc.GetByIndex(ID1).isSmooth() == false);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndex(ID1));
    BOOST_CHECK_THROW(tc.GetPtrByIndex(ID1), std::exception);
    BOOST_CHECK_THROW(tc.GetByIndex(ID1), std::exception);

    //add and remove with RemoveByPath()
    auto const ID2{ tc.AddByKey(KEY) };
    auto const PATH{ game::GameDataFile::Instance()->GetMediaPath(KEY) };
    BOOST_CHECK_MESSAGE(ID2 == 0, "the first/original slot should be reused");
    BOOST_CHECK(tc.GetPtrByIndex(ID2) != nullptr);
    BOOST_CHECK(tc.GetByIndex(ID2).getSize().x > 0.0f);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByPath(PATH));
    BOOST_CHECK_THROW(tc.GetPtrByIndex(ID2), std::exception);
    BOOST_CHECK_THROW(tc.GetByIndex(ID2), std::exception);

    //add and remove with RemoveByIndexVec()
    auto const ID3{ tc.AddByKey(KEY) };
    BOOST_CHECK_MESSAGE(ID3 == 0, "the first/original slot should be reused");
    BOOST_CHECK(tc.GetPtrByIndex(ID3) != nullptr);
    BOOST_CHECK(tc.GetByIndex(ID3).getSize().x > 0.0f);
    BOOST_REQUIRE_NO_THROW(tc.RemoveByIndexVec({ ID3 }));
    BOOST_CHECK_THROW(tc.GetPtrByIndex(ID3), std::exception);
    BOOST_CHECK_THROW(tc.GetByIndex(ID3), std::exception);

    //add and remove with RemoveAll()
    auto const ID4{ tc.AddByKey(KEY) };
    BOOST_CHECK_MESSAGE(ID4 == 0, "the first/original slot should be reused");
    BOOST_CHECK(tc.GetPtrByIndex(ID4) != nullptr);
    BOOST_CHECK(tc.GetByIndex(ID4).getSize().x > 0.0f);
    BOOST_REQUIRE_NO_THROW(tc.RemoveAll());
    BOOST_CHECK_THROW(tc.GetPtrByIndex(ID4), std::exception);
    BOOST_CHECK_THROW(tc.GetByIndex(ID4), std::exception);

    tc.Release();
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveIndividually)
{
    TextureCache tc;
    tc.Acquire();

    auto const KEY{ "media-anim-images-dir-inferno" };

    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at zero and didn't skip any
    for (std::size_t i(0); i < IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify and remove one-at-a-time
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetPtrByIndex(ID) != nullptr);
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
        BOOST_REQUIRE_NO_THROW(tc.RemoveByIndex(ID));
    }

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK_THROW(tc.GetByIndex(ID), std::exception);
    }

    tc.Release();
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveAll)
{
    TextureCache tc;
    tc.Acquire();

    auto const KEY{ "media-anim-images-dir-inferno" };

    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at zero and didn't skip any
    for (std::size_t i(0); i < IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify each
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetPtrByIndex(ID) != nullptr);
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
    }

    tc.RemoveAll();

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK_THROW(tc.GetByIndex(ID), std::exception);
    }

    tc.Release();
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveByIndexVec)
{
    TextureCache tc;
    tc.Acquire();

    auto const KEY{ "media-anim-images-dir-inferno" };

    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at zero and didn't skip any
    for (std::size_t i(0); i < IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify each
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetPtrByIndex(ID) != nullptr);
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
    }

    tc.RemoveByIndexVec(IDS);

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK_THROW(tc.GetByIndex(ID), std::exception);
    }

    tc.Release();
}


BOOST_AUTO_TEST_CASE(TextureCache_LoadMultipleAndRemoveByKey)
{
    TextureCache tc;
    tc.Acquire();

    auto const KEY{ "media-anim-images-dir-inferno" };

    auto const IDS{ tc.AddAllInDirectoryByKey(KEY) };
    auto const IDS_SIZE{ IDS.size() };
    BOOST_CHECK(IDS_SIZE > 0);

    //verify the IDs supplied started at zero and didn't skip any
    for (std::size_t i(0); i < IDS_SIZE; ++i)
    {
        BOOST_CHECK(std::find(begin(IDS), end(IDS), i) != end(IDS));
    }

    //verify each
    for (auto const ID : IDS)
    {
        BOOST_CHECK(tc.GetByIndex(ID).isSmooth());
        BOOST_CHECK(tc.GetPtrByIndex(ID) != nullptr);
        BOOST_CHECK(tc.GetByIndex(ID).getSize().x > 0.0f);
    }

    tc.RemoveByKey(KEY);

    //verify they are all removed
    for (auto const ID : IDS)
    {
        BOOST_CHECK_THROW(tc.GetByIndex(ID), std::exception);
    }

    tc.Release();
}
