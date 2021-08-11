// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_SETTINGS_FILE_HPP_INCLUDED
#define HEROESPATH_MISC_SETTINGS_FILE_HPP_INCLUDED
//
// settings-file.hpp
//  A Subsystem config file class that saves game settings. (i.e. volume, resolution, etc.)
//
#include "misc/key-value-file.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace misc
{

    // Subsystem class that manages a simple configuration file
    class SettingsFile : public KeyValueFile
    {
    public:
        SettingsFile();

        virtual ~SettingsFile();

        SettingsFile(const SettingsFile &) = delete;
        SettingsFile(SettingsFile &&) = delete;
        SettingsFile & operator=(const SettingsFile &) = delete;
        SettingsFile & operator=(SettingsFile &&) = delete;

        static misc::NotNull<SettingsFile *> Instance();
        static void Acquire();
        static void Release();

        void AcquireAndSave();
        void LoadAndRestore();
        void SetDefaults();

        static const std::string KEY_MUSIC_VOLUME_;
        static const std::string KEY_SOUND_FX_VOLUME_;
        static const std::string KEY_DISPLAY_WIDTH_;
        static const std::string KEY_DISPLAY_HEIGHT_;
        static const std::string KEY_DISPLAY_BIT_DEPTH_;
        static const std::string KEY_DISPLAY_WILL_VERTICAL_SYNC_;
        static const std::string KEY_DISPLAY_FRAMERATE_LIMIT_;
        static const std::string KEY_DISPLAY_ANTIALIAS_LEVEL_;

    private:
        static std::unique_ptr<SettingsFile> instanceUPtr_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_CONFIG_SETTINGSFILE_HPP_INCLUDED
