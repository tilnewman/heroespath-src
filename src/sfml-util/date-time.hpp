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
#ifndef HEROESPATH_SFMLUTIL_DATETIME_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_DATETIME_HPP_INCLUDED
//
// date-time.hpp
//  Simple wrapper structs for holding date and time.
//
#include "misc/boost-serialize-includes.hpp"

#include <SFML/System/Time.hpp>

#include <string>
#include <tuple>


namespace heroespath
{
namespace sfml_util
{

    //simple wrapper for dates
    struct Date
    {
        explicit Date(const int YEAR  = INVALID_VALUE_,
                      const int MONTH = INVALID_VALUE_,
                      const int DAY   = INVALID_VALUE_);

        const std::string ToString() const;

        bool IsValid() const;

        static const Date CurrentDate();

        static const int INVALID_VALUE_ = -1;

        int year;
        int month;
        int day;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & year;
            ar & month;
            ar & day;
        }
    };

    bool operator==(const Date & L, const Date & R);

    inline bool operator!=(const Date & L, const Date & R)
    {
        return ! (L == R);
    }

    inline bool operator<(const Date & L, const Date & R)
    {
        return std::tie(L.year, L.month, L.day) < std::tie(R.year, R.month, R.day);
    }


    //simple wrapper for times
    struct Time
    {
        explicit Time(const int HOUR   = INVALID_VALUE_,
                      const int MINUTE = INVALID_VALUE_,
                      const int SECOND = INVALID_VALUE_);

        explicit Time(const sf::Time & TIME_OBJ);

        const std::string ToString() const;

        bool IsValid() const;

        static const Time CurrentTime();

        int hours;
        int minutes;
        int seconds;

        static const int INVALID_VALUE_     = -1;
        static const int SECONDS_IN_MINUTE_ = 60;
        static const int SECONDS_IN_HOUR_   = (60 * 60);
        static const int SECONDS_IN_DAY_    = (60 * 60 * 24);
        static const int MINUTES_IN_HOUR_   = 60;
        static const int MINUTES_IN_DAY_    = (60 * 24);
        static const int HOURS_IN_DAY_      = 24;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & hours;
            ar & minutes;
            ar & seconds;
        }
    };

    inline bool operator==(const Time & L, const Time & R)
    {
        return std::tie(L.hours, L.minutes, L.seconds) == std::tie(R.hours, R.minutes, R.seconds);
    }

    inline bool operator!=(const Time & L, const Time & R)
    {
        return ! (L == R);
    }

    inline bool operator<(const Time & L, const Time & R)
    {
        return std::tie(L.hours, L.minutes, L.seconds) < std::tie(R.hours, R.minutes, R.seconds);
    }


    //simple wrapper for date and time
    class DateTime
    {
    public:
       explicit DateTime(const Date & DATE = Date(),
                         const Time & TIME = Time());

        const std::string ToString() const;
        inline bool IsValid() const                     { return (date.IsValid() && time.IsValid()); }
        static inline const DateTime CurrentDateTime()  { return DateTime(Date::CurrentDate(), Time::CurrentTime()); }

        Date date;
        Time time;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & date;
            ar & time;
        }
    };

    inline bool operator==(const DateTime & L, const DateTime & R)
    {
        return std::tie(L.date, L.time) == std::tie(R.date, R.time);
    }

    inline bool operator!=(const DateTime & L, const DateTime & R)
    {
        return ! (L == R);
    }

    inline bool operator<(const DateTime & L, const DateTime & R)
    {
        return std::tie(L.date, L.time) < std::tie(R.date, R.time);
    }

}
}

#endif //HEROESPATH_SFMLUTIL_DATETIME_HPP_INCLUDED
