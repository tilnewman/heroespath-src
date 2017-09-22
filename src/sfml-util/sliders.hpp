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
    //Ensure INITIAL [0, 1]
    template<typename T>
    class ZeroSliderOnce
    {
    public:
        explicit ZeroSliderOnce(
            const T SPEED   = 1.0,
            const T INITIAL = 0.0)
        :
            age_(0),//ignore these initializers because of Reset()
            spd_(0),
            val_(0),
            willContinue_(true)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((misc::IsRealZero(SPEED) == false),
                "sfml_util::sliders::ZeroSliderOnce::Constructor() given speed of zero.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc::IsRealZero(INITIAL) ||
                    misc::IsRealOne(INITIAL) ||
                    (INITIAL > static_cast<T>(0)) && (INITIAL < static_cast<T>(1))),
                "sfml_util::sliders::ZeroSliderOnce::Constructor() given initial value of "
                << INITIAL << ", which is not within [0,1].");

            Reset(SPEED, INITIAL);
        }

        inline T Current() const    { return val_; }
        inline T Speed() const      { return spd_; }
        inline bool IsDone() const  { return (false == willContinue_); }

        void Reset(
            const T SPEED   = 1.0,
            const T INITIAL = 0.0)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((misc::IsRealZero(SPEED) == false),
                "sfml_util::sliders::ZeroSliderOnce::Reset() given speed of zero.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc::IsRealZero(INITIAL) ||
                 misc::IsRealOne(INITIAL) ||
                 (INITIAL > static_cast<T>(0)) && (INITIAL < static_cast<T>(1))),
                "sfml_util::sliders::ZeroSliderOnce::Reset() given initial value of "
                << INITIAL << ", which is not within [0,1].");
        
            age_ = (THREE_QTR_PI_ + (boostmath::pi<T>() * INITIAL));
            spd_ = SPEED;
            val_ = static_cast<T>(0);
            willContinue_ = true;
            Update(static_cast<T>(0));
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
                    val_ = static_cast<T>(1);
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


    //Slides a value from THE_MIN to THE_MAX at THE_SPEED given.
    //Math_t must be signed and real. (i.e. float, double, etc.)
    //Ensure THE_MIN < THE_MAX.
    //Ensure (INITIAL >= THE_MIN && INITIAL <= THE_MAX).
    template<typename Value_t, typename Math_t = double>
    class SliderOnce
    {
    public:
        explicit SliderOnce(
            const Value_t THE_MIN = 0,
            const Value_t THE_MAX = 1,
            const Math_t  SPEED   = 1.0)
        :
            min_   (0), //ignore these initializers because of Reset() below
            diff_  (0.0),
            slider_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS((misc::IsRealZero(SPEED) == false),
                "sfml_util::sliders::SliderOnce::Constructor() given speed of zero.");

            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX),
                "sfml_util::sliders::SliderOnce::Constructor() was given THE_MIN="
                << THE_MIN << " that is not less than the THE_MAX=" << THE_MAX << ".");

            Reset(THE_MIN, THE_MAX, SPEED);
        }

        //The current value is not cached, so this is an expensive call.
        //Polling this function is not reccomended, instead use
        //Update(time_delta_since_last_call) periodically as time passes.
        inline Value_t Current() const  { return ApplyRange(slider_.Current()); }
        inline Math_t  Speed() const    { return slider_.Speed(); }
        inline bool    IsDone() const   { return slider_.IsDone(); }
        inline Value_t Min() const      { return min_; }

        inline Value_t Max() const
        {
            return static_cast<Value_t>(min_ + static_cast<Value_t>(diff_));
        }
        
        void Reset(
            const Value_t THE_MIN = 0,
            const Value_t THE_MAX = 1,
            const Math_t  SPEED = 1.0)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((misc::IsRealZero(SPEED) == false),
                "sfml_util::sliders::SliderOnce::Reset() given speed of zero.");

            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX),
                "sfml_util::sliders::SliderOnce::Reset() was given THE_MIN=" << THE_MIN
                << " that is not less than the THE_MAX=" << THE_MAX << ".");

            min_ = THE_MIN;
            diff_ = static_cast<Math_t>(THE_MAX - THE_MIN);
            slider_.Reset(SPEED);
        }

        template<typename Adjust_t>
        Value_t Update(const Adjust_t ADJUSTMENT)
        {
            return ApplyRange(slider_.Update(static_cast<Math_t>(ADJUSTMENT)));
        }

    private:
        inline Value_t ApplyRange(const Math_t ORIG) const
        {
            return static_cast<Value_t>(static_cast<Math_t>(min_) + (ORIG * diff_));
        }

    private:
        Value_t min_;
        Math_t diff_;
        ZeroSliderOnce<Math_t> slider_;
    };


    //Slides a value back and forth between THE_MIN and THE_MAX at the given speed.
    //Ensure (INITIAL >= THE_MIN && INITIAL <= THE_MAX).
    //Ensure THE_MIN < THE_MAX.
    //Speed_t must be signed and real. (i.e. float, double, etc.)
    template<typename Value_t, typename Speed_t = double>
    class Slider
    {
    public:
        explicit Slider(
            const Value_t THE_MIN = 0,
            const Value_t THE_MAX = 1,
            const Speed_t SPEED   = 1.0,
            const Value_t INITIAL = 0)
        :
            min_         (THE_MIN),//Note the call to Reset() in the constructor which sets these.
            max_         (THE_MAX),
            speed_       (SPEED),
            isIncreasing_(true),
            slider_      ()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((misc::IsRealClose(INITIAL, THE_MIN)) || (INITIAL > THE_MIN)),
                "sfml_util::sliders::Slider::Constructor given INITIAL=" << INITIAL
                << " which is not equal or greater than THE_MIN of " << THE_MIN);

            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc::IsRealClose(INITIAL, THE_MAX) || (INITIAL < THE_MAX)),
                "sfml_util::sliders::Slider::Constructor given INITIAL=" << INITIAL
                << " which is not less than or equal to THE_MAX=" << THE_MAX);

            M_ASSERT_OR_LOGANDTHROW_SS((misc::IsRealZero(speed_) == false),
                "sfml_util::sliders::Slider::Constructor given SPEED of zero.");

            Reset(THE_MIN, THE_MAX, SPEED, INITIAL);
        }

        inline Value_t Min() const { return min_; }
        inline Value_t Max() const { return max_; }
        inline Speed_t Speed() const { return speed_; }

        //SPEED==0 means the speed will not be reset/changed
        //INITIAL==0 means the INITIAL will be THE_MIN
        void Reset(
            const Value_t THE_MIN,
            const Value_t THE_MAX,
            const Speed_t SPEED   = 0,
            const Value_t INITIAL = 0)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX),
                "sfml_util::sliders::Slider::Reset() was given a min="
                << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

            min_ = THE_MIN;
            max_ = THE_MAX;

            if (misc::IsRealZero(SPEED) == false)
            {
                speed_ = SPEED;
            }

            auto initialToUse{ INITIAL };
            if (misc::IsRealZero(initialToUse))
            {
                initialToUse = THE_MIN;
            }

            slider_.Reset(initialToUse, THE_MAX, speed_);
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
                    slider_.Max() - (slider_.Update(ADJUSTMENT) - slider_.Min()));
            }
            
            if (slider_.IsDone())
            {
                isIncreasing_ = ! isIncreasing_;
                Reset(min_, max_, speed_);
            }

            return newCurrentVal;
        }

    private:
        Value_t min_;
        Value_t max_;
        Speed_t speed_;
        bool isIncreasing_;
        SliderOnce<Value_t, Speed_t> slider_;
    };


    //Drifts a value between THE_MIN and THE_MAX at a speed that varies.
    //Ensure (INITIAL >= THE_MIN && INITIAL <= THE_MAX).
    //Ensure THE_MIN < THE_MAX.
    //Ensure SPEED_MIN < SPEED_MAX.  (boost random functions will crash if both are equal!)
    //Speed_t must be signed and real. (i.e. float, double, etc.)
    template<typename Value_t, typename Speed_t = double>
    class Drifter
    {
    public:
        //This constructor uses a random initial target.
        Drifter(
            const Value_t THE_MIN,
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
        Drifter(
            const Value_t THE_MIN,
            const Value_t THE_MAX,
            const Speed_t SPEED_MIN,
            const Speed_t SPEED_MAX,
            const Value_t INITIAL,
            const Value_t TARGET)
        :
        min_         (THE_MIN),//Note the call to Reset() in the constructor
        max_         (THE_MAX),
        spdMin_      (SPEED_MIN),
        spdMax_      (SPEED_MAX),
        isIncreasing_(true),
        slider_      ()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == misc::IsRealClose(SPEED_MAX, Speed_t(0))),
            "sfml_util::sliders::Drifter::Constructor() given speed_max of zero.");

        M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL >= THE_MIN) && (INITIAL <= THE_MAX)),
            "sfml_util::sliders::Drifter::Constructor() given initial value of "
            << INITIAL << ", which is not within the min and max given: ["
            << THE_MIN << "," << THE_MAX << "].");

        M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX),
            "sfml_util::sliders::Drifter::Constructor() was given a min="
            << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

        Reset(THE_MIN, THE_MAX, SPEED_MIN, SPEED_MAX, INITIAL, TARGET);
    }

    inline Value_t GetMin() const   { return min_; }
    inline Value_t GetMax() const   { return max_; }
    inline Speed_t Speed() const    { return slider_.Speed(); }

    void Reset( const Value_t THE_MIN,
                const Value_t THE_MAX,
                const Speed_t SPEED_MIN,
                const Speed_t SPEED_MAX,
                const Value_t INITIAL,
                const Value_t TARGET)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == misc::IsRealClose(SPEED_MAX, Speed_t(0))),
            "sfml_util::sliders::Drifter::Reset() given speed_max of zero.");

        M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL >= THE_MIN) && (INITIAL <= THE_MAX)),
            "sfml_util::sliders::Drifter::Reset() given initial value of "
            << INITIAL << ", which is not within the min and max given: ["
            << THE_MIN << "," << THE_MAX << "].");

        M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX),
            "sfml_util::sliders::Drifter::Reset() was given a min="
            << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

        min_ = THE_MIN;
        max_ = THE_MAX;
        spdMin_ = SPEED_MIN;
        spdMax_ = SPEED_MAX;

        if (INITIAL < TARGET)
        {
            isIncreasing_ = true;
            slider_.Reset(INITIAL, TARGET, RandSpeed());
        }
        else
        {
            isIncreasing_ = false;
            slider_.Reset(TARGET, INITIAL, RandSpeed());
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
            newCurrentVal = slider_.Max() - (slider_.Update(ADJUSTMENT) - slider_.Min());
        }

        if (slider_.IsDone())
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
        bool isIncreasing_;
        SliderOnce<Value_t, Speed_t> slider_;
    };

}
}

#endif //SFMLUTIL_SLIDERS_INCLUDED
