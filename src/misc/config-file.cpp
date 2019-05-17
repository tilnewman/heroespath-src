// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// config-file.cpp
//
#include "config-file.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/platform.hpp"
#include "misc/strings.hpp"
#include "sfutil/vector-and-rect.hpp"

namespace heroespath
{
namespace misc
{

    const std::string ConfigFile::MEDIA_BASE_PATH_KEY_FOR_WINDOWS_ { "system-media-dir-win" };
    const std::string ConfigFile::MEDIA_BASE_PATH_KEY_FOR_NON_WINDOWS_ { "system-media-dir-linux" };

    std::unique_ptr<ConfigFile> ConfigFile::instanceUPtr_;

    ConfigFile::ConfigFile()
        : KeyValueFile()
        , mediaBasePathKey_()
        , mediaBasePath_()
    {
        if constexpr (platform_is_any_windows)
        {
            mediaBasePathKey_ = MEDIA_BASE_PATH_KEY_FOR_WINDOWS_;
        }
        else
        {
            mediaBasePathKey_ = MEDIA_BASE_PATH_KEY_FOR_NON_WINDOWS_;
        }

        M_HP_LOG_DBG(
            "Subsystem Construction: ConfigFile (mediaBasePathKey_=\"" << mediaBasePathKey_
                                                                       << "\"");
    }

    misc::NotNull<ConfigFile *> misc::ConfigFile::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: ConfigFile");
            Acquire();
        }

        return misc::NotNull<ConfigFile *>(instanceUPtr_.get());
    }

    void ConfigFile::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ConfigFile>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: ConfigFile");
        }
    }

    void ConfigFile::Release() { instanceUPtr_.reset(); }

    void ConfigFile::Initialize()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "ConfigFile::Initialize() found instanceUPtr that was null.");

        const auto LOAD_RESULT { instanceUPtr_->ClearAndLoad() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (LOAD_RESULT && (instanceUPtr_->Empty() == false)),
            "(\"" << Instance()->Path()
                  << "\")  ClearAndLoad() either returned false or failed to load anything from "
                     "the file.  Is the "
                     "\"config.txt\" file in the right place?");

        M_HP_LOG_DBG("ConfigFile successfully found and read from: " + M_HP_VAR_STR(Path()));

        mediaBasePath_ = filesystem::CleanPath(instanceUPtr_->Value(mediaBasePathKey_));

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (filesystem::ExistsAndIsDirectory(mediaBasePath_)),
            "misc::ConfigFile::Initialize() mediaBasePath_=\""
                << mediaBasePath_
                << "\" either does not exist or is not a directory.  Fix the config file, which is "
                   "here=\""
                << instanceUPtr_->Path() << "\".  Change the key named \"" << mediaBasePathKey_
                << "\" to a relative path that leads to where the heroespath media folder is.");

        M_HP_LOG_DBG(M_HP_VAR_STR(mediaBasePathKey_) + M_HP_VAR_STR(mediaBasePath_));
    }

    const std::string ConfigFile::GetMediaPath(const std::string & KEY) const
    {
        const auto RELATIVE_MEDIA_PATH_STR { Value(KEY) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (RELATIVE_MEDIA_PATH_STR.empty() == false),
            "(key=\"" << KEY << "\") was not found in the config file.");

        const auto ABSOLUTE_MEDIA_PATH_STR { misc::filesystem::CombinePaths(
            mediaBasePath_, RELATIVE_MEDIA_PATH_STR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (filesystem::Exists(ABSOLUTE_MEDIA_PATH_STR)),
            "(key=\"" << KEY << "\") was found in the config file to be \""
                      << RELATIVE_MEDIA_PATH_STR
                      << "\", which when combined with the media base path \"" << mediaBasePath_
                      << "\" became the complete path \"" << ABSOLUTE_MEDIA_PATH_STR
                      << "\" -but that path doesn't exist.");

        return ABSOLUTE_MEDIA_PATH_STR;
    }

    const sf::IntRect ConfigFile::GetTextureRect(const std::string & KEY) const
    {
        const auto VALUE_STR { Value(KEY + "-texture-rect") };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (!VALUE_STR.empty()), "(key=\"" << KEY << "\") was not found in the config file.");

        const auto NUMBER_STR_VEC { misc::SplitByChars(VALUE_STR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (NUMBER_STR_VEC.size() == 4),
            "(key=\"" << KEY << "\") was found in the config file but " << NUMBER_STR_VEC.size()
                      << " numbers were found instead of 4.");

        sf::IntRect rect;
        rect.left = misc::ToNumberOr(NUMBER_STR_VEC.at(0), -1);
        rect.top = misc::ToNumberOr(NUMBER_STR_VEC.at(1), -1);
        rect.width = misc::ToNumberOr(NUMBER_STR_VEC.at(2), -1);
        rect.height = misc::ToNumberOr(NUMBER_STR_VEC.at(3), -1);

        const int NUMBER_TOO_LARGE { 10000 };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            ((rect.left >= 0) && (rect.top >= 0) && (rect.width >= 0) && (rect.height >= 0)
             && (rect.left < NUMBER_TOO_LARGE) && (rect.top < NUMBER_TOO_LARGE)
             && (rect.width < NUMBER_TOO_LARGE) && (rect.height < NUMBER_TOO_LARGE)),
            "(key=\"" << KEY
                      << "\") was found in the config file, and four numbers were found, but some "
                         "of them were not valid values.  They all must be [0,"
                      << NUMBER_TOO_LARGE << "].  rect=" << rect);

        return rect;
    }

} // namespace misc
} // namespace heroespath
