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
// sparks-animation.hpp
//
#include "sparks-animation.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "misc/random.hpp"


namespace heroespath
{
namespace sfml_util
{
namespace animation
{

    Spark::Spark(const sf::Texture &  TEXTURE,
                 const sf::Vector2f & START_POS_V,
                 const sf::Vector2f & END_POS_V,
                 const float          BASE_SPEED,
                 const float          START_SCALE,
                 const float          END_SCALE,
                 const float          ROTATION_SPEED,
                 const sf::Color &    START_COLOR,
                 const sf::Color &    END_COLOR)
    :
        isFinished_   (false),
        sprite_       (TEXTURE),
        startPosV_    (START_POS_V),
        endPosV_      (END_POS_V),
        startScale_   (START_SCALE),
        endScale_     (END_SCALE),
        rotationSpeed_(ROTATION_SPEED),
        startColor_   (START_COLOR),
        endColor_     (END_COLOR),
        slider_       (BASE_SPEED, 0.5f)//start half way so everything moves fast at first and then slows down
    {
        //set initial sprite values (initial rotation is random)
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);
        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);
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

        auto const SLIDER_POS{ (slider_.Update(ELAPSED_TIME_SEC) - 0.5f) * 2.0f };

        //set rotation (must preset position and scale prior to rotating...grumble)
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);
        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);
        sprite_.rotate(rotationSpeed_ * SLIDER_POS);
        //sprite_.setOrigin(0.0f, 0.0f);

        //set position
        auto const POS_X{ startPosV_.x + ((endPosV_.x - startPosV_.x) * SLIDER_POS) };
        auto const POS_Y{ startPosV_.y + ((endPosV_.y - startPosV_.y) * SLIDER_POS) };
        sprite_.setPosition(POS_X, POS_Y);

        //set color
        sprite_.setColor( sfml_util::color::TransitionColor(startColor_, endColor_, SLIDER_POS) );

        //set scale
        auto const SCALE{ startScale_  - ((endScale_ - startScale_) * (1.0f - SLIDER_POS)) };
        sprite_.setScale(SCALE, SCALE);

        //check if done animating
        isFinished_ = slider_.IsDone();
        return isFinished_;
    }


    void Spark::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sprite_, states);
    }


    SparksAnimation::SparksAnimation(const bool            WILL_EMIT_RIGHT,
                                     const sf::FloatRect & REGION,
                                     const float           SPRAY_RATIO,
                                     const float           SCALE_BASE,
                                     const float           SCALE_VARIATION_RATIO,
                                     const float           EMIT_RATE_RATIO,
                                     const float           DURATION_SEC,
                                     const float           SPEED_BASE,
                                     const float           SPEED_VARIATION_RATIO,
                                     const float           ROTATION_SPEED_BASE,
                                     const float           ROTATION_SPEED_VARIATION_RATIO)
    :
        WILL_EMIT_RIGHT_         (WILL_EMIT_RIGHT),
        REGION_                  (REGION),
        SPRAY_RATIO_MINOR_       (SPRAY_RATIO),
        SPRAY_RATIO_MAJOR_       (1.0f - SPRAY_RATIO),
        SCALE_BASE_              (SCALE_BASE),
        SCALE_VAR_RATIO_         (SCALE_VARIATION_RATIO),
        SEC_PER_EMIT_            (1.0f / (50.0f + (EMIT_RATE_RATIO * 150.0f))),
        DURATION_SEC_            (DURATION_SEC),
        SPEED_BASE_              (SPEED_BASE),
        SPEED_VAR_RATIO_         (SPEED_VARIATION_RATIO),
        ROTATION_SPEED_BASE_     (ROTATION_SPEED_BASE),
        ROTATION_SPEED_VAR_RATIO_(ROTATION_SPEED_VARIATION_RATIO),
        emitTimerSec_            (0.0f),
        durationTimerSec_        (0.0f),
        isFinished_              (false),
        sparkTexture1_           (),
        sparkTexture2_           (),
        sparkTexture3_           (),
        sparkVec_                ()
    {
        LoadTexture(sparkTexture1_, game::GameDataFile::Instance()->
            GetMediaPath("media-images-misc-spark1"));

        LoadTexture(sparkTexture2_, game::GameDataFile::Instance()->
            GetMediaPath("media-images-misc-spark2"));

        LoadTexture(sparkTexture3_, game::GameDataFile::Instance()->
            GetMediaPath("media-images-misc-spark3"));

        sfml_util::Mask(sparkTexture1_);
        sfml_util::Mask(sparkTexture2_);
        sfml_util::Mask(sparkTexture3_);

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

            auto const EMITTER_VERTICAL_SPAN{ (SPRAY_RATIO_MINOR_ * REGION_.height) };
            auto const START_POS_TOP{ ((REGION_.top + (REGION_.height * 0.5f)) - (EMITTER_VERTICAL_SPAN * 0.5f)) + misc::random::Float(EMITTER_VERTICAL_SPAN) };

            auto const EMITTER_HORIZ_OFFSET{ misc::random::Float(SPRAY_RATIO_MINOR_ * 0.5f * REGION_.width) };
            auto const START_POS_LEFT{ ((WILL_EMIT_RIGHT_) ? REGION_.left + EMITTER_HORIZ_OFFSET : (REGION_.left + REGION_.width) - EMITTER_HORIZ_OFFSET) };

            auto const TARGET_VERT_SPAN{ misc::random::Float(REGION_.height * SPRAY_RATIO_MAJOR_) };
            auto const END_POS_TOP{ ((misc::random::Bool()) ? START_POS_TOP + TARGET_VERT_SPAN : START_POS_TOP - TARGET_VERT_SPAN) };

            auto const TARGET_HORIZ_SPAN_MIN{ (REGION_.width * SPRAY_RATIO_MINOR_) };
            auto const TARGET_HORIZ_SPAN_MAX{ (REGION_.width * SPRAY_RATIO_MAJOR_) };
            auto const TARGET_HORIZ_SPAN{ misc::random::Float(TARGET_HORIZ_SPAN_MIN, TARGET_HORIZ_SPAN_MAX) };
            auto const END_POS_LEFT{ ((WILL_EMIT_RIGHT_) ? START_POS_LEFT + TARGET_HORIZ_SPAN : START_POS_LEFT - TARGET_HORIZ_SPAN) };

            const sf::Texture * const TEXTURE_PTR{ [&]()
                {
                    auto const RAND{ misc::random::Int(2) };
                    if (RAND == 0)
                    {
                        return & sparkTexture1_;
                    }
                    else if (RAND == 1)
                    {
                        return & sparkTexture2_;
                    }
                    else
                    {
                        return & sparkTexture3_;
                    }
                }() };

            sparkVec_.push_back( Spark(
                * TEXTURE_PTR,
                sf::Vector2f(START_POS_LEFT, START_POS_TOP),
                sf::Vector2f(END_POS_LEFT, END_POS_TOP),
                ValueWithRandomVariance(SPEED_BASE_, SPEED_VAR_RATIO_),
                ValueWithRandomVariance(SCALE_BASE_, SCALE_VAR_RATIO_),
                0.001f,
                ValueWithRandomVariance(ROTATION_SPEED_BASE_, ROTATION_SPEED_VAR_RATIO_),
                sf::Color(255,
                          255,
                          static_cast<sf::Uint8>(misc::random::Int(255)),
                          static_cast<sf::Uint8>(misc::random::Int(0, 127))),
                sf::Color(255, 255, static_cast<sf::Uint8>(misc::random::Int(255)), 255) ) );
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


    float SparksAnimation::ValueWithRandomVariance(const float BASE, const float VARIANCE_RATIO) const
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
}
