// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// date-time.cpp
//
#include "date-time.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iomanip>
#include <sstream>

namespace heroespath
{
namespace gui
{

    Date::Date(const int YEAR, const int MONTH, const int DAY)
        : year(YEAR)
        , month(MONTH)
        , day(DAY)
    {}

    const std::string Date::ToString() const
    {
        std::string str;
        str.reserve(32);
        str += std::to_string(year);
        str += '-';
        str += std::to_string(month);
        str += '-';
        str += std::to_string(day);
        return str;
    }

    bool Date::IsValid() const { return ((year > 0) && (month > 0) && (day > 0)); }

    const Date Date::CurrentDate()
    {
        const auto DATE { boost::posix_time::second_clock::local_time().date() };
        return Date(DATE.year(), DATE.month(), DATE.day());
    }

    Time::Time(const int HOURS, const int MINUTES, const int SECONDS, const int SUBSECONDS)
        : hours(HOURS)
        , minutes(MINUTES)
        , seconds(SECONDS)
        , milliseconds(SUBSECONDS)
    {}

    Time::Time(const sf::Time & TIME_OBJ)
        : hours(0)
        , minutes(0)
        , seconds(0)
        , milliseconds(0)
    {
        const auto SECONDS_FLOAT { TIME_OBJ.asSeconds() };
        const auto SECONDS_INT { static_cast<int>(SECONDS_FLOAT) };
        hours = SECONDS_INT / SECONDS_IN_HOUR_;
        minutes = (SECONDS_INT % SECONDS_IN_HOUR_) / SECONDS_IN_MINUTE_;
        seconds = (SECONDS_INT - (hours * SECONDS_IN_HOUR_)) - (minutes * SECONDS_IN_MINUTE_);
        milliseconds = static_cast<int>((SECONDS_FLOAT - std::floor(SECONDS_FLOAT)) * 1000.0f);
    }

    const std::string Time::ToString() const
    {
        std::ostringstream ss;

        ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":"
           << std::setw(2) << seconds << "." << std::setw(2) << milliseconds;

        return ss.str();
    }

    bool Time::IsValid() const
    {
        return ((hours >= 0) && (minutes >= 0) && (seconds >= 0) && (milliseconds >= 0));
    }

    const Time Time::CurrentTime()
    {
        const auto TOD { boost::posix_time::microsec_clock::local_time().time_of_day() };

        return Time(
            static_cast<int>(TOD.hours()),
            static_cast<int>(TOD.minutes()),
            static_cast<int>(TOD.seconds()),
            static_cast<int>(
                TOD.total_milliseconds()
                - (static_cast<std::int64_t>(TOD.total_seconds())
                   * static_cast<std::int64_t>(1000))));
    }

    // simple wrapper for date and time
    DateTime::DateTime(const Date & DATE, const Time & TIME)
        : date(DATE)
        , time(TIME)
    {}

    const std::string DateTime::ToString() const { return date.ToString() + " " + time.ToString(); }

} // namespace gui
} // namespace heroespath
