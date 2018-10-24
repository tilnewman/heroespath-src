// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SLIDER_BASE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDER_BASE_HPP_INCLUDED
//
// slider-base.hpp
//
#include "sfml-util/moving-enum.hpp"
#include "sfml-util/sliders.hpp"

namespace heroespath
{
namespace sfml_util
{

    enum class WillOscillate : bool
    {
        No = false,
        Yes = true
    };

    enum class WillAutoStart : bool
    {
        No = false,
        Yes = true
    };

    // Common base class for all non-value Sliders of non-value/arithmetic types.
    //
    // Implement all pure virtual functions exactly as specified in their comments.  Your derived
    // class simply needs three members variables: from_, to_, and value_, and two member functions:
    //      T Value() const { return value_; }
    //  and
    //      T UpdateAndReturnValue(const float ADJUSTMENT) { Update(); return Value(); }
    //
    // Just setup the constructor values and call any of the Update() functions repeatedly.  Value()
    // will move smoothly back and forth over the interval [FROM, TO], and that is inclusive so you
    // will always reach both end-point values.  The direction will take care of itself and always
    // be correct.  For more about when the direction changes, see the comments below.
    //
    // If IsAtFrom() then IsMovingToward() always returns true and Direction() always returns
    // Toward, and conversly, if IsAtTo() then IsMovingToward() always returns false and Direction()
    // always returns Away. This is why the ReverseDirection() and SetDirection() functions might do
    // nothing and return false.
    class SliderBase
    {
    protected:
        // default constructor is all zero and false values and stopped
        SliderBase()
            : isStopped_(true)
            , isMovingToward_(true)
            , speed_(0.0f)
            , cycleCount_(0)
            , cycleCountLimit_(0)
            , willOscillate_(WillOscillate::No)
            , sliderZeroToOne_(1.0f)
        {}

        SliderBase(
            const float SPEED,
            const WillOscillate WILL_OSCILLATE,
            const WillAutoStart WILL_AUTO_START,
            const std::size_t CYCLE_COUNT_LIMIT)
            : isStopped_(((WILL_AUTO_START == WillAutoStart::Yes) ? false : true))
            , isMovingToward_(true)
            , speed_(SPEED)
            , cycleCount_(0)
            , cycleCountLimit_(CYCLE_COUNT_LIMIT)
            , willOscillate_(WILL_OSCILLATE)
            , sliderZeroToOne_(SPEED)
        {}

    public:
        virtual ~SliderBase() = default;

        SliderBase(const SliderBase &) = default;
        SliderBase(SliderBase &&) = default;
        SliderBase & operator=(const SliderBase &) = default;
        SliderBase & operator=(SliderBase &&) = default;

        float Speed() const { return speed_; }
        std::size_t CycleCount() const { return cycleCount_; }
        std::size_t CycleCountLimit() const { return cycleCountLimit_; }

        bool CycleCountLimitReached() const
        {
            return ((cycleCountLimit_ > 0) && (cycleCount_ >= cycleCountLimit_));
        }

        // returns (value_ == from_)
        virtual bool IsAtFrom() const = 0;

        // returns (value_ == to_)
        virtual bool IsAtTo() const = 0;

        // returns (IsAtFrom() || IsAtTo())
        bool IsAtFromOrTo() const { return (IsAtFrom() || IsAtTo()); }

        // returns [0,1] of Value()'s position within [FROM, TO]
        float PositionRatio() const
        {
            if (IsMovingToward())
            {
                return sliderZeroToOne_.Value();
            }
            else
            {
                return (1.0f - sliderZeroToOne_.Value());
            }
        }

        // returns true if the NEXT Update() will move toward TO, otherwise returns false
        bool IsMovingToward() const { return isMovingToward_; }

        // returns only Toward or Away based on IsMovingToward()
        Moving::Enum Direction() const
        {
            return ((IsMovingToward()) ? Moving::Toward : Moving::Away);
        }

        // If IsStopped() returns Still otherwise returns Direction()
        Moving::Enum Moving() const { return ((IsStopped()) ? Moving::Still : Direction()); }

        bool WillOscilate() const { return (WillOscillate::Yes == willOscillate_); }

