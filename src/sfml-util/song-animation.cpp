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
// song-animation.hpp
//
#include "song-animation.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "misc/random.hpp"

#include <algorithm>


namespace sfml_util
{
namespace animation
{

    Note::Note(const sf::Texture &  TEXTURE,
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

        //start half way so everything moves fast at first and then slows down
        slider_       (std::max(1.0f, BASE_SPEED), 0.5f)
    {
        //set initial sprite values (initial rotation is random)
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);
        
        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f,
                          sprite_.getLocalBounds().height * 0.5f);

        sprite_.rotate(misc::random::Float(360.0f));
        
        sprite_.setOrigin(0.0f, 0.0f);

        sprite_.setPosition(startPosV_);
        sprite_.setScale(startScale_, startScale_);
        sprite_.setColor(startColor_);
    }


    bool Note::Update(const float ELAPSED_TIME_SEC)
    {
        if (isFinished_)
        {
            return true;
        }
        
        auto const SLIDER_POS{ (slider_.Update(ELAPSED_TIME_SEC) - 0.5f) * 2.0f };

        //set rotation (must preset position and scale prior to rotating...grumble)
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);
        
        sprite_.setOrigin(sprite_.getLocalBounds().width * 0.5f,
                          sprite_.getLocalBounds().height * 0.5f);

        sprite_.rotate(rotationSpeed_ * SLIDER_POS);

        //set position
        auto const POS_X{ startPosV_.x + ((endPosV_.x - startPosV_.x) * SLIDER_POS) };
        auto const POS_Y{ startPosV_.y + ((endPosV_.y - startPosV_.y) * SLIDER_POS) };
        sprite_.setPosition(POS_X, POS_Y);

        //set color
        sprite_.setColor( sfml_util::color::TransitionColor(startColor_,
                                                            endColor_,
                                                            SLIDER_POS) );

        //set scale
        auto const SCALE{ startScale_  + ((endScale_ - startScale_) * SLIDER_POS) };
        sprite_.setScale(SCALE, SCALE);
        
