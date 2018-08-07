// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game-data-file.cpp
//
#include "game-data-file.hpp"

#include "log/log-macros.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/filesystem-helpers.hpp"
#include "misc/platform.hpp"

namespace heroespath
{
namespace game
{
    std::unique_ptr<GameDataFile> GameDataFile::instanceUPtr_;

    GameDataFile::GameDataFile()
        : ConfigBase("game-data.txt", "=", "#")
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
        , mediaBasePathKeyStr_("system-media-dir-win")
#else
        , mediaBasePathKeyStr_("system-media-dir-linux")
#endif
        , mediaBasePathStr_("")
    {
        M_HP_LOG_DBG("Subsystem Construction: GameDataFile");
    }

    misc::NotNull<GameDataFile *> game::GameDataFile::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: GameDataFile");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void GameDataFile::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<GameDataFile>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: GameDataFile");
        }
    }

    void GameDataFile::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "GameDataFile::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void GameDataFile::Initialize()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "GameDataFile::Initialize() found instanceUPtr that was null.");

        auto const LOAD_SUCCESS { Instance()->Load() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (LOAD_SUCCESS),
            "game::GameDataFile::Initialize() misc::ConfigBase::Load() returned false.");

        mediaBasePathStr_ = GetCopyStr(mediaBasePathKeyStr_);

        M_ASSERT_OR_LOGANDTHROW_SS(
            (mediaBasePathStr_.empty() == false),
            "game::GameDataFile::Initialize() mediaBasePathKeyStr_=" << mediaBasePathKeyStr_
                                                                     << " was not found.");
    }

    const std::string GameDataFile::GetMediaPath(const std::string & KEY) const
    {
        auto const PATH_STR { GetCopyStr(KEY) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (PATH_STR.empty() == false),
            "GameDataFile::GetMediaPath(\"" << KEY
                                            << "\") failed to find that key in the config file.");

        return misc::filesystem::CompletePath(mediaBasePathStr_, PATH_STR);
    }

} // namespace game
} // namespace heroespath
