#ifndef SFMLUTIL_LOADERS_INCLUDED
#define SFMLUTIL_LOADERS_INCLUDED
//
// loaders.hpp
//  Functions that help load assests and media.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-audio.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>


namespace sfml_util
{

    //ImageOrTexture_t must be sf::Image or sf::Texture
    template<typename ImageOrTexture_t>
    void LoadImageOrTexture(ImageOrTexture_t & imageOrTexture,
                            const std::string  PATH_STR)
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
                                const std::string       PATH_STR)
    {
        if (nullptr == imageOrTextureSPtr.get())
            imageOrTextureSPtr.reset( new typename ImageOrTextureSPtr_t::element_type);

        LoadImageOrTexture( * imageOrTextureSPtr, PATH_STR);
    }


    //Returns the number loaded into the std::vector<> from DIR_STR.
    template<typename ImageOrTextureSPtr_t>
    std::size_t LoadAllImageOrTextureInDir(std::vector< ImageOrTextureSPtr_t > & imageOrTextureSVec,
                                      const std::string &                   DIR_STR = ".")
    {
        namespace bfs = boost::filesystem;

        const bfs::path   DIR_OBJ    (bfs::system_complete(bfs::path(DIR_STR)));
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(DIR_OBJ), "LoadAllImageOrTextureInDir(\"" << DIR_OBJ_STR << "\") failed because that path does not exist!");

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_directory(DIR_OBJ), "LoadAllImageOrTextureInDir(\"" << DIR_OBJ_STR << "\") failed because that is not a directory.");

        const std::size_t ORIG_SIZE(imageOrTextureSVec.size());

        //attempt to load all files, ignore extensions
        bfs::directory_iterator endItr;
        for (bfs::directory_iterator dirItr(DIR_OBJ); endItr != dirItr; ++dirItr)
        {
            //ignore subdirectories, etc.
            if (bfs::is_regular_file(dirItr->status()))
            {
                ImageOrTextureSPtr_t tempImageOrTextureSPtr(new typename ImageOrTextureSPtr_t::element_type);
                LoadImageOrTexture(*tempImageOrTextureSPtr, dirItr->path().string());
                imageOrTextureSVec.push_back(tempImageOrTextureSPtr);
            }
        }

        const std::size_t TOTAL_ADDED(imageOrTextureSVec.size() - ORIG_SIZE);
        //std::cout << "LoadAllImageOrTextureInDirectory(\"" << DIR_OBJ_STR << "\") found " << TOTAL_ADDED << "." << std::endl;
        return TOTAL_ADDED;
    }


    void LoadFont(sf::Font & font, const std::string PATH_STR);


    void LoadFontSPtr(FontSPtr_t & fontSPtr, const std::string PATH_STR);


    //Returns the number loaded into the std::vector<> from DIR_STR.
    std::size_t LoadAllFontsInDir(std::vector< FontSPtr_t > & fontSVec,
                             const std::string &         DIR_STR = ".");


    void OpenMusicSPtr(MusicSPtr_t & musicSPtr, const std::string & PATH_STR);


    void LoadSoundBufferSPtr(SoundBufferSPtr_t & soundBufferSPtr, const std::string & PATH_STR);

}
#endif //SFMLUTIL_LOADERS_INCLUDED