        //check if done animating
        isFinished_ = slider_.GetIsDone();
        return isFinished_;
    }


    void Note::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = sf::BlendAdd;
        target.draw(sprite_, states);
    }


    SongAnimation::SongAnimation(const sf::FloatRect & REGION,
                                 const float           CENTER_VARATION_RATIO,
                                 const float           START_SCALE_BASE,
                                 const float           START_SCALE_VARIATION_RATIO,
                                 const float           END_SCALE_BASE,
                                 const float           END_SCALE_VARIATION_RATIO,
                                 const float           EMIT_RATE_BASE_PER_SEC,
                                 const float           EMIT_RATE_VARATION_RATIO,
                                 const float           DURATION_SEC,
                                 const float           SPEED_BASE,
                                 const float           SPEED_VARIATION_RATIO,
                                 const float           ROTATION_SPEED_BASE,
                                 const float           ROTATION_SPEED_VARIATION_RATIO)
    :
        REGION_                  (REGION),
        CENTER_VAR_RATIO_        (CENTER_VARATION_RATIO),
        START_SCALE_BASE_        (START_SCALE_BASE),
        START_SCALE_VAR_RATIO_   (START_SCALE_VARIATION_RATIO),
        END_SCALE_BASE_          (END_SCALE_BASE),
        END_SCALE_VAR_RATIO_     (END_SCALE_VARIATION_RATIO),
        SEC_PER_EMIT_BASE_       (1.0f / EMIT_RATE_BASE_PER_SEC),
        SEC_PER_EMIT_VAR_RATIO_  (EMIT_RATE_VARATION_RATIO),
        DURATION_SEC_            (DURATION_SEC),
        SPEED_BASE_              (SPEED_BASE),
        SPEED_VAR_RATIO_         (SPEED_VARIATION_RATIO),
        ROTATION_SPEED_BASE_     (ROTATION_SPEED_BASE),
        ROTATION_SPEED_VAR_RATIO_(ROTATION_SPEED_VARIATION_RATIO),
        emitTimerSec_            (0.0f),
        emitTimerDurationSec_    (0.0f),
        durationTimerSec_        (0.0f),
        isFinished_              (false),
        noteTexture1_            (),
        noteTexture2_            (),
        noteTexture3_            (),
        noteTexture4_            (),
        noteTexture5_            (),
        noteTexture6_            (),
        noteVec_                 ()
    {
        LoadTexture(noteTexture1_, game::GameDataFile::Instance()->GetMediaPath(
            "media-images-misc-note1"));
        
        LoadTexture(noteTexture2_, game::GameDataFile::Instance()->GetMediaPath(
            "media-images-misc-note2"));

        LoadTexture(noteTexture3_, game::GameDataFile::Instance()->GetMediaPath(
            "media-images-misc-note3"));

        LoadTexture(noteTexture4_, game::GameDataFile::Instance()->GetMediaPath(
            "media-images-misc-note4"));

        LoadTexture(noteTexture5_, game::GameDataFile::Instance()->GetMediaPath(
            "media-images-misc-note5"));

        LoadTexture(noteTexture6_, game::GameDataFile::Instance()->GetMediaPath(
            "media-images-misc-note6"));

        noteVec_.reserve(static_cast<std::size_t>(EMIT_RATE_BASE_PER_SEC * DURATION_SEC) * 2);
    }


    void SongAnimation::Update(const float ELAPSED_TIME_SEC)
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

            auto const HORIZ_CENTER     { REGION_.left + (REGION_.width * 0.5f) };
            auto const HORIZ_RAND_SPAN  { (REGION_.width * 0.5f) * CENTER_VAR_RATIO_ };
            auto const HORIZ_BASE       { HORIZ_CENTER - (HORIZ_RAND_SPAN * 0.5f) };
            auto const HORIZ_START_POS  { HORIZ_BASE + misc::random::Float(HORIZ_RAND_SPAN) };
            auto const HORIZ_END_POS    { ((HORIZ_START_POS < HORIZ_CENTER) ?
                (HORIZ_START_POS - misc::random::Float(HORIZ_START_POS - REGION_.left)) :
                (HORIZ_START_POS + misc::random::Float(
                    (REGION_.left + REGION_.width) - HORIZ_START_POS))) };

            auto const VERT_CENTER{ REGION_.top + (REGION_.height * 0.5f) };
            auto const VERT_RAND_SPAN{ (REGION_.height * 0.5f) * CENTER_VAR_RATIO_ };
            auto const VERT_BASE{ VERT_CENTER - (VERT_RAND_SPAN * 0.5f) };
            auto const VERT_START_POS{ VERT_BASE + misc::random::Float(VERT_RAND_SPAN) };
            auto const VERT_END_POS{ ((VERT_START_POS < VERT_CENTER) ?
                (VERT_START_POS - misc::random::Float(VERT_START_POS - REGION_.top)) :
                (VERT_START_POS + misc::random::Float(
                    (REGION_.top + REGION_.height) - VERT_START_POS))) };

            sf::Texture & textureRef{ noteTexture1_ };
            auto const WHICH_TEXTURE_NUM{ misc::random::Int(1, 100) };
            if (WHICH_TEXTURE_NUM < 30)
            {
                textureRef = noteTexture1_;
            }
            else if (WHICH_TEXTURE_NUM < 55)
            {
                textureRef = noteTexture2_;
            }
            else if (WHICH_TEXTURE_NUM < 75)
            {
                textureRef = noteTexture3_;
            }
            else if (WHICH_TEXTURE_NUM < 85)
            {
                textureRef = noteTexture4_;
            }
            else if (WHICH_TEXTURE_NUM < 90)
            {
                textureRef = noteTexture5_;
            }
            else
            {
                textureRef = noteTexture6_;
            }

            noteVec_.push_back( Note(
                textureRef,
                sf::Vector2f(HORIZ_START_POS, VERT_START_POS),
                sf::Vector2f(HORIZ_END_POS, VERT_END_POS),
                ValueWithRandomVariance(SPEED_BASE_, SPEED_VAR_RATIO_),
                ValueWithRandomVariance(START_SCALE_BASE_, START_SCALE_VAR_RATIO_),
                ValueWithRandomVariance(END_SCALE_BASE_, END_SCALE_VAR_RATIO_),
                ValueWithRandomVariance(ROTATION_SPEED_BASE_, ROTATION_SPEED_VAR_RATIO_),
                sf::Color(static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                          static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                          static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                          255),
                sf::Color(static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                          static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                          static_cast<sf::Uint8>(200 + misc::random::Int(55)),
                          0) ) );
        }

        auto areAllNotesFinished{ true };
        for (auto & nextNote : noteVec_)
        {
            if (nextNote.IsFinished() == false)
            {
                areAllNotesFinished = false;
                nextNote.Update(ELAPSED_TIME_SEC);
            }
        }

        isFinished_ = areAllNotesFinished;
    }


    void SongAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (auto & nextNote : noteVec_)
        {
            if (nextNote.IsFinished() == false)
            {
                nextNote.draw(target, states);
            }
        }
    }


    float SongAnimation::ValueWithRandomVariance(const float BASE,
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
