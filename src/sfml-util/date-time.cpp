// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// date-time.cpp
//
#include "date-time.hpp"

//suppress warnings that are safe to ignore in boost
#include "misc/platform.hpp"
#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef PLATFORM_DETECTED_IS_APPLE
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
