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

#include "game/log-macros.hpp"

#include "utilz/platform.hpp"
#include "utilz/boost-string-includes.hpp"

#include <memory>


namespace game
{

    std::unique_ptr<GameDataFile> GameDataFile::instanceUPtr_{ nullptr };


    GameDataFile::GameDataFile()
    :
        ConfigBase("game-data.txt", "=", "#")
    {
        Load();
    }


    GameDataFile::~GameDataFile()
    {}


    GameDataFile * GameDataFile::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void GameDataFile::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new GameDataFile);
        }
    }


    void GameDataFile::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::GameDataFile::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    const std::string GameDataFile::GetCopyStr(const std::string & KEY, const std::string & DEFAULT) const
    {
        std::string temp(DEFAULT);
        GetStr(temp, KEY);//throws if KEY is not found
        return temp;
    }


    int GameDataFile::GetCopyInt(const std::string & KEY, const int DEFAULT) const
    {
        return GetCopyDataFile<int>(KEY, DEFAULT);
    }


    std::size_t GameDataFile::GetCopySizet(const std::string & KEY, const std::size_t DEFAULT) const
    {
        return GetCopyDataFile<std::size_t>(KEY, DEFAULT);
    }


    bool GameDataFile::GetCopyBool(const std::string & KEY, const bool DEFAULT) const
    {
        //handle case where file contains true/false or yes/no, instead of the number
        namespace ba = boost::algorithm;
        const std::string STR_VERSION( ba::trim_copy(ba::to_lower_copy(GetCopyStr(KEY, ""))) );
        if ((STR_VERSION == "true") || (STR_VERSION == "yes"))
            return true;
        else
            if ((STR_VERSION == "false") || (STR_VERSION == "no"))
                return false;

        return GetCopyDataFile<bool>(KEY, DEFAULT);
    }


    float GameDataFile::GetCopyFloat(const std::string & KEY, const float DEFAULT) const
    {
        return GetCopyDataFile<float>(KEY, DEFAULT);
    }


    double GameDataFile::GetCopyDouble(const std::string & KEY, const double DEFAULT) const
    {
        return GetCopyDataFile<double>(KEY, DEFAULT);
    }


    const std::string GameDataFile::GetMediaPath(const std::string & KEY) const
    {
        const std::string PATH(GetCopyStr(KEY, ""));
        M_ASSERT_OR_LOGANDTHROW_SS((PATH.empty() == false), "GameDataFile::GetMediaPath(\"" << KEY << "\") failed to find that key in the config file.");

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
        const std::string MEDIA_BASE_PATH_KEY_STR((utilz::Platform::Instance()->IsWindows()) ? "system-media-dir-win" : "system-media-dir-linux");
        const std::string MEDIA_BASE_PATH_STR( GetCopyStr(MEDIA_BASE_PATH_KEY_STR, "") );
        M_ASSERT_OR_LOGANDTHROW_SS((MEDIA_BASE_PATH_STR.empty() == false), "GameDataFile::GetMediaBasePathStr(key=\"" << MEDIA_BASE_PATH_KEY_STR << "\") failed to find that key in the config file.");
        return MEDIA_BASE_PATH_STR;
    }


    bool GameDataFile::WillRunSystemTests() const
    {
        return GetCopyBool("system-will-execute-tests", false);
    }

}
