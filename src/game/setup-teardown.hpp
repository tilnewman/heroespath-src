// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
#define HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
//
// setup-teardown.hpp
//
#include <string>
#include <vector>

namespace heroespath
{
namespace game
{

    enum class SubsystemCollection
    {
        All,

        // Skips the settings file, command line args, framerate, vsync, stages, and game loop.
        // Calls GameStateFactory::MakeForNewGameForTesting() so tests have characters to work with.
        TestAll,

        // Same as Test (above) but the Display subsystem is not created.
        TestNoDisplay,

        // Only the Log and ConfigFile
        TestWithOnlyLogAndConfig
    };

    // Responsible for scoped setup and then teardown of the whole application.
    class SetupTeardown
    {
    public:
        SetupTeardown(
            const std::string & TITLE,
            const SubsystemCollection SUBSYSTEMS,
            const std::vector<std::string> & ARGS = {});

        ~SetupTeardown();

        SetupTeardown(const SetupTeardown &) = delete;
        SetupTeardown(SetupTeardown &&) = delete;
        SetupTeardown & operator=(const SetupTeardown &) = delete;
        SetupTeardown & operator=(SetupTeardown &&) = delete;

        void Play() const;

    private:
        void Setup(const std::string & TITLE, const std::vector<std::string> & ARGS) const;

        void Setup_Log() const;
        void Setup_PlatformDetection() const;
        void Setup_RandomNumberGenerator() const;
        void Setup_ConfigFile() const;
        void Setup_ParseCommandLineArguments(const std::vector<std::string> & ARGS) const;
        void Setup_LoadItemDetails() const;
        void Setup_Display(const std::string & TITLE) const;
        void Setup_FilesystemPaths() const;
        void Setup_SubsystemsAcquire() const;
        void Setup_SubsystemsInitialize() const;
        void Setup_HoldersFill() const;

        void Teardown() const;

        void Teardown_SettingsFile() const;
        void Teardown_CloseDisplay() const;
        void Teardown_EmptyHolders() const;
        void Teardown_ReleaseSubsystems() const;
        void Teardown_Logger() const;

    private:
        static inline SubsystemCollection m_subsystems { SubsystemCollection::All };
        static inline std::string m_name {};
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
