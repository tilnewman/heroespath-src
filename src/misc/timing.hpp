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
#include "misc/timing-scoped.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    struct TimeSampler
    {
        // If the times got mixed up and the resulting count is negative then
        // NOT_A_TIME_DURATION_COUNT is returned.
        static TimeCount_t Duration(
            const TimeRes::Enum RES,
            const TimeMoment_t & THEN,
            const TimeMoment_t & ACTUAL_STOP_TIME = DEFAULT_EMPTY_MOMENT_IN_TIME);

        static const TimeMoment_t DEFAULT_EMPTY_MOMENT_IN_TIME;
        static const TimeCount_t NOT_A_TIME_DURATION_COUNT;
    };

    // Wraps various statistics about a collection of time durations.  Negative time duration counts
    // will be thrown out.
    struct TimeStats
    {
        TimeStats() = default;

        // use this constructor to gather time statistics for a given vector of time duration counts
        TimeStats(
            const std::vector<TimeCount_t> & DURATION_COUNT_VEC_ORIG,
            const double OUTLIERS_IGNORE_RATIO,
            const std::size_t CANCELED_DUE_TO_LOGGING);

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
        std::size_t cancelOnLogCount_ = 0;
    };

    using TimeStatsVec_t = std::vector<TimeStats>;

    template <typename Key_t, typename Value_t>
    class VectorMap;

    using NameStatsVecMap_t = VectorMap<std::string, TimeStatsVec_t>;

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

#if !defined(HEROESPATH_MACRO_DISABLE_ALL) && !defined(HEROESPATH_MACRO_DISABLE_TIMING)

    // Use to find how many seconds/milliseconds/nanoseconds passes between Start() and Stop().
    class Timer
    {
    public:
        explicit Timer(const TimeRes::Enum RES, const bool WILL_CANCEL_ON_LOG);

        virtual ~Timer() = default;

        Timer(const Timer &) = default;
        Timer(Timer &&) = default;
        Timer & operator=(const Timer &) = default;
        Timer & operator=(Timer &&) = default;

        bool IsStarted() const { return isStarted_; }

        virtual void Start();

        virtual void Cancel();

        void Reset();

        virtual void
            Stop(const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME);

        TimeRes::Enum Resolution() const { return resolution_; }

        TimeCount_t Report() const { return durationCount_; }

        std::size_t CancelOnLogCount() const { return canceledDueToLogCount_; }

        void SetIsDoingBusyWork(const bool IS_DOING_BUSY_WORK)
        {
            isDoingBusyWork_ = IS_DOING_BUSY_WORK;
        }

    private:
        bool isStarted_;
        TimeRes::Enum resolution_;
        TimeMoment_t startTime_;
        TimeCount_t durationCount_;
        bool willCancelOnLog_;
        std::size_t longCountBeforeStart_;
        std::size_t canceledDueToLogCount_;
        bool isDoingBusyWork_;
    };

    // Same as Timer (above) except that whenever Stop() is called the duration is saved in a
    // collection that you can get statistical information about.
    //
    // 2018-11-11 Tested how many nanoseconds are reported if there is no code between Start() and
    // Stop() and it averaged about 160ns.
    class TimeCollecter : public Timer
    {
    public:
        TimeCollecter(
            const std::string & NAME,
            const TimeRes::Enum RES,
            const bool WILL_CANCEL_ON_LOG,
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

        void Reset();
        void Start() override;

        void Stop(const TimeMoment_t & ACTUAL_STOP_TIME = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME)
            override;

        const TimeStats Stats() const;

        void BusyWorkOnlyToHeatCache();

    private:
        std::string name_;
        std::vector<TimeCount_t> durationCounts_;
        double outliersIgnoreRatio_;
    };

    // Keeps tracks a set of TimeCollecters over multiple contests and creates summaries of each and
    // also a summary of all contests.
    class TimeTrials
    {
    public:
        TimeTrials(
            const std::string & NAME,
            const TimeRes::Enum RES,
            const bool WILL_CANCEL_ON_LOG,
            const std::size_t TIME_SAMPLES_PER_CONTEST,
            const double OUTLIERS_IGNORE_RATIO);

        ~TimeTrials();

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
        std::unique_ptr<NameStatsVecMap_t> contestResultsHistoryMapUPtr_;
        std::vector<std::string> contestWinnerNamesHistory_;
        bool willCancelOnLong_;
    };

#else

    class Timer
    {
    public:
        explicit Timer(const TimeRes::Enum) {}
        virtual ~Timer() = default;
        bool IsStarted() const { return false; }
        virtual void Start() {}
        virtual void Cancel() {}
        virtual void Stop(const TimeMoment_t &) {}
        TimeCount_t StopAndReport(const TimeMoment_t & JUNK) { return JUNK; }
        void Restart(const TimeMoment_t &) {}
        TimeCount_t RestartAndReport(const TimeMoment_t & JUNK) { return JUNK; }
        TimeRes::Enum Resolution() const { return TimeRes::Count; }
        TimeCount_t Report() const { return TimeCount_t(); }
    };

    class TimeCollecter : public Timer
    {
    public:
        TimeCollecter(const std::string &, const TimeRes::Enum, const std::size_t, const double) {}
        virtual ~TimeCollecter() = default;
        const std::string Name() const { return ""; }
        bool Empty() const { return true; }
        std::size_t Size() const { return 0; }
        void Clear() {}
        void Start() override {}
        void Stop(const TimeMoment_t &) override {}
        const TimeStats Stats() const { return TimeStats(); }
        void BusyWorkOnlyToHeatCache() {}
    };

    class TimeTrials
    {
    public:
        TimeTrials(const std::string &, const TimeRes::Enum, const std::size_t, const double) {}
        const std::string Name() const { return ""; }
        std::size_t CollectorCount() const { return 0; }
        std::size_t TimeSampleCount() const { return 0; }
        std::size_t ContestCount() const { return 0; }
        void ResetCurrentContest() {}
        std::size_t AddCollecter(const std::string &) { return 0; }
        void Start(const std::size_t) {}
        void Stop(const std::size_t, const TimeMoment_t &) {}
        void BusyWorkOnlyToHeatTheCache() {}
        bool HasContestEnded() { return false; }
        void EndContest() {}
        void ResetAllContests() {}
        void EndAllContests() {}
    };

#endif

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TIMING_HPP_INCLUDED
