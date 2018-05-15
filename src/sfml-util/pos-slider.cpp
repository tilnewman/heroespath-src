// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
//  pos-slider.cpp
//
#include "pos-slider.hpp"

#include "log/log-macros.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace sliders
    {

        PosSlider::PosSlider()
            : origFromPosV_(0.0f, 0.0f)
            , origToPosV_(0.0f, 0.0f)
            , fromPosV_(0.0f, 0.0f)
            , toPosV_(0.0f, 0.0f)
            , direction_(sfml_util::Moving::Toward)
            , isMoving_(false)
            , slider_()
        {}

        PosSlider::PosSlider(
            const sf::Vector2f & FROM_POS_V,
            const sf::Vector2f & TO_POS_V,
            const float SLIDER_SPEED)
            : origFromPosV_()
            , // see Constructor's call to Setup() for actual default values
            origToPosV_()
            , fromPosV_()
            , toPosV_()
            , direction_(sfml_util::Moving::Count)
            , isMoving_(false)
            , slider_()
        {
            // will not start moving immediately
            Setup(FROM_POS_V, TO_POS_V, SLIDER_SPEED);
        }

        void PosSlider::Setup(
            const sf::Vector2f & FROM_POS_V,
            const sf::Vector2f & TO_POS_V,
            const float SLIDER_SPEED)
        {
            origFromPosV_ = FROM_POS_V;
            origToPosV_ = TO_POS_V;
            fromPosV_ = FROM_POS_V;
            toPosV_ = TO_POS_V;
            slider_.Reset(SLIDER_SPEED);
            direction_ = Moving::Toward;
            isMoving_ = false;
        }

        void PosSlider::ChangeDirection()
        {
            if (direction_ == Moving::Toward)
            {
                fromPosV_ = Position();
                toPosV_ = origFromPosV_;
                slider_.Reset(slider_.Speed());
                direction_ = Moving::Away;
            }
            else if (direction_ == Moving::Away)
            {
                fromPosV_ = Position();
                toPosV_ = origToPosV_;
                slider_.Reset(slider_.Speed());
                direction_ = Moving::Toward;
            }
        }

        bool PosSlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
        {
            if (isMoving_)
            {
                slider_.Update(ELAPSED_TIME_SECONDS);
                isMoving_ = !slider_.IsDone();
                return true;
            }
            else
            {
                return false;
            }
        }

        const sf::Vector2f PosSlider::Position() const
        {
            auto const PROGRESS_RATIO{ ProgressRatio() };
            auto const POS_LEFT{ fromPosV_.x + ((toPosV_.x - fromPosV_.x) * PROGRESS_RATIO) };
            auto const POS_TOP{ fromPosV_.y + ((toPosV_.y - fromPosV_.y) * PROGRESS_RATIO) };
            return sf::Vector2f(POS_LEFT, POS_TOP);
        }
    } // namespace sliders
} // namespace sfml_util
} // namespace heroespath