        void WillOscilate(const bool WILL_OSCILLATE)
        {
            willOscillate_ = ((WILL_OSCILLATE) ? WillOscillate::Yes : WillOscillate::No);
        }

        void Start() { isStopped_ = false; }
        void Stop() { isStopped_ = true; }
        bool IsStopped() const { return isStopped_; }
        bool IsMoving() const { return (IsStopped() == false); }

        // returns true if value_ changed
        bool Update(const float ADJUSTMENT)
        {
            if (IsStopped())
            {
                return false;
            }

            sliderZeroToOne_.Update(ADJUSTMENT);
            const bool DID_SLIDER_STOP { sliderZeroToOne_.IsStopped() };

            const auto DID_VALUE_CHANGE { UpdateImpl() };

            Update_HandleValueChangeOrSliderStop(DID_VALUE_CHANGE, DID_SLIDER_STOP);

            return DID_VALUE_CHANGE;
        }

        // same as Update()
        bool UpdateAndReturnTrueIfValueChanged(const float ADJUSTMENT)
        {
            return Update(ADJUSTMENT);
        }

        bool UpdateAndReturnIsMovingToward(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return IsMovingToward();
        }

        bool UpdateAndReturnTrueIfIsMovingTowardChanged(const float ADJUSTMENT)
        {
            const auto IS_MOVING_TOWARD_BEFORE { IsMovingToward() };
            Update(ADJUSTMENT);
            return (IS_MOVING_TOWARD_BEFORE != IsMovingToward());
        }

        Moving::Enum UpdateAndReturnDirection(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return Direction();
        }

        bool UpdateAndReturnTrueIfDirectionChanged(const float ADJUSTMENT)
        {
            const auto DIRECTION_BEFORE { Direction() };
            Update(ADJUSTMENT);
            return (DIRECTION_BEFORE != Direction());
        }

        bool UpdateAndReturnIsMoving(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return IsMoving();
        }

        bool UpdateAndReturnTrueIfIsMovingChanged(const float ADJUSTMENT)
        {
            const auto IS_MOVING_BEFORE { IsMoving() };
            Update(ADJUSTMENT);
            return (IS_MOVING_BEFORE != IsMoving());
        }

        bool UpdateAndReturnIsStopped(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return IsStopped();
        }

        bool UpdateAndReturnTrueIfIsStoppedChanged(const float ADJUSTMENT)
        {
            const auto IS_STOPPED_BEFORE { IsStopped() };
            Update(ADJUSTMENT);
            return (IS_STOPPED_BEFORE != IsStopped());
        }

        float UpdateAndReturnPositionRatio(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return PositionRatio();
        }

        bool UpdateAndReturnTrueIfPositionRatioChanged(const float ADJUSTMENT)
        {
            const auto RATIO_BEFORE { PositionRatio() };
            Update(ADJUSTMENT);
            return (misc::IsRealClose(RATIO_BEFORE, PositionRatio()) == false);
        }

        // sets value_=from_ and isMoveToward_=true
        void SetFrom()
        {
            isMovingToward_ = true;
            sliderZeroToOne_ = SliderZeroToOne(Speed());
            SetFromImpl();
        }

        // sets value_=to_ and isMoveToward_=false
        void SetTo()
        {
            isMovingToward_ = false;
            sliderZeroToOne_ = SliderZeroToOne(Speed());
            SetToImpl();
        }

        // same as ResetFrom()
        void Reset() { ResetFrom(); }

        // sets cycleCount_ = 0
        void ResetCycleCount() { cycleCount_ = 0; }

        // calls ResetCycleCount(), SetFrom(), Start()
        void ResetFrom()
        {
            SetFrom();
            ResetCycleCount();
            Start();
        }

        // calls ResetCycleCount(), SetTo(), and Start()
        void ResetTo()
        {
            ResetCycleCount();
            SetTo();
            Start();
        }

        // if IsMovingToward() then calls SetDirectionAway() otherwise calls SetDirectionToward()
        bool ReverseDirection()
        {
            if (IsMovingToward())
            {
                return SetDirectionAway();
            }
            else
            {
                return SetDirectionToward();
            }
        }

