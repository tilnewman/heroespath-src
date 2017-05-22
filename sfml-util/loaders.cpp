//
// loaders.cpp
//
#include "loaders.hpp"
#include "sfml-util/sfml-util.hpp"


namespace sfml_util
{
    void LoadFont(sf::Font &          font,
                  const std::string & PATH_STR )
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


    //Returns true if loaded successfully.
    void LoadFontSPtr(FontSPtr_t &        fontSPtr,
                      const std::string & PATH_STR )
    {
        if (nullptr == fontSPtr.get())
            fontSPtr.reset(new typename FontSPtr_t::element_type);

        LoadFont( * fontSPtr, PATH_STR);
    }


    std::size_t LoadAllFontsInDir(std::vector< FontSPtr_t > & fontSVec,
                             const std::string &         DIR_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path   DIR_OBJ(bfs::system_complete(bfs::path(DIR_STR)));
        const std::string DIR_OBJ_STR(DIR_OBJ.string());

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(DIR_OBJ), "LoadAllFontsInDirectory(\"" << DIR_OBJ_STR << "\") failed because that path does not exist!");

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_directory(DIR_OBJ), "LoadAllFontsInDirectory(\"" << DIR_OBJ_STR << "\") failed because that is not a directory.");

        const std::size_t ORIG_SIZE(fontSVec.size());

        //attempt to load all files, ignore extensions
        bfs::directory_iterator endItr;
        for (bfs::directory_iterator dirItr(DIR_OBJ); endItr != dirItr; ++dirItr)
        {
            //ignore subdirectories, etc.
            if (bfs::is_regular_file(dirItr->status()))
            {
                FontSPtr_t tempFontSPtr(new typename FontSPtr_t::element_type);
                LoadFont( * tempFontSPtr, dirItr->path().string());
                fontSVec.push_back(tempFontSPtr);
            }
        }

        const std::size_t TOTAL_ADDED(fontSVec.size() - ORIG_SIZE);
        //std::cout << "LoadAllFontsInDirectory(\"" << DIR_OBJ_STR << "\") found " << TOTAL_ADDED << "." << std::endl;
        return TOTAL_ADDED;
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