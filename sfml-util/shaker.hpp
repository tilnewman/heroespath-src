#ifndef SFMLUTIL_SHAKER_HPP_INCLUDED
#define SFMLUTIL_SHAKER_HPP_INCLUDED
//
// shaker.hpp
//  Code that oscillates a number between a given min and max.
//
#include "sfml-util/sliders.hpp"

#include "heroespath/assertlogandthrow.hpp"

#include <sstream>
#include <string>


namespace sfml_util
{

    template<typename T>
    class Shaker
    {
    public:
        Shaker(const T THE_MIN      = T(0),
               const T THE_MAX      = T(1),
               const T SPEED        = T(50),
               const T START_AT     = T(0),
               const bool WILL_GROW = true)
        :
            min_      (),
            diff_     (),
            speed_    (),
            start_    (),
            isGrowing_(),
            slider_   (SPEED, START_AT)
        {
            Reset(THE_MIN, THE_MAX, SPEED, START_AT, WILL_GROW);
        }

        inline T Min() const            { return min_; }
        inline T Max() const            { return min_ + diff_; }
        inline T Current() const        { return MapToRange(slider_.GetCur()); }
        inline T Speed() const          { return slider_.GetSpd(); }
        inline void Reset()             { slider_.Reset(speed_); }
        inline bool IsGrowing() const   { return isGrowing_; }

        void Reset(const T    THE_MIN,
                   const T    THE_MAX,
                   const T    SPEED,
                   const T    START_AT  = T(0),
                   const bool WILL_GROW = true)
        {
            min_       = THE_MIN;
            diff_      = THE_MAX - THE_MIN;
            speed_     = SPEED;
            start_     = START_AT;
            isGrowing_ = WILL_GROW;

            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "sfml_util::Shaker::Reset(" << Description() << ")  Given a min that was not less than the max.");
            M_ASSERT_OR_LOGANDTHROW_SS((sfml_util::IsRealClose(THE_MIN, THE_MAX) == false), "sfml_util::Shaker::Reset(" << Description() << ")  Given a min and max that are too close.");
            M_ASSERT_OR_LOGANDTHROW_SS((SPEED > T(0)), "sfml_util::Shaker::Reset(" << Description() << ")  Given a speed that is not greater than zero.");
            M_ASSERT_OR_LOGANDTHROW_SS(((THE_MIN <= START_AT) && (THE_MAX >= START_AT)), "sfml_util::Shaker::Reset(" << Description() << ")  start at value was not within [max, min].");

            slider_.Reset(speed_, start_ / diff_);
        }

        T Update(const float ELAPSED_TIME_SECONDS)
        {
            const T NEW_VALUE( MapToRange(slider_.Update(ELAPSED_TIME_SECONDS)) );

            if (slider_.GetIsDone())
            {
                isGrowing_ = ! isGrowing_;
                Reset();
            }

            return NEW_VALUE;
        }

        const std::string Description(const bool WILL_WRAP = false) const
        {
            std::ostringstream ss;

            if (WILL_WRAP)
                ss << "(";

            ss << "min=" << min_ << ", max=" << (min_ + diff_) << ", spd=" << speed_ << ", start_at=" << start_ << ", will_grow=" << std::boolalpha << isGrowing_;

            if (WILL_WRAP)
                ss << ")";

            return ss.str();
        }

    private:
        T MapToRange(const T VAL)
        {
            if (isGrowing_)
            {
                return min_ + (VAL * diff_);
            }
            else
            {
                return (min_ + diff_) - (VAL * diff_);
            }
        }

    private:
        T min_;
        T diff_;
        T speed_;
        T start_;
        bool isGrowing_;
        sliders::ZeroSliderOnce<T> slider_;
    };

}

#endif //SFMLUTIL_SHAKER_HPP_INCLUDED
