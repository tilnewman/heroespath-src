// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// timing.cpp
//
#include "timing.hpp"

#include "misc/log-macros.hpp"
#include "misc/strings.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <utility>

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

namespace misc
{

    const TimeMoment_t TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME {};

    const TimeMoment_t TimeSampler::Now() { return std::chrono::high_resolution_clock::now(); }

    TimeCount_t TimeSampler::Duration(
        const TimeRes::Enum RES, const TimeMoment_t & THEN, const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        const auto NOW { (
            (ACTUAL_STOP_TIME == DEFAULT_EMPTY_MOMENT_IN_TIME) ? Now() : ACTUAL_STOP_TIME) };

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
            durationCount = std::chrono::duration_cast<std::chrono::seconds>(NOW - THEN).count();
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

    TimeStats::TimeStats(
        const std::vector<TimeCount_t> & DURATION_COUNT_VEC_ORIG,
        const double OUTLIERS_IGNORE_RATIO)
    {
        // all the code below depends on the collection being sorted from smallest to largest
        auto durationCountsToUse { DURATION_COUNT_VEC_ORIG };
        std::sort(std::begin(durationCountsToUse), std::end(durationCountsToUse));

        // remove any negative numbers
        durationCountsToUse.erase(
            std::remove_if(
                std::begin(durationCountsToUse),
                std::end(durationCountsToUse),
                [](const auto DURATION_COUNT) { return (DURATION_COUNT < 0); }),
            std::end(durationCountsToUse));

        num_before_outliers_removed = durationCountsToUse.size();

        if (0 == num_before_outliers_removed)
        {
            return;
        }

        // remove outliers
        outlier_ratio = OUTLIERS_IGNORE_RATIO;
        if ((OUTLIERS_IGNORE_RATIO > 0.0) && (durationCountsToUse.size() > 2))
        {
            // start with the outlierThrowOutCount based only on the ratio
            auto outlierThrowOutCount = static_cast<std::size_t>(
                OUTLIERS_IGNORE_RATIO * static_cast<double>(durationCountsToUse.size()));

            // reduce the outlierThrowOutCount if needed to prevent throwing out all values
            while ((outlierThrowOutCount != 0)
                   && ((outlierThrowOutCount * 2) >= durationCountsToUse.size()))
            {
                --outlierThrowOutCount;
            }

            // handle case where the outlierThrowOutCount based on the ratio was less than one
            if (0 == outlierThrowOutCount)
            {
                outlierThrowOutCount = 1;
            }

            const auto ITER_TO_FIRST_KEPT { std::begin(durationCountsToUse)
                                            + static_cast<std::ptrdiff_t>(outlierThrowOutCount) };

            const auto ITER_TO_LOWEST_OUTLIER_REMOVED { std::begin(durationCountsToUse)
                                                        + static_cast<std::ptrdiff_t>(
                                                            (durationCountsToUse.size() - 1)
                                                            - outlierThrowOutCount) };

            // find min/max of outliers to be removed
            outlier_min = *std::min_element(std::begin(durationCountsToUse), ITER_TO_FIRST_KEPT);

            outlier_max
                = *std::max_element(ITER_TO_LOWEST_OUTLIER_REMOVED, std::end(durationCountsToUse));

            // remove outliers
            durationCountsToUse.erase(std::begin(durationCountsToUse), ITER_TO_FIRST_KEPT);

            durationCountsToUse.erase(
                ITER_TO_LOWEST_OUTLIER_REMOVED, std::end(durationCountsToUse));
        }

        num = durationCountsToUse.size();

        if (0 == num)
        {
            return;
        }

        min = durationCountsToUse.front();
        max = durationCountsToUse.back();

        if (1 == num)
        {
            sum = min;
            dev = 0.0;
        }
        else
        {
            sum = std::accumulate(
                std::begin(durationCountsToUse), std::end(durationCountsToUse), 0);

            // calc the average
            using Math_t = long double;
            const auto AVERAGE_MATHT { static_cast<Math_t>(sum) / static_cast<Math_t>(num) };
            avg = static_cast<double>(AVERAGE_MATHT);

            // calc the standard deviation
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

        // reduce the precision of floating point numbers

        ReducePrecision(avg);
        ReducePrecision(dev);

        // figure out what the longest string lengths are
        longest_stat_string_length = 1;
        longest_sum_string_length = 1;

        auto updateLongestStringLengthFor = [&](const auto NUMBER, std::size_t & longest_length) {
            const auto STRING_LENGTH { misc::ToString(NUMBER).size() };

            if (STRING_LENGTH > longest_length)
            {
                longest_length = STRING_LENGTH;
            }
        };

        updateLongestStringLengthFor(num, longest_stat_string_length);
        updateLongestStringLengthFor(min, longest_stat_string_length);
        updateLongestStringLengthFor(max, longest_stat_string_length);
        updateLongestStringLengthFor(avg, longest_stat_string_length);
        updateLongestStringLengthFor(dev, longest_stat_string_length);
        updateLongestStringLengthFor(sum, longest_sum_string_length);
    }

    TimeStats::TimeStats(const std::vector<TimeStats> & STATS_VEC)
    {
        if (STATS_VEC.empty())
        {
            return;
        }

        outlier_ratio = STATS_VEC.front().outlier_ratio;

        outlier_min = STATS_VEC.front().outlier_min;
        outlier_max = STATS_VEC.front().outlier_max;

        TimeStats sums;

        for (const auto & STATS : STATS_VEC)
        {
            sums.num += STATS.num;
            sums.min += STATS.min;
            sums.max += STATS.max;
            sums.sum += STATS.sum;
            sums.avg += STATS.avg;
            sums.dev += STATS.dev;

            if (STATS.longest_stat_string_length > longest_stat_string_length)
            {
                longest_stat_string_length = STATS.longest_stat_string_length;
            }

            if (STATS.longest_sum_string_length > longest_sum_string_length)
            {
                longest_sum_string_length = STATS.longest_sum_string_length;
            }

            if (STATS.outlier_min < outlier_min)
            {
                outlier_min = STATS.outlier_min;
            }

            if (STATS.outlier_max < outlier_max)
            {
                outlier_max = STATS.outlier_max;
            }
        }

        const auto TOTAL_COUNT_DOUBLE { static_cast<double>(STATS_VEC.size()) };

        num = static_cast<std::size_t>(
            std::round(static_cast<double>(sums.num) / TOTAL_COUNT_DOUBLE));

        num_before_outliers_removed = num;

        min = static_cast<TimeCount_t>(
            std::round(static_cast<double>(sums.min) / TOTAL_COUNT_DOUBLE));

        max = static_cast<TimeCount_t>(
            std::round(static_cast<double>(sums.max) / TOTAL_COUNT_DOUBLE));

        sum = static_cast<TimeCount_t>(
            std::round(static_cast<double>(sums.sum) / TOTAL_COUNT_DOUBLE));

        avg = (sums.avg / TOTAL_COUNT_DOUBLE);
        dev = (sums.dev / TOTAL_COUNT_DOUBLE);

        ReducePrecision(avg);
        ReducePrecision(dev);
    }

    const std::string TimeStats::ToString(
        const bool WILL_INCLUDE_COUNT,
        const bool WILL_INCLUDE_SUM,
        const std::size_t FORCE_MIN_WIDTH) const
    {

        std::ostringstream ss;

        auto streamNumber = [&](const auto NUMBER, const std::size_t LONGEST_LENGTH) {
            int width { static_cast<int>(LONGEST_LENGTH) };

            if ((FORCE_MIN_WIDTH > 0) && (FORCE_MIN_WIDTH > LONGEST_LENGTH))
            {
                width = static_cast<int>(FORCE_MIN_WIDTH);
            }

            if (width > 0)
            {
                ss << std::setfill(' ') << std::setw(width);
            }

            ss << NUMBER;

            if (width > 0)
            {
                ss << std::setw(0);
            }
        };

        if (WILL_INCLUDE_COUNT)
        {
            streamNumber(num, longest_stat_string_length);
            ss << "x ";
        }

        ss << "[";
        streamNumber(min, longest_stat_string_length);
        ss << ", ";
        streamNumber(avg, longest_stat_string_length);
        ss << ", ";
        streamNumber(max, longest_stat_string_length);
        ss << "] (std_dev=";
        streamNumber(dev, longest_stat_string_length);

        if (WILL_INCLUDE_SUM)
        {
            ss << ", sum=" << sum;
        }

        ss << ")";

        if (outlier_ratio > 0.0)
        {
            ss << " (removed " << (num_before_outliers_removed - num) << " outliers ["
               << outlier_min << ", " << outlier_max << "])";
        }

        return ss.str();
    }

    void TimeStats::ReducePrecision(double & number)
    {
        if ((number < 10.0) == false)
        {
            // if >= 10 round to nearest integer
            number = std::round(number);
        }
        else if ((number < 2.0) == false)
        {
            // if (10, 2] round to nearest tenth
            number = std::round(number * 10.0) / 10.0;
        }
        else if (misc::IsRealZero(number))
        {
            number = 0;
        }
        else
        {
            // if < 2 round to nearest hundredth
            number = std::round(number * 100.0) / 100.0;
        }
    }

    IntervalTimer::IntervalTimer(const float DURATION)
        : timeSec_(0.0f)
        , durationSec_(DURATION)
    {}

    bool IntervalTimer::Update(const float ELAPSED_TIME_SEC)
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

    ScopedLogTimer::ScopedLogTimer(const TimeRes::Enum RES, const std::string & MESSAGE)
        : resolution_(RES)
        , message_(MESSAGE)
        , startTime_(std::chrono::high_resolution_clock::now())
    {}

    ScopedLogTimer::~ScopedLogTimer()
    {
        const auto DURATION { TimeSampler::Duration(resolution_, startTime_) };

        M_HP_LOG_DBG(
            "ScopedLogTimer \"" << message_ << "\" took " << DURATION
                                << TimeRes::ToString(resolution_));
    }

    Timer::Timer(const TimeRes::Enum RES)
        : isStarted_(false)
        , resolution_(RES)
        , startTime_()
        , durationCount_(0)
    {}

    void Timer::Start()
    {
        if (false == isStarted_)
        {
            // busy work only to heat the cache
            startTime_ = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME;

            isStarted_ = true;
            durationCount_ = 0;
            startTime_ = TimeSampler::Now();
        }
    }

    void Timer::Cancel()
    {
        if (isStarted_)
        {
            isStarted_ = false;
            durationCount_ = 0;
            startTime_ = TimeSampler::DEFAULT_EMPTY_MOMENT_IN_TIME;
        }
    }

    void Timer::Stop(const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        if (isStarted_)
        {
            durationCount_ = TimeSampler::Duration(resolution_, startTime_, ACTUAL_STOP_TIME);
            isStarted_ = false;
        }
    }

    TimeCount_t Timer::StopAndReport(const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        Stop(ACTUAL_STOP_TIME);
        return durationCount_;
    }

    void Timer::Restart(const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        Stop(ACTUAL_STOP_TIME);
        Start();
    }

    TimeCount_t Timer::RestartAndReport(const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        Restart(ACTUAL_STOP_TIME);
        return durationCount_;
    }

    TimeCollecter::TimeCollecter(
        const std::string & NAME,
        const TimeRes::Enum RES,
        const std::size_t EXPECTED_COUNT,
        const double OUTLIERS_IGNORE_RATIO)
        : Timer(RES)
        , name_(NAME)
        , durationCounts_()
        , outliersIgnoreRatio_(OUTLIERS_IGNORE_RATIO)
    {
        durationCounts_.reserve(EXPECTED_COUNT);
    }

    void TimeCollecter::Clear()
    {
        Cancel();
        const auto RESERVED { durationCounts_.capacity() };
        durationCounts_.clear();
        durationCounts_.reserve(RESERVED);
    }

    void TimeCollecter::Start()
    {
        if (IsStarted() == false)
        {
            BusyWorkOnlyToHeatCache();
            Timer::Start();
        }
    }

    void TimeCollecter::Stop(const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        if (IsStarted())
        {
            Timer::Stop(ACTUAL_STOP_TIME);
            durationCounts_.emplace_back(Report());
        }
    }

    void TimeCollecter::BusyWorkOnlyToHeatCache()
    {
        durationCounts_.emplace_back(0);
        Timer::Start();
        Timer::Stop();
        durationCounts_.pop_back();
    }

    TimeTrials::TimeTrials(
        const std::string & NAME,
        const TimeRes::Enum RES,
        const std::size_t TIME_SAMPLES_PER_CONTEST,
        const double OUTLIERS_IGNORE_RATIO)
        : name_(NAME)
        , resolution_(RES)
        , timeSamplesPerContest_(TIME_SAMPLES_PER_CONTEST)
        , collectors_()
        , outliersIgnoreRatio_(OUTLIERS_IGNORE_RATIO)
        , contestResultsHistoryMap_()
        , contestWinnerNamesHistory_()
    {
        collectors_.reserve(10);
    }

    std::size_t TimeTrials::TimeSampleCount() const
    {
        if (collectors_.empty())
        {
            return 0;
        }

        return collectors_.front().Size();
    }

    void TimeTrials::ResetCurrentContest()
    {
        for (auto & collector : collectors_)
        {
            collector.Clear();
        }
    }

    std::size_t TimeTrials::AddCollecter(const std::string & NAME)
    {
        collectors_.emplace_back(
            TimeCollecter(NAME, resolution_, timeSamplesPerContest_, outliersIgnoreRatio_));

        return collectors_.size() - 1;
    }

    void TimeTrials::Start(const std::size_t COLLECTER_INDEX)
    {
        if (HasContestEnded())
        {
            EndContest();
        }

        if (COLLECTER_INDEX < collectors_.size())
        {
            BusyWorkOnlyToHeatTheCache();
            collectors_[COLLECTER_INDEX].Start();
        }
    }

    void TimeTrials::Stop(const std::size_t COLLECTER_INDEX, const TimeMoment_t & ACTUAL_STOP_TIME)
    {
        if (COLLECTER_INDEX < collectors_.size())
        {
            collectors_[COLLECTER_INDEX].Stop(ACTUAL_STOP_TIME);
        }
    }

    void TimeTrials::BusyWorkOnlyToHeatTheCache()
    {
        for (auto & collecter : collectors_)
        {
            collecter.BusyWorkOnlyToHeatCache();
        }
    }

    bool TimeTrials::HasContestEnded()
    {
        if ((timeSamplesPerContest_ > 0) && !collectors_.empty())
        {
            return std::all_of(
                std::begin(collectors_), std::end(collectors_), [&](const auto & COLLECTOR) {
                    return (COLLECTOR.Size() >= timeSamplesPerContest_);
                });
        }
        else
        {
            return false;
        }
    }

    void TimeTrials::EndContest()
    {
        if (collectors_.empty())
        {
            ResetCurrentContest();
            return;
        }

        // create a collection that maps each collecter's name and stats
        //
        // along the way, get the length of the longest name for use in formatting summary
        // strings below
        using StatsNamePair_t = std::pair<TimeStats, std::string>;
        std::vector<StatsNamePair_t> statsNamePairs;
        std::size_t maxNameLength { 0 };

        for (const auto & COLLECTER : collectors_)
        {
            const auto COLLECTER_NAME { COLLECTER.Name() };
            const auto COLLECTER_NAME_SIZE { COLLECTER_NAME.size() };

            if (COLLECTER_NAME_SIZE > maxNameLength)
            {
                maxNameLength = COLLECTER_NAME_SIZE;
            }

            statsNamePairs.emplace_back(std::make_pair(COLLECTER.Stats(), COLLECTER_NAME));
        }

        // sort that collection by lowest time average (or time sum if the averages match)
        std::sort(
            std::begin(statsNamePairs),
            std::end(statsNamePairs),
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

        // add the contest winner's name to the history
        contestWinnerNamesHistory_.emplace_back(statsNamePairs.front().second);

        // add all the collecter's end-of-contest stats to the history
        for (const auto & STATS_NAME_PAIR : statsNamePairs)
        {
            contestResultsHistoryMap_[STATS_NAME_PAIR.second].emplace_back(STATS_NAME_PAIR.first);
        }

        // find the longest string length
        int longestStatStringLength { 0 };
        int longestSumStringLength { 0 };
        for (const auto & STATS_NAME_PAIR : statsNamePairs)
        {
            const auto NEXT_LONGEST_STAT_INT { static_cast<int>(
                STATS_NAME_PAIR.first.longest_stat_string_length) };

            if (longestStatStringLength < NEXT_LONGEST_STAT_INT)
            {
                longestStatStringLength = NEXT_LONGEST_STAT_INT;
            }

            const auto NEXT_LONGEST_SUM_INT { static_cast<int>(
                STATS_NAME_PAIR.first.longest_sum_string_length) };

            if (longestSumStringLength < NEXT_LONGEST_SUM_INT)
            {
                longestSumStringLength = NEXT_LONGEST_SUM_INT;
            }
        }

        // create the end-of-contest summary string
        std::ostringstream ss;
        ss << std::setfill(' ');
        ss << name_ << " Time Contest Results in " << TimeRes::ToString(resolution_) << ":";

        for (std::size_t index(0); index < statsNamePairs.size(); ++index)
        {
            const auto & STATS_NAME_PAIR_CURR { statsNamePairs.at(index) };

            if (statsNamePairs.size() > 1)
            {
                ss << "\n";
            }

            ss << "\t#" << (index + 1) << " " << std::right
               << std::setw(static_cast<int>(maxNameLength)) << STATS_NAME_PAIR_CURR.second
               << ": avg=" << std::right << std::setw(longestStatStringLength)
               << STATS_NAME_PAIR_CURR.first.avg << ", sum=" << std::setw(longestSumStringLength)
               << STATS_NAME_PAIR_CURR.first.sum << "  "
               << STATS_NAME_PAIR_CURR.first.ToString(
                      true, false, static_cast<std::size_t>(longestStatStringLength));
        }

        M_HP_LOG_DBG(ss.str());
        ResetCurrentContest();
    }

    void TimeTrials::ResetAllContests()
    {
        ResetCurrentContest();
        contestResultsHistoryMap_.Clear();
        contestWinnerNamesHistory_.clear();
    }

    void TimeTrials::EndAllContests()
    {
        if (contestResultsHistoryMap_.Empty())
        {
            ResetAllContests();
            return;
        }

        // collect summaries of each contest
        struct CollectorContestSummary
        {
            std::string name;
            std::size_t win_count;
            TimeStats stats;
        };

        std::vector<CollectorContestSummary> summaries;
        std::size_t maxNameLength { 0 };

        for (const auto & COLLECTER : collectors_)
        {
            CollectorContestSummary summary;

            summary.name = COLLECTER.Name();

            const auto COLLECTER_NAME_SIZE { summary.name.size() };

            if (COLLECTER_NAME_SIZE > maxNameLength)
            {
                maxNameLength = COLLECTER_NAME_SIZE;
            }

            summary.win_count = static_cast<std::size_t>(std::count(
                std::begin(contestWinnerNamesHistory_),
                std::end(contestWinnerNamesHistory_),
                summary.name));

            summary.stats = TimeStats(contestResultsHistoryMap_[summary.name]);

            summaries.emplace_back(summary);
        }

        // sort contest summaries by highest win_count or lowest average time duration
        std::sort(std::begin(summaries), std::end(summaries), [](const auto & A, const auto & B) {
            if (A.win_count != B.win_count)
            {
                return (A.win_count > B.win_count);
            }
            else
            {
                return (A.stats.avg < B.stats.avg);
            }
        });

        // find the longest string length
        int longestStatStringLength { 0 };
        int longestSumStringLength { 0 };
        for (const auto & SUMMARY : summaries)
        {
            const auto NEXT_LONGEST_STAT_INT { static_cast<int>(
                SUMMARY.stats.longest_stat_string_length) };

            if (longestStatStringLength < NEXT_LONGEST_STAT_INT)
            {
                longestStatStringLength = NEXT_LONGEST_STAT_INT;
            }

            const auto NEXT_LONGEST_SUM_INT { static_cast<int>(
                SUMMARY.stats.longest_sum_string_length) };

            if (longestSumStringLength < NEXT_LONGEST_SUM_INT)
            {
                longestSumStringLength = NEXT_LONGEST_SUM_INT;
            }
        }

        std::ostringstream ss;
        ss << std::setfill(' ');
        ss << name_ << " Time Contests Final Results in " << TimeRes::ToString(resolution_)
           << " after " << contestResultsHistoryMap_.Size() << " contests:";

        const auto MAX_NAME_LENGTH_INT { static_cast<int>(maxNameLength) };

        for (std::size_t index(0); index < summaries.size(); ++index)
        {
            const auto & SUMMARY_CURR { summaries.at(index) };

            if (summaries.size() > 1)
            {
                ss << "\n";
            }

            ss << "\tFinal #" << (index + 1) << " " << std::left << std::setw(MAX_NAME_LENGTH_INT)
               << SUMMARY_CURR.name << " with wins=" << std::right << std::setw(3)
               << SUMMARY_CURR.win_count << ", avg_avg=" << std::setw(longestStatStringLength)
               << SUMMARY_CURR.stats.avg << ", sum_avg=" << std::setw(longestSumStringLength)
               << SUMMARY_CURR.stats.sum
               << SUMMARY_CURR.stats.ToString(
                      true, true, static_cast<std::size_t>(longestStatStringLength));
        }

        M_HP_LOG_WRN(ss.str());
        ResetAllContests();
    }

} // namespace misc
} // namespace heroespath
