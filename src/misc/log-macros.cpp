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
#include "misc/timing.hpp"

#include <iostream>
#include <sstream>

namespace heroespath
{

std::unique_ptr<misc::TimeTrials> LogMacroHelper::loggingTimeTrialsUPtr_;

void LogMacroHelper::Append(
    const misc::LogPriority::Enum PRIORITY,
    const std::string & MSG,
    const std::string & FILE,
    const std::string & FUNCTION,
    const int LINE)
{
    misc::Log::Instance()->Append(PRIORITY, MSG, FILE, FUNCTION, LINE);
}

void LogMacroHelper::LogTimingStart()
{
    loggingTimeTrialsUPtr_
        = std::make_unique<misc::TimeTrials>("Log Macro", TimeRes::Micro, false, 10, 0.0);

    loggingTimeTrialsUPtr_->AddCollecter("LogMacro");
}

void LogMacroHelper::LogTimingStop()
{
    if (loggingTimeTrialsUPtr_)
    {
        loggingTimeTrialsUPtr_->EndAllContests();
        loggingTimeTrialsUPtr_.reset();
    }
}

misc::TimeTrials * LogMacroHelper::GetLoggingTimeTrialsPtr()
{
    return loggingTimeTrialsUPtr_.get();
}

} // namespace heroespath
