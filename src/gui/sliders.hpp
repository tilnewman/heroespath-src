// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDERS_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDERS_HPP_INCLUDED
//
// sliders.hpp
//
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/math-constants.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"
#include "misc/type-helpers.hpp"

#include "misc/nameof.hpp"

#include <string>
#include <tuple>
#include <type_traits>

namespace heroespath
{
namespace gui
{

    // All Sliders start at the given initial value and change when Update() is called until they
    // reach their target value or until Stop() is called.  Once IsStopped() all Sliders will stay
    // stopped.  SliderZeroToOne and SliderFromTo will both Stop() once they reach their target
    // value.  SliderOscillator and SliderDrift will always keep moving and never stop on their own.

    // All Sliders have been designed to tolerate as much invalid input as possible.  If a Slider
    // cannot be constructed with the params given then Stop() will be called and a detailed error
    // will be logged.  This will also happen if a slider is constructed in a way that will prevent
    // Value() from ever changing no matter how often Update() is called or what values are passed
    // to Update().  So you can check IsStopped() after construction to determine if the slider will
    // work.

    // All sliders can be default constructed which will leave them with all zero members and
    // stopped without logging any errors.

    // All sliders are designed to work with typical game framerate second values typically between
    // 30fps and 60fps or [0.016, 0.033].  These called ADJUSTMENT in the code below and are passed
    // to Update().  All slider are also designed to work with speed values typically between [1,
    // 10]. Since the first thing that the Update() function does is multiply ADJUSTMENT*SPEED, that
    // reveals that sliders are designed to work in increments of approximately [0.016, 0.33], which
    // will result the following number of iterations required to slide Value() from From() to
    // To(): [16, 300].  So if you will be calling Update() with values outside of [0.016, 0.033]
    // then you can simply adjust your speed to compensate.

    // A collection of functions that help Sliders validate their setup parameters.
    struct SliderValidators
    {
        template <typename T>
        static void SpeedShouldNotBeNegative(
            bool & isStopped, const T SPEED, const std::string & FILE_FUNC_LINE_STR)
        {
            if (SPEED < 0.0f)
            {
                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The given SPEED<" + NAMEOF_TYPE_T_STR(T)
                    + ">=" + misc::ToString(SPEED)
                    + " is less than zero which is invalid.  This slider will be "
                      "stopped.");

                isStopped = true;
            }
        }

        template <typename T>
        static T StartAtClamp(
            const T FROM, const T TO, const T START_AT_ORIG, const std::string & FILE_FUNC_LINE_STR)
        {
            auto startAtFinal { START_AT_ORIG };

            if (START_AT_ORIG < FROM)
            {
                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The given START_AT_ORIG<" + NAMEOF_TYPE_T_STR(T)
                    + ">=" + misc::ToString(START_AT_ORIG) + " is invalid (<FROM)(<"
                    + misc::ToString(FROM)
                    + ").  The actual starting ratio will be set to that FROM("
                    + misc::ToString(FROM) + ").");

                startAtFinal = FROM;
            }
            else if (START_AT_ORIG > TO)
            {
                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The given START_AT_ORIG<" + NAMEOF_TYPE_T_STR(T)
                    + ">=" + misc::ToString(START_AT_ORIG) + " is invalid (>TO)(>"
                    + misc::ToString(TO) + ").  The actual start value will be set to "
                    + misc::ToString(TO) + ".");

                startAtFinal = TO;
            }

