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
// animation.cpp
//
#include "animation.hpp"

#include "sfml-util/loaders.hpp"


namespace sfml_util
{

    SingleTextureAnimation::SingleTextureAnimation( const std::string &   ENTITY_NAME,
                                                    const std::string &   TEXTURE_FILE_PATH,
                                                    const float           SCREEN_POS_LEFT,
                                                    const float           SCREEN_POS_TOP,
                                                    const unsigned int    FRAME_WIDTH,
                                                    const unsigned int    FRAME_HEIGHT,
                                                    const float           TIME_BETWEEN_FRAMES,
                                                    const std::size_t     FRAME_COUNT,
                                                    const sf::BlendMode & BLEND_MODE,
                                                    const float           SCALE_HORIZ,
                                                    const float           SCALE_VERT,
                                                    const sf::Color &     COLOR,
                                                    const unsigned int    FIRST_FRAME_POS_LEFT,
                                                    const unsigned int    FIRST_FRAME_POS_TOP,
                                                    const unsigned int    FRAME_HORIZ_GAP,
                                                    const unsigned int    FRAME_VERT_GAP )
    :
        GuiEntity               (std::string(ENTITY_NAME).append("_SingleTextureAnimation"), SCREEN_POS_LEFT, SCREEN_POS_TOP),
        FRAME_WIDTH_            (FRAME_WIDTH),
        FRAME_HEIGHT_           (FRAME_HEIGHT),
        FRAME_COUNT_            (FRAME_COUNT),
        FIRST_FRAME_POS_LEFT_   (FIRST_FRAME_POS_LEFT),
        FIRST_FRAME_POS_TOP_    (FIRST_FRAME_POS_TOP),
        FRAME_HORIZ_GAP_        (FRAME_HORIZ_GAP),
        FRAME_VERT_GAP_         (FRAME_VERT_GAP),
        BLEND_MODE_             (BLEND_MODE),
        timeBetweenFrames_      (TIME_BETWEEN_FRAMES),
        sprite_                 (),
        textureSPtr_            (),
        rects_                  (),
        currentFrame_           (0),
        frameTimerSec_          (0.0f),
        color_                  (COLOR)
    {
        sfml_util::LoadImageOrTextureSPtr(textureSPtr_, TEXTURE_FILE_PATH);
        textureSPtr_->setSmooth(true);

        sprite_.setTexture( * textureSPtr_ );
        sprite_.setPosition(SCREEN_POS_LEFT, SCREEN_POS_TOP);
        sprite_.setScale(SCALE_HORIZ, SCALE_VERT);
        sprite_.setColor(color_);

        SetEntityRegion( sf::FloatRect(SCREEN_POS_LEFT, SCREEN_POS_TOP, static_cast<float>(FRAME_WIDTH), static_cast<float>(FRAME_HEIGHT)) );

        auto const TEXTURE_WIDTH (textureSPtr_->getSize().x);
        auto const TEXTURE_HEIGHT(textureSPtr_->getSize().y);

        auto posX(FIRST_FRAME_POS_LEFT_);
        auto posY(FIRST_FRAME_POS_TOP_);

        while(posY < (TEXTURE_HEIGHT - FRAME_HEIGHT_))
        {
            if (rects_.empty() == false)
                posY += FRAME_HEIGHT_ + FRAME_VERT_GAP_;

            while (posX < (TEXTURE_WIDTH - FRAME_WIDTH_))
            {
                if (rects_.empty() == false)
                    posX += FRAME_WIDTH_ + FRAME_HORIZ_GAP_;

                rects_.push_back( sf::IntRect(static_cast<int>(posX), static_cast<int>(posY), static_cast<int>(FRAME_WIDTH_), static_cast<int>(FRAME_HEIGHT_)) );

                if ((FRAME_COUNT_ != 0) && (rects_.size() == FRAME_COUNT_))
                    break;
            };
            posX = FIRST_FRAME_POS_LEFT_;

            if ((FRAME_COUNT_ != 0) && (rects_.size() == FRAME_COUNT_))
                break;
        };
    }


    SingleTextureAnimation::~SingleTextureAnimation()
    {}


