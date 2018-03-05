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
#ifndef HEROESPATH_CONFIG_SETTINGSFILE_HPP_INCLUDED
#define HEROESPATH_CONFIG_SETTINGSFILE_HPP_INCLUDED
//
// settings-file.hpp
//  A singleton config file class that saves game settings. (i.e. volume, resolution, etc.)
//
#include "config/configbase.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace config
{

    // Singleton class that manages a simple configuration file
    class SettingsFile : public ConfigBase
    {
        SettingsFile(const SettingsFile &) = delete;
        SettingsFile(const SettingsFile &&) = delete;
        SettingsFile & operator=(const SettingsFile &) = delete;
        SettingsFile & operator=(const SettingsFile &&) = delete;

    public:
        SettingsFile();
        virtual ~SettingsFile();
        static SettingsFile * Instance();
        static void Acquire();
        static void Release();

        void AcquireAndSave();
        void LoadAndRestore();

    public:
        static const std::string KEY_THEMEMUSIC_VOL_;
        static const std::string KEY_SOUNDEFFECTS_VOL_;
        static const std::string KEY_RESOLUTION_WIDTH_;
        static const std::string KEY_RESOLUTION_HEIGHT_;
        static const std::string KEY_RESOLUTION_BITDEPTH_;
        static const std::string KEY_VERTICAL_SYNC_;
        static const std::string KEY_FRAMERATE_LIMIT_;
        static const std::string KEY_ANTIALIAS_LEVEL_;

    private:
        static std::unique_ptr<SettingsFile> instanceUPtr_;
    };
} // namespace config
} // namespace heroespath

#endif // HEROESPATH_CONFIG_SETTINGSFILE_HPP_INCLUDED
