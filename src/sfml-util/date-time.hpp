// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

    // simple wrapper for dates
    struct Date
    {
        explicit Date(const int YEAR = 0, const int MONTH = 0, const int DAY = 0);

        Date(const Date &) = default;
        Date(Date &&) = default;
        Date & operator=(const Date &) = default;
        Date & operator=(Date &&) = default;

        const std::string ToString() const;

        bool IsValid() const;

        static const Date CurrentDate();

        int year;
        int month;
        int day;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & year;
            ar & month;
            ar & day;
        }
    };

    inline bool operator==(const Date & L, const Date & R)
    {
        return ((L.year == R.year) && (L.month == R.month) && (L.day == R.day));
    }

    inline bool operator!=(const Date & L, const Date & R) { return !(L == R); }

    inline bool operator<(const Date & L, const Date & R)
    {
        return std::tie(L.year, L.month, L.day) < std::tie(R.year, R.month, R.day);
    }

    // simple wrapper for times
    struct Time
    {
        explicit Time(
            const int HOUR = 0,
            const int MINUTE = 0,
            const int SECOND = 0,
            const int MILLISECONDS = 0);

        Time(const Time &) = default;
        Time(Time &&) = default;
        Time & operator=(const Time &) = default;
        Time & operator=(Time &&) = default;

        explicit Time(const sf::Time & TIME_OBJ);

        const std::string ToString() const;

        bool IsValid() const;

        static const Time CurrentTime();

        int hours;
        int minutes;
        int seconds;
        int milliseconds;

        static const int SECONDS_IN_MINUTE_ = 60;
        static const int SECONDS_IN_HOUR_ = (60 * 60);
        static const int SECONDS_IN_DAY_ = (60 * 60 * 24);
        static const int MINUTES_IN_HOUR_ = 60;
        static const int MINUTES_IN_DAY_ = (60 * 24);
        static const int HOURS_IN_DAY_ = 24;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & hours;
            ar & minutes;
            ar & seconds;
            ar & milliseconds;
        }
    };

    inline bool operator==(const Time & L, const Time & R)
    {
        return std::tie(L.hours, L.minutes, L.seconds, L.milliseconds)
            == std::tie(R.hours, R.minutes, R.seconds, R.milliseconds);
    }

    inline bool operator!=(const Time & L, const Time & R) { return !(L == R); }

    inline bool operator<(const Time & L, const Time & R)
    {
        return std::tie(L.hours, L.minutes, L.seconds, L.milliseconds)
            < std::tie(R.hours, R.minutes, R.seconds, R.milliseconds);
    }

    // simple wrapper for date and time
    class DateTime
    {
    public:
        explicit DateTime(const Date & DATE = Date(), const Time & TIME = Time());

        DateTime(const DateTime &) = default;
        DateTime(DateTime &&) = default;
        DateTime & operator=(const DateTime &) = default;
        DateTime & operator=(DateTime &&) = default;

        const std::string ToString() const;

        bool IsValid() const { return (date.IsValid() && time.IsValid()); }

        static const DateTime CurrentDateTime()
        {
            return DateTime(Date::CurrentDate(), Time::CurrentTime());
        }

        Date date;
        Time time;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
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

    inline bool operator!=(const DateTime & L, const DateTime & R) { return !(L == R); }

    inline bool operator<(const DateTime & L, const DateTime & R)
    {
        return std::tie(L.date, L.time) < std::tie(R.date, R.time);
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_DATETIME_HPP_INCLUDED