        // if IsAtTo() returns false, otherwise sets isMovingToward_=true and returns true
        bool SetDirectionToward()
        {
            if (IsAtTo())
            {
                return false;
            }
            else
            {
                isMovingToward_ = true;
                return true;
            }
        }

        // if IsAtFrom() returns false, otherwise sets isMovingToward_=false and returns true
        bool SetDirectionAway()
        {
            if (IsAtFrom())
            {
                return false;
            }
            else
            {
                isMovingToward_ = false;
                return true;
            }
        }

    protected:
        // calculates and sets the new adjusted value_, returns true is the new value is diifferent
        virtual bool UpdateImpl() = 0;

        // sets value_ = from_
        virtual void SetFromImpl() = 0;

        // sets value_ = to_
        virtual void SetToImpl() = 0;

        // Only the Update() function calls these OnUpdateChange functions, and only when Update()
        // actually changes value_, and only after the Update function is finished and all members
        // have been changed to their final valid values, and each is only called once per call of
        // Update(), and only in this order they are declared in
        virtual void OnUpdateChange() {}
        virtual void OnUpdateChangeStop() {}
        virtual void OnUpdateChangeReachesFrom() {}
        virtual void OnUpdateChangeReachesTo() {}
        virtual void OnUpdateChangeReachesCycleCountLimit() {}

    private:
        struct CallbackFlags
        {
            bool stopped = false;
            bool reached_from = false;
            bool reached_to = false;
            bool reached_cycle_limit = false;
        };

        void Update_HandleValueChangeOrSliderStop(
            const bool DID_VALUE_CHANGE, const bool DID_SLIDER_STOP)
        {
            if ((DID_VALUE_CHANGE == false) && (DID_SLIDER_STOP == false))
            {
                return;
            }

            CallbackFlags callbackFlags;

            if ((IsMovingToward() == false) && (IsAtFrom() || DID_SLIDER_STOP))
            {
                callbackFlags = Update_HandleValueChangeOrSliderStop_HandleReachingFrom();
            }
            else if (IsMovingToward() && (IsAtTo() || DID_SLIDER_STOP))
            {
                callbackFlags = Update_HandleValueChangeOrSliderStop_HandleReachingTo();
            }

            Update_HandleValueChangeOrSliderStop_HandleCallingOnChangeFunctions(callbackFlags);
        }

        const CallbackFlags Update_HandleValueChangeOrSliderStop_HandleReachingFrom()
        {
            CallbackFlags flags;
            flags.reached_from = true;

            SetFrom();

            ++cycleCount_;
            const auto HAS_CYCLE_COUNT_LIMIT_BEEN_REACHED { CycleCountLimitReached() };

            flags.reached_cycle_limit = HAS_CYCLE_COUNT_LIMIT_BEEN_REACHED;

            const auto WILL_STOP { (
                (WillOscilate() == false) || HAS_CYCLE_COUNT_LIMIT_BEEN_REACHED) };

            if (WILL_STOP)
            {
                Stop();
                flags.stopped = true;
            }

            return flags;
        }

        const CallbackFlags Update_HandleValueChangeOrSliderStop_HandleReachingTo()
        {
            CallbackFlags flags;
            flags.reached_to = true;

            SetTo();

            const auto WILL_STOP { (WillOscilate() == false) };
            if (WILL_STOP)
            {
                Stop();
                flags.stopped = true;
            }

            return flags;
        }

        void Update_HandleValueChangeOrSliderStop_HandleCallingOnChangeFunctions(
            const CallbackFlags & FLAGS)
        {
            OnUpdateChange();

            if (FLAGS.stopped)
            {
                OnUpdateChangeStop();
            }

            if (FLAGS.reached_from)
            {
                OnUpdateChangeReachesFrom();
            }

            if (FLAGS.reached_to)
            {
                OnUpdateChangeReachesTo();
            }

            if (FLAGS.reached_cycle_limit)
            {
                OnUpdateChangeReachesCycleCountLimit();
            }
        }

        bool isStopped_;
        bool isMovingToward_;
        float speed_;
        std::size_t cycleCount_;
        std::size_t cycleCountLimit_;
        WillOscillate willOscillate_;
        SliderZeroToOne sliderZeroToOne_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDER_BASE_HPP_INCLUDED