    void SingleTextureAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
        states.blendMode = BLEND_MODE_;
        target.draw(sprite_, states);
        states.blendMode = ORIG_BLEND_MODE;
    }


    void SingleTextureAnimation::SetEntityPos(const sf::Vector2f & V)
    {
        entityRegion_ = sf::FloatRect(V.x, V.y, entityRegion_.width, entityRegion_.height);
        sprite_.setPosition(V);
    }


    void SingleTextureAnimation::SetEntityPos(const float LEFT, const float TOP)
    {
        SetEntityPos(sf::Vector2f(LEFT, TOP));
    }


    bool SingleTextureAnimation::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        auto didReachEndOfFrames{ false };

        frameTimerSec_ += ELAPSED_TIME_SEC;
        if (frameTimerSec_ > timeBetweenFrames_)
        {
            frameTimerSec_ -= timeBetweenFrames_;
            if (++currentFrame_ >= (rects_.size() - 1))
            {
                currentFrame_ = 0;
                didReachEndOfFrames = true;
            }

            sprite_.setTextureRect( rects_[currentFrame_] );
            sprite_.setColor(color_);
        }

        return didReachEndOfFrames;
    }


    void SingleTextureAnimation::MovePosition(const float HORIZ, const float VERT)
    {
        entityRegion_ = sf::FloatRect(entityRegion_.left + HORIZ, entityRegion_.top + VERT, entityRegion_.width, entityRegion_.height);
        sprite_.move(HORIZ, VERT);
    }


    void SingleTextureAnimation::SetPosition(const float POS_LEFT, const float POS_TOP)
    {
        entityRegion_ = sf::FloatRect(POS_LEFT, POS_TOP, entityRegion_.width, entityRegion_.height);
        sprite_.setPosition(POS_LEFT, POS_TOP);
    }


    MultiTextureAnimation::MultiTextureAnimation(const std::string &   ENTITY_NAME,
                                                 const std::string &   TEXTURES_DIRECTORY,
                                                 const float           SCREEN_POS_LEFT,
                                                 const float           SCREEN_POS_TOP,
                                                 const float           TIME_BETWEEN_FRAMES,
                                                 const float           SCALE_HORIZ,
                                                 const float           SCALE_VERT,
                                                 const sf::Color &     COLOR,
                                                 const sf::BlendMode & BLEND_MODE)
    :
        GuiEntity           (std::string(ENTITY_NAME).append("_MutiTextureAnimation"), SCREEN_POS_LEFT, SCREEN_POS_TOP),
        BLEND_MODE_         (BLEND_MODE),
        timeBetweenFrames_  (TIME_BETWEEN_FRAMES),
        sprite_             (),
        textureSVec_        (),
        currentFrame_       (0),
        frameTimerSec_      (0.0f),
        color_              (COLOR)
    {
        sfml_util::LoadAllImageOrTextureInDir(textureSVec_, TEXTURES_DIRECTORY);

        for (auto nextTextureSPtr : textureSVec_)
            nextTextureSPtr->setSmooth(true);

        sprite_.setTexture( * textureSVec_[0] );
        sprite_.setPosition(SCREEN_POS_LEFT, SCREEN_POS_TOP);
        sprite_.setScale(SCALE_HORIZ, SCALE_VERT);
        sprite_.setColor(color_);

        SetEntityRegion( sf::FloatRect(SCREEN_POS_LEFT, SCREEN_POS_TOP, sprite_.getGlobalBounds().width, sprite_.getGlobalBounds().height) );
    }


    MultiTextureAnimation::~MultiTextureAnimation()
    {}


    void MultiTextureAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
        states.blendMode = BLEND_MODE_;
        target.draw(sprite_, states);
        states.blendMode = ORIG_BLEND_MODE;
    }


    void MultiTextureAnimation::SetEntityPos(const sf::Vector2f & V)
    {
        entityRegion_ = sf::FloatRect(V.x, V.y, entityRegion_.width, entityRegion_.height);
        sprite_.setPosition(V);
    }


    void MultiTextureAnimation::SetEntityPos(const float LEFT, const float TOP)
    {
        SetEntityPos( sf::Vector2f(LEFT, TOP) );
    }


    bool MultiTextureAnimation::UpdateTime(const float SECONDS)
    {
        auto didReachEndOfFrames{ false };

        frameTimerSec_ += SECONDS;
        if (frameTimerSec_ > timeBetweenFrames_)
        {
            frameTimerSec_ -= timeBetweenFrames_;
            if (++currentFrame_ >= (textureSVec_.size() - 1))
            {
                currentFrame_ = 0;
                didReachEndOfFrames = true;
            }

            sprite_.setTexture( * textureSVec_[currentFrame_] );
            sprite_.setColor(color_);
        }

        return didReachEndOfFrames;
    }


    void MultiTextureAnimation::MovePosition(const float HORIZ, const float VERT)
    {
        entityRegion_ = sf::FloatRect(entityRegion_.left + HORIZ, entityRegion_.top + VERT, entityRegion_.width, entityRegion_.height);
        sprite_.move(HORIZ, VERT);
    }


    void MultiTextureAnimation::SetPosition(const float POS_LEFT, const float POS_TOP)
    {
        entityRegion_ = sf::FloatRect(POS_LEFT, POS_TOP, entityRegion_.width, entityRegion_.height);
        sprite_.setPosition(POS_LEFT, POS_TOP);
    }

}
