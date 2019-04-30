// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
#define HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
//
// startup-shutdown.hpp
//
#include <string>

namespace heroespath
{
namespace game
{

    // Responsible for setting up and then tearing down the application.
    class StartupShutdown
    {
    public:
        StartupShutdown(const StartupShutdown &) = delete;
        StartupShutdown(StartupShutdown &&) = delete;
        StartupShutdown & operator=(const StartupShutdown &) = delete;
        StartupShutdown & operator=(StartupShutdown &&) = delete;
        StartupShutdown() = delete;

        StartupShutdown(const std::string & APPLICATION_NAME, int ARGC, char * argv[]);
        ~StartupShutdown();

        void Run() const;

    private:
        void Setup(const std::string & APPLICATION_NAME, int ARGC, char * argv[]) const;

        void Teardown() const;

        void Teardown_SettingsFile() const;
        void Teardown_CloseDisplay() const;
        void Teardown_EmptyHolders() const;
        void Teardown_ReleaseSubsystems() const;
        void Teardown_Logger() const;

        void Setup_ParseCommandLineArguments(const int ARGC, char * argv[]) const;
        void Setup_Display(const std::string & APPLICATION_NAME) const;
        void Setup_FilesystemPaths() const;
        void Setup_HoldersFill() const;
        void Setup_SubsystemsAcquire() const;
        void Setup_SubsystemsInitialize() const;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
