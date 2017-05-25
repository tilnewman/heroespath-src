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
#ifndef GAME_SETTINGSFILE_INCLUDED
#define GAME_SETTINGSFILE_INCLUDED
//
// settings-file.hpp
//  A singleton config file class that saves game settings.  (i.e. volume, resolution, etc.)
//
#include "configbase/configbase.hpp"
#include <memory>
#include <string>


namespace game
{

    //types required by singleton implementation
    class SettingsFile;
    using SettingsFileSPtr_t = std::shared_ptr<SettingsFile>;


    //Singleton class that manages a simple configuration file
    class SettingsFile : public appbase::ConfigBase
    {
        //prevent copy assignment
        SettingsFile(const SettingsFile &) =delete;

        //prevent copy assignment
        SettingsFile & operator=(const SettingsFile &) =delete;

        //prevent non-singleton construction
        SettingsFile();

    public:
        virtual ~SettingsFile();
        static SettingsFileSPtr_t Instance();

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
        static SettingsFileSPtr_t instanceSPtr_;
    };

}
#endif //GAME_SETTINGSFILE_INCLUDED
