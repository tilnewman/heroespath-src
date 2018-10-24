// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// log-macros.cpp
//
#include "log-macros.hpp"

#include "misc/log.hpp"

#include <sstream>

namespace heroespath
{

void LogMacroHelper::Append(
    const misc::LogPriority::Enum PRIORITY,
    const std::string & MSG,
    const std::string & FILE,
    const std::string & FUNCTION,
    const int LINE)
{
    misc::Log::Instance()->Append(PRIORITY, MSG, FILE, FUNCTION, LINE);
}

} // namespace heroespath
