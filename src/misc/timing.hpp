// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_TIMING_HPP_INCLUDED
#define HEROESPATH_MISC_TIMING_HPP_INCLUDED
//
// timing.hpp
//
#include "misc/enum-common.hpp"
#include "misc/vector-map.hpp"

#include <chrono>
#include <string>
#include <vector>

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
        Second,
        Count,
    };

    static const std::string ToString(const Enum RES);
};

namespace misc
{

    struct TimeSampler
    {
        static const TimeMoment_t Now();

        // if the time duration between THEN and NOW is negative then zero will be returned
        static TimeCount_t Duration(
            const TimeRes::Enum RES,
            const TimeMoment_t & THEN,
            const TimeMoment_t & ACTUAL_STOP_TIME = DEFAULT_EMPTY_MOMENT_IN_TIME);

        static const TimeMoment_t DEFAULT_EMPTY_MOMENT_IN_TIME;
    };

    // Wraps various statistics about a collection of time durations.  Negative time duration counts
    // will be thrown out.
    struct TimeStats
    {
        TimeStats() = default;

        // use this constructor to gather time statistics for a given vector of time duration counts
        TimeStats(
            const std::vector<TimeCount_t> & DURATION_COUNT_VEC_ORIG,
            const double OUTLIERS_IGNORE_RATIO);

        TimeStats(const std::vector<TimeStats> & STATS_VEC);

        TimeStats(const TimeStats &) = default;
        TimeStats(TimeStats &&) = default;
        TimeStats & operator=(const TimeStats &) = default;
        TimeStats & operator=(TimeStats &&) = default;

        const std::string ToString(
            const bool WILL_INCLUDE_COUNT,
            const bool WILL_INCLUDE_SUM,
            const std::size_t FORCE_MIN_WIDTH = 0) const; // zero means use the member variables

        static void ReducePrecision(double & number);

        std::size_t num = 0;
        TimeCount_t min = 0;
        TimeCount_t max = 0;
        TimeCount_t sum = 0;
        double avg = 0.0;
        double dev = 0.0;
        double outlier_ratio = 0.0;
        std::size_t num_before_outliers_removed = 0;
        std::size_t longest_stat_string_length = 1;
        std::size_t longest_sum_string_length = 1;
        TimeCount_t outlier_min = 0;
        TimeCount_t outlier_max = 0;
    };

    using TimeStatsVec_t = std::vector<TimeStats>;

    // Use with the game engine's UpdateTime(float ELAPSED_TIME_SEC) functions to repeatedly check
    // if a given amount of time in seconds has elapsed.
    class IntervalTimer
    {
    public:
        explicit IntervalTimer(const float DURATION);

        float Time() const { return timeSec_; }
        float Duration() const { return durationSec_; }
        void Reset() { timeSec_ = 0.0f; }

        bool Update(const float ELAPSED_TIME_SEC);

    private:
        float timeSec_;
        float durationSec_;
    };

    // Construct one of these to log how long it takes from that moment until the end of the current
    // scope.  Time duration counts will never be negative.
    //
    // Tested 100 times inside an empty scope resulting in durations of [100, 900] nanoseconds
    // averaging 230, with only ten above 300, only five above 500, and only two above 800.
    class ScopedLogTimer
    {
    public:
        ScopedLogTimer(const TimeRes::Enum RES, const std::string & MESSAGE);

        ~ScopedLogTimer();

        ScopedLogTimer(const ScopedLogTimer &) = delete;
        ScopedLogTimer(ScopedLogTimer &&) = delete;
        ScopedLogTimer & operator=(const ScopedLogTimer &) = delete;
        ScopedLogTimer & operator=(ScopedLogTimer &&) = delete;

    private:
        const TimeRes::Enum resolution_;
        const std::string message_;
        const TimeMoment_t startTime_;
    };

    // Use to find how many seconds/milliseconds/nanoseconds passes between Start() and Stop(). Time
    // duration counts will never be negative.
    //
    // Tested 100 times with nothing in between Start() and Stop() resulted in [50, 250] nanoseconds
    // averaging 92, with only three above 120 and all of those were [200, 250].
    class Timer
    {
    public:
        explicit Timer(const TimeRes::Enum RES);

        virtual ~Timer() = default;

        Timer(const Timer &) = default;
        Timer(Timer &&) = default;
        Timer & operator=(const Timer &) = default;
        Timer & operator=(Timer &&) = default;

        bool IsStarted() const { return isStarted_; }

        virtual void Start();

        virtual void Cancel();

        virtual void
            Stop(const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME);

        TimeCount_t StopAndReport(
            const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME);

        void Restart(
            const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME);

        TimeCount_t RestartAndReport(
            const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME);

        TimeRes::Enum Resolution() const { return resolution_; }

