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
//
// loaders.cpp
//
#include "loaders.hpp"

#include "sfml-util/sfml-util.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <boost/filesystem.hpp>

namespace heroespath
{
namespace sfml_util
{

    void LoadTexture(sf::Texture & texture, const std::string & PATH_STR, const bool WILL_SMOOTH)
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR(PATH_OBJ.string());

        // check if the path exists
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(PATH_OBJ),
            "LoadTexture(\"" << PATH_OBJ_STR << "\") failed because that file does not exist.");

        // ignore subdirectories, irregulare files, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_regular_file(PATH_OBJ),
            "LoadTexture(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file.");

        // verify the load()
        M_ASSERT_OR_LOGANDTHROW_SS(
            texture.loadFromFile(PATH_OBJ_STR.c_str()),
            "During LoadTexture(), sf::(Image or Texture).loadFromFile(\""
                << PATH_OBJ_STR << "\") failed.  Check console output for information.");

        if (WILL_SMOOTH)
        {
            texture.setSmooth(true);
        }
    }

    // Returns the number loaded into textureVec
    std::size_t LoadAllTexturesInDir(
        std::vector<sf::Texture> & textureVec, const std::string & DIR_STR, const bool WILL_SMOOTH)
    {
        namespace bfs = boost::filesystem;

        const bfs::path DIR_OBJ(bfs::system_complete(bfs::path(DIR_STR)));
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(DIR_OBJ),
            "LoadAllTexturesInDir(\"" << DIR_OBJ_STR
                                      << "\") failed because that path does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_directory(DIR_OBJ),
            "LoadAllTexturesInDir(\"" << DIR_OBJ_STR
                                      << "\") failed because that is not a directory.");

        const std::size_t ORIG_SIZE(textureVec.size());

        bfs::directory_iterator endItr;
        for (bfs::directory_iterator dirItr(DIR_OBJ); endItr != dirItr; ++dirItr)
        {
            auto const DIR_PATH_STR{ dirItr->path().string() };

            if (bfs::is_regular_file(dirItr->status()) == false)
            {
                continue;
            }

            const std::vector<std::string> INVALID_TEXT_VEC = { ".txt", ".DS_Store", "sample.gif" };

            auto didFindInvalid{ false };
            for (auto const & NEXT_INVALID_STRING : INVALID_TEXT_VEC)
            {
                if (boost::algorithm::icontains(DIR_PATH_STR, NEXT_INVALID_STRING))
                {
                    didFindInvalid = true;
                    break;
                }
            }

            if (didFindInvalid)
            {
                continue;
            }

            sf::Texture t;
            LoadTexture(t, DIR_PATH_STR, WILL_SMOOTH);
            textureVec.emplace_back(t);
        }

        const std::size_t TOTAL_ADDED(textureVec.size() - ORIG_SIZE);
        return TOTAL_ADDED;
    }

    void LoadFont(sf::Font & font, const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR(PATH_OBJ.string());

        // check if the path even exists
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(PATH_OBJ),
            "LoadFont(\"" << PATH_OBJ_STR << "\") failed because that file does not exist!");

        // ignore subdirectories, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_regular_file(PATH_OBJ),
            "LoadFont(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file!");

        M_ASSERT_OR_LOGANDTHROW_SS(
            font.loadFromFile(PATH_OBJ_STR.c_str()),
            "During LoadFont(), sf::Font::loadFromFile(\""
                << PATH_OBJ_STR << "\") failed!  "
                << "Check console output for information.");
    }

    MusicUPtr_t LoadMusic(const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR(PATH_OBJ.string());

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(PATH_OBJ),
            "LoadMusicSPtr(\"" << PATH_OBJ_STR << "\") failed because that file does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_regular_file(PATH_OBJ),
            "LoadMusic(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file.");

        auto musicUPtr{ std::make_unique<sf::Music>() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            musicUPtr->openFromFile(PATH_OBJ_STR.c_str()),
            "LoadMusic(\"" << PATH_OBJ_STR << "\"), sf::Music::OpenFromFile() returned false.  "
                           << "See console output for more information.");

        return musicUPtr;
    }
} // namespace sfml_util
} // namespace heroespath
