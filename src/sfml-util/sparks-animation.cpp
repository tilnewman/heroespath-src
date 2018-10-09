// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sparks-animation.hpp
//
#include "sparks-animation.hpp"

#include "misc/random.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace sfml_util
{
    namespace animation
    {

        Spark::Spark(
            const sf::Texture & TEXTURE,
            const sf::Vector2f & START_POS_V,
            const sf::Vector2f & END_POS_V,
            const float BASE_SPEED,
            const float START_SCALE,
            const float END_SCALE,
            const float ROTATION_SPEED,
            const sf::Color & START_COLOR,
            const sf::Color & END_COLOR)
            : isFinished_(false)
            , sprite_(TEXTURE)
            , startPosV_(START_POS_V)
            , endPosV_(END_POS_V)
            , startScale_(START_SCALE)
            , endScale_(END_SCALE)
            , rotationSpeed_(ROTATION_SPEED)
            , startColor_(START_COLOR)
            , endColor_(END_COLOR)
            , slider_(
                  BASE_SPEED,
                  0.5f) // start half way so everything moves fast at first and then slows down
        {
            // set initial sprite values (initial rotation is random)
            sprite_.setPosition(0.0f, 0.0f);
            sprite_.setScale(1.0f, 1.0f);
            sprite_.setOrigin(
                sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);
            sprite_.rotate(misc::random::Float(360.0f));
            sprite_.setOrigin(0.0f, 0.0f);
            //
            sprite_.setPosition(startPosV_);
            sprite_.setScale(startScale_, startScale_);
            sprite_.setColor(startColor_);
        }

        bool Spark::Update(const float ELAPSED_TIME_SEC)
        {
            if (isFinished_)
            {
                return true;
            }

            const auto SLIDER_POS { (slider_.Update(ELAPSED_TIME_SEC) - 0.5f) * 2.0f };

            // set rotation (must preset position and scale prior to rotating...grumble)
            sprite_.setPosition(0.0f, 0.0f);
            sprite_.setScale(1.0f, 1.0f);
            sprite_.setOrigin(
                sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);
            sprite_.rotate(rotationSpeed_ * SLIDER_POS);
            // sprite_.setOrigin(0.0f, 0.0f);

            // set position
            const auto POS_X { startPosV_.x + ((endPosV_.x - startPosV_.x) * SLIDER_POS) };
            const auto POS_Y { startPosV_.y + ((endPosV_.y - startPosV_.y) * SLIDER_POS) };
            sprite_.setPosition(POS_X, POS_Y);

            // set color
            sprite_.setColor(sfutil::Transition(startColor_, endColor_, SLIDER_POS));

            // set scale
            const auto SCALE { startScale_ - ((endScale_ - startScale_) * (1.0f - SLIDER_POS)) };
            sprite_.setScale(SCALE, SCALE);

            // check if done animating
            isFinished_ = slider_.IsDone();
            return isFinished_;
        }

        void Spark::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            target.draw(sprite_, states);
        }

        SparksAnimation::SparksAnimation(
            const bool WILL_EMIT_RIGHT,
            const sf::FloatRect & REGION,
            const float SPRAY_RATIO,
            const float SCALE_BASE,
            const float SCALE_VARIATION_RATIO,
            const float EMIT_RATE_RATIO,
            const float DURATION_SEC,
            const float SPEED_BASE,
            const float SPEED_VARIATION_RATIO,
            const float ROTATION_SPEED_BASE,
            const float ROTATION_SPEED_VARIATION_RATIO)
            : WILL_EMIT_RIGHT_(WILL_EMIT_RIGHT)
            , REGION_(REGION)
            , SPRAY_RATIO_MINOR_(SPRAY_RATIO)
            , SPRAY_RATIO_MAJOR_(1.0f - SPRAY_RATIO)
            , SCALE_BASE_(SCALE_BASE)
            , SCALE_VAR_RATIO_(SCALE_VARIATION_RATIO)
            , SEC_PER_EMIT_(1.0f / (50.0f + (EMIT_RATE_RATIO * 150.0f)))
            , DURATION_SEC_(DURATION_SEC)
            , SPEED_BASE_(SPEED_BASE)
            , SPEED_VAR_RATIO_(SPEED_VARIATION_RATIO)
            , ROTATION_SPEED_BASE_(ROTATION_SPEED_BASE)
            , ROTATION_SPEED_VAR_RATIO_(ROTATION_SPEED_VARIATION_RATIO)
            , emitTimerSec_(0.0f)
            , durationTimerSec_(0.0f)
            , isFinished_(false)
            , sparkCachedTexture1_(
                  "media-images-misc-spark1", ImageOptions(ImageOpt::Default, sf::Color::Black))
            , sparkCachedTexture2_(
                  "media-images-misc-spark2", ImageOptions(ImageOpt::Default, sf::Color::Black))
            , sparkCachedTexture3_(
                  "media-images-misc-spark3", ImageOptions(ImageOpt::Default, sf::Color::Black))
            , sparkVec_()
        {
            sparkVec_.reserve(static_cast<std::size_t>((1.0f / SEC_PER_EMIT_) * DURATION_SEC) + 2);
        }

        void SparksAnimation::Update(const float ELAPSED_TIME_SEC)
        {
            durationTimerSec_ += ELAPSED_TIME_SEC;
            isFinished_ = (durationTimerSec_ > DURATION_SEC_);

            emitTimerSec_ += ELAPSED_TIME_SEC;
            while ((false == isFinished_) && (emitTimerSec_ > SEC_PER_EMIT_))
            {
                emitTimerSec_ -= SEC_PER_EMIT_;

                const auto EMITTER_VERTICAL_SPAN { (SPRAY_RATIO_MINOR_ * REGION_.height) };
                const auto START_POS_TOP { ((REGION_.top + (REGION_.height * 0.5f))
                                            - (EMITTER_VERTICAL_SPAN * 0.5f))
                                           + misc::random::Float(EMITTER_VERTICAL_SPAN) };

                const auto EMITTER_HORIZ_OFFSET { misc::random::Float(
                    SPRAY_RATIO_MINOR_ * 0.5f * REGION_.width) };
                const auto START_POS_LEFT { (
                    (WILL_EMIT_RIGHT_) ? REGION_.left + EMITTER_HORIZ_OFFSET
                                       : (REGION_.left + REGION_.width) - EMITTER_HORIZ_OFFSET) };

                const auto TARGET_VERT_SPAN { misc::random::Float(
                    REGION_.height * SPRAY_RATIO_MAJOR_) };
                const auto END_POS_TOP { (
                    (misc::random::Bool()) ? START_POS_TOP + TARGET_VERT_SPAN
                                           : START_POS_TOP - TARGET_VERT_SPAN) };

                const auto TARGET_HORIZ_SPAN_MIN { (REGION_.width * SPRAY_RATIO_MINOR_) };
                const auto TARGET_HORIZ_SPAN_MAX { (REGION_.width * SPRAY_RATIO_MAJOR_) };
                const auto TARGET_HORIZ_SPAN { misc::random::Float(
                    TARGET_HORIZ_SPAN_MIN, TARGET_HORIZ_SPAN_MAX) };
                const auto END_POS_LEFT { (
                    (WILL_EMIT_RIGHT_) ? START_POS_LEFT + TARGET_HORIZ_SPAN
                                       : START_POS_LEFT - TARGET_HORIZ_SPAN) };

                const sf::Texture & RANDOM_TEXTURE_REF { [&]() {
                    const auto RAND { misc::random::Int(2) };
                    if (RAND == 0)
                    {
                        return sparkCachedTexture1_.Get();
                    }
                    else if (RAND == 1)
                    {
                        return sparkCachedTexture2_.Get();
                    }
                    else
                    {
                        return sparkCachedTexture3_.Get();
                    }
                }() };

                sparkVec_.emplace_back(Spark(
                    RANDOM_TEXTURE_REF,
                    sf::Vector2f(START_POS_LEFT, START_POS_TOP),
                    sf::Vector2f(END_POS_LEFT, END_POS_TOP),
                    ValueWithRandomVariance(SPEED_BASE_, SPEED_VAR_RATIO_),
                    ValueWithRandomVariance(SCALE_BASE_, SCALE_VAR_RATIO_),
                    0.001f,
                    ValueWithRandomVariance(ROTATION_SPEED_BASE_, ROTATION_SPEED_VAR_RATIO_),
                    sf::Color(
                        255,
                        255,
                        static_cast<sf::Uint8>(misc::random::Int(255)),
                        static_cast<sf::Uint8>(misc::random::Int(0, 127))),
                    sf::Color(255, 255, static_cast<sf::Uint8>(misc::random::Int(255)), 255)));
            }

            for (auto & nextSpark : sparkVec_)
            {
                if (nextSpark.IsFinished() == false)
                {
                    nextSpark.Update(ELAPSED_TIME_SEC);
                }
            }
        }

        void SparksAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            for (auto & nextSpark : sparkVec_)
            {
                if (nextSpark.IsFinished() == false)
                {
                    nextSpark.draw(target, states);
                }
            }
        }

        float SparksAnimation::ValueWithRandomVariance(
            const float BASE, const float VARIANCE_RATIO) const
        {
            if (misc::IsRealZero(VARIANCE_RATIO))
            {
                return BASE;
            }
            else
            {
                const auto VARIATION_SPAN { BASE * VARIANCE_RATIO };
                return (BASE - (VARIATION_SPAN * 0.5f)) + misc::random::Float(VARIATION_SPAN);
            }
        }

    } // namespace animation
} // namespace sfml_util
} // namespace heroespath
