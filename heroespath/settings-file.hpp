#ifndef APPBASE_HEROESPATH_SETTINGSFILE_INCLUDED
#define APPBASE_HEROESPATH_SETTINGSFILE_INCLUDED
//
// settings-file.hpp
//  A singleton config file class that saves game settings.  (i.e. volume, resolution, etc.)
//
#include "configbase/configbase.hpp"
#include <memory>
#include <string>


namespace heroespath
{

    //types required by singleton implementation
    class SettingsFile;
    using SettingsFileSPtr_t = std::shared_ptr<SettingsFile>;


    //Singleton class that manages a simple configuration file
    class SettingsFile : public appbase::ConfigBase
    {
        //prevent copy assignment
        SettingsFile(const SettingsFile &);

        //prevent copy assignment
        SettingsFile & operator=(const SettingsFile &);

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
#endif //APPBASE_HEROESPATH_SETTINGSFILE_INCLUDED
