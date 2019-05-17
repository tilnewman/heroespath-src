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
#include "game/setup-teardown.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(const int ARGC, const char * ARGV[])
{
    try
    {
        const std::vector<std::string> ARGS(ARGV, ARGV + ARGC);

        heroespath::game::SetupTeardown setupThenTeardown(
            "Heroes' Path", heroespath::game::SubsystemCollection::All, ARGS);

        setupThenTeardown.Play();
        return EXIT_SUCCESS;
    }
    catch (const std::exception & EXCEPTION)
    {
        std::cout << "exception: " << EXCEPTION.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "unknown exception" << std::endl;
    }

    return EXIT_FAILURE;
}
