// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_CONFIG_FILE_HPP_INCLUDED
#define HEROESPATH_MISC_CONFIG_FILE_HPP_INCLUDED
//
// config-file.hpp
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
    class ConfigFile : public misc::KeyValueFile
    {
    public:
        ConfigFile();

        virtual ~ConfigFile() = default;

        ConfigFile(const ConfigFile &) = delete;
        ConfigFile(ConfigFile &&) = delete;
        ConfigFile & operator=(const ConfigFile &) = delete;
        ConfigFile & operator=(ConfigFile &&) = delete;

        static misc::NotNull<ConfigFile *> Instance();
        static void Acquire();
        static void Release();

        void Initialize();

        const std::string GetMediaPath(const std::string & KEY) const;

    private:
        static const std::string MEDIA_BASE_PATH_KEY_FOR_WINDOWS_;
        static const std::string MEDIA_BASE_PATH_KEY_FOR_NON_WINDOWS_;

        std::string mediaBasePathKey_;
        std::string mediaBasePath_;

        static std::unique_ptr<ConfigFile> instanceUPtr_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_CONFIG_FILE_HPP_INCLUDED
