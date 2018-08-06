// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sparkle-animation.hpp
//
#include "sparkle-animation.hpp"

#include "misc/random.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>

namespace heroespath
{
namespace sfml_util
{
    namespace animation
    {

        Sparkle::Sparkle(
            const sf::Texture & TEXTURE,
            const sf::Vector2f & POS_V,
            const float SPEED,
            const float TARGET_SCALE)
            : isFinished_(false)
            , sprite_(TEXTURE)
            , posV_(POS_V)
            , targetScale_(TARGET_SCALE)
            ,

            // start half way so everything moves fast at first and then slows down
            slider_(std::max(1.0f, SPEED))
        {
            sprite_.setPosition(0.0f, 0.0f);
            sprite_.setScale(1.0f, 1.0f);

            sprite_.setOrigin(
                sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);

            sprite_.setRotation(misc::random::Float(360.0f));

            sprite_.setPosition(posV_);
            sprite_.setScale(0.0f, 0.0f);

            sprite_.setColor(sf::Color(
                static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                static_cast<sf::Uint8>(200 + misc::random::Int(55))));
        }

        bool Sparkle::Update(const float ELAPSED_TIME_SEC)
        {
            if (isFinished_)
            {
                return true;
            }

            // rotate
            sprite_.setPosition(0.0f, 0.0f);
            sprite_.setScale(1.0f, 1.0f);

            sprite_.setOrigin(
                sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);

            sprite_.rotate(misc::random::Float(0.5f, 3.0f));

            auto const SLIDER_POS { slider_.Update(ELAPSED_TIME_SEC) };

            // set scale
            auto const SCALE { (
                (SLIDER_POS < 0.5f) ? targetScale_ * (SLIDER_POS * 2.0f)
                                    : targetScale_ * (1.0f - ((SLIDER_POS - 0.5f) * 2.0f))) };

            sprite_.setScale(SCALE, SCALE);

            sprite_.setOrigin(
                sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);

            sprite_.setPosition(posV_);

            // check if done animating
            isFinished_ = slider_.IsDone();
            return isFinished_;
        }

        void Sparkle::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            states.blendMode = sf::BlendAdd;
            target.draw(sprite_, states);
        }

        SparkleAnimation::SparkleAnimation(
            const sf::FloatRect & REGION,
            const float SCALE_BASE,
            const float SCALE_VARIATION_RATIO,
            const float EMIT_RATE_BASE_PER_SEC,
            const float EMIT_RATE_VARATION_RATIO,
            const float DURATION_SEC,
            const float SPEED_BASE,
            const float SPEED_VARIATION_RATIO)
            : REGION_(REGION)
            , SCALE_BASE_(SCALE_BASE)
            , SCALE_VAR_RATIO_(SCALE_VARIATION_RATIO)
            , SEC_PER_EMIT_BASE_(1.0f / EMIT_RATE_BASE_PER_SEC)
            , SEC_PER_EMIT_VAR_RATIO_(EMIT_RATE_VARATION_RATIO)
            , DURATION_SEC_(DURATION_SEC)
            , SPEED_BASE_(SPEED_BASE)
            , SPEED_VAR_RATIO_(SPEED_VARIATION_RATIO)
            , emitTimerSec_(0.0f)
            , emitTimerDurationSec_(0.0f)
            , durationTimerSec_(0.0f)
            , isFinished_(false)
            , sparkCachedTexture1_("media-images-misc-spark1")
            , sparkCachedTexture2_("media-images-misc-spark2")
            , sparkCachedTexture3_("media-images-misc-spark3")
            , sparkleVec_()
        {}

        void SparkleAnimation::Update(const float ELAPSED_TIME_SEC)
        {
            durationTimerSec_ += ELAPSED_TIME_SEC;
            if (durationTimerSec_ < DURATION_SEC_)
            {
                emitTimerSec_ += ELAPSED_TIME_SEC;
            }

            if ((false == isFinished_) && (emitTimerSec_ > emitTimerDurationSec_))
            {
                emitTimerSec_ = 0.0f;

                emitTimerDurationSec_
                    = ValueWithRandomVariance(SEC_PER_EMIT_BASE_, SEC_PER_EMIT_VAR_RATIO_);

                auto const ADJ_RECT { sfml_util::ShrinkToSquareAndReCenterCopy(REGION_) };

                auto const POS_LEFT { ADJ_RECT.left + misc::random::Float(ADJ_RECT.width) };
                auto const POS_TOP { ADJ_RECT.top + misc::random::Float(ADJ_RECT.height) };

                const sf::Texture & RANDOM_TEXTURE_REF { [&]() {
                    auto const RAND { misc::random::Int(2) };
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

                sparkleVec_.emplace_back(Sparkle(
                    RANDOM_TEXTURE_REF,
                    sf::Vector2f(POS_LEFT, POS_TOP),
                    ValueWithRandomVariance(SPEED_BASE_, SPEED_VAR_RATIO_),
                    ValueWithRandomVariance(SCALE_BASE_, SCALE_VAR_RATIO_)));
            }

            auto areAllSparklesFinished { true };
            for (auto & nextSparkle : sparkleVec_)
            {
                if (nextSparkle.IsFinished() == false)
                {
                    areAllSparklesFinished = false;
                    nextSparkle.Update(ELAPSED_TIME_SEC);
                }
            }

            isFinished_ = areAllSparklesFinished;
        }

        void SparkleAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            for (auto & nextSparkle : sparkleVec_)
            {
                if (nextSparkle.IsFinished() == false)
                {
                    nextSparkle.draw(target, states);
                }
            }
        }

        float SparkleAnimation::ValueWithRandomVariance(
            const float BASE, const float VARIANCE_RATIO) const
        {
            if (misc::IsRealZero(VARIANCE_RATIO))
            {
                return BASE;
            }
            else
            {
                auto const VARIATION_SPAN { BASE * VARIANCE_RATIO };
                return (BASE - (VARIATION_SPAN * 0.5f)) + misc::random::Float(VARIATION_SPAN);
            }
        }

    } // namespace animation
} // namespace sfml_util
} // namespace heroespath
