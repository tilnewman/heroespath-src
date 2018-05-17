// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// loaders.cpp
//
#include "loaders.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "sfml-util/sfml-util.hpp"

namespace heroespath
{
namespace sfml_util
{

    const std::vector<std::string> Loaders::INVALID_TEXT_VEC_{ ".txt", ".DS_Store", "sample.gif" };

    void Loaders::Texture(
        sf::Texture & texture, const std::string & PATH_STR, const bool WILL_SMOOTH)
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH{ PATH_STR };

        // check if the path exists
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(PATH),
            "sfml_util::Loaders::Texture(\"" << MakePathPretty(PATH)
                                             << "\") failed because that file does not exist.");

        // ignore subdirectories, irregular files, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_regular_file(PATH),
            "sfml_util::Loaders::Texture(\"" << MakePathPretty(PATH)
                                             << "\") failed because that is not a regular file.");

        // verify the load()
        M_ASSERT_OR_LOGANDTHROW_SS(
            texture.loadFromFile(PATH.string()),
            "sfml_util::Loaders::Texture(), sf::(Image or Texture).loadFromFile(\""
                << MakePathPretty(PATH) << "\") failed.  Check console output for information.");

        if (WILL_SMOOTH)
        {
            texture.setSmooth(true);
        }
    }

    // Returns the number loaded into textureVec
    std::size_t Loaders::AllTexturesInDir(
        std::vector<sf::Texture> & textureVec, const std::string & DIR_STR, const bool WILL_SMOOTH)
    {
        namespace bfs = boost::filesystem;

        const bfs::path DIR_PATH{ DIR_STR };

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(DIR_PATH),
            "sfml_util::Loaders::AllTexturesInDir(\""
                << MakePathPretty(DIR_PATH) << "\") failed because that path does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_directory(DIR_PATH),
            "sfml_util::Loaders::AllTexturesInDir(\""
                << MakePathPretty(DIR_PATH) << "\") failed because that is not a directory.");

        auto const ORIG_SIZE{ textureVec.size() };

        for (auto const & PATH : DIR_PATH)
        {
            if (bfs::is_regular_file(PATH) == false)
            {
                continue;
            }

            auto didFindInvalid{ false };
            for (auto const & NEXT_INVALID_STRING : INVALID_TEXT_VEC_)
            {
                if (boost::algorithm::icontains(PATH.leaf().string(), NEXT_INVALID_STRING))
                {
                    didFindInvalid = true;
                    break;
                }
            }

            if (didFindInvalid)
            {
                continue;
            }

            textureVec.emplace_back(sf::Texture());
            Texture(textureVec.back(), PATH.string(), WILL_SMOOTH);
        }

        const std::size_t TOTAL_ADDED(textureVec.size() - ORIG_SIZE);
        return TOTAL_ADDED;
    }

    void Loaders::Font(sf::Font & font, const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH{ PATH_STR };

        // check if the path even exists
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(PATH),
            "sfml_util::Loaders::Font(\"" << MakePathPretty(PATH)
                                          << "\") failed because that file does not exist.");

        // ignore subdirectories, etc.
        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_regular_file(PATH),
            "sfml_util::Loaders::Font(\"" << MakePathPretty(PATH)
                                          << "\") failed because that is not a regular file.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            font.loadFromFile(PATH.string()),
            "sfml_util::Loaders::Font(), sf::Font::loadFromFile(\""
                << MakePathPretty(PATH) << "\") failed.  "
                << "Check console output for information.");
    }

    MusicUPtr_t Loaders::Music(const std::string & PATH_STR)
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH{ PATH_STR };

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(PATH),
            "sfml_util::Loaders::Music(\"" << MakePathPretty(PATH)
                                           << "\") failed because that file does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_regular_file(PATH),
            "sfml_util::Loaders::Music(\"" << MakePathPretty(PATH)
                                           << "\") failed because that is not a regular file.");

        auto musicUPtr{ std::make_unique<sf::Music>() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            musicUPtr->openFromFile(PATH.string()),
            "sfml_util::Loaders::Music(\"" << MakePathPretty(PATH)
                                           << "\"), sf::Music::OpenFromFile() returned false.  "
                                           << "See console output for more information.");

        return musicUPtr;
    }

    const std::string Loaders::MakePathPretty(const boost::filesystem::path & PATH)
    {
        namespace bfs = boost::filesystem;
        bfs::path prettyPath{ bfs::system_complete(PATH) };
        prettyPath.normalize();
        return prettyPath.string();
    }

} // namespace sfml_util
} // namespace heroespath
