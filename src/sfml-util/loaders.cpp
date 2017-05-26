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


namespace sfml_util
{

    void LoadFont(sf::Font & font, const std::string & PATH_STR )
    {
        namespace bfs = boost::filesystem;

        const bfs::path   PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR(PATH_OBJ.string());

        //check if the path even exists
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(PATH_OBJ), "LoadFont(\"" << PATH_OBJ_STR << "\") failed because that file does not exist!");

        //ignore subdirectories, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_regular_file(PATH_OBJ), "LoadFont(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file!");

        M_ASSERT_OR_LOGANDTHROW_SS(font.loadFromFile(PATH_OBJ_STR.c_str()), "During LoadFont(), sf::Font::loadFromFile(\"" << PATH_OBJ_STR << "\") failed!  Check console output for information.");
    }


    void OpenMusicSPtr(MusicSPtr_t & musicSPtr, const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path   PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR(PATH_OBJ.string());

        //check if the path even exists
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(PATH_OBJ), "LoadMusicSPtr(\"" << PATH_OBJ_STR << "\") failed because that file does not exist.");

        //ignore directories, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_regular_file(PATH_OBJ), "LoadMusic(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file.");

        musicSPtr.reset( new sf::Music );

        //verify open success
        M_ASSERT_OR_LOGANDTHROW_SS(musicSPtr->openFromFile(PATH_OBJ_STR.c_str()), "LoadMusic(\"" << PATH_OBJ_STR << "\"), sf::Music::OpenFromFile() returned false.  See console output for more information.");
    }


    void LoadSoundBufferSPtr(SoundBufferSPtr_t & soundBufferSPtr, const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path   PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR(PATH_OBJ.string());

        //check if the path even exists
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(PATH_OBJ), "LoadSoundBufferSPtr(\"" << PATH_OBJ_STR << "\") failed because that file does not exist.");

        //ignore directories, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_regular_file(PATH_OBJ), "LoadSoundBufferSPtr(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file.");

        soundBufferSPtr.reset( new sf::SoundBuffer );

        //verify open success
        M_ASSERT_OR_LOGANDTHROW_SS(soundBufferSPtr->loadFromFile(PATH_OBJ_STR.c_str()), "LoadSoundBuffer(\"" << PATH_OBJ_STR << "\"), sf::SoundBuffer::loadFromFile() returned false.  See console output for more information.");
    }

}
