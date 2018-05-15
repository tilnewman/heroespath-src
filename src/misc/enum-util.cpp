// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enum-util.cpp
//
#include "enum-util.hpp"

#include "game/loop-manager.hpp"

namespace heroespath
{
namespace misc
{
    namespace enum_util
    {

        void TestLog(const std::string & MSG_STR)
        {
            game::LoopManager::Instance()->TestingStrIncrement(MSG_STR);
        }

    } // namespace enum_util

} // namespace misc
} // namespace heroespath