        TimeCount_t Report() const { return durationCount_; }

    private:
        bool isStarted_;
        TimeRes::Enum resolution_;
        TimeMoment_t startTime_;
        TimeCount_t durationCount_;
    };

    // Same as Timer (above) except that whenever Stop() is called the duration is saved in a
    // collection that you can get statistical information about.
    class TimeCollecter : public Timer
    {
    public:
        TimeCollecter(
            const std::string & NAME,
            const TimeRes::Enum RES,
            const std::size_t EXPECTED_COUNT,
            const double OUTLIERS_IGNORE_RATIO);

        virtual ~TimeCollecter() = default;

        TimeCollecter(const TimeCollecter &) = default;
        TimeCollecter(TimeCollecter &&) = default;
        TimeCollecter & operator=(const TimeCollecter &) = default;
        TimeCollecter & operator=(TimeCollecter &&) = default;

        const std::string Name() const { return name_; }

        bool Empty() const { return durationCounts_.empty(); }

        std::size_t Size() const { return durationCounts_.size(); }

        void Clear();

        void Start() override;

        void Stop(const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME)
            override;

        const TimeStats Stats() const { return TimeStats(durationCounts_, outliersIgnoreRatio_); }

        void BusyWorkOnlyToHeatCache();

    private:
        std::string name_;
        std::vector<TimeCount_t> durationCounts_;
        double outliersIgnoreRatio_;
    };

    // If the scope is empty then this averages about 500 nanoseconds.
    class ScopedCollecterTimer
    {
    public:
        explicit ScopedCollecterTimer(TimeCollecter & timeCollecter)
            : timeCollecter_(timeCollecter)
        {
            timeCollecter_.Start();
        }

        ~ScopedCollecterTimer() { timeCollecter_.Stop(); }

        ScopedCollecterTimer(const ScopedCollecterTimer &) = delete;
        ScopedCollecterTimer(ScopedCollecterTimer &&) = delete;
        ScopedCollecterTimer & operator=(const ScopedCollecterTimer &) = delete;
        ScopedCollecterTimer & operator=(ScopedCollecterTimer &&) = delete;

    private:
        TimeCollecter & timeCollecter_;
    };

    // Keeps tracks a set of TimeCollecters over multiple contests and creates summaries of each and
    // also a summary of all contests.
    class TimeTrials
    {
    public:
        TimeTrials(
            const std::string & NAME,
            const TimeRes::Enum RES,
            const std::size_t TIME_SAMPLES_PER_CONTEST,
            const double OUTLIERS_IGNORE_RATIO);

        const std::string Name() const { return name_; }
        std::size_t CollectorCount() const { return collectors_.size(); }
        std::size_t TimeSampleCount() const;
        std::size_t ContestCount() const { return contestWinnerNamesHistory_.size(); }

        void ResetCurrentContest();

        // returns the index of the added TimeCollecter
        std::size_t AddCollecter(const std::string & NAME);

        void Start(const std::size_t COLLECTER_INDEX);

        void Stop(
            const std::size_t COLLECTER_INDEX,
            const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME);

        void BusyWorkOnlyToHeatTheCache();
        bool HasContestEnded();
        void EndContest();
        void ResetAllContests();
        void EndAllContests();

    private:
        std::string name_;
        TimeRes::Enum resolution_;
        std::size_t timeSamplesPerContest_;
        std::vector<TimeCollecter> collectors_;
        double outliersIgnoreRatio_;
        misc::VectorMap<std::string, TimeStatsVec_t> contestResultsHistoryMap_;
        std::vector<std::string> contestWinnerNamesHistory_;
    };

    // If the scope is empty then this averages about 500 nanoseconds.
    class ScopedContestTimer
    {
    public:
        ScopedContestTimer(TimeTrials & timeContest, const std::size_t COLLECTER_INDEX)
            : timeContest_(timeContest)
            , collecterIndex_(COLLECTER_INDEX)
        {
            timeContest_.Start(collecterIndex_);
        }

        ~ScopedContestTimer()
        {
            // capture the current (stop) time now because the distance between this point in the
            // code and where the stop time would normally be captured is longer than I would like
            // and might incur cache misses that throw the timing off.
            const auto ACTUAL_STOP_TIME { TimeSampler::Now() };

            timeContest_.Stop(collecterIndex_, ACTUAL_STOP_TIME);
        }

        ScopedContestTimer(const ScopedContestTimer &) = delete;
        ScopedContestTimer(ScopedContestTimer &&) = delete;
        ScopedContestTimer & operator=(const ScopedContestTimer &) = delete;
        ScopedContestTimer & operator=(ScopedContestTimer &&) = delete;

    private:
        TimeTrials & timeContest_;
        std::size_t collecterIndex_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TIMING_HPP_INCLUDED
