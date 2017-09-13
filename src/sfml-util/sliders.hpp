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
#ifndef SFMLUTIL_SLIDERS_INCLUDED
#define SFMLUTIL_SLIDERS_INCLUDED
//
// sliders.hpp
//
#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

//prevent boost warnings that can be ignored
#include "misc/platform.hpp"
#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <boost/math/constants/constants.hpp> //for boost::math::constants::pi<double>() etc.

#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wundef"
#endif


namespace sfml_util
{
namespace sliders
{

    namespace boostmath = boost::math::constants;


    //Slides a number from 0.0 to 1.0 at the speed and starting point given.
    //Call the Update(time_delta) function periodically to get the changing current value.
    //T must be signed and real. (i.e. float, double, etc.)
    //Ensure (INITIAL_VAL >= 0.0 && INITIAL_VAL <= 1.0).
    template<typename T>
    class ZeroSliderOnce
    {
    public:
        explicit ZeroSliderOnce(const T SPEED       = 1.0,
                                const T INITIAL_VAL = 0.0)
        :
            age_(0),//ignore these initializers because of Reset()
            spd_(0),
            val_(0),
            willContinue_(true)
        {
            Reset(SPEED, INITIAL_VAL);
        }

        inline T GetCur() const       { return val_; }
        inline T GetSpd() const       { return spd_; }
        inline bool GetIsDone() const { return (false == willContinue_); }

        void Reset( const T SPEED       = static_cast<T>(1),
                    const T INITIAL_VAL = static_cast<T>(0))
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == misc::IsRealClose(SPEED, static_cast<T>(0))),
                "ZeroSliderOnce::Reset() given speed of zero.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((INITIAL_VAL >= static_cast<T>(0)) && (INITIAL_VAL <= static_cast<T>(1))),
                "ZeroSliderOnce::Reset() given initial value of " << INITIAL_VAL
                << ", which is not within [0,1].");

