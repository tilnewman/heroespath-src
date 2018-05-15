// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// main.cpp
//
#include "game/startup-shutdown.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char * argv[])
{
    using namespace heroespath::game;

    StartupShutdown startStop;

    int runExitCode{ EXIT_FAILURE };

    if (startStop.Setup("Heroes' Path", argc, argv))
    {
        runExitCode = startStop.Run();
    }

    auto const TEARDOWN_EXIT_CODE{ startStop.Teardown() };

    auto const SUCCESS{ ((EXIT_SUCCESS == runExitCode) && (TEARDOWN_EXIT_CODE == EXIT_SUCCESS)) };

    if (SUCCESS == false)
    {
        std::cerr << "Heroes' Path error detected: run_exit_code=" << runExitCode
                  << ", teardown_exit_code=" << TEARDOWN_EXIT_CODE << "  (" << EXIT_SUCCESS
                  << " means success)" << std::endl;
    }

    return ((SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
}
