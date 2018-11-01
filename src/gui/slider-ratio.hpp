// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDER_RATIO_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDER_RATIO_HPP_INCLUDED
//
//  slider-ratio.hpp
//
#include "gui/moving-enum.hpp"
#include "gui/slider-base.hpp"

namespace heroespath
{
namespace gui
{

    // Uses the smooth motion slider method to oscillate a float between [0.0f, 1.0f].
    class RatioSlider : public SliderBase
    {
    public:
        // default constructor has all zero values and is stopped
        RatioSlider()
            : SliderBase()
            , from_(0.0f)
            , to_(0.0f)
            , value_(0.0f)
        {}

        RatioSlider(
            const float SPEED,
            const WillOscillate WILL_OSCILLATE,
            const WillAutoStart WILL_AUTO_START,
            const std::size_t CYCLE_COUNT_LIMIT = 0)
            : SliderBase(SPEED, WILL_OSCILLATE, WILL_AUTO_START, CYCLE_COUNT_LIMIT)
            , from_(0.0f)
            , to_(1.0f)
            , value_(0.0f)
        {}

        virtual ~RatioSlider() = default;

        RatioSlider(const RatioSlider &) = default;
        RatioSlider(RatioSlider &&) = default;
        RatioSlider & operator=(const RatioSlider &) = default;
        RatioSlider & operator=(RatioSlider &&) = default;

        bool IsAtFrom() const override { return IsEqual(value_, from_); }
        bool IsAtTo() const override { return IsEqual(value_, to_); }

        float From() const { return from_; }
        float To() const { return to_; }
        float Value() const { return value_; }

        float UpdateAndReturnValue(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return Value();
        }

    protected:
        bool UpdateImpl() override
        {
            const auto VALUE_BEFORE { value_ };
            value_ = PositionRatio();
            return (IsEqual(VALUE_BEFORE, value_) == false);
        }

        void SetFromImpl() override { value_ = from_; }
        void SetToImpl() override { value_ = to_; }

    private:
        bool IsEqual(const float & LHS, const float & RHS) const
        {
            return misc::IsRealClose(LHS, RHS);
        }

        float from_;
        float to_;
        float value_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDER_RATIO_HPP_INCLUDED
