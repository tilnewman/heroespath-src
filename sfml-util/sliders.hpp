#ifndef SFMLUTIL_SLIDERS_INCLUDED
#define SFMLUTIL_SLIDERS_INCLUDED
//
// sliders.hpp
//
#include "utilz/random.hpp"
#include "utilz/assertlogandthrow.hpp"

//prevent boost warnings that can be ignored
#include "utilz/platform.hpp"
#ifdef PLATFORMDETECT__APPPLE_OOSS
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <boost/math/constants/constants.hpp> //for boost::math::constants::pi<double>() etc.

#ifdef PLATFORMDETECT__APPPLE_OOSS
#pragma GCC diagnostic warning "-Wundef"
#endif

namespace sfml_util
{
namespace sliders
{

    namespace boostmath = boost::math::constants;


    //Slides a number between (roughly) 0.0 to (roughly) 1.0 at the speed, direction, and starting point given.
    //Call the Update(time_delta) function periodically to get the current value.
    //T must be signed and real. (i.e. float, double, etc.)
    //Ensure (INITIAL_VAL >= 0.0 && INITIAL_VAL <= 1.0).
    //NOTE:  This ZeroSlider is faster but less accurate than its cousin ZeroSliderOnce,
    //       because it is not guarranteed to reach both zero and one on each pass.
    template<typename T>
    class ZeroSlider
    {
    public:
        ZeroSlider( const T    SPEED               = static_cast<T>(1),
                    const T    INITIAL_VAL         = static_cast<T>(0),
                    const bool WILL_START_FORWARD = true)
        :
            age_  (0),//ignore these initializers because of Reset()
            speed_(0)
        {
            Reset(SPEED, INITIAL_VAL, WILL_START_FORWARD);
        }

        //NOTE: The current value is not cached, so this is an expensive call.  Polling this functino is not reccomended.
        inline T GetCur()       { return Update(static_cast<T>(0) ); }

        inline T GetSpd() const { return speed_; }

