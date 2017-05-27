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
#ifndef SFMLUTIL_LOADERS_INCLUDED
#define SFMLUTIL_LOADERS_INCLUDED
//
// loaders.hpp
//  Functions that help load assests and media.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-audio.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>


namespace sfml_util
{

    //ImageOrTexture_t must be sf::Image or sf::Texture
    template<typename ImageOrTexture_t>
    void LoadImageOrTexture(ImageOrTexture_t &  imageOrTexture,
                            const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path   PATH_OBJ(bfs::system_complete(bfs::path(PATH_STR)));
        const std::string PATH_OBJ_STR( PATH_OBJ.string() );

        //check if the path even exists
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(PATH_OBJ), "LoadImageOrTexture(\"" << PATH_OBJ_STR << "\") failed because that file does not exist!");

        //ignore subdirectories, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_regular_file(PATH_OBJ), "LoadImageOrTexture(\"" << PATH_OBJ_STR << "\") failed because that is not a regular file!");

        //verify the load()
        M_ASSERT_OR_LOGANDTHROW_SS(imageOrTexture.loadFromFile(PATH_OBJ_STR.c_str()), "During LoadImageOrTexture(), sf::(Image or Texture).loadFromFile(\"" << PATH_OBJ_STR << "\") failed!  Check console output for information.");

        //std::cout << "LoadImageOrTexture \"" << PATH_OBJ_STR << "\"" << std::endl;
    }


    //ImageOrTextureSPtr_t must be ImageSPtr_t or TextureSPtr_t.
    template<typename ImageOrTextureSPtr_t>
    void LoadImageOrTextureSPtr(ImageOrTextureSPtr_t &  imageOrTextureSPtr,
                                const std::string &     PATH_STR)
    {
        if (nullptr == imageOrTextureSPtr.get())
            imageOrTextureSPtr.reset( new typename ImageOrTextureSPtr_t::element_type);

        LoadImageOrTexture( * imageOrTextureSPtr, PATH_STR);
    }


    //Returns the number loaded into the std::vector<> from DIR_STR.
    template<typename ImageOrTexture_t>
    std::size_t LoadAllImageOrTextureInDir(std::vector< ImageOrTexture_t > &
        imageOrTextureVec,
        const std::string & DIR_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path DIR_OBJ(bfs::system_complete(bfs::path(DIR_STR)));
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(DIR_OBJ), "LoadAllImageOrTextureInDir(\""
            << DIR_OBJ_STR << "\") failed because that path does not exist!");

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_directory(DIR_OBJ), "LoadAllImageOrTextureInDir(\""
            << DIR_OBJ_STR << "\") failed because that is not a directory.");

        const std::size_t ORIG_SIZE(imageOrTextureVec.size());

        bfs::directory_iterator endItr;
        for (bfs::directory_iterator dirItr(DIR_OBJ); endItr != dirItr; ++dirItr)
        {
            auto const DIR_PATH_STR{ dirItr->path().string() };

            if (bfs::is_regular_file(dirItr->status()) == false)
            {
                continue;
            }

            const std::vector<std::string> INVALID_TEXT_VEC = { ".txt",
                                                                ".DS_Store",
                                                                "sample.gif" };

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

            ImageOrTexture_t tempImageOrTexture;
            LoadImageOrTexture(tempImageOrTexture, DIR_PATH_STR);
            imageOrTextureVec.push_back(tempImageOrTexture);
        }

        const std::size_t TOTAL_ADDED(imageOrTextureVec.size() - ORIG_SIZE);
        return TOTAL_ADDED;
    }


    void LoadFont(sf::Font & font, const std::string & PATH_STR);


    void OpenMusicSPtr(MusicSPtr_t & musicSPtr, const std::string & PATH_STR);


    void LoadSoundBufferSPtr(SoundBufferSPtr_t & soundBufferSPtr, const std::string & PATH_STR);

}
#endif //SFMLUTIL_LOADERS_INCLUDED
