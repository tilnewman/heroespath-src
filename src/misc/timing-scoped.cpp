// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// timing.cpp
//
#include "timing-scoped.hpp"

#if !defined(HEROESPATH_MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_TIMING)

#include "misc/log.hpp"
#include "misc/timing.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <sstream>

namespace heroespath
{

const std::string TimeRes::ToString(const Enum RES)
{
    switch (RES)
    {
        case Nano:
        {
            return "ns";
        }
        case Milli:
        {
            return "ms";
        }
        case Micro:
        {
            return "us";
        }
        case Second:
        {
            return "s";
        }
        case Count:
        {
            return "(Count)";
        }
        default:
        {
            std::ostringstream ss;

            ss << "enum_value=" << static_cast<EnumUnderlying_t>(RES)
               << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count) << ")";

            // can't use log macros because they can cause endless recursion in the timing code
            misc::Log::Instance()->Append(
                misc::LogPriority::Error, ss.str(), __FILE__, __func__, __LINE__);

            return "";
        }
    }
}

namespace misc
{

    ScopedTimeLogger::ScopedTimeLogger(
        const TimeRes::Enum RES, const std::string & MESSAGE, const bool WILL_CANCEL_ON_LOG)
        : resolution_(RES)
        , message_(MESSAGE)
        , logCountBeforeStarting_(((WILL_CANCEL_ON_LOG) ? Log::Instance()->LineCount() : 0))
        , startTime_(std::chrono::high_resolution_clock::now())

    {}

    ScopedTimeLogger::~ScopedTimeLogger()
    {
        const auto ACTUAL_STOP_TIME { std::chrono::high_resolution_clock::now() };

        try
        {
            const auto DURATION { TimeSampler::Duration(
                resolution_, startTime_, ACTUAL_STOP_TIME) };

            if ((logCountBeforeStarting_ == 0)
                || (Log::Instance()->LineCount() == logCountBeforeStarting_))
            {
                // can't use log macros because they can cause endless recursion in the timing code
                std::ostringstream ss;

                ss << "ScopedLogTimer \"" << message_ << "\" took " << DURATION
                   << TimeRes::ToString(resolution_);

                Log::Instance()->Append(
                    LogPriority::Default, ss.str(), __FILE__, __func__, __LINE__);
            }
        }
        catch (const std::exception & EXCEPTION)
        {
            std::cerr << "ScopedTimeLogger::~ScopedTimeLogger(res=" << int(resolution_)
                      << ", msg=\"" << message_
                      << "\", log_count_before=" << logCountBeforeStarting_
                      << ") threw exception \"" << EXCEPTION.what() << "\"" << std::endl;
        }
    }

    ScopedTimeCollecter::ScopedTimeCollecter(TimeCollecter & timeCollecter)
        : timeCollecter_(timeCollecter)
    {
        timeCollecter_.BusyWorkOnlyToHeatCache();
        timeCollecter_.Start();
    }

    ScopedTimeCollecter::~ScopedTimeCollecter()
    {
        const auto ACTUAL_STOP_TIME { std::chrono::high_resolution_clock::now() };

        try
        {
            timeCollecter_.Stop(ACTUAL_STOP_TIME);
        }
        catch (const std::exception & EXCEPTION)
        {
            std::cerr << "ScopedTimeCollecter::~ScopedTimeCollecter() during timeCollecter_.Stop() "
                         "threw exception \""
                      << EXCEPTION.what() << "\"" << std::endl;
        }
    }

    ScopedTimeTrial::ScopedTimeTrial(TimeTrials & timeContest, const std::size_t COLLECTER_INDEX)
        : timeTrials_(timeContest)
        , collecterIndex_(COLLECTER_INDEX)
    {
        timeTrials_.Start(collecterIndex_);
    }

    ScopedTimeTrial::~ScopedTimeTrial()
    {
        // capture the stop time now because the distance between this point in the code and where
        // the stop time would normally be captured is longer than I would like and might incur
        // cache misses that throw the timing off.
        const auto ACTUAL_STOP_TIME { std::chrono::high_resolution_clock::now() };

        try
        {
            timeTrials_.Stop(collecterIndex_, ACTUAL_STOP_TIME);
        }
        catch (const std::exception & EXCEPTION)
        {
            std::cerr << "ScopedTimeTrial::~ScopedTimeTrial(index=" << collecterIndex_
                      << ") during timeTrials_.Stop() threw exception \"" << EXCEPTION.what()
                      << "\"" << std::endl;
        }
    }

} // namespace misc
} // namespace heroespath

#endif
