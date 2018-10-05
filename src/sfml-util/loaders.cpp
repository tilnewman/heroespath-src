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
#include "misc/filesystem.hpp"

namespace heroespath
{
namespace sfml_util
{

    void Loaders::Texture(
        sf::Texture & texture, const std::string & PATH_STR_ORIG, const bool WILL_SMOOTH)
    {
        auto const PATH_STR_COMPLETE { misc::filesystem::CleanPath(PATH_STR_ORIG) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsFile(PATH_STR_COMPLETE)),
            "sfml_util::Loaders::Texture(\""
                << PATH_STR_COMPLETE
                << "\") failed because that file either does not exist or is not a regular file.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            texture.loadFromFile(PATH_STR_COMPLETE),
            "sfml_util::Loaders::Texture(), sf::(Image or Texture).loadFromFile(\""
                << PATH_STR_COMPLETE << "\") failed.  Check console output for information.");

        if (WILL_SMOOTH)
        {
            texture.setSmooth(true);
        }
    }

    std::size_t Loaders::AllTexturesInDir(
        std::vector<sf::Texture> & textureVec,
        const std::string & DIR_PATH_STR_ORIG,
        const bool WILL_SMOOTH)
    {
        auto const DIR_PATH_STR_COMPLETE { misc::filesystem::CleanPath(DIR_PATH_STR_ORIG) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsDirectory(DIR_PATH_STR_COMPLETE)),
            "sfml_util::Loaders::AllTexturesInDir(\""
                << DIR_PATH_STR_COMPLETE
                << "\") failed because either that path is not a directory or it does not exist.");

        auto const FILE_PATH_STRINGS { misc::filesystem::FindFiles(
            false,
            DIR_PATH_STR_COMPLETE,
            "",
            "",
            misc::filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_) };

        auto const ORIG_SIZE { textureVec.size() };

        for (auto const & PATH_STR : FILE_PATH_STRINGS)
        {
            textureVec.emplace_back(sf::Texture());
            Texture(textureVec.back(), PATH_STR, WILL_SMOOTH);
        }

        return textureVec.size() - ORIG_SIZE;
    }

    void Loaders::Font(sf::Font & font, const std::string & PATH_STR_ORIG)
    {
        auto const PATH_STR_COMPLETE { misc::filesystem::CleanPath(PATH_STR_ORIG) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsFile(PATH_STR_COMPLETE)),
            "sfml_util::Loaders::Font(\""
                << PATH_STR_COMPLETE
                << "\") failed because that file either does not exist or is not a regular file.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (font.loadFromFile(PATH_STR_COMPLETE)),
            "sfml_util::Loaders::Font(), sf::Font::loadFromFile(\""
                << PATH_STR_COMPLETE << "\") failed.  "
                << "Check console output for information.");
    }

    MusicUPtr_t Loaders::Music(const std::string & PATH_STR_ORIG)
    {
        auto const PATH_STR_COMPLETE { misc::filesystem::CleanPath(PATH_STR_ORIG) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsFile(PATH_STR_COMPLETE)),
            "sfml_util::Loaders::Music(\""
                << PATH_STR_COMPLETE
                << "\") failed because that file either does not exist or is not a regular file.");

        auto musicUPtr { std::make_unique<sf::Music>() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (musicUPtr->openFromFile(PATH_STR_COMPLETE)),
            "sfml_util::Loaders::Music(\"" << PATH_STR_COMPLETE
                                           << "\"), sf::Music::OpenFromFile() returned false.  "
                                           << "See console output for more information.");

        return musicUPtr;
    }

} // namespace sfml_util
} // namespace heroespath
