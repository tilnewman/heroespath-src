// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SLIDER_POSITION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDER_POSITION_HPP_INCLUDED
//
//  slider-position.hpp
//
#include "sfml-util/moving-enum.hpp"
#include "sfml-util/slider-base.hpp"

#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{

    // Uses the smooth motion slider method to oscillate a position between two points.
    class PosSlider : public SliderBase
    {
    public:
        // default constructor has all zero values and is stopped
        PosSlider()
            : SliderBase()
            , from_(0.0f, 0.0f)
            , to_(0.0f, 0.0f)
            , value_(0.0f, 0.0f)
        {}

        PosSlider(
            const sf::Vector2f & FROM,
            const sf::Vector2f & TO,
            const float SPEED,
            const WillOscillate WILL_OSCILLATE,
            const WillAutoStart WILL_AUTO_START,
            const std::size_t CYCLE_COUNT_LIMIT = 0)
            : SliderBase(SPEED, WILL_OSCILLATE, WILL_AUTO_START, CYCLE_COUNT_LIMIT)
            , from_(FROM)
            , to_(TO)
            , value_(FROM)
        {}

        virtual ~PosSlider() = default;

        PosSlider(const PosSlider &) = default;
        PosSlider(PosSlider &&) = default;
        PosSlider & operator=(const PosSlider &) = default;
        PosSlider & operator=(PosSlider &&) = default;

        bool IsAtFrom() const override { return IsEqual(value_, from_); }
        bool IsAtTo() const override { return IsEqual(value_, to_); }

        const sf::Vector2f From() const { return from_; }
        const sf::Vector2f To() const { return to_; }
        const sf::Vector2f Value() const { return value_; }

        const sf::Vector2f UpdateAndReturnValue(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return Value();
        }

    protected:
        bool UpdateImpl() override
        {
            const auto VALUE_BEFORE { value_ };

            const auto RATIO { PositionRatio() };

            const auto HORIZ_DIFF { static_cast<float>(To().x - From().x) };
            const auto VERT_DIFF { static_cast<float>(To().y - From().y) };

            const auto NEW_POS_LEFT { static_cast<float>(From().x) + (HORIZ_DIFF * RATIO) };
            const auto NEW_POS_TOP { static_cast<float>(From().y) + (VERT_DIFF * RATIO) };

            value_ = sf::Vector2f(NEW_POS_LEFT, NEW_POS_TOP);

            return (IsEqual(VALUE_BEFORE, value_) == false);
        }

        void SetFromImpl() override { value_ = from_; }
        void SetToImpl() override { value_ = to_; }

    private:
        bool IsEqual(const sf::Vector2f & LHS, const sf::Vector2f & RHS) const
        {
            return (misc::IsRealClose(LHS.x, RHS.x) && misc::IsRealClose(LHS.y, RHS.y));
        }

        sf::Vector2f from_;
        sf::Vector2f to_;
        sf::Vector2f value_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDER_POSITION_HPP_INCLUDED
