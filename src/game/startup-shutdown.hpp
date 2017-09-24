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
#ifndef GAME_STARTUPSHUTDOWN_HPP_INCLUDED
#define GAME_STARTUPSHUTDOWN_HPP_INCLUDED
//
// startup-shutdown.hpp
//
#include <string>


namespace game
{

    //responsible for starting and stopping the application
    struct StartupShutdown
    {
        static bool Setup(
            const std::string & APPLICATION_NAME,
            int ARGC,
            char * argv[]);

        static int Run();

        static int Teardown();

    private:
        static void ParseCommandLineArguments(const int ARGC, char * argv[]);
        static void DetectLogAndCheckPlatform();
        static void SetupDisplay(const std::string & APPLICATION_NAME);
        static void SetManagerClassResourcePaths();
        static void WarehousesFill();
        static void WarehousesEmpty();
        static void SingletonsAcquireAndInitialize();
        static void SingletonsRelease();
    };

}

#endif //GAME_STARTUPSHUTDOWN_HPP_INCLUDED
