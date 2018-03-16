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
// main.cpp
//
#include "game/startup-shutdown.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char * argv[])
{
    using namespace heroespath::game;

    int runExitCode{ EXIT_FAILURE };

    if (StartupShutdown::Setup("Heroes' Path", argc, argv))
    {
        runExitCode = StartupShutdown::Run();
    }

    auto const TEARDOWN_EXIT_CODE{ StartupShutdown::Teardown() };

    auto const SUCCESS{ ((EXIT_SUCCESS == runExitCode) && (TEARDOWN_EXIT_CODE == EXIT_SUCCESS)) };

    if (SUCCESS == false)
    {
        std::cerr << "Heroes' Path error detected: run_exit_code=" << runExitCode
                  << ", teardown_exit_code=" << TEARDOWN_EXIT_CODE << "  (" << EXIT_SUCCESS
                  << " means success)" << std::endl;
    }

    return ((SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
}
