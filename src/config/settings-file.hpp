// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CONFIG_SETTINGSFILE_HPP_INCLUDED
#define HEROESPATH_CONFIG_SETTINGSFILE_HPP_INCLUDED
//
// settings-file.hpp
//  A Subsystem config file class that saves game settings. (i.e. volume, resolution, etc.)
//
#include "config/configbase.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace config
{

    // Subsystem class that manages a simple configuration file
    class SettingsFile : public ConfigBase
    {
    public:
        SettingsFile(const SettingsFile &) = delete;
        SettingsFile(SettingsFile &&) = delete;
        SettingsFile & operator=(const SettingsFile &) = delete;
        SettingsFile & operator=(SettingsFile &&) = delete;

    public:
        SettingsFile();
        virtual ~SettingsFile();
        static misc::NotNull<SettingsFile *> Instance();
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
