// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
//  random.cpp
//
#include "misc/random.hpp"
#include <random>

namespace heroespath
{
namespace misc
{
    namespace random
    {

        std::mt19937 MersenneTwister::engine;

    } // namespace random
} // namespace misc
} // namespace heroespath