        void Reset( const T    SPEED              = static_cast<T>(1),
                    const T    INITIAL_VAL        = static_cast<T>(0),
                    const bool WILL_START_FORWARD = true)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == utilz::IsRealClose(SPEED, static_cast<T>(0))), "ZeroSlider::Reset() given speed of zero.");
            M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= static_cast<T>(0)) && (INITIAL_VAL <= static_cast<T>(1))), "ZeroSlider::Reset() given initial value of " << INITIAL_VAL << ", which is not [0.0, 1.0].");

            const T DIRECTION((WILL_START_FORWARD) ? static_cast<T>(1) : static_cast<T>(-1));
            age_ = (THREE_QTR_PI_ + (boostmath::pi<T>() * INITIAL_VAL * DIRECTION));
            speed_ = SPEED;
        }

        T Update(const T ADJUSTMENT)
        {
            age_ += ADJUSTMENT * speed_;
            return static_cast<T>(0.5f + (sin(std::fmod(age_, 2.0f * boostmath::pi<T>())) * 0.5f));
        }

    private:
        T age_;
        T speed_;
        static const T THREE_QTR_PI_;
    };
    template<typename T> const T ZeroSlider<T>::THREE_QTR_PI_((boostmath::pi<T>() / static_cast<T>(2)) + boostmath::pi<T>());



    //Slides a value back and forth between THE_MIN and THE_MAX at THE_SPEED and direction given.
    //Math_t must be signed and real. (i.e. float, double, etc.)
    //THE_MIN must be < THE_MAX.
    //Ensure (INITIAL_VAL >= THE_MIN && INITIAL_VAL <= THE_MAX).
    //NOTE:  This Slider is less accurate than its cousin SliderOnce and Slider2,
    //       see note for ZeroSlider above.
    template<typename Value_t, typename Math_t = double>
    class Slider
    {
    public:
        Slider( const Value_t THE_MIN            = Value_t(0),
                const Value_t THE_MAX            = Value_t(1),
                const Math_t  SPEED              = Math_t (1),
                const Value_t INITIAL_VAL        = Value_t(0),
                const bool    WILL_START_FORWARD = true)
        :
            start_     (0), //ignore these initializers because of Reset() below.
            diff_      (0),
            zeroSlider_()
        {
            Reset(THE_MIN, THE_MAX, SPEED, INITIAL_VAL, WILL_START_FORWARD);
        }

        //NOTE: The current value is not cached, so this is an expensive call.  Polling this function is not reccomended.
        inline Value_t GetCur() const { return static_cast<Value_t>(start_ + (zeroSlider_.GetCur() * diff_)); }
        inline Value_t GetMin() const { return start_; }
        inline Value_t GetMax() const { return start_ + diff_; }
        inline Math_t  GetSpd() const { return zeroSlider_.GetSpd(); }

        void Reset( const Value_t THE_MIN            = Value_t(0),
                    const Value_t THE_MAX            = Value_t(1),
                    const Math_t  SPEED              = Math_t (1),
                    const Value_t INITIAL_VAL        = Value_t(0),
                    const bool    WILL_START_FORWARD = true)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == utilz::IsRealClose(SPEED, Math_t(0))), "Slider::Reset() given speed of zero.");
            M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= THE_MIN) && (INITIAL_VAL <= THE_MAX)), "Slider::Reset() given initial value of " << INITIAL_VAL << ", which is not within the min and max given: [" << THE_MIN << "," << THE_MAX << "].");
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "Slider::Reset() was given a min=" << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

            start_ = THE_MIN;
            diff_ = static_cast<Math_t>(THE_MAX - THE_MIN);
            zeroSlider_.Reset(SPEED, static_cast<Math_t>(INITIAL_VAL) / diff_, WILL_START_FORWARD);
        }

        template<typename Adjust_t>
        Value_t Update(const Adjust_t ADJUSTMENT)
        {
            return static_cast<Value_t>(start_ + (zeroSlider_.Update(static_cast<Math_t>(ADJUSTMENT)) * diff_));
        }

    private:
        Value_t start_;
        Math_t  diff_;
        ZeroSlider<Math_t> zeroSlider_;
    };



    //Slides a number from 0.0 to 1.0 at the speed and starting point given.
    //Call the Update(time_delta) function periodically to get the current value.
    //T must be signed and real. (i.e. float, double, etc.)
    //Ensure (INITIAL_VAL >= 0.0 && INITIAL_VAL <= 1.0).
    template<typename T>
    class ZeroSliderOnce
    {
    public:
        ZeroSliderOnce( const T SPEED       = static_cast<T>(1),
                        const T INITIAL_VAL = static_cast<T>(0))
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
            M_ASSERT_OR_LOGANDTHROW_SS((false == utilz::IsRealClose(SPEED, static_cast<T>(0))), "ZeroSliderOnce::Reset() given speed of zero.");
            M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= static_cast<T>(0)) && (INITIAL_VAL <= static_cast<T>(1))), "ZeroSliderOnce::Reset() given initial value of " << INITIAL_VAL << ", which is not within [0,1].");

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

                if ((val_ < NEW_VAL) || (utilz::IsRealClose(val_, NEW_VAL)))
                    val_ = NEW_VAL;
                else
                    willContinue_ = false;
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

    template<typename T> const T ZeroSliderOnce<T>::TWO_PI_(boostmath::pi<T>() * static_cast<T>(2));
    template<typename T> const T ZeroSliderOnce<T>::THREE_QTR_PI_((boostmath::pi<T>() / static_cast<T>(2)) + boostmath::pi<T>());



    //Slides a value from BEGIN to END at THE_SPEED given.
    //Math_t must be signed and real. (i.e. float, double, etc.)
    //Ensure BEGIN < END.
    //Ensure (INITIAL_VAL >= BEGIN && INITIAL_VAL <= END).
    template<typename Value_t, typename Math_t = double>
    class SliderOnce
    {
    public:
        SliderOnce( const Value_t BEGIN       = Value_t(0),
                    const Value_t END         = Value_t(1),
                    const Math_t  SPEED       = Math_t (1))
        :
            begin_ (0), //ignore these initializers because of Reset() below.
            diff_  (0),
            slider_()
        {
            Reset(BEGIN, END, SPEED);
        }

        //Note:  The current value is not cached, so this is an expensive call.  Polling is not reccomended.
        inline Value_t GetCur() const    { return ApplyRange(slider_.GetCur()); }
        inline Value_t GetBeg() const    { return begin_; }
        inline Value_t GetEnd() const    { return static_cast<Value_t>(begin_ + static_cast<Value_t>(diff_)); }
        inline Math_t  GetSpd() const    { return slider_.GetSpd(); }
        inline bool    GetIsDone() const { return slider_.GetIsDone(); }

        void Reset( const Value_t BEGIN = Value_t(0),
                    const Value_t END   = Value_t(1),
                    const Math_t  SPEED = Math_t (1))
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == utilz::IsRealClose(SPEED, Math_t(0))), "SliderOnce::Reset() given speed of zero.");
            M_ASSERT_OR_LOGANDTHROW_SS((BEGIN <= END), "SliderOnce::Reset() was given a begin=" << BEGIN << " that is not less than the end=" << END << ".");

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
        inline Value_t ApplyRange(const Math_t ORIG) const { return static_cast<Value_t>(static_cast<Math_t>(begin_) + (ORIG * diff_)); }

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
        Slider2(const Value_t THE_MIN     = Value_t(0),
                const Value_t THE_MAX     = Value_t(1),
                const Speed_t SPEED       = Speed_t(1),
                const Value_t INITIAL_VAL = Value_t(0))
        :
            min_         (THE_MIN),//Note the call to Reset() in the constructor, which will change these initializer values.
            max_         (THE_MAX),
            speed_       (SPEED),
            isIncreasing_(true),
            slider_      ()
        {
            M_ASSERT_OR_LOGANDTHROW_SS((false == utilz::IsRealClose(speed_, Speed_t(0))), "Slider2::Constructor given SPEED of zero.");
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
            M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= THE_MIN) && (INITIAL_VAL <= THE_MAX)), "Slider2::Reset() given initial value of " << INITIAL_VAL << ", which is not within the min and max given: [" << THE_MIN << "," << THE_MAX << "].");
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "Slider2::Reset() was given a min=" << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

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
                newCurrentVal = static_cast<Value_t>(slider_.GetEnd() - (slider_.Update(ADJUSTMENT) - slider_.GetBeg()));
            }

            if (slider_.GetIsDone())
                Reset(min_, max_, newCurrentVal, ((isIncreasing_) ? min_ : max_ ));

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
            min_         (THE_MIN),//Note the call to Reset() in the constructor, which will change these initializer values.
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
        M_ASSERT_OR_LOGANDTHROW_SS((false == utilz::IsRealClose(SPEED_MAX, Speed_t(0))), "Drifter::Reset() given speed_max of zero.");
        M_ASSERT_OR_LOGANDTHROW_SS(((INITIAL_VAL >= THE_MIN) && (INITIAL_VAL <= THE_MAX)), "Drifter::Reset() given initial value of " << INITIAL_VAL << ", which is not within the min and max given: [" << THE_MIN << "," << THE_MAX << "].");
        M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "Drifter::Reset() was given a min=" << THE_MIN << " that is not less than the max=" << THE_MAX << ".");

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
            newCurrentVal = slider_.Update(ADJUSTMENT);
        else
            newCurrentVal = slider_.GetEnd() - (slider_.Update(ADJUSTMENT) - slider_.GetBeg());

        if (slider_.GetIsDone())
            Reset(min_, max_, spdMin_, spdMax_, newCurrentVal, RandRange());

        return newCurrentVal;
    }

    private:
        Value_t RandRange()  const { return static_cast<Value_t>(utilz::random::Double(min_, max_)); }
        Speed_t RandSpeed()  const { return static_cast<Speed_t>(utilz::random::Double(spdMin_, spdMax_)); }

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