            return startAtFinal;
        }

        template <typename T>
        static void FromTo(
            bool & isStopped, const T FROM, const T TO, const std::string & FILE_FUNC_LINE_STR)
        {
            if (misc::IsRealClose(FROM, TO))
            {
                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The given FROM<" + NAMEOF_TYPE_T_STR(T) + ">="
                    + misc::ToString(FROM) + " is the same as the given TO=" + misc::ToString(TO)
                    + ", which is invalid.  This slider will be stopped.");

                isStopped = true;
            }
        }

        // returns (VALUE_TO_START_AT_FINAL, VALUE_OF_FIRST_TARGET_FINAL)
        template <typename T>
        static const std::tuple<T, T> Drift(
            bool & isStopped,
            const T VALUE_MIN_ORIG,
            const T VALUE_MAX_ORIG,
            const T VALUE_TO_START_AT_ORIG,
            const T VALUE_OF_FIRST_TARGET_ORIG,
            const std::string & FILE_FUNC_LINE_STR)
        {
            const T VALUE_MIN_FINAL { std::min(VALUE_MIN_ORIG, VALUE_MAX_ORIG) };
            const T VALUE_MAX_FINAL { std::max(VALUE_MIN_ORIG, VALUE_MAX_ORIG) };

            if (misc::IsRealClose(VALUE_MIN_FINAL, VALUE_MAX_FINAL))
            {
                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The VALUE_MIN_FINAL<" + NAMEOF_TYPE_T_STR(T)
                    + ">(" + misc::ToString(VALUE_MIN_FINAL) + ") is equal to the VALUE_MAX_FINAL("
                    + misc::ToString(VALUE_MAX_FINAL)
                    + ", which is invalid.  This slider will be stopped.");

                isStopped = true;
            }

            auto valueToStartAtFinal { VALUE_TO_START_AT_ORIG };

            if ((VALUE_TO_START_AT_ORIG < VALUE_MIN_FINAL)
                || (VALUE_TO_START_AT_ORIG > VALUE_MAX_FINAL))
            {
                valueToStartAtFinal
                    = std::clamp(VALUE_TO_START_AT_ORIG, VALUE_MIN_FINAL, VALUE_MAX_FINAL);

                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The VALUE_TO_START_AT_ORIG<"
                    + NAMEOF_TYPE_T_STR(T) + ">(" + misc::ToString(VALUE_TO_START_AT_ORIG)
                    + ") not within the valid interval of [VALUE_MIN_FINAL, VALUE_MAX_FINAL] or ["
                    + misc::ToString(VALUE_MIN_FINAL) + ", " + misc::ToString(VALUE_MAX_FINAL)
                    + "].  VALUE_TO_START_AT_ORIG will be changed/clamped to "
                    + misc::ToString(valueToStartAtFinal) + ".");
            }

            auto valueOfFirstTargetFinal { VALUE_OF_FIRST_TARGET_ORIG };

            if ((VALUE_OF_FIRST_TARGET_ORIG < VALUE_MIN_FINAL)
                || (VALUE_OF_FIRST_TARGET_ORIG > VALUE_MAX_FINAL))
            {
                valueOfFirstTargetFinal
                    = std::clamp(VALUE_OF_FIRST_TARGET_ORIG, VALUE_MIN_FINAL, VALUE_MAX_FINAL);

                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The VALUE_OF_FIRST_TARGET_ORIG<"
                    + NAMEOF_TYPE_T_STR(T) + ">(" + misc::ToString(VALUE_OF_FIRST_TARGET_ORIG)
                    + ") not within the valid interval of [VALUE_MIN_FINAL, VALUE_MAX_FINAL] or ["
                    + misc::ToString(VALUE_MIN_FINAL) + ", " + misc::ToString(VALUE_MAX_FINAL)
                    + "].  VALUE_OF_FIRST_TARGET_ORIG will be changed/clamped to "
                    + misc::ToString(valueOfFirstTargetFinal) + ".");
            }

            if (misc::IsRealClose(valueToStartAtFinal, valueOfFirstTargetFinal))
            {
                auto const VALUE_OF_FIRST_TARGET_FINAL_BEFORE { valueOfFirstTargetFinal };

                if (VALUE_MIN_FINAL < valueToStartAtFinal)
                {
                    valueOfFirstTargetFinal = VALUE_MIN_FINAL;
                }
                else
                {
                    valueOfFirstTargetFinal = VALUE_MAX_FINAL;
                }

                M_HP_LOG_ERR(
                    "(" + FILE_FUNC_LINE_STR + ")  The valueToStartAtFinal<" + NAMEOF_TYPE_T_STR(T)
                    + ">(" + misc::ToString(valueToStartAtFinal) + ")=valueOfFirstTargetFinal("
                    + misc::ToString(VALUE_OF_FIRST_TARGET_FINAL_BEFORE)
                    + "), which is invalid.  So valueOfFirstTargetFinal has been changed to  "
                    + misc::ToString(valueOfFirstTargetFinal) + ".");
            }

            return std::make_tuple(valueToStartAtFinal, valueOfFirstTargetFinal);
        }
    };

    // Slides Value() from 0 to 1 smoothly using sine motion and then stops.
    //
    // If SPEED < zero then it will be changed to zero, Stop() will be called, and an error
    // will be logged.  If START_AT is not within [0, 1] then it will be clamped to that
    // interval and an error will be logged. If START_AT is not zero then the speed will not
    // start slow and smooth.  Motion is fastest (bounce-like) when START_AT=0.5.
    class SliderZeroToOne
    {
    public:
        SliderZeroToOne()
            : isStopped_(true)
            , speed_(0.0f)
            , value_(0.0f)
            , radiansFrom_(0.0f)
            , radiansTo_(0.0f)
            , radians_(0.0f)
        {}

        explicit SliderZeroToOne(const float SPEED, const float START_AT_ORIG = 0.0f)
            : isStopped_(false)
            , speed_(SPEED)
            , value_(SliderValidators::StartAtClamp(
                  0.0f, 1.0f, START_AT_ORIG, M_HP_FILE_FUNC_LINE_STR))
            , radiansFrom_(misc::math::half_pi<float>)
            , radiansTo_(1.5f * misc::math::pi<float>)
            , radians_(radiansFrom_ + (misc::math::pi<float> * value_))
        {
            SliderValidators::SpeedShouldNotBeNegative(isStopped_, SPEED, M_HP_FILE_FUNC_LINE_STR);

            if (misc::IsRealClose(value_, 1.0f))
            {
                M_HP_LOG_ERR(
                    "The final START_AT=" + misc::ToString(value_)
                    + " is equal to the TO value of 1.0f.  That means this slider is starting "
                      "where it ends and cannot move, so "
                      "this slider will be stopped.");

                isStopped_ = true;
            }
        }

        virtual ~SliderZeroToOne() = default;

        SliderZeroToOne(const SliderZeroToOne &) = default;
        SliderZeroToOne(SliderZeroToOne &&) = default;
        SliderZeroToOne & operator=(const SliderZeroToOne &) = default;
        SliderZeroToOne & operator=(SliderZeroToOne &&) = default;

        float From() const { return 0.0f; }
        float To() const { return 1.0f; }
        float Value() const { return value_; }
        float Speed() const { return speed_; }
        bool IsStopped() const { return isStopped_; }
        bool IsMoving() const { return !IsStopped(); }
        void Stop() { isStopped_ = true; }

        float Update(const float ADJUSTMENT)
        {
            if (!isStopped_)
            {
                radians_ += (ADJUSTMENT * speed_);

                if (radians_ < radiansFrom_)
                {
                    radians_ = radiansFrom_;
                    value_ = 0.0f;
                }
                else if (misc::IsRealClose(radians_, radiansTo_) || (radians_ > radiansTo_))
                {
                    radians_ = radiansTo_;
                    value_ = 1.0f;
                    Stop();
                }
                else
                {
                    value_ = ((2.0f - (sin(radians_) + 1.0f)) * 0.5f);
                    value_ = std::clamp(value_, 0.0f, 1.0f);
                }
            }

            return value_;
        }

    private:
        bool isStopped_;
        float speed_;
        float value_;
        float radiansFrom_;
        float radiansTo_;
        float radians_;
    };

    // Slides Value() over [FROM, TO] smoothly using sine motion and then stops.
    //
    // If SPEED <= zero then it will be changed to zero, Stop() will be called, and an error
    // will be logged.  FROM>TO is supported and not considered an error, however, FROM==TO is
    // not supported and is considered an error that will cause Stop() to be called and an error
    // to be logged.
    template <typename T, typename = std::enable_if_t<misc::are_arithmetic_nobool_v<T>>>
    class SliderFromTo
    {
    public:
        SliderFromTo()
            : isStopped_(true)
            , from_(T(0))
            , to_(T(0))
            , max_(T(0))
            , min_(T(0))
            , diff_(0.0f)
            , speed_(0.0f)
            , value_(T(0))
            , sliderZeroToOne_()
        {}

        SliderFromTo(const T FROM, const T TO, const float SPEED)
            : isStopped_(false)
            , from_(FROM)
            , to_(TO)
            , max_(std::max(FROM, TO))
            , min_(std::min(FROM, TO))
            , diff_(static_cast<float>(TO - FROM))
            , speed_(SPEED)
            , value_(FROM)
            , sliderZeroToOne_(speed_)
        {
            SliderValidators::SpeedShouldNotBeNegative(isStopped_, SPEED, M_HP_FILE_FUNC_LINE_STR);
            SliderValidators::FromTo(isStopped_, FROM, TO, M_HP_FILE_FUNC_LINE_STR);

            if (isStopped_)
            {
                sliderZeroToOne_.Stop();
            }
        }

        virtual ~SliderFromTo() = default;

        SliderFromTo(const SliderFromTo &) = default;
        SliderFromTo(SliderFromTo &&) = default;
        SliderFromTo & operator=(const SliderFromTo &) = default;
        SliderFromTo & operator=(SliderFromTo &&) = default;

        T From() const { return from_; }
        T To() const { return to_; }
        T Value() const { return value_; }
        float Speed() const { return speed_; }
        bool IsStopped() const { return isStopped_; }
        bool IsMoving() const { return !IsStopped(); }
        void Stop() { isStopped_ = true; }

        T Update(const float ADJUSTMENT)
        {
            if (!isStopped_)
            {
                const float RATIO { sliderZeroToOne_.Update(ADJUSTMENT) };
                value_ = static_cast<T>(static_cast<float>(from_) + (diff_ * RATIO));
                value_ = std::clamp(value_, min_, max_);
                isStopped_ = sliderZeroToOne_.IsStopped();
            }

            return value_;
        }

    private:
        bool isStopped_;
        T from_;
        T to_;
        T max_;
        T min_;
        float diff_;
        float speed_;
        T value_;
        SliderZeroToOne sliderZeroToOne_;
    };

    // Slides Value() back and forth over [FROM, TO] smoothly using sine motion.
    //
    // If SPEED <= zero then it will be changed to zero, Stop() will be called, and an error
    // will be logged.  Supports FROM<TO and FROM>TO, but if FROM==TO then Stop will be
    // called and an error will be logged. If START_AT is not within [0, 1] then it will be
    // clamped to that interval and an error will be logged. If START_AT is not zero then
    // the speed will not start slow and smooth.  Motion is fastest (bounce-like) when
    // START_AT=0.5.
    template <typename T, typename = std::enable_if_t<misc::are_arithmetic_nobool_v<T>>>
    class SliderOscillator
    {
    public:
        SliderOscillator()
            : isStopped_(true)
            , from_(T(0))
            , to_(T(0))
            , speed_(0.0f)
            , value_(T(0))
            , sliderFromTo_()
        {}

        // Use this constructor to start Value() at FROM.
        SliderOscillator(const T FROM, const T TO, const float SPEED)
            : isStopped_(false)
            , from_(FROM)
            , to_(TO)
            , speed_(0.0f)
            , value_(0)
            , sliderFromTo_()
        {
            Setup(FROM, TO, SPEED, FROM, M_HP_FILE_FUNC_LINE_STR);
        }

        // Use this constructor if you want to specify the starting value.
        SliderOscillator(const T FROM, const T TO, const float SPEED, const T START_AT)
            : isStopped_(false)
            , from_(FROM)
            , to_(TO)
            , speed_(0.0f)
            , value_(0)
            , sliderFromTo_()
        {
            Setup(FROM, TO, SPEED, START_AT, M_HP_FILE_FUNC_LINE_STR);
        }

        virtual ~SliderOscillator() = default;

        SliderOscillator(const SliderOscillator &) = default;
        SliderOscillator(SliderOscillator &&) = default;
        SliderOscillator & operator=(const SliderOscillator &) = default;
        SliderOscillator & operator=(SliderOscillator &&) = default;

        T From() const { return from_; }
        T To() const { return to_; }
        float Speed() const { return speed_; }
        T Value() const { return value_; }
        bool IsStopped() const { return isStopped_; }
        bool IsMoving() const { return !IsStopped(); }
        void Stop() { isStopped_ = true; }

        T Update(const float ADJUSTMENT)
        {
            if (!isStopped_)
            {
                value_ = sliderFromTo_.Update(ADJUSTMENT);

                if (sliderFromTo_.IsStopped())
                {
                    // restart the slave slider targeting the end opposite what was just reached.
                    //
                    // Why not just swap sliderFromTo_'s from/to?  -Because the first time the
                    // target is reached and sliderFromTo_ stops, it might have been the first time,
                    // which might have started with a custom START_AT, meaning that
                    // sliderFromTo_.From() might not be the same as from_, so we can't use
                    // sliderFromTo_.From() here.
                    if (misc::IsRealClose(sliderFromTo_.To(), to_))
                    {
                        sliderFromTo_ = SliderFromTo<T>(to_, from_, speed_);
                    }
                    else
                    {
                        sliderFromTo_ = SliderFromTo<T>(from_, to_, speed_);
                    }
                }
            }

            return value_;
        }

    private:
        void Setup(
            const T FROM,
            const T TO,
            const float SPEED,
            const T START_AT_ORIG,
            const std::string & FILE_FUNC_LINE_STR)
        {
            SliderValidators::FromTo(isStopped_, FROM, TO, FILE_FUNC_LINE_STR);
            SliderValidators::SpeedShouldNotBeNegative(isStopped_, SPEED, FILE_FUNC_LINE_STR);

            value_ = SliderValidators::StartAtClamp(
                std::min(FROM, TO), std::max(FROM, TO), START_AT_ORIG, FILE_FUNC_LINE_STR);

            if (false == isStopped_)
            {
                // If StartAtClamp() set value_ to TO then start reversed
                if (misc::IsRealClose(value_, TO))
                {
                    sliderFromTo_ = SliderFromTo<T>(to_, from_, speed_);
                }
                else
                {
                    sliderFromTo_ = SliderFromTo<T>(value_, TO, speed_);
                }
            }
        }

        bool isStopped_;
        T from_;
        T to_;
        float speed_;
        T value_;
        SliderFromTo<T> sliderFromTo_;
    };

    // Slides Value() from it's starting point to a randomly selected target within
    // [VALUE_MIN, VALUE_MAX] smoothly using sine motion at a speed randomly chosen within
    // [SPEED_MIN, SPEED_MAX].  The speed is changed every time the target changes.
    //
    // It is not an error if VALUE_MIN>VALUE_MAX, however, if VALUE_MIN==VALUE_MAX then
    // Stop() will be called and an error will be logged.  It is not an error if
    // SPEED_MIN>SPEED_MAX or if SPEED_MIN=SPEED_MAX.  If VALUE_TO_START_AT or
    // VALUE_OF_FIRST_TARGET is not within [VALUE_MIN, VALUE_MAX] then it is clamped and
    // an error is logged.  If VALUE_TO_START_AT=VALUE_OF_FIRST_TARGET then
    // VALUE_OF_FIRST_TARGET will be changed to either VALUE_MIN or VALUE_MAX.
    template <typename T, typename = std::enable_if_t<misc::are_arithmetic_nobool_v<T>>>
    class SliderDrift
    {
    public:
        SliderDrift()
            : isStopped_(true)
            , valueMin_(T(0))
            , valueMax_(T(0))
            , value_(T(0))
            , speedMin_(0.0f)
            , speedMax_(0.0f)
            , speed_(0.0f)
            , sliderFromTo_()
        {}

        SliderDrift(
            const T VALUE_MIN, const T VALUE_MAX, const float SPEED_MIN, const float SPEED_MAX)
            : isStopped_(false)
            , valueMin_(VALUE_MIN)
            , valueMax_(VALUE_MAX)
            , value_(VALUE_MIN)
            , speedMin_(SPEED_MIN)
            , speedMax_(SPEED_MAX)
            , speed_(SPEED_MIN)
            , sliderFromTo_()
        {
            const auto [RANDOM_VALUE_TO_START_AT, RANDOM_VALUE_OF_FIRST_TARGET]
                = RandomStartAndTargetValue(VALUE_MIN, VALUE_MAX);

            Setup(RANDOM_VALUE_TO_START_AT, RANDOM_VALUE_OF_FIRST_TARGET, M_HP_FILE_FUNC_LINE_STR);
        }

        // This constructor to set a specific starting value and target.
        SliderDrift(
            const T VALUE_MIN,
            const T VALUE_MAX,
            const float SPEED_MIN,
            const float SPEED_MAX,
            const T VALUE_TO_START_AT,
            const T VALUE_OF_FIRST_TARGET)
            : isStopped_(false)
            , valueMin_(VALUE_MIN)
            , valueMax_(VALUE_MAX)
            , value_(VALUE_MIN)
            , speedMin_(SPEED_MIN)
            , speedMax_(SPEED_MAX)
            , speed_(SPEED_MIN)
            , sliderFromTo_()
        {
            Setup(VALUE_TO_START_AT, VALUE_OF_FIRST_TARGET, M_HP_FILE_FUNC_LINE_STR);
        }

        virtual ~SliderDrift() = default;

        SliderDrift(const SliderDrift &) = default;
        SliderDrift(SliderDrift &&) = default;
        SliderDrift & operator=(const SliderDrift &) = default;
        SliderDrift & operator=(SliderDrift &&) = default;

        T ValueMin() const { return valueMin_; }
        T ValueMax() const { return valueMax_; }
        T Value() const { return value_; }

        float SpeedMin() const { return speedMin_; }
        float SpeedMax() const { return speedMax_; }
        float Speed() const { return speed_; }

        bool IsStopped() const { return isStopped_; }
        bool IsMoving() const { return !IsStopped(); }
        void Stop() { isStopped_ = true; }

        T Update(const float ADJUSTMENT)
        {
            if (!isStopped_)
            {
                value_ = sliderFromTo_.Update(ADJUSTMENT);

                if (sliderFromTo_.IsStopped())
                {
                    sliderFromTo_ = SliderFromTo<T>(
                        value_,
                        RandomValueWithinIntervalThatIsNot(valueMin_, valueMax_, value_),
                        RandomSpeed());
                }
            }

            return value_;
        }

    private:
        float RandomSpeed()
        {
            if (misc::IsRealClose(speedMin_, speedMax_))
            {
                return speedMin_;
            }
            else
            {
                return misc::Random(std::min(speedMin_, speedMax_), std::max(speedMin_, speedMax_));
            }
        }

        T RandomValueWithinIntervalThatIsNot(
            const T MIN_ORIG, const T MAX_ORIG, const T VALUE_TO_AVOID_ORIG) const
        {
            // convert to doubles because there is no misc::random<T> yet...
            const double MIN_DOUBLE { static_cast<double>(std::min(MIN_ORIG, MAX_ORIG)) };
            const double MAX_DOUBLE { static_cast<double>(std::max(MIN_ORIG, MAX_ORIG)) };
            const T RANDOM_VALUE { static_cast<T>(misc::Random(MIN_DOUBLE, MAX_DOUBLE)) };

            if (misc::IsRealClose(RANDOM_VALUE, VALUE_TO_AVOID_ORIG))
            {
                // if RANDOM_VALUE=VALUE_TO_AVOID then pick either the min or max
                if (misc::IsRealClose(MIN_ORIG, VALUE_TO_AVOID_ORIG))
                {
                    return MAX_ORIG;
                }
                else
                {
                    return MIN_ORIG;
                }
            }
            else
            {
                return RANDOM_VALUE;
            }
        }

        // returns (RANDOM_VALUE_TO_START_AT, RANDOM_VALUE_OF_TARGET)
        const std::tuple<T, T> RandomStartAndTargetValue(const T MIN_ORIG, const T MAX_ORIG)
        {
            const T MIN_FINAL { std::min(MIN_ORIG, MAX_ORIG) };
            const T MAX_FINAL { std::max(MIN_ORIG, MAX_ORIG) };

            const auto RANDOM_VALUE_TO_START_AT { RandomValueWithinIntervalThatIsNot(
                MIN_FINAL, MAX_FINAL, MIN_FINAL) };

            const auto RANDOM_VALUE_OF_TARGET { RandomValueWithinIntervalThatIsNot(
                MIN_FINAL, MAX_FINAL, RANDOM_VALUE_TO_START_AT) };

            return std::make_tuple(RANDOM_VALUE_TO_START_AT, RANDOM_VALUE_OF_TARGET);
        }

        void Setup(
            const T VALUE_TO_START_AT_ORIG,
            const T VALUE_OF_FIRST_TARGET_ORIG,
            const std::string & FILE_FUNC_LINE_STR)
        {
            const auto [VALUE_TO_START_AT_FINAL, VALUE_OF_FIRST_TARGET_FINAL]
                = SliderValidators::Drift(
                    isStopped_,
                    valueMin_,
                    valueMax_,
                    VALUE_TO_START_AT_ORIG,
                    VALUE_OF_FIRST_TARGET_ORIG,
                    FILE_FUNC_LINE_STR);

            value_ = VALUE_TO_START_AT_FINAL;

            speed_ = RandomSpeed();

            if (false == isStopped_)
            {
                sliderFromTo_
                    = SliderFromTo<T>(VALUE_TO_START_AT_FINAL, VALUE_OF_FIRST_TARGET_FINAL, speed_);
            }
        }

        bool isStopped_;
        T valueMin_;
        T valueMax_;
        T value_;
        float speedMin_;
        float speedMax_;
        float speed_;
        SliderFromTo<T> sliderFromTo_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDERS_HPP_INCLUDED
