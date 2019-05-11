// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_TIMING_SCOPED_HPP_INCLUDED
#define HEROESPATH_MISC_TIMING_SCOPED_HPP_INCLUDED
//
// timing-scoped.hpp
//
#include "misc/enum-common.hpp"

#include <chrono>
#include <cstddef> //for std::size_t

namespace heroespath
{

// these types chosen to match what the <chrono> library uses
using TimeCount_t = long long;
using TimeMoment_t = std::chrono::high_resolution_clock::time_point;

struct TimeRes : public EnumBaseCounting<EnumFirstValue::Valid>
{
    enum Enum : EnumUnderlying_t
    {
        Nano = 0,
        Milli,
        Micro,
        Second,
        Count
    };
};

namespace misc
{

#if !defined(HEROESPATH_MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_TIMING)

    // Captures the time between construction and destruction and logs the result.
    // 2018-11-11 Ran test when placed in an empty scope and the average was 100ns.
    class ScopedTimeLogger
    {
    public:
        ScopedTimeLogger(
            const TimeRes::Enum RES, const std::string & MESSAGE, const bool WILL_CANCEL_ON_LOG);

        ~ScopedTimeLogger();

        ScopedTimeLogger(const ScopedTimeLogger &) = delete;
        ScopedTimeLogger(ScopedTimeLogger &&) = delete;
        ScopedTimeLogger & operator=(const ScopedTimeLogger &) = delete;
        ScopedTimeLogger & operator=(ScopedTimeLogger &&) = delete;

    private:
        const TimeRes::Enum resolution_;
        const std::string message_;
        const std::size_t logCountBeforeStarting_;
        const TimeMoment_t startTime_;
    };

    class TimeCollecter;

    // Captures the time from construction to destruction and adds it to a given collecter.
    class ScopedTimeCollecter
    {
    public:
        explicit ScopedTimeCollecter(TimeCollecter & timeCollecter);
        ~ScopedTimeCollecter();

        ScopedTimeCollecter(const ScopedTimeCollecter &) = delete;
        ScopedTimeCollecter(ScopedTimeCollecter &&) = delete;
        ScopedTimeCollecter & operator=(const ScopedTimeCollecter &) = delete;
        ScopedTimeCollecter & operator=(ScopedTimeCollecter &&) = delete;

    private:
        TimeCollecter & timeCollecter_;
    };

    class TimeTrials;

    // Captures the time between construction and destruction and adds the result to the timeTrial's
    // given collecter.
    //
    // 2018-11-11 Ran test when placed in an empty scope and the average was 50ns.
    class ScopedTimeTrial
    {
    public:
        ScopedTimeTrial(TimeTrials & timeTrials, const std::size_t COLLECTER_INDEX);
        ~ScopedTimeTrial();

        ScopedTimeTrial(const ScopedTimeTrial &) = delete;
        ScopedTimeTrial(ScopedTimeTrial &&) = delete;
        ScopedTimeTrial & operator=(const ScopedTimeTrial &) = delete;
        ScopedTimeTrial & operator=(ScopedTimeTrial &&) = delete;

    private:
        TimeTrials & timeTrials_;
        const std::size_t collecterIndex_;
    };

    // the only reason for these macros is so that we can use a macros to vanish all timing easily

#define M_HP_SCOPED_TIME_LOGGER(time_res, message_str, will_cancel_on_log)                         \
    misc::ScopedTimeLogger scopedTimeLogger_MadeByMacro(time_res, message_str, will_cancel_on_log);

#define M_HP_SCOPED_TIME_COLLECTER(var_name, collecter)                                            \
    misc::ScopedTimeCollecter scopedTimeCollecter_MadeByMacro(collecter);

#define M_HP_SCOPED_TIME_TRIAL(time_trials, collecter_index)                                       \
    misc::ScopedTimeTrial scopedTimeTrial_MadeByMacro(time_trials, collecter_index);

#else

#define M_HP_SCOPED_TIME_LOGGER(var_name, time_res, message_str, will_cancel_on_log) ;
#define M_HP_SCOPED_TIME_COLLECTER(var_name, collecter) ;
#define M_HP_SCOPED_TIME_TRIAL(var_name, time_trials, collecter_index) ;

#endif

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TIMING_SCOPED_HPP_INCLUDED
