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
#ifndef HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
#define HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
//
// startup-shutdown.hpp
//
#include "misc/platform.hpp"

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

        StartupShutdown();

        bool Setup(const std::string & APPLICATION_NAME, int ARGC, char * argv[]) const;
        int Run() const;
        int Teardown() const;

    private:
        void Teardown_SettingsFile(int & exitCode_OutParam) const;
        void Teardown_CloseDisplay(int & exitCode_OutParam) const;
        void Teardown_EmptyHolders(int & exitCode_OutParam) const;
        void Teardown_ReleaseSubsystems(int & exitCode_OutParam) const;
        void Teardown_Logger(int & exitCode_OutParam) const;

        void Setup_ParseCommandLineArguments(const int ARGC, char * argv[]) const;
        void Setup_Display(const std::string & APPLICATION_NAME) const;
        void Setup_ManagerClassResourcePaths() const;
        void Setup_HoldersFill() const;
        void Setup_SubsystemsAcquire() const;
        void Setup_SubsystemsInitialize() const;

        misc::Platform platform_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_STARTUPSHUTDOWN_HPP_INCLUDED
