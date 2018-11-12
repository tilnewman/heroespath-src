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
#include "misc/log-macros.hpp"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
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

    static const std::string ToString(const Enum RES)
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
            case Second:
            {
                return "sec";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(RES) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }
};

namespace misc
{
    namespace timing
    {
        inline const TimeMoment_t Now() { return std::chrono::high_resolution_clock::now(); }

        inline TimeCount_t Duration(
            const TimeRes::Enum RES,
            const TimeMoment_t & THEN,
            const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t())
        {
            const auto NOW = [&]() {
                if (ACTUAL_STOP_TIME == TimeMoment_t())
                {
                    return Now();
                }
                else
                {
                    return ACTUAL_STOP_TIME;
                }
            }();

            TimeCount_t durationCount;

            if (RES == TimeRes::Nano)
            {
                durationCount
                    = std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - THEN).count();
            }
            else if (RES == TimeRes::Milli)
            {
                durationCount
                    = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - THEN).count();
            }
            else
            {
                durationCount
                    = std::chrono::duration_cast<std::chrono::seconds>(NOW - THEN).count();
            }

            if (durationCount < 0)
            {
                return 0;
            }
            else
            {
                return durationCount;
            }
        }
    } // namespace timing

    // Use with the game engine's UpdateTime(float ELAPSED_TIME_SEC) functions to repeatedly check
    // if a given amount of time in seconds has elapsed.
    class IntervalTimer
    {
    public:
        explicit IntervalTimer(const float DURATION)
            : timeSec_(0.0f)
            , durationSec_(DURATION)
        {}

        float Time() const { return timeSec_; }
        float Duration() const { return durationSec_; }
        void Reset() { timeSec_ = 0.0f; }

        bool Update(const float ELAPSED_TIME_SEC)
        {
            timeSec_ += ELAPSED_TIME_SEC;

            if (timeSec_ > durationSec_)
            {
                timeSec_ = 0.0f;
                return true;
            }
            else
            {
                return false;
            }
        }

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
        ScopedLogTimer(const TimeRes::Enum RES, const std::string & MESSAGE)
            : resolution_(RES)
            , message_(MESSAGE)
            , startTime_(std::chrono::high_resolution_clock::now())
        {}

        ~ScopedLogTimer()
        {
            const auto DURATION { timing::Duration(resolution_, startTime_) };

            M_HP_LOG_DBG(
                "ScopedLogTimer \"" << message_ << "\" took " << DURATION
                                    << TimeRes::ToString(resolution_));
        }

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
        explicit Timer(const TimeRes::Enum RES)
            : isStarted_(false)
            , resolution_(RES)
            , startTime_()
            , durationCount_(0)
        {}

        virtual ~Timer() = default;

        Timer(const Timer &) = default;
        Timer(Timer &&) = default;
        Timer & operator=(const Timer &) = default;
        Timer & operator=(Timer &&) = default;

        bool IsStarted() const { return isStarted_; }

        virtual void Start()
        {
            if (false == isStarted_)
            {
                // busy work only to heat the cache
                startTime_ = TimeMoment_t();

                isStarted_ = true;
                durationCount_ = 0;
                startTime_ = timing::Now();
            }
        }

        virtual void Stop(const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t())
        {
            if (isStarted_)
            {
                durationCount_ = timing::Duration(resolution_, startTime_, ACTUAL_STOP_TIME);
                isStarted_ = false;
            }
        }

        TimeCount_t StopAndReport(const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t())
        {
            Stop(ACTUAL_STOP_TIME);
            return durationCount_;
        }

        void Restart(const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t())
        {
            Stop(ACTUAL_STOP_TIME);
            Start();
        }

        TimeCount_t RestartAndReport(const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t())
        {
            Restart(ACTUAL_STOP_TIME);
            return durationCount_;
        }

        TimeRes::Enum Resolution() const { return resolution_; }

        TimeCount_t Report() const { return durationCount_; }

    private:
        bool isStarted_;
        TimeRes::Enum resolution_;
        TimeMoment_t startTime_;
        TimeCount_t durationCount_;
    };

    // Wraps various statistics about a collection of time durations.  Negative time duration counts
    // will be thrown out.
    struct TimeStats
    {
        TimeStats() = default;

        TimeStats(
            const std::vector<TimeCount_t> & DURATION_COUNT_VEC_ORIG,
            const double OUTLIERS_IGNORE_RATIO = 0.0)
        {
            if (DURATION_COUNT_VEC_ORIG.empty())
            {
                return;
            }

            outlier_ratio = OUTLIERS_IGNORE_RATIO;
            num_orig = DURATION_COUNT_VEC_ORIG.size();

            auto durationCountsToUse { DURATION_COUNT_VEC_ORIG };
            std::sort(std::begin(durationCountsToUse), std::end(durationCountsToUse));

            if ((OUTLIERS_IGNORE_RATIO > 0.0) && (durationCountsToUse.size() > 2))
            {
                std::size_t outlierThrowOutCount { 0 };

                outlierThrowOutCount = static_cast<std::size_t>(
                    OUTLIERS_IGNORE_RATIO * static_cast<double>(durationCountsToUse.size()));

                if (outlierThrowOutCount < 1)
                {
                    outlierThrowOutCount = 1;
                }

                if ((outlierThrowOutCount * 2) >= durationCountsToUse.size())
                {
                    outlierThrowOutCount = 1;
                }

                durationCountsToUse.erase(
                    std::begin(durationCountsToUse),
                    std::begin(durationCountsToUse)
                        + static_cast<std::ptrdiff_t>(outlierThrowOutCount));

                while ((outlierThrowOutCount > 0) && !durationCountsToUse.empty())
                {
                    durationCountsToUse.pop_back();
                    --outlierThrowOutCount;
                }
            }

            num = 0;
            min = durationCountsToUse.front();
            max = durationCountsToUse.front();
            sum = 0;

            for (const auto DURATION_COUNT : durationCountsToUse)
            {
                if (DURATION_COUNT < 0)
                {
                    continue;
                }

                ++num;

                if (DURATION_COUNT < min)
                {
                    min = DURATION_COUNT;
                }

                if (DURATION_COUNT > max)
                {
                    max = DURATION_COUNT;
                }

                sum += DURATION_COUNT;
            }

            if (0 == num)
            {
                num = 0;
                min = 0;
                max = 0;
                sum = 0;
                avg = 0.0;
                dev = 0.0;
                return;
            }

            using Math_t = long double;

            const auto AVERAGE_MATHT { static_cast<Math_t>(sum) / static_cast<Math_t>(num) };

            avg = static_cast<double>(AVERAGE_MATHT);

            if (num <= 1)
            {
                dev = 0.0;
                return;
            }

            const auto DEVIATION_SUM_MATHT { std::accumulate(
                std::begin(durationCountsToUse),
                std::begin(durationCountsToUse) + static_cast<std::ptrdiff_t>(num),
                Math_t(0),
                [&](const Math_t SUM_SO_FAR, const auto DURATION_COUNT) {
                    return SUM_SO_FAR
                        + std::pow(
                               (static_cast<Math_t>(DURATION_COUNT) - AVERAGE_MATHT), Math_t(2));
                }) };

            dev = static_cast<double>(
                std::sqrt(DEVIATION_SUM_MATHT / static_cast<Math_t>(num - 1)));
        }

        TimeStats(const TimeStats &) = default;
        TimeStats(TimeStats &&) = default;
        TimeStats & operator=(const TimeStats &) = default;
        TimeStats & operator=(TimeStats &&) = default;

        const std::string ToString(
            const bool WILL_INCLUDE_COUNT,
            const bool WILL_INCLUDE_SUM,
            const std::size_t WIDTH_ORIG) const
        {
            std::ostringstream ss;
            ss << std::setfill(' ');

            auto streamNumber = [&](const auto NUMBER) {
                const auto WIDTH { static_cast<int>(WIDTH_ORIG) };

                if (WIDTH != 0)
                {
                    ss << std::fixed << std::setw(WIDTH) << std::setprecision(WIDTH);
                }

                if (NUMBER < 10)
                {
                    ss << NUMBER;
                }
                else
                {
                    ss << static_cast<long long>(NUMBER);
                }

                if (WIDTH != 0)
                {
                    ss << std::setw(0) << std::setprecision(0);
                }
            };

            if (WILL_INCLUDE_COUNT)
            {
                streamNumber(num);
                ss << "x ";
            }

            ss << "[";
            streamNumber(min);
            ss << ", ";
            streamNumber(avg);
            ss << ", ";
            streamNumber(max);
            ss << "] (std_dev=";
            streamNumber(dev);

            if (WILL_INCLUDE_SUM)
            {
                ss << ", sum=" << sum;
            }

            ss << ")";

            if (outlier_ratio > 0.0)
            {
                ss << "(outlier_ratio=" << std::fixed << std::setw(3) << std::setprecision(3)
                   << outlier_ratio << " eliminated " << (num_orig - num) << ")";
            }

            return ss.str();
        }

        std::size_t num = 0;
        TimeCount_t min = 0;
        TimeCount_t max = 0;
        TimeCount_t sum = 0;
        double avg = 0.0;
        double dev = 0.0;
        double outlier_ratio = 0.0;
        std::size_t num_orig = 0;
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
            const double OUTLIERS_IGNORE_RATIO = 0.0)
            : Timer(RES)
            , name_(NAME)
            , durationCounts_()
            , outliersIgnoreRatio_(OUTLIERS_IGNORE_RATIO)
            , isCacheValid_(true)
            , cachedStats_()
        {
            durationCounts_.reserve(EXPECTED_COUNT);
        }

        virtual ~TimeCollecter() = default;

        TimeCollecter(const TimeCollecter &) = default;
        TimeCollecter(TimeCollecter &&) = default;
        TimeCollecter & operator=(const TimeCollecter &) = default;
        TimeCollecter & operator=(TimeCollecter &&) = default;

        const std::string Name() const { return name_; }

        bool Empty() const { return durationCounts_.empty(); }

        std::size_t Size() const { return durationCounts_.size(); }

        void Clear()
        {
            isCacheValid_ = false;
            const auto RESERVED { durationCounts_.capacity() };
            durationCounts_.clear();
            durationCounts_.reserve(RESERVED);
        }

        void Start() override
        {
            BusyWorkOnlyToHeatCache();
            Timer::Start();
        }

        void Stop(const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t()) override
        {
            Timer::Stop(ACTUAL_STOP_TIME);
            durationCounts_.emplace_back(Report());
            isCacheValid_ = false;
        }

        const TimeStats Stats() const
        {
            if (false == isCacheValid_)
            {
                cachedStats_ = TimeStats(durationCounts_, outliersIgnoreRatio_);
                isCacheValid_ = true;
            }

            return cachedStats_;
        }

        void BusyWorkOnlyToHeatCache()
        {
            durationCounts_.emplace_back(0);
            Timer::Start();
            Timer::Stop();
            durationCounts_.pop_back();
        }

    private:
        std::string name_;
        std::vector<TimeCount_t> durationCounts_;
        double outliersIgnoreRatio_;
        mutable bool isCacheValid_;
        mutable TimeStats cachedStats_;
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

    // Keeps track of multiple TimeCollecters and creates comparative reports.
    class TimeContest
    {
    public:
        TimeContest(
            const std::string & NAME,
            const TimeRes::Enum RES,
            const std::size_t EXPECTED_COUNT,
            const double OUTLIERS_IGNORE_RATIO = 0.0)
            : name_(NAME)
            , resolution_(RES)
            , expectedCountToReserve_(EXPECTED_COUNT)
            , collectors_()
            , outliersIgnoreRatio_(OUTLIERS_IGNORE_RATIO)
        {
            collectors_.reserve(10);
        }

        const std::string Name() const { return name_; }

        std::size_t CollectorCount() const { return collectors_.size(); }

        std::size_t DurationCount() const
        {
            if (collectors_.empty())
            {
                return 0;
            }

            return collectors_.front().Size();
        }

        void Reset()
        {
            for (auto & collector : collectors_)
            {
                collector.Clear();
            }
        }

        // returns the index of the added TimeCollecter
        std::size_t AddCollecter(const std::string & NAME)
        {
            collectors_.emplace_back(
                TimeCollecter(NAME, resolution_, expectedCountToReserve_, outliersIgnoreRatio_));
            return collectors_.size() - 1;
        }

        void Start(const std::size_t COLLECTER_INDEX)
        {
            BusyWorkOnlyToHeatTheCache();

            if (COLLECTER_INDEX < collectors_.size())
            {
                collectors_[COLLECTER_INDEX].Start();
            }
        }

        void Stop(
            const std::size_t COLLECTER_INDEX,
            const TimeMoment_t & ACTUAL_STOP_TIME = TimeMoment_t())
        {
            if (COLLECTER_INDEX < collectors_.size())
            {
                collectors_[COLLECTER_INDEX].Stop(ACTUAL_STOP_TIME);
            }
        }

        void BusyWorkOnlyToHeatTheCache()
        {
            for (auto & collecter : collectors_)
            {
                collecter.BusyWorkOnlyToHeatCache();
            }
        }

        const std::string ToString(const std::size_t WIDTH) const
        {
            using StatNamePair_t = std::pair<TimeStats, std::string>;
            std::vector<StatNamePair_t> statNamePairs;

            std::size_t maxNameLength { 0 };
            for (const auto & COLLECTER : collectors_)
            {
                if (COLLECTER.Name().size() > maxNameLength)
                {
                    maxNameLength = COLLECTER.Name().size();
                }

                statNamePairs.emplace_back(std::make_pair(COLLECTER.Stats(), COLLECTER.Name()));
            }

            std::sort(
                std::begin(statNamePairs),
                std::end(statNamePairs),
                [](const auto & A, const auto & B) {
                    if (misc::IsRealClose(A.first.avg, B.first.avg))
                    {
                        return (A.first.sum < B.first.sum);
                    }
                    else
                    {
                        return (A.first.avg < B.first.avg);
                    }
                });

            std::ostringstream ss;
            ss << std::setfill(' ');
            ss << name_ << " Time Contest Results:";

            for (std::size_t index(0); index < statNamePairs.size(); ++index)
            {
                const auto & STAT_NAME_PAIR_CURR { statNamePairs.at(index) };

                ss << "\n\t#" << (index + 1) << " " << std::setw(static_cast<int>(maxNameLength))
                   << std::left << STAT_NAME_PAIR_CURR.second << " at "
                   << std::setw(static_cast<int>(WIDTH + 1)) << STAT_NAME_PAIR_CURR.first.sum
                   << TimeRes::ToString(resolution_);

                if (index < (statNamePairs.size() - 1))
                {
                    const auto & STAT_NAME_PAIR_NEXT { statNamePairs.at(index + 1) };

                    ss << " (by " << std::setw(static_cast<int>(WIDTH + 1))
                       << (STAT_NAME_PAIR_NEXT.first.sum - STAT_NAME_PAIR_CURR.first.sum) << ")";
                }
                else
                {
                    ss << std::string(7 + WIDTH, ' ');
                }

                ss << "  " << STAT_NAME_PAIR_CURR.first.ToString(true, false, WIDTH);
            }

            return ss.str();
        }

    private:
        std::string name_;
        TimeRes::Enum resolution_;
        std::size_t expectedCountToReserve_;
        std::vector<TimeCollecter> collectors_;
        double outliersIgnoreRatio_;
    };

    // If the scope is empty then this averages about 500 nanoseconds.
    class ScopedContestTimer
    {
    public:
        ScopedContestTimer(TimeContest & timeContest, const std::size_t COLLECTER_INDEX)
            : timeContest_(timeContest)
            , collecterIndex_(COLLECTER_INDEX)
        {
            timeContest_.Start(collecterIndex_);
        }

        ~ScopedContestTimer()
        {
            const auto ACTUAL_STOP_TIME { timing::Now() };
            timeContest_.Stop(collecterIndex_, ACTUAL_STOP_TIME);
        }

        ScopedContestTimer(const ScopedContestTimer &) = delete;
        ScopedContestTimer(ScopedContestTimer &&) = delete;
        ScopedContestTimer & operator=(const ScopedContestTimer &) = delete;
        ScopedContestTimer & operator=(ScopedContestTimer &&) = delete;

    private:
        TimeContest & timeContest_;
        std::size_t collecterIndex_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TIMING_HPP_INCLUDED
