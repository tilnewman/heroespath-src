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
// settings-file.cpp
//
#include "settings-file.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/display.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "game/log-macros.hpp"


namespace game
{

    SettingsFileSPtr_t SettingsFile::instanceSPtr_;
    const std::string  SettingsFile::KEY_THEMEMUSIC_VOL_     ("volume_music");
    const std::string  SettingsFile::KEY_SOUNDEFFECTS_VOL_   ("volume_sound_effects");
    const std::string  SettingsFile::KEY_RESOLUTION_WIDTH_   ("display_width");
    const std::string  SettingsFile::KEY_RESOLUTION_HEIGHT_  ("display_height");
    const std::string  SettingsFile::KEY_RESOLUTION_BITDEPTH_("display_bit_depth");
    const std::string  SettingsFile::KEY_VERTICAL_SYNC_      ("display_vertical_sync");
    const std::string  SettingsFile::KEY_FRAMERATE_LIMIT_    ("display_frame_rate_limit");
    const std::string  SettingsFile::KEY_ANTIALIAS_LEVEL_    ("display_antialias_level");


    SettingsFile::SettingsFile()
    :
        ConfigBase("settings.dat", "=", "#")
    {}


    SettingsFile::~SettingsFile()
    {}


    SettingsFileSPtr_t SettingsFile::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new SettingsFile );

        return instanceSPtr_;
    }


    void SettingsFile::AcquireAndSave()
    {
        try
        {
            SetFloat(KEY_THEMEMUSIC_VOL_, sfml_util::SoundManager::Instance()->MusicVolume());
            SetFloat(KEY_SOUNDEFFECTS_VOL_, sfml_util::SoundManager::Instance()->SoundEffectVolume());
            SetInt(KEY_RESOLUTION_WIDTH_, static_cast<int>(sfml_util::Display::Instance()->GetWinWidthu()));
            SetInt(KEY_RESOLUTION_HEIGHT_, static_cast<int>(sfml_util::Display::Instance()->GetWinHeightu()));
            SetBool(KEY_VERTICAL_SYNC_, sfml_util::Display::Instance()->GetVerticalSync());
            SetInt(KEY_FRAMERATE_LIMIT_, sfml_util::Display::Instance()->GetFrameRateLimit());
            SetInt(KEY_ANTIALIAS_LEVEL_, static_cast<int>(sfml_util::Display::Instance()->AntialiasLevel()));

            unsigned bitDepth(sfml_util::Display::Instance()->WinColorDepth());
            if (0 == bitDepth)
                bitDepth = 32;
            SetInt(KEY_RESOLUTION_BITDEPTH_, static_cast<int>(bitDepth));

            M_ASSERT_OR_LOGANDTHROW_SS(Save(), "SettingsFile::AcquireAndSave() failed to Save().");
        }
        catch (const std::exception & E)
        {
            M_HP_LOG("SettingsFile::AcquireAndSave() threw std::exception(" << E.what() << ")");
        }
        catch (...)
        {
            M_HP_LOG("SettingsFile::AcquireAndSave() threw UNKNOWN exception.");
        }
    }


    void SettingsFile::LoadAndRestore()
    {
        //skip restoring settings if the load failed
        if (Load())
        {
            const float SAVED_THEMEMUSIC_VOL(GetCopyFloat(KEY_THEMEMUSIC_VOL_, -1.0f));
            if (SAVED_THEMEMUSIC_VOL > -0.1f)
            {
                M_HP_LOG("SettingsFile::LoadAndRestore() setting MusicVolume to " << SAVED_THEMEMUSIC_VOL);
                sfml_util::SoundManager::Instance()->MusicVolumeSet(SAVED_THEMEMUSIC_VOL);
            }

            const float SAVED_SOUNDEFFECTS_VOL(GetCopyFloat(KEY_SOUNDEFFECTS_VOL_, -1.0f));
            if (SAVED_SOUNDEFFECTS_VOL > -0.1f)
            {
                M_HP_LOG("SettingsFile::LoadAndRestore() setting SoundEffectVolume to " << SAVED_SOUNDEFFECTS_VOL);
                sfml_util::SoundManager::Instance()->SoundEffectVolumeSet(SAVED_SOUNDEFFECTS_VOL);
            }

            const unsigned ERROR_VALUE(1024);
            const unsigned SAVED_ANTIALIAS_LEVEL(static_cast<unsigned>(GetCopyInt(KEY_ANTIALIAS_LEVEL_, ERROR_VALUE)));
            if (SAVED_ANTIALIAS_LEVEL != ERROR_VALUE)
            {
                M_HP_LOG("SettingsFile::LoadAndRestore() setting display antialias level to " << SAVED_ANTIALIAS_LEVEL);
                sfml_util::Display::Instance()->SetFrameRateLimit(static_cast<int>(SAVED_ANTIALIAS_LEVEL));
            }

            const int WIDTH(GetCopyInt(KEY_RESOLUTION_WIDTH_, -1));
            const int HEIGHT(GetCopyInt(KEY_RESOLUTION_HEIGHT_, -1));
            const int BITDEPTH(GetCopyInt(KEY_RESOLUTION_BITDEPTH_, -1));
            if ((WIDTH > 0) &&
                (HEIGHT > 0) &&
                (BITDEPTH > 0) &&
                ((static_cast<int>(sfml_util::Display::Instance()->GetWinWidthu()) != WIDTH)   ||
                 (static_cast<int>(sfml_util::Display::Instance()->GetWinHeightu()) != HEIGHT) ||
                 (sfml_util::Display::Instance()->AntialiasLevel() != SAVED_ANTIALIAS_LEVEL)))
            {
                const sf::VideoMode NEW_VIDEO_MODE(static_cast<unsigned>(WIDTH), static_cast<unsigned>(HEIGHT), static_cast<unsigned>(BITDEPTH));
                M_HP_LOG("SettingsFile::LoadAndRestore() setting display resolution to " << sfml_util::VideoModeToString(NEW_VIDEO_MODE) << " AA=" << SAVED_ANTIALIAS_LEVEL);
                sfml_util::Display::ChangeVideoMode(NEW_VIDEO_MODE, SAVED_ANTIALIAS_LEVEL);
            }

            const int SAVED_FRAME_RATE_LIMIT( GetCopyInt(KEY_FRAMERATE_LIMIT_, -1) );
            if (SAVED_FRAME_RATE_LIMIT > -1)
            {
                M_HP_LOG("SettingsFile::LoadAndRestore() setting display frame rate limit to " << SAVED_FRAME_RATE_LIMIT);
                sfml_util::Display::Instance()->SetFrameRateLimit(SAVED_FRAME_RATE_LIMIT);
            }

            const bool SAVED_VERTICAL_SYNC( GetCopyBool(KEY_VERTICAL_SYNC_, true) );
            M_HP_LOG("SettingsFile::LoadAndRestore() setting the display vertical sync to " << std::boolalpha << SAVED_VERTICAL_SYNC);
            sfml_util::Display::Instance()->SetVerticalSync(SAVED_VERTICAL_SYNC);
        }
        else
        {
            M_HP_LOG("SettingsFile::LoadAndRestore() was unable to load the settings file.");

            M_HP_LOG("SettingsFile::LoadAndRestore() setting MusicVolume to 50% by default.");
            sfml_util::SoundManager::Instance()->MusicVolumeSet(50.0f);

            M_HP_LOG("SettingsFile::LoadAndRestore() setting SoundEffectVolume to 50% by default.");
            sfml_util::SoundManager::Instance()->SoundEffectVolumeSet(50.0f);
        }
    }

}