            age_ = (THREE_QTR_PI_ + (boostmath::pi<T>() * INITIAL_VAL));
            spd_ = SPEED;
            val_ = static_cast<T>(0);
            willContinue_ = true;
        }

        T Update(const T ADJUSTMENT)
        {
            if (willContinue_)
            {
                age_ += ADJUSTMENT * spd_;
                const T NEW_VAL(static_cast<T>(0.5f + (sin(std::fmod(age_, TWO_PI_)) * 0.5f)));

                if ((val_ < NEW_VAL) || (misc::IsRealClose(val_, NEW_VAL)))
                {
                    val_ = NEW_VAL;
                }
                else
                {
                    willContinue_ = false;
                }
            }

            return val_;
        }

    private:
        T age_;
        T spd_;
        T val_;
        bool willContinue_;
    public:
        static const T TWO_PI_;
        static const T THREE_QTR_PI_;
    };

    template<typename T>
    const T ZeroSliderOnce<T>::TWO_PI_(boostmath::pi<T>() * static_cast<T>(2));

    template<typename T>
    const T ZeroSliderOnce<T>::THREE_QTR_PI_(
        (boostmath::pi<T>() / static_cast<T>(2)) + boostmath::pi<T>());


    //Slides a value from BEGIN to END at THE_SPEED given.
    //Math_t must be signed and real. (i.e. float, double, etc.)
    //Ensure BEGIN < END.
    //Ensure (INITIAL_VAL >= BEGIN && INITIAL_VAL <= END).
    template<typename Value_t, typename Math_t = double>
    class SliderOnce
    {
    public:
        explicit SliderOnce(const Value_t BEGIN = static_cast<Value_t>(0),
                            const Value_t END   = static_cast<Value_t>(1),
                            const Math_t  SPEED = static_cast<Math_t>(1))
        :
            begin_ (0), //ignore these initializers because of Reset() below.
            diff_  (0),
            slider_()
        {
            Reset(BEGIN, END, SPEED);
        }

        //The current value is not cached, so this is an expensive call.
        //Polling this function is not reccomended, instead use
        //Update(time_delta_since_last_call) periodically as time passes.
        inline Value_t GetCur() const    { return ApplyRange(slider_.GetCur()); }

        inline Math_t  GetSpd() const    { return slider_.GetSpd(); }
        inline bool    GetIsDone() const { return slider_.GetIsDone(); }
        inline Value_t GetBeg() const    { return begin_; }

        inline Value_t GetEnd() const
        {
            return static_cast<Value_t>(begin_ + static_cast<Value_t>(diff_));
        }
        
        
        void Reset( const Value_t BEGIN = Value_t(0),
                    const Value_t END   = Value_t(1),
                    const Math_t  SPEED = Math_t (1))
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == misc::IsRealClose(SPEED, Math_t(0))),
                "SliderOnce::Reset() given speed of zero.");

            M_ASSERT_OR_LOGANDTHROW_SS((BEGIN <= END),
                "SliderOnce::Reset() was given a begin=" << BEGIN
                << " that is not less than the end=" << END << ".");

            begin_ = BEGIN;
            diff_  = static_cast<Math_t>(END - BEGIN);
            slider_.Reset(SPEED, Math_t(0));
        }

        template<typename Adjust_t>
        Value_t Update(const Adjust_t ADJUSTMENT)
        {
            return ApplyRange(slider_.Update(static_cast<Math_t>(ADJUSTMENT)));
        }

    private:
        inline Value_t ApplyRange(const Math_t ORIG) const
        {
            return static_cast<Value_t>(static_cast<Math_t>(begin_) + (ORIG * diff_));
        }

    private:
        Value_t begin_;
        Math_t  diff_;
        ZeroSliderOnce<Math_t> slider_;
    };


    //Slides a value between THE_MIN and THE_MAX at the given speed.
    //Ensure (INITIAL_VAL >= THE_MIN && INITIAL_VAL <= THE_MAX).
    //Ensure THE_MIN < THE_MAX.
    //Speed_t must be signed and real. (i.e. float, double, etc.)
    template<typename Value_t, typename Speed_t = double>
    class Slider2
    {
    public:
        //This constructor uses a random initial target.
        explicit Slider2(const Value_t THE_MIN     = static_cast<Value_t>(0),
                         const Value_t THE_MAX     = static_cast<Value_t>(1),
                         const Speed_t SPEED       = static_cast<Speed_t>(1),
                         const Value_t INITIAL_VAL = static_cast<Value_t>(0))
        :
            min_         (THE_MIN),//Note the call to Reset() in the constructor which sets these.
            max_         (THE_MAX),
            speed_       (SPEED),
            isIncreasing_(true),
            slider_      ()
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == misc::IsRealClose(speed_, Speed_t(0))),
                "Slider2::Constructor given SPEED of zero.");

            Reset(THE_MIN, THE_MAX, INITIAL_VAL, THE_MAX);
        }

        inline Value_t GetMin() const { return min_; }
        inline Value_t GetMax() const { return max_; }
        inline Speed_t GetSpd() const { return speed_; }

        void Reset(const Value_t THE_MIN,
                   const Value_t THE_MAX,
                   const Value_t INITIAL_VAL,
                   const Value_t TARGET)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= THE_MIN) && (INITIAL_VAL <= THE_MAX)),
                "Slider2::Reset() given initial value of " << INITIAL_VAL
                << ", which is not within the min and max given: ["
                << THE_MIN << "," << THE_MAX << "].");

            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "Slider2::Reset() was given a min="
                << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

            min_ = THE_MIN;
            max_ = THE_MAX;

            if (INITIAL_VAL < TARGET)
            {
                isIncreasing_ = true;
                slider_.Reset(INITIAL_VAL, TARGET, speed_);
            }
            else
            {
                isIncreasing_ = false;
                slider_.Reset(TARGET, INITIAL_VAL, speed_);
            }
        }

        template<typename Adjust_t>
        Value_t Update(const Adjust_t ADJUSTMENT)
        {
            Value_t newCurrentVal(0);

            if (isIncreasing_)
            {
                newCurrentVal = slider_.Update(ADJUSTMENT);
            }
            else
            {
                newCurrentVal = static_cast<Value_t>(
                    slider_.GetEnd() - (slider_.Update(ADJUSTMENT) - slider_.GetBeg()));
            }
            
            if (slider_.GetIsDone())
            {
                Reset(min_, max_, newCurrentVal, ((isIncreasing_) ? min_ : max_));
            }

            return newCurrentVal;
        }

    private:
        Value_t min_;
        Value_t max_;
        Speed_t speed_;
        bool    isIncreasing_;
        SliderOnce<Value_t, Speed_t> slider_;
    };


    //Drifts a value between THE_MIN and THE_MAX at a speed that varies.
    //Ensure (INITIAL_VAL >= THE_MIN && INITIAL_VAL <= THE_MAX).
    //Ensure THE_MIN < THE_MAX.
    //Ensure SPEED_MIN < SPEED_MAX.  (boost random functions will crash if both are equal!)
    //Speed_t must be signed and real. (i.e. float, double, etc.)
    template<typename Value_t, typename Speed_t = double>
    class Drifter
    {
    public:
        //This constructor uses a random initial target.
        Drifter(const Value_t THE_MIN,
                const Value_t THE_MAX,
                const Speed_t SPEED_MIN,
                const Speed_t SPEED_MAX)
        :
            min_         (THE_MIN),//Note the call to Reset() in the constructor that sets these.
            max_         (THE_MAX),
            spdMin_      (SPEED_MIN),
            spdMax_      (SPEED_MAX),
            isIncreasing_(true),
            slider_      ()
        {
            Reset(THE_MIN, THE_MAX, SPEED_MIN, SPEED_MAX, RandRange(), RandRange());
        }

        //This constructor allows setting the initial target.
        Drifter(const Value_t   THE_MIN,
                const Value_t   THE_MAX,
                const Speed_t   SPEED_MIN,
                const Speed_t   SPEED_MAX,
                const Value_t   INITIAL_VAL,
                const Value_t   INITIAL_TARGET)
        :
        min_         (THE_MIN),//Note the call to Reset() in the constructor
        max_         (THE_MAX),
        spdMin_      (SPEED_MIN),
        spdMax_      (SPEED_MAX),
        isIncreasing_(true),
        slider_      ()
    {
        Reset(THE_MIN, THE_MAX, SPEED_MIN, SPEED_MAX, INITIAL_VAL, INITIAL_TARGET);
    }

    inline Value_t GetMin() const { return min_; }
    inline Value_t GetMax() const { return max_; }
    inline Speed_t GetSpd() const { return slider_.GetSpd(); }

    void Reset( const Value_t THE_MIN,
                const Value_t THE_MAX,
                const Speed_t SPEED_MIN,
                const Speed_t SPEED_MAX,
                const Value_t INITIAL_VAL,
                const Value_t TARGET)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == misc::IsRealClose(SPEED_MAX, Speed_t(0))),
            "Drifter::Reset() given speed_max of zero.");

        M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= THE_MIN) && (INITIAL_VAL <= THE_MAX)),
            "Drifter::Reset() given initial value of " << INITIAL_VAL
            << ", which is not within the min and max given: ["
            << THE_MIN << "," << THE_MAX << "].");

        M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX),
            "Drifter::Reset() was given a min=" << THE_MIN
            << " that is not less than the max=" << THE_MAX << ".");

        min_ = THE_MIN;
        max_ = THE_MAX;
        spdMin_ = SPEED_MIN;
        spdMax_ = SPEED_MAX;

        if (INITIAL_VAL < TARGET)
        {
            isIncreasing_ = true;
            slider_.Reset(INITIAL_VAL, TARGET, RandSpeed());
        }
        else
        {
            isIncreasing_ = false;
            slider_.Reset(TARGET, INITIAL_VAL, RandSpeed());
        }
    }

    template<typename Adjust_t>
    Value_t Update(const Adjust_t ADJUSTMENT)
    {
        Value_t newCurrentVal(0);

        if (isIncreasing_)
        {
            newCurrentVal = slider_.Update(ADJUSTMENT);
        }
        else
        {
            newCurrentVal = slider_.GetEnd() - (slider_.Update(ADJUSTMENT) - slider_.GetBeg());
        }

        if (slider_.GetIsDone())
        {
            Reset(min_, max_, spdMin_, spdMax_, newCurrentVal, RandRange());
        }

        return newCurrentVal;
    }

    private:
        Value_t RandRange() const
        {
            return static_cast<Value_t>(misc::random::Double(min_, max_));
        }

        Speed_t RandSpeed() const
        {
            return static_cast<Speed_t>(misc::random::Double(spdMin_, spdMax_));
        }

    private:
        Value_t min_;
        Value_t max_;
        Speed_t spdMin_;
        Speed_t spdMax_;
        bool    isIncreasing_;
        SliderOnce<Value_t, Speed_t> slider_;
    };

}
}

#endif //SFMLUTIL_SLIDERS_INCLUDED
