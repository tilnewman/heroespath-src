// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// sparkle-animation.hpp
//
#include "sparkle-animation.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "misc/random.hpp"

#include <algorithm>


namespace sfml_util
{
namespace animation
{

    Sparkle::Sparkle(const sf::Texture &  TEXTURE,
                     const sf::Vector2f & POS_V,
                     const float          SPEED,
                     const float          TARGET_SCALE)
    :
        isFinished_ (false),
        sprite_     (TEXTURE),
        posV_       (POS_V),
        targetScale_(TARGET_SCALE),
        
        //start half way so everything moves fast at first and then slows down
        slider_       (std::max(1.0f, SPEED))
    {
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);

        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f,
                          sprite_.getLocalBounds().height * 0.5f);
        
        sprite_.setRotation(misc::random::Float(360.0f));
        
        sprite_.setPosition(posV_);
        sprite_.setScale(0.0f, 0.0f);

        sprite_.setColor( sf::Color(static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                                    static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                                    static_cast<sf::Uint8>(200 + misc::random::Int(55))) );
    }


    bool Sparkle::Update(const float ELAPSED_TIME_SEC)
    {
        if (isFinished_)
        {
            return true;
        }

        //rotate
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);
        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f,
                          sprite_.getLocalBounds().height * 0.5f);
        sprite_.rotate(misc::random::Float(0.5f, 3.0f));

        
        auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };

        //set scale
        auto const SCALE{ ((SLIDER_POS < 0.5f) ?
            targetScale_ * (SLIDER_POS * 2.0f) :
            targetScale_ * (1.0f - ((SLIDER_POS - 0.5f) * 2.0f))) };

        sprite_.setScale(SCALE, SCALE);

        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f,
                          sprite_.getLocalBounds().height * 0.5f);

        sprite_.setPosition(posV_);
        
        //check if done animating
        isFinished_ = slider_.IsDone();
        return isFinished_;
    }


    void Sparkle::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = sf::BlendAdd;
        target.draw(sprite_, states);
    }


    SparkleAnimation::SparkleAnimation(const sf::FloatRect & REGION,
                                       const float           SCALE_BASE,
                                       const float           SCALE_VARIATION_RATIO,
                                       const float           EMIT_RATE_BASE_PER_SEC,
                                       const float           EMIT_RATE_VARATION_RATIO,
                                       const float           DURATION_SEC,
                                       const float           SPEED_BASE,
                                       const float           SPEED_VARIATION_RATIO)
    :
        REGION_                  (REGION),
        SCALE_BASE_              (SCALE_BASE),
        SCALE_VAR_RATIO_         (SCALE_VARIATION_RATIO),
        SEC_PER_EMIT_BASE_       (1.0f / EMIT_RATE_BASE_PER_SEC),
        SEC_PER_EMIT_VAR_RATIO_  (EMIT_RATE_VARATION_RATIO),
        DURATION_SEC_            (DURATION_SEC),
        SPEED_BASE_              (SPEED_BASE),
        SPEED_VAR_RATIO_         (SPEED_VARIATION_RATIO),
        emitTimerSec_            (0.0f),
        emitTimerDurationSec_    (0.0f),
        durationTimerSec_        (0.0f),
        isFinished_              (false),
        sparkTexture1_           (),
        sparkTexture2_           (),
        sparkTexture3_           (),
        sparkleVec_              ()
    {
        LoadTexture(sparkTexture1_, game::GameDataFile::Instance()->
            GetMediaPath("media-images-misc-spark1"));

        LoadTexture(sparkTexture2_, game::GameDataFile::Instance()->
            GetMediaPath("media-images-misc-spark2"));

        LoadTexture(sparkTexture3_, game::GameDataFile::Instance()->
            GetMediaPath("media-images-misc-spark3"));

        sparkleVec_.reserve(static_cast<std::size_t>(EMIT_RATE_BASE_PER_SEC * DURATION_SEC) * 2);
    }


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

            emitTimerDurationSec_ = ValueWithRandomVariance(SEC_PER_EMIT_BASE_,
                                                            SEC_PER_EMIT_VAR_RATIO_);

            auto const ADJ_RECT{ sfml_util::MakeMinimalSquareAndCenter(REGION_) };

            auto const POS_LEFT{ ADJ_RECT.left + misc::random::Float(ADJ_RECT.width) };
            auto const POS_TOP{ ADJ_RECT.top + misc::random::Float(ADJ_RECT.height) };

            const sf::Texture * const TEXTURE_PTR{ [&]()
                {
                    auto const RAND{ misc::random::Int(2) };
                    if (RAND == 0)
                    {
                        return &sparkTexture1_;
                    }
                    else if (RAND == 1)
                    {
                        return &sparkTexture2_;
                    }
                    else
                    {
                        return &sparkTexture3_;
                    }
                }() };

            sparkleVec_.push_back( Sparkle(
                * TEXTURE_PTR,
                sf::Vector2f(POS_LEFT, POS_TOP),
                ValueWithRandomVariance(SPEED_BASE_, SPEED_VAR_RATIO_),
                ValueWithRandomVariance(SCALE_BASE_, SCALE_VAR_RATIO_)) );
        }

        auto areAllSparklesFinished{ true };
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


    float SparkleAnimation::ValueWithRandomVariance(const float BASE,
                                                  const float VARIANCE_RATIO) const
    {
        if (misc::IsRealZero(VARIANCE_RATIO))
        {
            return BASE;
        }
        else
        {
            auto const VARIATION_SPAN{ BASE * VARIANCE_RATIO };
            return (BASE - (VARIATION_SPAN * 0.5f)) + misc::random::Float(VARIATION_SPAN);
        }
    }

}
}
