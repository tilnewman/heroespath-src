//
// date-time.cpp
//
#include "date-time.hpp"

//suppress warnings that are safe to ignore in boost
#include "utilz/platform.hpp"
#ifdef PPLATFFORMDDETECT__APPPPLE_OOSS
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef PPLATFFORMDDETECT__APPPPLE_OOSS
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wswitch-enum"
#endif

#include <iostream>
#include <iomanip>
#include <sstream>


namespace sfml_util
{

    Date::Date(const int YEAR,
               const int MONTH,
               const int DAY)
    :
        year(YEAR),
        month(MONTH),
        day(DAY)
    {}


    const std::string Date::ToString() const
    {
        std::ostringstream ss;
        ss << year << "-" << month << "-" << day;
        return ss.str();
    }


    bool Date::IsValid() const
    {
        return ((year > 0) && (month > 0) && (day > 0));
    }


    const Date Date::CurrentDate()
    {
        const boost::posix_time::ptime PTIME(boost::posix_time::microsec_clock::local_time());
        return Date(PTIME.date().year(), PTIME.date().month(), PTIME.date().day());
    }


    bool operator==(const Date & L, const Date & R)
    {
        return ((L.year == R.year) &&
                (L.month == R.month) &&
                (L.day == R.day));
    }


    Time::Time(const int HOURS,
               const int MINUTES,
               const int SECONDS)
    :
        hours(HOURS),
        minutes(MINUTES),
        seconds(SECONDS)
    {}


    Time::Time(const sf::Time & TIME_OBJ)
    :
        hours(0),
        minutes(0),
        seconds(0)
    {
        const int SECONDS_INT(static_cast<int>(TIME_OBJ.asSeconds()));
        hours = SECONDS_INT / SECONDS_IN_HOUR_;
        minutes = (SECONDS_INT % SECONDS_IN_HOUR_) / SECONDS_IN_MINUTE_;
        seconds = (SECONDS_INT - (hours * SECONDS_IN_HOUR_)) - (minutes * SECONDS_IN_MINUTE_);
        if (seconds < 0)
            seconds = 0;
    }


    const std::string Time::ToString() const
    {
        std::ostringstream ss;
        ss << std::setfill('0')
           << std::setw(2) << hours << ":"
           << std::setw(2) << minutes << ":"
           << std::setw(2) << seconds;

        return ss.str();
    }


    bool Time::IsValid() const
    {
        return ((hours >= 0) && (minutes >= 0) && (seconds >= 0));
    }


    const Time Time::CurrentTime()
    {
        const boost::posix_time::ptime PTIME(boost::posix_time::microsec_clock::local_time());
        return Time(PTIME.time_of_day().hours(), PTIME.time_of_day().minutes(), PTIME.time_of_day().seconds());
    }


    //simple wrapper for date and time
    DateTime::DateTime(const Date & DATE, const Time & TIME)
    :
        date(DATE),
        time(TIME)
    {}


    const std::string DateTime::ToString() const
    {
        std::ostringstream ss;
        ss << date.ToString() << " " << time.ToString();
        return ss.str();
    }

}
