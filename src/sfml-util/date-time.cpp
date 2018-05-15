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

// suppress warnings that are safe to ignore in boost
#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wswitch-enum"
#endif

#include <iomanip>
#include <iostream>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    Date::Date(const int YEAR, const int MONTH, const int DAY)
        : year(YEAR)
        , month(MONTH)
        , day(DAY)
    {}

    const std::string Date::ToString() const
    {
        std::ostringstream ss;
        ss << year << "-" << month << "-" << day;
        return ss.str();
    }

    bool Date::IsValid() const { return ((year > 0) && (month > 0) && (day > 0)); }

    const Date Date::CurrentDate()
    {
        auto const DATE{ boost::posix_time::microsec_clock::local_time().date() };
        return Date(DATE.year(), DATE.month(), DATE.day());
    }

    bool operator==(const Date & L, const Date & R)
    {
        return ((L.year == R.year) && (L.month == R.month) && (L.day == R.day));
    }

    Time::Time(const int HOURS, const int MINUTES, const int SECONDS)
        : hours(HOURS)
        , minutes(MINUTES)
        , seconds(SECONDS)
    {}

    Time::Time(const sf::Time & TIME_OBJ)
        : hours(0)
        , minutes(0)
        , seconds(0)
    {
        auto const SECONDS_INT{ static_cast<int>(TIME_OBJ.asSeconds()) };
        hours = SECONDS_INT / SECONDS_IN_HOUR_;
        minutes = (SECONDS_INT % SECONDS_IN_HOUR_) / SECONDS_IN_MINUTE_;
        seconds = (SECONDS_INT - (hours * SECONDS_IN_HOUR_)) - (minutes * SECONDS_IN_MINUTE_);

        if (seconds < 0)
        {
            seconds = 0;
        }
    }

    const std::string Time::ToString() const
    {
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":"
           << std::setw(2) << seconds;

        return ss.str();
    }

    bool Time::IsValid() const { return ((hours >= 0) && (minutes >= 0) && (seconds >= 0)); }

    const Time Time::CurrentTime()
    {
        auto const TOD{ boost::posix_time::microsec_clock::local_time().time_of_day() };

        return Time(
            static_cast<int>(TOD.hours()),
            static_cast<int>(TOD.minutes()),
            static_cast<int>(TOD.seconds()));
    }

    // simple wrapper for date and time
    DateTime::DateTime(const Date & DATE, const Time & TIME)
        : date(DATE)
        , time(TIME)
    {}

    const std::string DateTime::ToString() const
    {
        std::ostringstream ss;
        ss << date.ToString() << " " << time.ToString();
        return ss.str();
    }
} // namespace sfml_util
} // namespace heroespath
