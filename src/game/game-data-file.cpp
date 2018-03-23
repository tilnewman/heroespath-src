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
// game-data-file.cpp
//
#include "game-data-file.hpp"

#include "log/log-macros.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/platform.hpp"

#include <memory>

namespace heroespath
{
namespace game
{
    std::unique_ptr<GameDataFile> GameDataFile::instanceUPtr_{ nullptr };

    GameDataFile::GameDataFile()
        : ConfigBase("game-data.txt", "=", "#")
    {
        M_HP_LOG_DBG("Singleton Construction: GameDataFile");
    }

    GameDataFile * game::GameDataFile::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): GameDataFile");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void GameDataFile::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<GameDataFile>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: GameDataFile");
        }
    }

    void GameDataFile::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_.get() != nullptr),
            "GameDataFile::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void GameDataFile::Initialize()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_.get() != nullptr),
            "GameDataFile::Initialize() found instanceUPtr that was null.");

        instanceUPtr_->config::ConfigBase::Load();
    }

    const std::string GameDataFile::GetMediaPath(const std::string & KEY) const
    {
        const std::string PATH(GetCopyStr(KEY));
        M_ASSERT_OR_LOGANDTHROW_SS(
            (PATH.empty() == false),
            "GameDataFile::GetMediaPath(\"" << KEY
                                            << "\") failed to find that key in the config file.");

        std::ostringstream ss;
        ss << GetMediaBasePathStr() << PATH;
        return ss.str();
    }

    const std::string GameDataFile::CreateMediaPath(const std::string & PATH) const
    {
        std::ostringstream ss;
        ss << GetMediaBasePathStr() << PATH;
        return ss.str();
    }

    const std::string GameDataFile::GetMediaBasePathStr() const
    {
        const std::string MEDIA_BASE_PATH_KEY_STR(
            (misc::Platform::Instance()->IsWindows()) ? "system-media-dir-win"
                                                      : "system-media-dir-linux");

        const std::string MEDIA_BASE_PATH_STR(GetCopyStr(MEDIA_BASE_PATH_KEY_STR));

        M_ASSERT_OR_LOGANDTHROW_SS(
            (MEDIA_BASE_PATH_STR.empty() == false),
            "GameDataFile::GetMediaBasePathStr(key=\""
                << MEDIA_BASE_PATH_KEY_STR << "\") failed to find that key in the config file.");

        return MEDIA_BASE_PATH_STR;
    }
} // namespace game
} // namespace heroespath
